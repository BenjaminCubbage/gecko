#pragma once
#include <string>
#include "gecko/models/DeviceStatus.h"

namespace Gecko::API::Models
{
    struct DeviceWithStatus
    {
        int deviceID;
        std::string name;
        DeviceStatus status;
    };
}