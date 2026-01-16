#pragma once
#include <atomic>
#include <mutex>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>
#include "MQTTAsync.h"

namespace Gecko::API::MQTT
{
    class MQTTClient
    {
    public:
        using ReceivedHandler = void (*)(void* context1, void* context2, std::string_view topic, std::span<uint8_t> message);
        using InflightHandler = void (*)(void* context1, void* context2);

        enum class ConnectionStatus
        {
            NotConnected,
            Connecting,
            Connected,
            ConnectionFailed
        };

        MQTTClient(std::string address,
                   std::string brokerCertPath,
                   std::string username,
                   std::string password)
            : m_address(std::move(address)),
              m_brokerCertPath(std::move(brokerCertPath)),
              m_username(std::move(username)),
              m_password(std::move(password)) {}

        bool Connect();

        bool ConnectSync();

        std::optional<uint32_t> SubscribeToTopic(const std::string& topic,
                                                 InflightHandler subSucc,
                                                 InflightHandler subFail,
                                                 ReceivedHandler messageReceived,
                                                 void* context1 = nullptr,
                                                 void* context2 = nullptr);

    private:
        struct Receiver
        {
            ReceivedHandler handler;
            void* context1;
            void* context2;
        };

        struct Inflight
        {
            InflightHandler handlerSucc;
            InflightHandler handlerFail;
            void* context1;
            void* context2;
        };

        void Callback_Connected(MQTTAsync_successData5*);
        void Callback_ConnectionFailed(MQTTAsync_failureData5*);
        void Callback_ConnectionLost(char*);
        void Callback_Subscribed(MQTTAsync_successData5* s);
        void Callback_SubscriptionFailed(MQTTAsync_failureData5* s);
        int Callback_MessageReceived(char *topicName,
                                     int topicLen,
                                     MQTTAsync_message *message);

        std::string m_address;
        std::string m_brokerCertPath;
        std::string m_username;
        std::string m_password;

        MQTTAsync m_client;

        std::atomic<ConnectionStatus> m_connectionStatus{ ConnectionStatus::NotConnected };

        std::mutex                                    m_inflightMutex;
        std::unordered_map<MQTTAsync_token, Inflight> m_inflight;

        std::mutex                             m_subscriptionsMutex;
        std::unordered_map<uint32_t, Receiver> m_subscriptions;

        std::atomic<uint32_t> m_nextSubscriptionID{ 1 };
    };
}