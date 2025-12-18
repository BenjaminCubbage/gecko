#pragma once
#include <memory>
#include <shared_mutex>
#include <span>
#include <string_view>
#include <string>
#include <unordered_map>
#include "gecko/mqtt/MQTTClient.h"

namespace Gecko::API::Topics
{
    class DevicesStatusTopic
    {
    public:
        // Offline:    device reported it was offline
        // Online:     device reported it was online
        // MaybeLater: no retained message has been received (device may not even exist, who knows)
        // NotTracked: device isn't currently subscribed to (call BeginTrackingDevice to track)
        enum class Status
        {
            Offline,
            Online,
            MaybeLater,
            NotTracked
        };

        DevicesStatusTopic(std::shared_ptr<MQTT::MQTTClient> mqttClient)
            : m_mqttClient(std::move(mqttClient))
        {
            m_mqttClient->OnMessageReceived(
                [this] (std::string_view topic, std::span<uint8_t> payload) {
                    MessageReceivedHandler(topic, payload);
                });
        }

        Status GetDeviceStatus(const std::string& deviceID);

        void BeginTrackingDevice(const std::string& deviceID);

    private:
        struct SVTransparentHash
        {
            using is_transparent = void;
            size_t operator()(std::string_view sv)    const { return std::hash<std::string_view>{}(sv);  }
            size_t operator()(const std::string& str) const { return std::hash<std::string_view>{}(str); }
        };

        void MessageReceivedHandler(std::string_view topic, std::span<uint8_t> payload);

        std::shared_ptr<MQTT::MQTTClient> m_mqttClient;
        std::shared_mutex                 m_trackedDevicesMutex;
        std::unordered_map<
            std::string,
            Status,
            SVTransparentHash,
            std::equal_to<>> m_trackedDevices;
    };
}