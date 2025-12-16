#pragma once
#include <memory>
#include <string>
#include "gecko/db/DevicesTable.h"
#include "gecko/topics/DevicesStatusTopic.h"

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

        DevicesService(std::shared_ptr<Topics::DevicesStatusTopic> devicesStatusTopic,
                       DB::DevicesTable devicesTable)
            : m_devicesStatusTopic(devicesStatusTopic), m_devicesTable(devicesTable) {}

        Result GetDeviceStatus(int deviceID,
                               DeviceStatus *outStatus);

    private:
        std::shared_ptr<Topics::DevicesStatusTopic> m_devicesStatusTopic;
        DB::DevicesTable                            m_devicesTable;
    };
}