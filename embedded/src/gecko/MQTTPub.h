#pragma once
#include "gecko/Log.h"
#include "gecko/MQTTConn.h"

namespace Gecko::Embedded
{
    class MQTTPub
    {
    public:
        enum class Result { Success, NotConnected };
        enum class Status { Online, Offline };

        static Result PublishStatus(Status status, void (*cb)(err_t))
        {
            static constexpr int StatusQOS    = 1;
            static constexpr int StatusRetain = 1;
            static constexpr std::string_view OnlMessage{ "on" };
            static constexpr std::string_view OffMessage{ "off" };

            const MQTTConn::Connection *conn = MQTTConn::ConnectionState();

            cyw43_arch_lwip_begin();
            if (!conn->connected)
                return Result::NotConnected;

            const std::string_view msg = 
                status == Status::Online
                    ? OnlMessage
                    : OffMessage;

            mqtt_publish(
                conn->client,
                MQTTConn::StatusTopic(),
                msg.data(),
                msg.size(),
                StatusQOS,
                StatusRetain,
                PublishCallback,
                reinterpret_cast<void*>(cb));
            cyw43_arch_lwip_end();

            return Result::Success;
        }

    private:
        static void PublishCallback(void* state, err_t err) {
            if (err)
                Log_Error("MQTTPub: Publish failed: %d\n", err);
            else
                Log_Debug("MQTTPub: Published a message successfully\n");

            reinterpret_cast<void (*)(err_t)>(state)(err);
        }
    };
}