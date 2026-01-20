#pragma once
#include "gecko/Log.h"
#include "gecko/Wifi.h"
#include "gecko/MQTTConn.h"
#include "lwip/apps/mqtt.h"

namespace Gecko::Embedded
{
    class MQTTPub
    {
      public:
        enum PubStatus
        {
            // Uninitialized -> Ready -> Publishing -> (Published | Error) -> Publishing
            Uninitialized, Ready, Publishing, Published, Error
        };

        static bool Init(const std::string_view& username)
        {
            static constexpr std::string_view HeartbeatTopicL{ "devices/" };
            static constexpr std::string_view HeartbeatTopicR{ "/out/heartbeat" };

            // note(ben): Topic is like devices/<username>/out/heartbeat
            const int topicLen = HeartbeatTopicL.size() + username.size() + HeartbeatTopicR.size();
            if (topicLen + 1 > sizeof(s_heartbeatTopic))
            {
                Log_Error("MQTTPub: Devices heartbeat topic exceeded max length: "
                          "(%d Bytes, %d Allocated)", topicLen + 1, sizeof(s_heartbeatTopic));
                return false;
            }

            size_t n = 0;
            std::memcpy(&s_heartbeatTopic[n], HeartbeatTopicL.data(), HeartbeatTopicL.size()); n += HeartbeatTopicL.size();
            std::memcpy(&s_heartbeatTopic[n], username.data(),        username.size());        n += username.size();
            std::memcpy(&s_heartbeatTopic[n], HeartbeatTopicR.data(), HeartbeatTopicR.size()); n += HeartbeatTopicR.size();
            s_heartbeatTopic[n] = '\0';

            s_heartbeatPubStatus = PubStatus::Ready;
            return true;
        }

        static bool PublishHeartbeat()
        {
            static constexpr int HeartbeatQOS    = 1;
            static constexpr int HeartbeatRetain = 0;
            const MQTTConn::Connection *conn = MQTTConn::ConnectionState();

            cyw43_arch_lwip_begin();
            if (MQTTConn::ConnectionStatus() != MQTTConn::Status::Connected)
            {
                Log_Error("MQTTPub: Tried to publish heartbeat but not connected\n");
                return false;
            }

            if (s_heartbeatPubStatus != PubStatus::Ready &&
                s_heartbeatPubStatus != PubStatus::Published &&
                s_heartbeatPubStatus != PubStatus::Error)
            {
                Log_Error("MQTTPub: Tried to publish heartbeat but not ready yet\n");
                return false;
            }

            Log_Debug("Publishing to heartbeat topic: %s\n", s_heartbeatTopic);

            mqtt_publish(
                conn->client,
                s_heartbeatTopic,
                ":)", 2,
                HeartbeatQOS,
                HeartbeatRetain,
                &PublishCallback,
                &s_heartbeatPubStatus);
            s_heartbeatPubStatus = PubStatus::Publishing;

            cyw43_arch_lwip_end();

            return true;
        }

        static bool PublishHeartbeatSync()
        {
            if (!PublishHeartbeat())
                return false;

            while (s_heartbeatPubStatus == PubStatus::Publishing)
            {
                Wifi::Poll();
                sleep_ms(90);
            }

            return s_heartbeatPubStatus == PubStatus::Published;
        }

      private:
        static void PublishCallback(void* state, err_t err)
        {
            PubStatus* status = reinterpret_cast<PubStatus*>(state);

            if (err)
            {
                *status = PubStatus::Error;
                Log_Error("MQTTPub: Publish failed: %d\n", err);
            }
            else
            {
                *status = PubStatus::Published;
                Log_Debug("MQTTPub: Published a message successfully\n");
            }
        }

        // note(ben): 100 should always be plenty
        static char s_heartbeatTopic[100];
        static PubStatus s_heartbeatPubStatus;
    };
}