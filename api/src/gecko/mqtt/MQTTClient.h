#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "MQTTAsync.h"

namespace Gecko::API::MQTT
{
    class MQTTClient
    {
    public:
        MQTTClient(std::string address, std::string brokerCertPath, std::string username, std::string password)
            : m_address(std::move(address)),
              m_brokerCertPath(std::move(brokerCertPath)),
              m_username(std::move(username)),
              m_password(std::move(password)) {}

        void Connect();

        bool ConnectSync();

        void SubscribeToTopic(const std::string& topic, std::function<void()> success, std::function<void(int)> failure);

        void OnMessageReceived(std::function<void(std::string_view, std::span<uint8_t>)> handler);
        
    private:
        struct SubscribeContext
        {
            std::function<void()> onSuccess;
            std::function<void(int)> onFailure;
        };

        void HandleConnect(MQTTAsync_successData* response);

        void HandleConnectFailure(MQTTAsync_failureData* response);

        void HandleConnectionLost(char* cause);

        int HandleMessageReceived(char *topicName, int topicLen, MQTTAsync_message *message);

        // note (ben): It's hacky, but connectedOrFailedMutex guards
        // m_connected AND m_connectFailed
        std::condition_variable m_connectedOrFailedSignal;
        std::mutex              m_connectedOrFailedMutex;
        bool m_connected{ false };
        bool m_connectFailed{ false };
        int  m_connectFailedCode{ 0 };

        std::mutex                                                             m_messageReceivedHandlersMutex;
        std::vector<std::function<void(std::string_view, std::span<uint8_t>)>> m_messageReceivedHandlers;

        std::mutex                                m_subscribeHandlersMutex;
        std::unordered_map<int, SubscribeContext> m_subscribeHandlers{ 0 };
        int                                       m_subscribeKeyIncrementer{ 0 };

        MQTTAsync m_client;
        std::string m_address;
        std::string m_brokerCertPath;
        std::string m_username;
        std::string m_password;

        static thread_local MQTTAsync_failureData s_genericFailure;
    };
}