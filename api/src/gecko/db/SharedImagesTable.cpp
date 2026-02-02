#include "gecko/db/SharedImagesTable.h"

namespace Gecko::API::DB
{
    SharedImagesTable::Result
    SharedImagesTable::CreateSharedImage(int senderUserID,
                                         int recipientDeviceID,
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
                connection.value()->sql("INSERT INTO SharedImageBlobs (idempotency_key, bytes) "
                                        "VALUES (?, ?)")
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
                even support error codes :^p
            */

            auto insertSharedImageResult =
                connection.value()->sql("INSERT INTO SharedImages "
                                            "(image_blob_id, sender_user_id, recipient_device_id) "
                                        "VALUES (?, ?, ?)")
                    .bind(blobID, senderUserID, recipientDeviceID)
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
    SharedImagesTable::GetLatestReceivedImageBlob(int recipientUserID,
                                                  std::vector<uint8_t>* outBlob)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("SELECT sib.bytes FROM SharedImageBlobs sib "
                                            "JOIN SharedImages si ON sib.image_blob_id = si.image_blob_id "
                                            "JOIN Devices d ON d.device_id = si.recipient_device_id "
                                        "WHERE d.owner_id = ? "
                                        "ORDER BY si.image_id DESC "
                                        "LIMIT 1")
                    .bind(recipientUserID)
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