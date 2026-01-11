#pragma once
#include <iostream>
#include <string>
#include "gecko/logging/Logger.h"

namespace Gecko::API::Logging
{
    class SilentLogger final : public Logger
    {
    public:
        void operator<<(const std::string& v) override {}
    };
}
