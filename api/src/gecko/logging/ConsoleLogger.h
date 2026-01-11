#pragma once
#include <iostream>
#include <string>
#include "gecko/logging/Logger.h"

namespace Gecko::API::Logging
{
    class ConsoleLogger final : public Logger
    {
    public:
        void operator<<(const std::string& v) override
        {
            std::cout << m_prefix << v << std::endl;
        }

        ConsoleLogger(std::string prefix)
            : m_prefix(std::move(prefix)) { }

    private:
        const std::string m_prefix;
    };
}