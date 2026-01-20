#pragma once
#include "json/json.h"


namespace Gecko::API::JSON
{
    class MergePatch
    {
      public:
        // RFC 7386
        static Json::Value Merge(const Json::Value& target,
                                 const Json::Value& patch);
    };
}