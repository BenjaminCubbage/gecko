#include "gecko/env/EnvParse.h"

namespace Gecko::API::Env
{
    std::unordered_map<std::string, std::string> EnvParse::Parse(const std::string &str)
    {
        std::unordered_map<std::string, std::string> keyValues;

        int pos = 0;

        while (pos < str.size())
        {
            if (str[pos] == '#')
                continue;

            int eqPos = pos;
            while (eqPos < str.size() && str[eqPos] != '\n' && str[eqPos] != '=')
                ++eqPos;

            int nlPos = eqPos;
            while (nlPos < str.size() && str[nlPos] != '\n')
                ++nlPos;

            if (eqPos - pos > 1 && nlPos - eqPos > 1)
                keyValues.insert_or_assign
                (
                    str.substr(pos, eqPos - pos),
                    std::string(str.data() + eqPos + 1, str.data() + nlPos)
                );

            pos = nlPos + 1;
        }

        return keyValues;
    }
}