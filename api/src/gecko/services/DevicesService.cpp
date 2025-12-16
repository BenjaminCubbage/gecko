#include "gecko/services/DevicesService.h"
#include "gecko/db/DevicesTable.h"

#define EXPECT(cond, failResult) do { if (!(cond)) return failResult; } while(0)

namespace Gecko::API::Services
{
    DevicesService::Result DevicesService::GetDeviceStatus(int deviceID,
                                                           DeviceStatus *outStatus)
    {
        std::string deviceIDStr = std::to_string(deviceID);
        auto status = m_devicesStatusTopic->GetDeviceStatus(deviceIDStr);

        switch (status)
        {
        case Topics::DevicesStatusTopic::Status::Online:
            *outStatus = DeviceStatus::Online;
            return Result::Success;

        case Topics::DevicesStatusTopic::Status::Offline:
            *outStatus = DeviceStatus::Offline;
            return Result::Success;

        case Topics::DevicesStatusTopic::Status::MaybeLater:
            *outStatus = DeviceStatus::Pending;
            return Result::Success;
        }
        
        bool exists{};
        EXPECT(m_devicesTable.DeviceExists(deviceID, &exists) == DB::DevicesTable::Result::Success, Result::DatabaseError);
        EXPECT(exists, Result::DeviceNotFound);

        m_devicesStatusTopic->BeginTrackingDevice(deviceIDStr);
        *outStatus = DeviceStatus::Pending;
        return Result::Success;
    }
}