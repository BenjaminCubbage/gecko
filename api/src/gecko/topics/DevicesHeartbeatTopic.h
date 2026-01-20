#pragma once
#include <chrono>
#include <memory>
#include <shared_mutex>
#include <span>
#include <string_view>
#include <string>
#include <unordered_map>
#include "gecko/mqtt/MQTTClient.h"

namespace Gecko::API::Topics
{
    /*
        intent(DevicesHeartbeatTopic): To subscribe to the
        devices/+/out/heartbeat topic and store which devices
        were recently online in a std::unordered_map.

        This can be used to e.g. display to users which of
        another user's devices are currently connected to the
        internet and ready to receive a shared image.
    */
    class DevicesHeartbeatTopic
    {
    public:
        using Clock = std::chrono::system_clock;

        /*
            If a device does not send a message within this amount
            of time, it is considered offline.
        */
        static constexpr std::chrono::minutes GracePeriod{ 25 };

        /*
            Pending: No heartbeat yet, but system started up recently
            Offline: No heartbeat within grace period
            Online:  Received a heartbeat within the grace period
        */
        enum class DeviceStatus
        {
            Pending,
            Offline,
            Online,
        };

        DevicesHeartbeatTopic(MQTT::MQTTClient* mqttClient)
            : m_mqttClient(mqttClient) {}

        /*
            Initialize and start listening for heartbeats.
        */
        void Start();

        /*
            Get the status of the associated device. Use this only
            after calling Start().
        */
        DeviceStatus GetDeviceStatus(const std::string& deviceID);

    private:
        struct SVTransparentHash
        {
            using is_transparent = void;
            size_t operator()(std::string_view sv)   const { return std::hash<std::string_view>{}(sv); }
            size_t operator()(const std::string& sv) const { return std::hash<std::string_view>{}(sv); }
        };

        void Callback_MessageReceived(void* context,
                                      std::string_view topic,
                                      std::span<uint8_t> payload);

        MQTT::MQTTClient* m_mqttClient;
        std::shared_mutex m_devicesLastSeenMutex;
        std::unordered_map<
            std::string,
            std::chrono::time_point<Clock>,
            SVTransparentHash,
            std::equal_to<>> m_devicesLastSeen;
        std::chrono::time_point<Clock> m_startupTime;
    };
}