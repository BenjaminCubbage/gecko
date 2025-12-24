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
    class DevicesHeartbeatTopic
    {
    public:
        // Offline: No heartbeat in grace period
        // Online:  Heartbeat in grace period
        // Pending: No heartbeat yet, but system started up recently
        enum class Status
        {
            Offline,
            Online,
            Pending
        };

        DevicesHeartbeatTopic(MQTT::MQTTClient* mqttClient)
            : m_mqttClient(std::move(mqttClient)) {}

        void Start();

        Status GetDeviceStatus(const std::string& deviceID);

    private:
        struct SVTransparentHash
        {
            using is_transparent = void;
            size_t operator()(std::string_view sv)    const { return std::hash<std::string_view>{}(sv);  }
            size_t operator()(const std::string& str) const { return std::hash<std::string_view>{}(str); }
        };

        void MessageReceivedHandler(std::string_view topic, std::span<uint8_t> payload);

        static inline std::chrono::seconds EpochNow()
        {
            return std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch());
        }

        MQTT::MQTTClient* m_mqttClient;
        std::shared_mutex m_epochDevicesLastSeenMutex;
        std::unordered_map<
            std::string,
            std::chrono::seconds,
            SVTransparentHash,
            std::equal_to<>> m_epochDevicesLastSeen;
        std::chrono::seconds m_epochStartup;

        static constexpr std::chrono::minutes GracePeriod{ 25 };
    };
}