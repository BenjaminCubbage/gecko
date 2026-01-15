#pragma once
#include "drivers/fatfs/ff.h"

namespace Gecko::Embedded
{
    /*
        intent(FS): Wrapper for fatfs disk I/O.

        Use FSTwoPhase instead if you need resiliency against partial
        writes. If you only need to read, this module is sufficient.
    */
    class FS
    {
    public:
        /*
            Calls f_mount() on the default drive.
        */
        static bool Mount();

        /*
            Opens the file, reads the contents of the file (up to
            readBytes bytes), then closes the file.

            Returns true if the file was successfully read.
            Otherwise, false.
        */
        static bool ReadFile(const char* filename, unsigned int readBytes, char* outBuffer, unsigned int* outBytesRead);

        /*
            Writes data to the supplied file.

            If writeOverExisting is true, the file is opened with the 
            FA_OPEN_ALWAYS flag, otherwise FA_CREATE_ALWAYS is used.
            
            Returns true if the data was successfully written.
            Otherwise, false.
        */
        static bool WriteFile(const char* filename, const char* bytes, unsigned int numBytes, bool writeOverExisting);

        static const char* FResultToStr(FRESULT result);

    private:
        static FATFS s_fs;
    };
}