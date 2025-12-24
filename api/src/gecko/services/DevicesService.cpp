#include "gecko/services/DevicesService.h"
#include "gecko/db/DevicesTable.h"

#define EXPECT(cond, failResult) do { if (!(cond)) return failResult; } while(0)

namespace Gecko::API::Services
{
    DevicesService::Result
    DevicesService::GetDeviceStatus(int deviceID,
                                    DeviceStatus *outStatus)
    {
        std::string deviceIDStr = std::to_string(deviceID);
        auto status = m_devicesHeartbeatTopic->GetDeviceStatus(deviceIDStr);

        if (status == Topics::DevicesHeartbeatTopic::Status::Online)
        {
            *outStatus = DeviceStatus::Online;
            return Result::Success;
        }

        bool exists{};
        if (m_existingDevicesCache.contains(deviceID))
            exists = true;
        else
        {
            EXPECT(m_dbDevices->DeviceExists(deviceID, &exists) == DB::DevicesTable::Result::Success, Result::DatabaseError);

            if (exists)
                m_existingDevicesCache.insert(deviceID);
        }

        EXPECT(exists, Result::DeviceNotFound);

        switch (status)
        {
            case Topics::DevicesHeartbeatTopic::Status::Offline:
                *outStatus = DeviceStatus::Offline;
                break;

            case Topics::DevicesHeartbeatTopic::Status::Pending:
            default:
                *outStatus = DeviceStatus::Pending;
                break;
        }

        return Result::Success;
    }

    DevicesService::Result
    DevicesService::GetUsersDevices(int ownerID,
                                    std::vector<Models::Device>* outDevices)
    {
        EXPECT(m_dbDevices->GetDevicesByOwnerID(ownerID, outDevices) == DB::DevicesTable::Result::Success, Result::DatabaseError);

        if (outDevices->size() > 0)
            return Result::Success;

        bool exists{};
        EXPECT(m_dbUsers->UserExists(ownerID, &exists) == DB::UsersTable::Result::Success, Result::DatabaseError);
        EXPECT(exists, Result::UserNotFound);

        return Result::Success;
    }
}