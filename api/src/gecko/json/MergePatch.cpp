#include "gecko/json/MergePatch.h"
#include <set>
#include <string>


namespace Gecko::API::JSON
{
    // RFC 7386
    //
    // define MergePatch(Target, Patch):
    //      if Patch is an Object:
    //          if Target is not an Object:
    //              Target = {}  # Ignore the contents and set it to an empty Object
    //
    //          for each Name/Value pair in Patch:
    //              if Value is null:
    //                  if Name exists in Target:
    //                      remove the Name/Value pair from Target
    //              else:
    //                  Target[Name] = MergePatch(Target[Name], Value)
    //
    //          return Target
    //      else:
    //          return Patch
    Json::Value MergePatch::Merge(const Json::Value& target,
                                  const Json::Value& patch)
    {
        if (!patch.isObject())
            return patch;

        Json::Value result = target.isObject()
            ? target
            : Json::Value{};

        for (const auto& member : patch.getMemberNames())
        {
            if (patch[member].isNull())
            {
                if (result.isMember(member))
                    result.removeMember(member);
            }
            else
                result[member] = Merge(result[member], patch[member]);
        }

        return result;
    }
}