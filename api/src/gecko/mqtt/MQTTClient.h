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
    /*
        intent(MQTTClient): I needed a thread-safe wrapper around
        Paho MQTT's C-style API.

        This class is designed to keep track of inflight requests
        and notify consumers of incoming messages. When subscribing
        to a topic, a consumer can specify a handler that will only
        be called for incoming messages on that topic.
    */
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

        /*
            Connect to the server.

            Returns true if the connection process was commenced.
            Otherwise, false.
        */
        bool Connect();

        /*
            Connect to the server synchronously.

            Returns true if the connection process was completed
            successfully.
            Otherwise, false.
        */
        bool ConnectSync();

        /*
            Try to subscribe to a topic.

            messageReceived is called when an incoming message matches
            the associated subscribed topic.

            Returns a handle to the subscription if commenced.
            Otherwise, std::nullopt. In this case, subFail will not be
            called.
        */
        std::optional<uint32_t> SubscribeToTopic(const std::string& topic,
                                                 InflightHandler subSucc,
                                                 InflightHandler subFail,
                                                 ReceivedHandler messageReceived,
                                                 void* context1 = nullptr,
                                                 void* context2 = nullptr);

        /*
            Try to publish a message.

            Returns true if the publishing process was commenced.
            Otherwise, false.
        */
        bool PublishMessage(const std::string& topic,
                            const std::span<const uint8_t> message,
                            bool retained,
                            InflightHandler pubSucc,
                            InflightHandler pubFail,
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
        void Callback_Published(MQTTAsync_successData5* s);
        void Callback_PublishFailed(MQTTAsync_failureData5* s);
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

        std::atomic<uint32_t> m_nextSubscriptionID{ 1 };

        std::mutex                             m_subscriptionsMutex;
        std::unordered_map<uint32_t, Receiver> m_subscriptions;
    };
}