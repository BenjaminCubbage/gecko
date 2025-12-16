#pragma once
#include <functional>
#include <memory>
#include <string_view>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <span>
#include "gecko/mqtt/MQTTClient.h"
#include "gecko/rand/UUID.h"

namespace Gecko::API::Topics
{
    class DevicesStatusTopic
    {
    public:
        // Offline:    device reported it was offline
        // Online:     device reported it was online
        // MaybeLater: no retained message has been received (device may not even exist, who knows)
        // NotTracked: device isn't currently subscribed to (call TrackDevice to track)
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
            const auto it = m_trackedDevices.find(deviceID);

            return it != m_trackedDevices.end()
                ? it->second
                : Status::NotTracked;
        }

        void BeginTrackingDevice(const std::string& deviceID)
        {
            if (m_trackedDevices.contains(deviceID))
                return; 

            const std::string topic = "devices/" + deviceID + "/status";

            m_mqttClient->SubscribeToTopic(
                topic,
                [topic] {
                    std::cout << "Subscribed to topic: " << topic << std::endl;
                },
                [topic = std::move(topic)] (int) {
                    std::cout << "Failed to subscribe to topic: " << topic << std::endl;
                });

            m_trackedDevices.insert_or_assign(deviceID, Status::MaybeLater);
        }


    private:
        void MessageReceivedHandler(std::string_view topic, std::span<uint8_t> payload)
        {
            constexpr auto prefix = std::string_view("devices/");
            constexpr auto suffix = std::string_view("/status");

            constexpr int maxTopicLen = prefix.size() + Rand::UUID::UUIDLength + suffix.size();
            constexpr int minTopicLen = prefix.size() + 1                      + suffix.size();

            if (topic.size() < minTopicLen ||
                topic.size() > maxTopicLen ||
                !topic.starts_with(prefix) ||
                !topic.ends_with(suffix))
            {
                return;
            }

            std::string_view deviceID = topic.substr(prefix.size(), topic.size() - prefix.size() - suffix.size());

            if (payload.size() >= 2 &&
                payload[0] == 'o'   && 
                payload[1] == 'n')
                m_trackedDevices.insert_or_assign(std::string(deviceID), Status::Online);
            else
                m_trackedDevices.insert_or_assign(std::string(deviceID), Status::Offline);
        }

        std::shared_ptr<MQTT::MQTTClient>       m_mqttClient;
        std::unordered_map<std::string, Status> m_trackedDevices;
    };
}