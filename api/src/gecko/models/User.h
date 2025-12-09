#pragma once

namespace Gecko::API::Models
{
    struct User
    {
        int userID;
        std::string username;
        std::string oidcIss;
        std::string oidcSub;
    };
}