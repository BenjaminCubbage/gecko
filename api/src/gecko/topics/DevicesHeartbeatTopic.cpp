#include "gecko/topics/DevicesHeartbeatTopic.h"

namespace Gecko::API::Topics
{
    DevicesHeartbeatTopic::Status
    DevicesHeartbeatTopic::GetDeviceStatus(const std::string& deviceID)
    {
        std::shared_lock lk{ m_epochDevicesLastSeenMutex };
        const auto it = m_epochDevicesLastSeen.find(deviceID);

        if (it == m_epochDevicesLastSeen.end())
            return EpochNow() - m_epochStartup <= GracePeriod
                ? Status::Pending
                : Status::Offline;

        return EpochNow() - it->second <= GracePeriod
            ? Status::Online
            : Status::Offline;
    }

    void DevicesHeartbeatTopic::MessageReceivedHandler(std::string_view topic,
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

        std::unique_lock lk{ m_epochDevicesLastSeenMutex };
        m_epochDevicesLastSeen.insert_or_assign(std::string(deviceID), EpochNow());
    }
}