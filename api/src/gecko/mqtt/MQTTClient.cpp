#include "gecko/mqtt/MQTTClient.h"

namespace Gecko::API::MQTT
{
    thread_local MQTTAsync_failureData MQTTClient::s_genericFailure{ 0, -1, NULL };

    void MQTTClient::Connect()
    {
        const auto connect = [](void* context, MQTTAsync_successData* response) {
            reinterpret_cast<MQTTClient *>(context)->HandleConnect(response);
        };

        const auto connectFailure = [](void* context, MQTTAsync_failureData* response) {
            reinterpret_cast<MQTTClient *>(context)->HandleConnectFailure(response);
        };

        const auto connectionLost = [](void* context, char* cause) {
            reinterpret_cast<MQTTClient *>(context)->HandleConnectionLost(cause);
        };

        const auto messageReceived = [](void *context, char *topicName, int topicLen, MQTTAsync_message *message) {
            return reinterpret_cast<MQTTClient *>(context)->HandleMessageReceived(topicName, topicLen, message);
        };

        MQTTAsync_createOptions createOpts = MQTTAsync_createOptions_initializer5;

        MQTTAsync_createWithOptions(
            &m_client,
            m_address.c_str(),
            "",
            MQTTCLIENT_PERSISTENCE_NONE,
            NULL,
            &createOpts);

        MQTTAsync_setCallbacks(
            m_client,
            this,
            connectionLost,
            messageReceived,
            NULL
        );

        MQTTAsync_connectOptions connOpts = MQTTAsync_connectOptions_initializer;

        connOpts.keepAliveInterval = 240;
        connOpts.cleansession      = 1;
        connOpts.onSuccess         = connect;
        connOpts.onFailure         = connectFailure;
        connOpts.context           = this;
        connOpts.username          = m_username.c_str();
        connOpts.password          = m_password.c_str();

        if (MQTTAsync_connect(m_client, &connOpts) != MQTTASYNC_SUCCESS)
            HandleConnectFailure(&s_genericFailure);
    }

    bool MQTTClient::ConnectSync()
    {
        Connect();

        std::unique_lock lk{ m_connectedOrFailedMutex };
        m_connectedOrFailedSignal.wait(lk, [this]() {
            return m_connected || m_connectFailed;
        });

        return m_connected;
    }

    void MQTTClient::SubscribeToTopic(const std::string& topic,
                                      std::function<void()> onSuccess,
                                      std::function<void(int)> onFailure)
    {
        // note(ben): new / delete semantics ended up being simplest here. I'm not
        // super happy about it but it's difficult to effectively manage lifetimes
        // w/ RAII beyond the C++ -> C boundary

        // The concern would be that it might be possible in same cases for
        // MQTTAsync_subscribe to return a success value, but for neither subscribe
        // nor subscribeFailure to be called. This would be a memory leak, but it's
        // not entirely clear if it's _possible_ for this to happen in the first
        // place.

        const auto subscribe = [](void* context, MQTTAsync_successData* response) {
            SubscribeContext *subscribeContext = reinterpret_cast<SubscribeContext *>(context);
            subscribeContext->onSuccess();
            delete subscribeContext;
        };

        const auto subscribeFailure = [](void* context, MQTTAsync_failureData* response) {
            SubscribeContext *subscribeContext = reinterpret_cast<SubscribeContext *>(context);
            subscribeContext->onFailure(response->code);
            delete subscribeContext;
        };

        MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
        opts.onSuccess = subscribe;
        opts.onFailure = subscribeFailure;
        opts.context   = new SubscribeContext{ onSuccess, onFailure };

        if (MQTTAsync_subscribe(m_client, topic.c_str(), 1, &opts) != MQTTASYNC_SUCCESS)
        {
            delete reinterpret_cast<SubscribeContext *>(opts.context);
            onFailure(s_genericFailure.code);
        }
    }

    void MQTTClient::OnMessageReceived(std::function<void(std::string_view, std::span<uint8_t>)> handler)
    {
        std::unique_lock lk{ m_messageReceivedHandlersMutex };
        m_messageReceivedHandlers.push_back(handler);
    }

    void MQTTClient::HandleConnect(MQTTAsync_successData* response)
    {
        {
            std::unique_lock lk{ m_connectedOrFailedMutex };
            m_connected = true;
        }

        m_connectedOrFailedSignal.notify_all();
    }

    void MQTTClient::HandleConnectFailure(MQTTAsync_failureData* response)
    {
        {
            std::unique_lock lk{ m_connectedOrFailedMutex };
            m_connectFailed = true;
        }

        m_connectedOrFailedSignal.notify_all();
    }

    void MQTTClient::HandleConnectionLost(char* cause)
    {
        // In response, I frown.


        // :(
    }

    int MQTTClient::HandleMessageReceived(char *topicName, int topicLen, MQTTAsync_message *message)
    {
        std::string_view topic{ topicName, static_cast<size_t>(topicLen) };
        std::span payload{ reinterpret_cast<uint8_t*>(message->payload),
                           static_cast<size_t>(message->payloadlen) };

        {
            std::unique_lock lk{ m_messageReceivedHandlersMutex };
            for (const auto& handler : m_messageReceivedHandlers)
                handler(topic, payload);
        }

        MQTTAsync_freeMessage(&message);
        MQTTAsync_free(topicName);
        return 1;
    }
}