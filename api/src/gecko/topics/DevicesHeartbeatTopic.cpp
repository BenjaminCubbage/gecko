#include "gecko/topics/DevicesHeartbeatTopic.h"

namespace Gecko::API::Topics
{
    void DevicesHeartbeatTopic::Start()
    {
        static constexpr const char* HeartbeatTopic{ "devices/+/out/heartbeat" };

        const auto callback_messageReceived = [] (void *c1, void* c2, 
                                                  std::string_view sv, 
                                                  std::span<uint8_t> data) {
            reinterpret_cast<DevicesHeartbeatTopic*>(c1)->Callback_MessageReceived(c2, sv, data);
        };

        m_mqttClient->SubscribeToTopic(
            HeartbeatTopic,
            nullptr,
            nullptr,
            callback_messageReceived,
            this,
            nullptr);

        m_startupTime = Clock::now();
    }

    DevicesHeartbeatTopic::DeviceStatus
    DevicesHeartbeatTopic::GetDeviceStatus(const std::string& deviceID)
    {
        std::shared_lock lk{ m_devicesLastSeenMutex };
        const auto it = m_devicesLastSeen.find(deviceID);

        if (it == m_devicesLastSeen.end())
            return Clock::now() - m_startupTime <= GracePeriod
                ? DeviceStatus::Pending
                : DeviceStatus::Offline;

        return Clock::now() - it->second <= GracePeriod
            ? DeviceStatus::Online
            : DeviceStatus::Offline;
    }

    void DevicesHeartbeatTopic::Callback_MessageReceived(void* context,
                                                         std::string_view topic,
                                                         std::span<uint8_t> payload)
    {
        constexpr auto prefix = std::string_view("devices/");
        constexpr auto suffix = std::string_view("/out/heartbeat");

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

        std::unique_lock lk{ m_devicesLastSeenMutex };
        m_devicesLastSeen.insert_or_assign(std::string(deviceID), Clock::now());
    }
}