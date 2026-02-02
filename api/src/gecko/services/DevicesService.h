#pragma once
#include <memory>
#include <string>
#include <unordered_set>
#include "gecko/db/DevicesTable.h"
#include "gecko/db/UsersTable.h"
#include "gecko/models/Device.h"
#include "gecko/topics/DevicesHeartbeatTopic.h"

namespace Gecko::API::Services
{
    class DevicesService
    {
      public:
        enum class Result
        {
            OK,

            /* Service-space */
            DeviceNotFound,
            UserNotFound,

            /* DB fallthrough */
            DatabaseError
        };

        enum class DeviceStatus
        {
            Online,
            Offline,
            Pending
        };

        DevicesService(Topics::DevicesHeartbeatTopic* devicesHeartbeatTopic,
                       DB::DevicesTable* devicesTable,
                       DB::UsersTable* usersTable)
            : m_devicesHeartbeatTopic(devicesHeartbeatTopic),
              m_dbDevices(devicesTable),
              m_dbUsers(usersTable) {}

        Result GetDeviceExists(int deviceID,
                               bool* outExists,
                               int* outOwnerID);

        Result GetDeviceStatus(int deviceID,
                               DeviceStatus *outStatus);

        Result GetUsersDevices(int ownerID,
                               std::vector<Models::Device>* outDevices);

      private:
        Topics::DevicesHeartbeatTopic* m_devicesHeartbeatTopic;
        DB::DevicesTable*              m_dbDevices;
        DB::UsersTable*                m_dbUsers;
        std::unordered_set<int>        m_existingDevicesCache;
    };
}