#pragma once
#include "fatfs/ff.h"

namespace Gecko::Embedded
{
    class FS
    {
    public:
        static bool Mount();
        static FRESULT ReadFile(const char* filename, int readBytes, char* outBuffer, unsigned int* outBytesRead);
        static const char* FResultToStr(FRESULT result);
        
    private:
        static FATFS s_fs;
    };
}