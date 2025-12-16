#pragma once

namespace Gecko::API::Models
{
    struct SharedImage
    {
        int imageBlobID;
        std::vector<uint8_t> bytes;
    };
}