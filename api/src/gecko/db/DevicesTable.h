#pragma once
#include <string>
#include "gecko/db/ConnectionPool.h"
#include "gecko/models/Device.h"
#include "mysqlx/xdevapi.h"

namespace Gecko::API::DB
{
    class DevicesTable
    {
      public:
        enum class Result { OK, Failure };

        DevicesTable(ConnectionPool* connectionPool)
            : m_connectionPool(connectionPool) {}

        Result DeviceExists(int deviceID,
                            bool *outExists);

        Result DeviceExists(int deviceID,
                            bool *outExists,
                            int *outOwnerID);

        Result GetDevicesByOwnerID(int ownerID,
                                   std::vector<Models::Device>* outDevices);

      private:
        ConnectionPool* m_connectionPool;
    };
}