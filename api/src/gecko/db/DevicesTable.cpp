#include "gecko/db/DevicesTable.h"

namespace Gecko::API::DB
{
    DevicesTable::Result
    DevicesTable::DeviceExists(int deviceID,
                               bool *outExists)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            auto result =
                connection->sql("SELECT 1 FROM Devices WHERE device_id=?")
                    .bind(deviceID)
                    .execute();

            *outExists = result.count() == 1;
            return Result::Success;
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

            auto result =
                connection->sql("SELECT owner_id FROM Devices "
                                "WHERE device_id=?")
                    .bind(deviceID)
                    .execute();

            *outExists = result.count() == 1;
            *outOwnerID = *outExists
                ? result.fetchOne().get(0).get<int>()
                : -1;

            return Result::Success;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }
}