#pragma once
#include <ostream>
#include "gecko/env/Env.h"

namespace Gecko::API::Server
{
    class Server
    {
    public:
        bool Start(const Env::Env& env, std::ostream& log);
    };
}