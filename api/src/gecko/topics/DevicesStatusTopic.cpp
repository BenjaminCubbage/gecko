#include "gecko/topics/DevicesStatusTopic.h"

namespace Gecko::API::Topics
{
    DevicesStatusTopic::Status
    DevicesStatusTopic::GetDeviceStatus(const std::string& deviceID)
    {
        std::shared_lock lk{ m_trackedDevicesMutex };
        const auto it = m_trackedDevices.find(deviceID);

        return it != m_trackedDevices.end()
            ? it->second
            : Status::NotTracked;
    }

    void DevicesStatusTopic::BeginTrackingDevice(const std::string& deviceID)
    {
        {
            std::unique_lock lk{ m_trackedDevicesMutex };

            if (m_trackedDevices.contains(deviceID))
                return;

            m_trackedDevices.insert_or_assign(deviceID, Status::MaybeLater);
        }

        const std::string topic = "devices/" + deviceID + "/status";

        m_mqttClient->SubscribeToTopic(topic, [topic] {
            std::cout << "Subscribed to topic " << topic << std::endl;
        }, [topic = std::move(topic)] (int err) {
            std::cout << "Failed to subscribe to topic " << topic << ": " << err << std::endl;
        });
    }

    void DevicesStatusTopic::MessageReceivedHandler(std::string_view topic,
                                                    std::span<uint8_t> payload)
    {
        constexpr auto prefix = std::string_view("devices/");
        constexpr auto suffix = std::string_view("/out/status");

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
}