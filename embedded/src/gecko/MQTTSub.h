#pragma once
#include "gecko/Log.h"
#include "gecko/MQTTConn.h"
#include <atomic>
#include "lwip/apps/mqtt.h"

namespace Gecko::Embedded
{
    class MQTTSub
    {
    public:
        enum class Status
        {
            Uninitialized, Ready
        };

        enum class SubStatus
        {
            // Uninitialized -> Ready -> Subscribing -> (Subscribed | Error) -> Subscribing
            Uninitialized, Ready, Subscribing, Subscribed, Error
        };

    private:
        struct Inbox
        {
            Inbox(char* buffer, size_t capacity)
                : buffer(buffer), capacity(capacity) {}

            // Flags
            bool reading{ false };
            bool done{ false };
            bool error{ false };

            // Result
            size_t bytesReceived{ 0 };

            // Buffer
            size_t capacity;
            char* buffer;
        };

    public:
        static bool Init(const std::string_view& username)
        {
            if (s_status != Status::Uninitialized)
            {
                Log_Error("MQTTSub: Tried to initialize more than once\n");
                return false;
            }

            if (MQTTConn::ConnectionStatus() == MQTTConn::Status::Uninitialized)
            {
                Log_Error("MQTTSub: MQTTConn must be initialized first\n");
                return false;
            }

            mqtt_set_inpub_callback(
                MQTTConn::ConnectionState()->client,
                &IncomingPublCallback,
                &IncomingDataCallback,
                nullptr);

            static constexpr std::string_view LatestImageIDTopicL{ "devices/" };
            static constexpr std::string_view LatestImageIDTopicR{ "/in/latest-image-id" };

            static constexpr std::string_view LatestImageTopicL{ "devices/" };
            static constexpr std::string_view LatestImageTopicR{ "/in/latest-image" };

            const int latestImageIDTopicLen = LatestImageIDTopicL.size() + username.size() + LatestImageIDTopicR.size();
            const int latestImageTopicLen   = LatestImageTopicL.size()   + username.size() + LatestImageTopicR.size();

            if (latestImageIDTopicLen > sizeof(s_latestImageIDTopic))
            {
                Log_Error("MQTTSub: Latest image ID topic length exceeded maximum allotted: "
                          "(%d Bytes, %d Allocated)", latestImageIDTopicLen + 1, sizeof(s_latestImageIDTopic));
                return false;
            }

            if (latestImageTopicLen > sizeof(s_latestImageTopic))
            {
                Log_Error("MQTTSub: Latest image ID topic length exceeded maximum allotted: "
                          "(%d Bytes, %d Allocated)", latestImageTopicLen + 1, sizeof(s_latestImageTopic));
                return false;
            }

            {
                size_t n = 0;
                std::memcpy(&s_latestImageIDTopic[n], LatestImageIDTopicL.data(), LatestImageIDTopicL.size()); n += LatestImageIDTopicL.size();
                std::memcpy(&s_latestImageIDTopic[n], username.data(),            username.size());            n += username.size();
                std::memcpy(&s_latestImageIDTopic[n], LatestImageIDTopicR.data(), LatestImageIDTopicR.size()); n += LatestImageIDTopicR.size();
                s_latestImageIDTopic[n] = '\0';
            }

            {
                size_t n = 0;
                std::memcpy(&s_latestImageTopic[n], LatestImageTopicL.data(), LatestImageTopicL.size()); n += LatestImageTopicL.size();
                std::memcpy(&s_latestImageTopic[n], username.data(),          username.size());          n += username.size();
                std::memcpy(&s_latestImageTopic[n], LatestImageTopicR.data(), LatestImageTopicR.size()); n += LatestImageTopicR.size();
                s_latestImageTopic[n] = '\0';
            }

            s_status = Status::Ready;

            s_latestImageIDSubStatus = SubStatus::Ready;
            s_latestImageSubStatus   = SubStatus::Ready;

            return true;
        }

        static bool SubscribeToLatestImageID()
        {
            static const constexpr int QOS{ 1 };

            cyw43_arch_lwip_begin();
            if (MQTTConn::ConnectionStatus() != MQTTConn::Status::Connected ||
                s_status != Status::Ready)
            {
                Log_Error("MQTTSub: Tried to subscribe to the latest image ID "
                          "but either not connected or uninitialized\n");
                goto fail;
            }

            if (s_latestImageIDSubStatus != SubStatus::Ready &&
                s_latestImageIDSubStatus != SubStatus::Subscribed &&
                s_latestImageIDSubStatus != SubStatus::Error)
            {
                Log_Error("MQTTSub: Tried to subscribe to the latest image ID but not "
                          "ready yet\n");
                goto fail;
            }

            mqtt_subscribe(
                MQTTConn::ConnectionState()->client,
                s_latestImageIDTopic,
                QOS,
                SubscribeCallback,
                &s_latestImageIDSubStatus);
            s_latestImageIDSubStatus = SubStatus::Subscribing;

            cyw43_arch_lwip_end();
            return true;

        fail:
            cyw43_arch_lwip_end();
            return false;
        }

        static bool SubscribeToLatestImageIDSync()
        {
            if (!SubscribeToLatestImageID())
                return false;

            while (s_latestImageIDSubStatus == SubStatus::Subscribing)
            {
                Wifi::Poll();
                sleep_ms(90);
            }

            return s_latestImageIDSubStatus == SubStatus::Subscribed;
        }

        static bool WaitForLatestImageID(int secondsTimeout, char** outBuffer, unsigned int* outBytesReceived)
        {
            if (s_latestImageID.error)
                return false;

            if (s_latestImageID.done)
                goto success;

            for (int i = 0; i < secondsTimeout; ++i)
            for (int j = 0; j < 10; ++j)
            {
                Wifi::Poll();
                sleep_ms(100);

                if (s_latestImageID.done)
                    break;
            }

            if (!s_latestImageID.done)
            {
                Log_Error("MQTTSub: Timed out while waiting for latest image ID\n");
                return false;
            }

            if (s_latestImageID.error)
                return false;

        success:
            *outBuffer        = s_latestImageID.buffer;
            *outBytesReceived = s_latestImageID.bytesReceived;
            return true;
        }

    private:
        static void SubscribeCallback(void* state, err_t err)
        {
            SubStatus* status = reinterpret_cast<SubStatus*>(state);

            if (err)
            {
                *status = SubStatus::Error;
                Log_Error("MQTTSub: Subscribe failed: %d\n", err);
            }
            else
            {
                *status = SubStatus::Subscribed;
                Log_Debug("MQTTSub: Subscribed to a topic successfully\n");
            }
        }

        static void IncomingPublCallback(void* arg, const char* topic, uint32_t totLen)
        {
            if (s_incoming->reading)
            {
                Log_Error("MQTTSub: Incoming publish called before the last "
                          "message was done being read\n");
                return;
            }

            if (s_incoming->done)
            {
                Log_Warn("MQTTSub: Incoming publish callback called more "
                         "than once (ignoring)\n");
                return;
            }

            if (!GetInboxFromTopicString(topic, &s_incoming))
            {
                Log_Warn("MQTTSub: Incoming topic was not recognized: %s\n", topic);
                return;
            }

            if (totLen > s_incoming->capacity)
            {
                Log_Error("MQTTSub: Incoming publish length was larger than capacity "
                          "(topic: %s, len: %u, capacity: %zu)\n",
                          topic, (unsigned)totLen, s_incoming->capacity);
                return;
            }

            Log_Debug("MQTTSub: Incoming publish at topic %s with total "
                      "length %u\n", topic, (unsigned)totLen);

            s_incoming->reading = true;
        }

        static void IncomingDataCallback(void* arg, const uint8_t* incData, uint16_t len, uint8_t flags)
        {
            if (s_incoming->done)
            {
                if (flags | MQTT_DATA_FLAG_LAST)
                    s_incoming->reading = false;
                return;
            }

            if (!s_incoming->reading)
            {
                Log_Error("MQTTSub: Received data from MQTT broker but wasn't "
                          "expecting any\n");
                return;
            }

            if (len + s_incoming->bytesReceived > s_incoming->capacity)
            {
                Log_Error("MQTTSub: Number of bytes to write exceeded capacity of the "
                          "allocated buffer\n");

                s_incoming->error   = true;
                s_incoming->done    = true;
                return;
            }

            std::memcpy(s_incoming->buffer, incData, len);
            s_incoming->bytesReceived += len;

            if (flags | MQTT_DATA_FLAG_LAST)
            {
                Log_Debug("MQTTSub: Fully received incoming publish\n");
                s_incoming->done    = true;
                s_incoming->reading = false;
            }
        }

        static bool GetInboxFromTopicString(const char* topic, Inbox** outInbox)
        {
            if (std::strcmp(topic, s_latestImageIDTopic) == 0)
            {
                *outInbox = &s_latestImageID;
                return true;
            }

            if (std::strcmp(topic, s_latestImageTopic) == 0)
            {
                *outInbox = &s_latestImage;
                return true;
            }

            return false;
        }

        static Status s_status;

        static char s_latestImageIDTopic[100];
        static char s_latestImageTopic[100];

        static SubStatus s_latestImageIDSubStatus;
        static SubStatus s_latestImageSubStatus;

        static Inbox* s_incoming;

        static Inbox s_latestImageID;
        static Inbox s_latestImage;

        static char s_latestImageIDBuffer[128];
        static char s_latestImageBuffer[1024*10];
    };
}