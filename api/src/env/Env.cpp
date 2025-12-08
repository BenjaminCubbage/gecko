#include "gecko/env/Env.h"
#include <fstream>
#include <iostream>


namespace Gecko::API
{
    std::optional<Env> Env::LoadFromFilepath(const std::string& filepath)
    {
        std::ifstream file{ filepath };
        if (!file.good()) 
            return std::nullopt;

        file.seekg(0, file.end);
        int fileLen = file.tellg();
        file.seekg(0, file.beg);

        if (!file.good() || !fileLen)
            return std::nullopt;

        std::string str(fileLen, '\0');
        file.read(str.data(), fileLen);

        if (!file.good())
            return std::nullopt;

        return LoadFromString(std::move(str));
    }
    
    std::optional<Env> Env::LoadFromString(const std::string& str)
    {
        Env env;

        for (int pos = 0; pos < str.size();)
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
                env.m_env.insert_or_assign
                (
                    str.substr(pos, eqPos - pos),
                    std::string(str.data() + eqPos + 1, str.data() + nlPos)
                );

            pos = nlPos + 1;
        }

        
        return env;
    }
}
