#pragma once
#include <string>
#include <vector>
#include <type_traits>
#include "gecko/db/ConnectionPool.h"
#include "gecko/models/SharedImage.h"
#include "mysqlx/xdevapi.h"

namespace Gecko::API::DB
{
    class SharedImagesTable
    {
    public:
        enum class Result { Success, Failure };

        SharedImagesTable(std::shared_ptr<ConnectionPool>& connectionPool)
            : m_connectionPool(connectionPool) {}

        Result CreateSharedImage(int senderID,
                                 int receiverID,
                                 const std::string& idempotencyKey,
                                 const std::vector<uint8_t>& bytes);

        Result IdempotencyKeyExists(const std::string& idempotencyKey,
                                    bool *outExists);

        Result GetLatestReceivedImageBlob(int receiverID,
                                          std::vector<uint8_t>* outBlob);

    private:
        std::shared_ptr<ConnectionPool> m_connectionPool;
    };
}