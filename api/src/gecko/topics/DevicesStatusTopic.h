#pragma once
#include <functional>
#include <memory>
#include <shared_mutex>
#include <span>
#include <string_view>
#include <string>
#include <unordered_map>
#include <vector>
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

        Status GetDeviceStatus(const std::string& deviceID)
        {
            std::shared_lock lk{ m_trackedDevicesMutex };
            const auto it = m_trackedDevices.find(deviceID);

            return it != m_trackedDevices.end()
                ? it->second
                : Status::NotTracked;
        }

        void BeginTrackingDevice(const std::string& deviceID)
        {
            {
                std::unique_lock lk{ m_trackedDevicesMutex };

                if (m_trackedDevices.contains(deviceID))
                    return; 

                m_trackedDevices.insert_or_assign(deviceID, Status::MaybeLater);
            }

            const std::string topic = "devices/" + deviceID + "/status";

            m_mqttClient->SubscribeToTopic(topic, [] {}, [] (int) {});
        }


    private:
        struct SVTransparentHash
        {
            using is_transparent = void;
            size_t operator()(std::string_view sv)    const { return std::hash<std::string_view>{}(sv);  }
            size_t operator()(const std::string& str) const { return std::hash<std::string_view>{}(str); }
        };
        
        void MessageReceivedHandler(std::string_view topic, std::span<uint8_t> payload)
        {
            constexpr auto prefix = std::string_view("devices/");
            constexpr auto suffix = std::string_view("/status");

            constexpr int maxTopicLen = prefix.size() + 36 + suffix.size();
            constexpr int minTopicLen = prefix.size() + 1  + suffix.size();

            if (topic.size() < minTopicLen ||
                topic.size() > maxTopicLen ||
                !topic.starts_with(prefix) ||
                !topic.ends_with(suffix))
            {
                return;
            }

            std::string_view deviceID = topic.substr(prefix.size(), topic.size() - prefix.size() - suffix.size());

            std::unique_lock lk{ m_trackedDevicesMutex };

            if (payload.size() >= 2 &&
                payload[0] == 'o'   && 
                payload[1] == 'n')
                m_trackedDevices.insert_or_assign(std::string(deviceID), Status::Online);
            else
                m_trackedDevices.insert_or_assign(std::string(deviceID), Status::Offline);
        }

        std::shared_ptr<MQTT::MQTTClient> m_mqttClient;
        std::shared_mutex                 m_trackedDevicesMutex;
        std::unordered_map<
            std::string, 
            Status, 
            SVTransparentHash, 
            std::equal_to<>> m_trackedDevices;
    };
}