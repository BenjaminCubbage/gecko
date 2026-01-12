#include "gecko/db/DevicesTable.h"

namespace Gecko::API::DB
{
    using DB::ConnectionPool;

    DevicesTable::Result
    DevicesTable::DeviceExists(int deviceID,
                               bool *outExists)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("SELECT 1 FROM Devices WHERE device_id=?")
                    .bind(deviceID)
                    .execute();

            *outExists = result.count() == 1;
            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    DevicesTable::Result
    DevicesTable::DeviceExists(int deviceID,
                               bool *outExists,
                               int *outOwnerID)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("SELECT owner_id FROM Devices "
                                "WHERE device_id=?")
                    .bind(deviceID)
                    .execute();

            *outExists = result.count() == 1;
            *outOwnerID = *outExists
                ? result.fetchOne().get(0).get<int>()
                : -1;

            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    DevicesTable::Result
    DevicesTable::GetDevicesByOwnerID(int ownerID,
                                      std::vector<Models::Device>* outDevices)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("SELECT device_id, name FROM Devices "
                                "WHERE owner_id=?")
                    .bind(ownerID)
                    .execute();

            *outDevices = {};
            outDevices->reserve(result.count());

            for (auto row : result)
                outDevices->push_back({
                    .deviceID = row.get(0).get<int>(),
                    .name     = row.get(1).get<std::string>()
                });

            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }
}