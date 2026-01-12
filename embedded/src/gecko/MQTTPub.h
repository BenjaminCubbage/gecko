#pragma once
#include "gecko/Log.h"
#include "gecko/MQTTConn.h"

namespace Gecko::Embedded
{
    class MQTTPub
    {
    public:
        enum class Result { OK, NotConnected };
        enum class Status { Online, Offline };

        static bool Init(const std::string& username)
        {
            static constexpr std::string_view HeartbeatTopicL{ "devices/" };
            static constexpr std::string_view HeartbeatTopicR{ "/out/heartbeat" };

            // note(ben): Topic is like devices/<username>/out/heartbeat
            int topicLen = HeartbeatTopicL.size() + username.size() + HeartbeatTopicR.size();
            if (topicLen + 1 > sizeof(s_heartbeatTopic))
            {
                Log_Error("MQTTConn: Devices heartbeat topic exceeded max length: "
                          "(%d Bytes, %d Allocated)", topicLen + 1, sizeof(s_heartbeatTopic));
                return false;
            }

            size_t n = 0;
            std::memcpy(&s_heartbeatTopic[n], HeartbeatTopicL.data(), HeartbeatTopicL.size()); n += HeartbeatTopicL.size();
            std::memcpy(&s_heartbeatTopic[n], username.c_str(),       username.size());        n += username.size();
            std::memcpy(&s_heartbeatTopic[n], HeartbeatTopicR.data(), HeartbeatTopicR.size()); n += HeartbeatTopicR.size();
            s_heartbeatTopic[n] = '\0';
            return true;
        }

        static Result PublishHeartbeat(void (*cb)(err_t))
        {
            static constexpr int HeartbeatQOS    = 1;
            static constexpr int HeartbeatRetain = 0;
            const MQTTConn::Connection *conn = MQTTConn::ConnectionState();

            cyw43_arch_lwip_begin();
            if (!conn->connected)
                return Result::NotConnected;

            Log_Debug("Publishing to heartbeat topic: %s\n", s_heartbeatTopic);
            mqtt_publish(
                conn->client,
                s_heartbeatTopic,
                ":)", 2,
                HeartbeatQOS,
                HeartbeatRetain,
                PublishCallback,
                reinterpret_cast<void*>(cb));
            cyw43_arch_lwip_end();

            return Result::OK;
        }

    private:
        static void PublishCallback(void* state, err_t err) {
            if (err)
                Log_Error("MQTTPub: Publish failed: %d\n", err);
            else
                Log_Debug("MQTTPub: Published a message successfully\n");

            if (state)
                reinterpret_cast<void (*)(err_t)>(state)(err);
        }

        // note(ben): 59 should always be plenty
        // Number is upper bound, not _entirely_ magic:
        //      59 = strlen("devices/") + 36 + strlen("/out/heartbeat\0")
        //      where 36 is the length of an RFC9562 UUID
        static char s_heartbeatTopic[59];
    };
}