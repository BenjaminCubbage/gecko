#pragma once
#include "drivers/fatfs/ff.h"

namespace Gecko::Embedded
{
    class FS
    {
    public:
        enum class State
        {
            NotMounted, Mounted
        };

        static bool Mount();
        static bool ReadFile(const char* filename, unsigned int readBytes, char* outBuffer, unsigned int* outBytesRead);
        static bool WriteFile(const char* filename, const char* bytes, unsigned int numBytes, bool writeOverExisting);
        static const char* FResultToStr(FRESULT result);
        
    private:
        static State s_state;
        static FATFS s_fs;
    };
}