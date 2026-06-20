#include "gecko/services/DevicesService.h"
#include "gecko/db/DevicesTable.h"

#define EXPECT(cond, failResult) do { if (!(cond)) return failResult; } while(0)

namespace Gecko::API::Services
{
    DevicesService::Result
    DevicesService::GetDeviceExists(int deviceID,
                                    bool* outExists,
                                    int* outOwnerID)
    {
        EXPECT(m_dbDevices->DeviceExists(deviceID, outExists, outOwnerID)
            == DB::DevicesTable::Result::OK, Result::DatabaseError);

        return Result::OK;
    }

    DevicesService::Result
    DevicesService::GetDeviceStatus(int deviceID,
                                    Models::DeviceStatus *outStatus)
    {
        *outStatus = GetDeviceStatusWithoutCheck(deviceID);

        if (*outStatus == Models::DeviceStatus::Online)
            return Result::OK;

        /*
            Caching device existence because devices are very rarely
            deleted.
        */
        bool exists{};
        if (m_existingDevicesCache.contains(deviceID))
            exists = true;
        else
        {
            EXPECT(m_dbDevices->DeviceExists(deviceID, &exists) == DB::DevicesTable::Result::OK, Result::DatabaseError);

            if (exists)
                m_existingDevicesCache.insert(deviceID);
        }

        EXPECT(exists, Result::DeviceNotFound);
        return Result::OK;
    }

    DevicesService::Result
    DevicesService::GetUsersDevices(int ownerID,
                                    std::vector<Models::DeviceWithStatus>* outDevices)
    {
        *outDevices = {};

        /*
            Get raw (no-status) devices first
        */
        std::vector<Models::Device> rawDevices;
        EXPECT(m_dbDevices->GetDevicesByOwnerID(ownerID, &rawDevices) == DB::DevicesTable::Result::OK, Result::DatabaseError);

        if (rawDevices.size() == 0) {
            /*
                Does the user not exist or do they have no devices?
            */
            bool exists{};
            EXPECT(m_dbUsers->UserExists(ownerID, &exists) == DB::UsersTable::Result::OK, Result::DatabaseError);
            EXPECT(exists, Result::UserNotFound);
            return Result::OK;
        }

        /*
            Copy result with statuses included.
        */
        outDevices->reserve(rawDevices.size());
        for (const auto& rawDevice : rawDevices)
            outDevices->push_back(Models::DeviceWithStatus{
                .deviceID = rawDevice.deviceID,
                .name     = std::move(rawDevice.name),
                .status   = GetDeviceStatusWithoutCheck(rawDevice.deviceID)
            });

        return Result::OK;
    }

    Models::DeviceStatus DevicesService::GetDeviceStatusWithoutCheck(int deviceID)
    {
        std::string deviceIDStr = std::to_string(deviceID);
        auto status = m_devicesHeartbeatTopic->GetDeviceStatus(deviceIDStr);

        switch (status)
        {
            case Topics::DevicesHeartbeatTopic::DeviceStatus::Online:
                return Models::DeviceStatus::Online;

            case Topics::DevicesHeartbeatTopic::DeviceStatus::Offline:
                return Models::DeviceStatus::Offline;
        }

        return Models::DeviceStatus::Pending;
    }
}