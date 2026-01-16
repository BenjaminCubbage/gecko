#include "gecko/db/SharedImagesTable.h"

namespace Gecko::API::DB
{
    SharedImagesTable::Result
    SharedImagesTable::CreateSharedImage(int senderID,
                                         int receiverID,
                                         const std::string& idempotencyKey,
                                         const std::vector<uint8_t>& bytes,
                                         int* outSharedImageID)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            /*
                note(ben): mysqlx::byte is unsigned char
            */

            const mysqlx::bytes span{ reinterpret_cast<const mysqlx::byte*>(bytes.data()), bytes.size() };

            auto insertBlobResult =
                connection.value()->sql("INSERT INTO SharedImageBlobs (idempotency_key, bytes) VALUES (?, ?)")
                    .bind(idempotencyKey, span)
                    .execute();

            if (insertBlobResult.getAffectedItemsCount() == 0)
                return Result::Failure;

            /*
                note(ben): Multi-statements are not supported in this
                library. LAST_INSERT_ID() is not affected by other open
                sessions (no racing)
            */

            auto queryBlobIDResult =
                connection.value()->sql("SELECT LAST_INSERT_ID()").execute();

            if (queryBlobIDResult.count() != 1)
                return Result::Failure;

            const int blobID = queryBlobIDResult.fetchOne().get(0);

            /*
                note(ben): Potential for orphaned blob if this fails.
                In general this... isn't a good way of doing things. I
                really shouldn't have gone with a library that doesn't
                support error codes, of all things :^p
            */

            auto insertSharedImageResult =
                connection.value()->sql("INSERT INTO SharedImages (image_blob_id, sender_id, receiver_id) "
                                "VALUES (?, ?, ?)")
                    .bind(blobID, senderID, receiverID)
                    .execute();

            if (insertSharedImageResult.getAffectedItemsCount() <= 0)
                return Result::Failure;

            auto querySharedImageIDResult =
                connection.value()->sql("SELECT LAST_INSERT_ID()").execute();

            if (querySharedImageIDResult.count() != 1)
                return Result::Failure;

            *outSharedImageID = querySharedImageIDResult.fetchOne().get(0);

            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    SharedImagesTable::Result
    SharedImagesTable::IdempotencyKeyExists(const std::string& idempotencyKey,
                                            bool *outExists)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("SELECT 1 FROM SharedImageBlobs "
                                "WHERE idempotency_key = ?")
                    .bind(idempotencyKey)
                    .execute();

            *outExists = result.hasData() && result.count() > 0;
            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    SharedImagesTable::Result
    SharedImagesTable::GetLatestReceivedImageBlob(int receiverID,
                                                  std::vector<uint8_t>* outBlob)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("SELECT ib.bytes FROM SharedImages si "
                                    "LEFT JOIN SharedImageBlobs ib "
                                    "ON si.image_blob_id=ib.image_blob_id "
                                "WHERE receiver_id=? "
                                "ORDER BY image_id DESC LIMIT 1")
                    .bind(receiverID)
                    .execute();

            if (!result.hasData() || result.count() != 1)
                return Result::Failure;

            const mysqlx::bytes bytes = result.fetchOne().get(0).get<mysqlx::bytes>();

            *outBlob = {};
            outBlob->resize(bytes.size());
            std::memcpy(outBlob->data(), bytes.begin(), bytes.size());

            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }
}