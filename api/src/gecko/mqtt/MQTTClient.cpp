#include "gecko/mqtt/MQTTClient.h"
#include "gecko/logging/Logger.h"

namespace Gecko::API::MQTT
{
    using Logging::Logger;

    bool MQTTClient::Connect()
    {
        /*
            Callback wrappers
        */

        const auto callback_connected = [](void* context, MQTTAsync_successData5* response) {
            reinterpret_cast<MQTTClient *>(context)->Callback_Connected(response); };

        const auto callback_connectionFailed = [](void* context, MQTTAsync_failureData5* response) {
            reinterpret_cast<MQTTClient *>(context)->Callback_ConnectionFailed(response); };

        const auto callback_connectionLost = [](void* context, char* cause) {
            reinterpret_cast<MQTTClient *>(context)->Callback_ConnectionLost(cause); };

        const auto callback_messageReceived = [](void *context, char *topicName, int topicLen, MQTTAsync_message *message) {
            return reinterpret_cast<MQTTClient *>(context)->Callback_MessageReceived(topicName, topicLen, message); };

        /*
            CAS state -> Connecting
        */

        auto swap = m_connectionStatus.load(std::memory_order_acquire);

        if (swap != ConnectionStatus::NotConnected &&
            swap != ConnectionStatus::ConnectionFailed)
            return false;

        if (!m_connectionStatus.compare_exchange_strong(
                swap,
                ConnectionStatus::Connecting,
                std::memory_order_acquire,
                std::memory_order_relaxed))
            return false;

        /*
            Init
        */

        MQTTAsync_createOptions createOpts = MQTTAsync_createOptions_initializer5;

        if (int r; (r = MQTTAsync_createWithOptions(
                &m_client,
                m_address.c_str(),
                "",
                MQTTCLIENT_PERSISTENCE_NONE,
                NULL,
                &createOpts)) != MQTTASYNC_SUCCESS) {
            Logger::Error() << "[MQTTClient.Connect]: Couldn't create MQTT client";
            Logger::Error() << "[MQTTClient.Connect]: ~ Code: " + std::to_string(r);

            m_connectionStatus.store(
                ConnectionStatus::ConnectionFailed, std::memory_order_relaxed);
            m_connectionStatus.notify_all();
            return false;
        }

        MQTTAsync_setCallbacks(
            m_client,
            this,
            callback_connectionLost,
            callback_messageReceived,
            NULL);

        MQTTAsync_SSLOptions sslOpts = MQTTAsync_SSLOptions_initializer;
        sslOpts.enableServerCertAuth = 1;
        sslOpts.trustStore           = m_brokerCertPath.c_str();
        sslOpts.sslVersion           = MQTT_SSL_VERSION_TLS_1_2;

        MQTTAsync_connectOptions connOpts = MQTTAsync_connectOptions_initializer5;

        connOpts.keepAliveInterval = 240;
        connOpts.onSuccess5        = callback_connected;
        connOpts.onFailure5        = callback_connectionFailed;
        connOpts.context           = this;
        connOpts.username          = m_username.c_str();
        connOpts.password          = m_password.c_str();
        connOpts.ssl               = &sslOpts;

        if (auto r = MQTTAsync_connect(m_client, &connOpts))
        {
            Logger::Error() << "[MQTTClient.Connect]: Failed immediately while trying to connect";
            Logger::Error() << "[MQTTClient.Connect]: ~ Code: " + std::to_string(r);

            /* Fail */
            m_connectionStatus.store(
                ConnectionStatus::ConnectionFailed, std::memory_order_relaxed);
            m_connectionStatus.notify_all();
            return false;
        }

        return true;
    }

    bool MQTTClient::ConnectSync()
    {
        if (!Connect()) return false;

        m_connectionStatus.wait(ConnectionStatus::Connecting);
        return m_connectionStatus.load(std::memory_order_relaxed)
            == ConnectionStatus::Connected;
    }

    std::optional<uint32_t> MQTTClient::SubscribeToTopic(const std::string& topic,
                                                         InflightHandler subSucc,
                                                         InflightHandler subFail,
                                                         ReceivedHandler messageReceived,
                                                         void* context1,
                                                         void* context2)
    {
        const auto callback_subscribed = [](void* c, MQTTAsync_successData5* response) {
            reinterpret_cast<MQTTClient*>(c)->Callback_Subscribed(response); };

        const auto callback_subscriptionFailed = [](void* c, MQTTAsync_failureData5* response) {
            reinterpret_cast<MQTTClient*>(c)->Callback_SubscriptionFailed(response); };

        uint32_t subscriptionID
            = m_nextSubscriptionID.fetch_add(1, std::memory_order_relaxed);

        MQTTProperty prop;
        prop.identifier     = MQTTPROPERTY_CODE_SUBSCRIPTION_IDENTIFIER;
        prop.value.integer4 = subscriptionID;

        MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
        opts.onSuccess5 = callback_subscribed;
        opts.onFailure5 = callback_subscriptionFailed;
        opts.context    = this;
        opts.properties = MQTTProperties_initializer;
        opts.properties.max_count = 1;
        opts.properties.count     = 1;
        opts.properties.length    = sizeof(MQTTProperty);
        opts.properties.array     = &prop;

        m_subscriptionsMutex.lock();
        auto [it, inserted] = m_subscriptions.insert({ subscriptionID, Receiver{
            .handler  = messageReceived,
            .context1 = context1,
            .context2 = context2
        } });
        m_subscriptionsMutex.unlock();

        if (!inserted)
            /* Fail (couldn't insert) */
            return std::nullopt;

        if (auto r = MQTTAsync_subscribe(m_client, topic.c_str(), 1, &opts))
        {
            Logger::Warn() << "[MQTTClient.SubscribeToTopic]: Couldn't subscribe to topic " + topic;
            Logger::Warn() << "[MQTTClient.SubscribeToTopic]: ~ Code: " + std::to_string(r);

            /* Fail (couldn't subscribe) */
            m_subscriptionsMutex.lock();
            m_subscriptions.erase(subscriptionID);
            m_subscriptionsMutex.unlock();

            return std::nullopt;
        }

        /*
            note(ben): taking opts.token and using it as a key here is
            technically a race condition. This is explicitely mentioned
            as a "gotcha" in the Paho Async MQTT documentation and the
            behavior is referenced by Paho MQTT Issue 24.

            https://github.com/eclipse-paho/paho.mqtt.c/issues/24

            The issue is that the event may have already been completed
            by the time we are able to use opts.token. So we're given
            this token, but by the time MQTTAsync_subscribe is completed
            the success/failure event associated with the token may have
            already been run.

            A potential solution would be to check if the event was
            completed early in this function and, if it was, handle
            everything here manually.
        */

        m_inflightMutex.lock();
        auto [it_2, inserted_2] = m_inflight.insert({ opts.token, Inflight{
            .handlerSucc = subSucc,
            .handlerFail = subFail,
            .context1    = context1,
            .context2    = context2
        } });
        m_inflightMutex.unlock();

        if (!inserted)
        {
            m_subscriptionsMutex.lock();
            m_subscriptions.erase(subscriptionID);
            m_subscriptionsMutex.unlock();

            return std::nullopt;
        }

        return subscriptionID;
    }

    bool MQTTClient::PublishMessage(const std::string& topic,
                                    const std::span<const uint8_t> message,
                                    bool retained,
                                    InflightHandler pubSucc,
                                    InflightHandler pubFail,
                                    void* context1,
                                    void* context2)
    {
        const auto callback_published = [](void* context, MQTTAsync_successData5* response) {
            reinterpret_cast<MQTTClient *>(context)->Callback_Connected(response); };

        const auto callback_publishFailed = [](void* context, MQTTAsync_failureData5* response) {
            reinterpret_cast<MQTTClient *>(context)->Callback_ConnectionFailed(response); };

        MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
        opts.onSuccess5 = callback_published;
        opts.onFailure5 = callback_publishFailed;
        opts.context = this;

        if (auto r = MQTTAsync_send(
                m_client,
                topic.c_str(),
                message.size(),
                message.data(),
                1,
                static_cast<int>(retained),
                &opts))
        {
            /* Fail */
            Logger::Warn() << "[MQTTClient.PublishMessage]: Couldn't publish message to " + topic;
            Logger::Warn() << "[MQTTClient.PublishMessage]: ~ Code: " + std::to_string(r);
            return false;
        }

        if (!pubSucc && !pubFail)
            /* No need to set callbacks */
            return true;

        /*
            note(ben): This is a race condition for the same
            reason as described in SubscribeToTopic
        */

        m_inflightMutex.lock();
        auto [it, inserted] = m_inflight.insert({ opts.token, Inflight{
            .handlerSucc = pubSucc,
            .handlerFail = pubFail,
            .context1    = context1,
            .context2    = context2
        } });
        m_inflightMutex.unlock();

        if (!inserted)
        {
            Logger::Error() <<
            "[MQTTClient.PublishMessage]: Couldn't insert "
            "inflight message for some reason";

            /*
                note(ben): We return false, though we _did_ publish the
                message, which is strange.
            */
            return false;
        }

        return true;
    }

    /*
        Callbacks
    */

    void MQTTClient::Callback_Connected(MQTTAsync_successData5*)
    {
        auto swap = ConnectionStatus::Connecting;
        if (m_connectionStatus.compare_exchange_strong(
                swap,
                ConnectionStatus::Connected,
                std::memory_order_release,
                std::memory_order_relaxed))
            m_connectionStatus.notify_all();
    }

    void MQTTClient::Callback_ConnectionFailed(MQTTAsync_failureData5*)
    {
        auto swap = ConnectionStatus::Connecting;
        if (m_connectionStatus.compare_exchange_strong(
                swap,
                ConnectionStatus::ConnectionFailed,
                std::memory_order_release,
                std::memory_order_relaxed))
            m_connectionStatus.notify_all();
    }

    void MQTTClient::Callback_ConnectionLost(char*)
    {
        auto swap = ConnectionStatus::Connected;
        if (m_connectionStatus.compare_exchange_strong(
                swap,
                ConnectionStatus::ConnectionFailed,
                std::memory_order_release,
                std::memory_order_relaxed))
            m_connectionStatus.notify_all();
    }

    void MQTTClient::Callback_Subscribed(MQTTAsync_successData5* s)
    {
        std::lock_guard lk{ m_inflightMutex };

        if (auto it = m_inflight.find(s->token); it != m_inflight.end())
        {
            if (Inflight& inflight = it->second; inflight.handlerSucc)
                inflight.handlerSucc(inflight.context1, inflight.context2);
            m_inflight.erase(it);
        }
    }

    void MQTTClient::Callback_SubscriptionFailed(MQTTAsync_failureData5* s)
    {
        std::lock_guard lk{ m_inflightMutex };

        if (auto it = m_inflight.find(s->token); it != m_inflight.end())
        {
            if (Inflight& inflight = it->second; inflight.handlerFail)
                inflight.handlerFail(inflight.context1, inflight.context2);
            m_inflight.erase(it);
        }
    }

    void MQTTClient::Callback_Published(MQTTAsync_successData5* s)
    {
        std::lock_guard lk{ m_inflightMutex };

        if (auto it = m_inflight.find(s->token); it != m_inflight.end())
        {
            if (Inflight& inflight = it->second; inflight.handlerSucc)
                inflight.handlerSucc(inflight.context1, inflight.context2);
            m_inflight.erase(it);
        }
    }

    void MQTTClient::Callback_PublishFailed(MQTTAsync_failureData5* s)
    {
        std::lock_guard lk{ m_inflightMutex };

        if (auto it = m_inflight.find(s->token); it != m_inflight.end())
        {
            if (Inflight& inflight = it->second; inflight.handlerFail)
                inflight.handlerFail(inflight.context1, inflight.context2);
            m_inflight.erase(it);
        }
    }

    int MQTTClient::Callback_MessageReceived(char *topicName,
                                             int topicLen,
                                             MQTTAsync_message *message)
    {
        std::unique_lock lk{ m_subscriptionsMutex };

        for (size_t i = 0; i < message->properties.count; ++i)
        {
            auto& property = message->properties.array[i];

            if (property.identifier
                == MQTTPROPERTY_CODE_SUBSCRIPTION_IDENTIFIER)
            {
                if (auto it = m_subscriptions.find(property.value.integer4);
                    it != m_subscriptions.end() && it->second.handler)
                {
                    it->second.handler(
                        it->second.context1,
                        it->second.context2,
                        std::string_view{ topicName, static_cast<size_t>(topicLen) },
                        std::span<uint8_t>{
                            reinterpret_cast<uint8_t*>(message->payload),
                            static_cast<size_t>(message->payloadlen) });
                }
            }
        }

        lk.unlock();
        MQTTAsync_freeMessage(&message);
        MQTTAsync_free(topicName);
        return 1;
    }
}