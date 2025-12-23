#pragma once
#include <memory>
#include <string>
#include <unordered_set>
#include "gecko/db/DevicesTable.h"
#include "gecko/topics/DevicesHeartbeatTopic.h"

namespace Gecko::API::Services
{
    class DevicesService
    {
    public:
        enum class Result
        {
            Success,

            /* Service-space */
            DeviceNotFound,

            /* DB fallthrough */
            DatabaseError
        };

        enum class DeviceStatus
        {
            Online,
            Offline,
            Pending
        };

        DevicesService(std::shared_ptr<Topics::DevicesHeartbeatTopic> devicesHeartbeatTopic,
                       DB::DevicesTable devicesTable)
            : m_devicesHeartbeatTopic(devicesHeartbeatTopic), m_devicesTable(devicesTable) {}

        Result GetDeviceStatus(int deviceID,
                               DeviceStatus *outStatus);

    private:
        std::shared_ptr<Topics::DevicesHeartbeatTopic> m_devicesHeartbeatTopic;
        DB::DevicesTable                               m_devicesTable;
        std::unordered_set<int>                        m_existingDevicesCache;
    };
}