#include "gecko/topics/LatestImageTopic.h"

namespace Gecko::API::Topics
{
    void LatestImageTopic::PushLatestImage(int imageID,
                                           const std::vector<uint8_t>& image)
    {
        static constexpr const char* LatestImageIDTopic{ "devices/+/out/latest-image-id" };

        
    }
}