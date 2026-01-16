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
            Uninitialized, Ready, Subscribing, Subscribed, Receiving, Received, Error
        };

    private:
        struct Inbox
        {
            static constexpr size_t MaxTopicLen{ 128 };

            Inbox(char* messageBuffer, size_t messageCapacity)
                : message(messageBuffer), capacity(messageCapacity) {}

            SubStatus status{ SubStatus::Uninitialized };

            char topic[MaxTopicLen + 1]{};
            size_t topicLen;

            // Buffer
            size_t capacity;
            char* message;
            size_t messageLen{ 0 };
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
                &Callback_IncomingPubl,
                &Callback_IncomingData,
                nullptr);

            static constexpr std::string_view LatestImageIDTopicL{ "devices/" };
            static constexpr std::string_view LatestImageIDTopicR{ "/in/latest-image-id" };

            static constexpr std::string_view LatestImageTopicL{ "devices/" };
            static constexpr std::string_view LatestImageTopicR{ "/in/latest-image" };

            /*
                Check string lengths
            */

            {
                const int latestImageIDTopicLen = LatestImageIDTopicL.size() + username.size() + LatestImageIDTopicR.size();
                const int latestImageTopicLen   = LatestImageTopicL.size()   + username.size() + LatestImageTopicR.size();

                if (latestImageIDTopicLen > Inbox::MaxTopicLen)
                {
                    Log_Error("MQTTSub: Latest image ID topic length exceeded maximum allotted: "
                            "(%d chars long, %d max)", latestImageIDTopicLen, Inbox::MaxTopicLen);
                    return false;
                }

                if (latestImageTopicLen > Inbox::MaxTopicLen)
                {
                    Log_Error("MQTTSub: Latest image ID topic length exceeded maximum allotted: "
                            "(%d chars long, %d max)", latestImageTopicLen, Inbox::MaxTopicLen);
                    return false;
                }
            }

            /*
                Init inbox topic names
            */

            {
                size_t n = 0;
                std::memcpy(&s_latestImageID.topic[n], LatestImageIDTopicL.data(), LatestImageIDTopicL.size()); n += LatestImageIDTopicL.size();
                std::memcpy(&s_latestImageID.topic[n], username.data(),            username.size());            n += username.size();
                std::memcpy(&s_latestImageID.topic[n], LatestImageIDTopicR.data(), LatestImageIDTopicR.size()); n += LatestImageIDTopicR.size();
                s_latestImageID.topic[n] = '\0';
                s_latestImageID.topicLen = n;
            }

            {
                size_t n = 0;
                std::memcpy(&s_latestImage.topic[n], LatestImageTopicL.data(), LatestImageTopicL.size()); n += LatestImageTopicL.size();
                std::memcpy(&s_latestImage.topic[n], username.data(),          username.size());          n += username.size();
                std::memcpy(&s_latestImage.topic[n], LatestImageTopicR.data(), LatestImageTopicR.size()); n += LatestImageTopicR.size();
                s_latestImage.topic[n] = '\0';
                s_latestImage.topicLen = n;
            }

            s_status = Status::Ready;

            s_latestImageID.status = SubStatus::Ready;
            s_latestImage.status   = SubStatus::Ready;

            return true;
        }

        static bool GetLatestImageIDSync(int secondsTimeout,
                                         const char** outMessage,
                                         unsigned int* outMessageLen)
        {
            return SubscribeAndWaitForInbox(
                &s_latestImageID,
                secondsTimeout,
                outMessage,
                outMessageLen);
        }

        static bool GetLatestImageSync(int secondsTimeout,
                                       const char** outMessage,
                                       unsigned int* outMessageLen)
        {
            return SubscribeAndWaitForInbox(
                &s_latestImage,
                secondsTimeout,
                outMessage,
                outMessageLen);
        }

        static bool SubscribeAndWaitForInbox(Inbox* inbox,
                                             int secondsTimeout,
                                             const char** outMessage,
                                             unsigned int* outMessageLen)
        {
            if (!SubscribeToInboxSync(inbox) ||
                !WaitForInbox(inbox, secondsTimeout))
                return false;

            *outMessage    = inbox->message;
            *outMessageLen = inbox->messageLen;
            return true;
        }

        static bool SubscribeToInbox(Inbox* inbox)
        {
            static const constexpr int QOS{ 1 };

            cyw43_arch_lwip_begin();
            if (inbox->status != SubStatus::Ready &&
                inbox->status != SubStatus::Error)
            {
                Log_Error("MQTTSub: Tried to subscribe to the latest image ID "
                          "but not ready or already subscribed\n");
                cyw43_arch_lwip_end();
                return false;
            }

            if (auto r = mqtt_subscribe(
                MQTTConn::ConnectionState()->client,
                inbox->topic,
                QOS,
                Callback_Subscribed,
                inbox))
            {
                Log_Error("MQTTSub: Trying to subscribe failed immediately. "
                          "Error code: %d\n", r);
                cyw43_arch_lwip_end();
                return false;
            }

            inbox->status = SubStatus::Subscribing;
            cyw43_arch_lwip_end();
            return true;
        }

        static bool SubscribeToInboxSync(Inbox* inbox)
        {
            if (!SubscribeToInbox(inbox))
                return false;

            while (inbox->status == SubStatus::Subscribing)
            {
                Wifi::Poll();
                sleep_ms(90);
            }

            return inbox->status == SubStatus::Subscribed ||
                   inbox->status == SubStatus::Receiving ||
                   inbox->status == SubStatus::Received;
        }

        static bool WaitForInbox(Inbox* inbox,
                                 int secondsTimeout)
        {
            if (inbox->status == SubStatus::Received) return true;
            if (inbox->status == SubStatus::Error)    return false;

            for (int i = 0; i < secondsTimeout; ++i)
            for (int j = 0; j < 10; ++j)
            {
                Wifi::Poll();
                sleep_ms(100);

                if (inbox->status == SubStatus::Received ||
                    inbox->status == SubStatus::Error)
                    break;
            }

            if (inbox->status != SubStatus::Received &&
                inbox->status != SubStatus::Error)
            {
                Log_Error("MQTTSub: Timed out while waiting for topic: %s\n",
                          inbox->topic);
                return false;
            }

            return inbox->status != SubStatus::Error;
        }

    private:
        static void Callback_Subscribed(void* context, err_t err)
        {
            auto inbox = reinterpret_cast<Inbox*>(context);

            if (err)
            {
                inbox->status = SubStatus::Error;
                Log_Error("MQTTSub: Subscribe failed: %d\n", err);
            }
            else
                inbox->status = SubStatus::Subscribed;
        }

        static void Callback_IncomingPubl(void*, const char* topic, uint32_t totLen)
        {
            if (!GetInboxFromTopicString(topic, &s_incoming))
            {
                Log_Warn("MQTTSub: Incoming topic was not recognized: %s\n", topic);
                return;
            }

            if (s_incoming->status != SubStatus::Subscribed)
                return;
            
            Log_Debug("MQTTSub: Incoming publish to topic %s "
                      "(total length: %d)\n", topic, totLen);

            if (s_incoming->status == SubStatus::Receiving)
            {
                Log_Error("MQTTSub: Incoming publish callback called before "
                          "the last message was done being read\n");
                return;
            }

            if (totLen > s_incoming->capacity)
            {
                Log_Error("MQTTSub: Incoming publish length was larger than capacity "
                          "(topic: %s, length: %u, capacity: %zu)\n",
                          topic, (unsigned)totLen, s_incoming->capacity);
                return;
            }

            s_incoming->status = SubStatus::Receiving;
        }

        static void Callback_IncomingData(void*, const uint8_t* incData, uint16_t len, uint8_t flags)
        {
            if (s_incoming == nullptr || 
                s_incoming->status != SubStatus::Receiving)
            {
                Log_Error("MQTTSub: Got incoming data but s_incoming was null or not receiving.\n");
                return;   
            }

            if (s_incoming->messageLen + len > s_incoming->capacity)
            {
                Log_Error("MQTTSub: Number of bytes to write exceeded capacity of the "
                          "allocated buffer.\n");

                s_incoming->status = SubStatus::Error;
                return;
            }

            std::memcpy(s_incoming->message + s_incoming->messageLen, incData, len);
            s_incoming->messageLen += len;

            if (flags & MQTT_DATA_FLAG_LAST)
            {
                Log_Debug("MQTTSub: Fully received incoming publish "
                          "(length: %d)\n", s_incoming->messageLen);
                s_incoming->status = SubStatus::Received;
            }
        }

        static bool GetInboxFromTopicString(const char* topic, Inbox** outInbox)
        {
            if (std::strcmp(topic, s_latestImageID.topic) == 0)
            {
                *outInbox = &s_latestImageID;
                return true;
            }

            if (std::strcmp(topic, s_latestImage.topic) == 0)
            {
                *outInbox = &s_latestImage;
                return true;
            }

            return false;
        }

        static Status s_status;

        static Inbox* s_incoming;
        static Inbox s_latestImageID;
        static Inbox s_latestImage;

        static char s_latestImageIDBuffer[128];
        static char s_latestImageBuffer[1024*10];
    };
}