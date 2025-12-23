#include "gecko/FS.h"
#include "gecko/Log.h"

namespace Gecko::Embedded
{
    FATFS FS::s_fs;

    bool FS::Mount()
    {
        Log_Debug("FS: Mounting SD card\n");

        FRESULT r;
        if (r = f_mount(&s_fs, "", 1))
            Log_Error("FS: Could not mount SD card: %s\n", FResultToStr(r));
        else
            Log_Info("FS: Successfully mounted\n");

        return r == 0;
    }

    FRESULT FS::ReadFile(const char* filename, int readBytes, char* outBuffer, unsigned int* outBytesRead)
    {
        FRESULT r{};
        FIL fp{};
        
        if (r = f_open(&fp, filename, FA_READ))
        {
            Log_Error("FS: Could not open file %s: %s\n", filename, FResultToStr(r));
            return r;
        }

        if (r = f_read(&fp, outBuffer, readBytes, outBytesRead))
            Log_Error("FS: Could not read file %s: %s\n", filename, FResultToStr(r));

        f_close(&fp);
        return r;
    }
    
    const char* FS::FResultToStr(FRESULT result)
    {
        switch (result)
        {
            case FR_OK:				     return "(0) Succeeded";
            case FR_DISK_ERR:			 return "(1) A hard error occurred in the low level disk I/O layer";
            case FR_INT_ERR:			 return "(2) Assertion failed";
            case FR_NOT_READY:			 return "(3) The physical drive cannot work";
            case FR_NO_FILE:			 return "(4) Could not find the file";
            case FR_NO_PATH:			 return "(5) Could not find the path";
            case FR_INVALID_NAME:		 return "(6) The path name format is invalid";
            case FR_DENIED:				 return "(7) Access denied due to prohibited access or directory full";
            case FR_EXIST:				 return "(8) Access denied due to prohibited access";
            case FR_INVALID_OBJECT:		 return "(9) The file/directory object is invalid";
            case FR_WRITE_PROTECTED:	 return "(10) The physical drive is write protected";
            case FR_INVALID_DRIVE:		 return "(11) The logical drive number is invalid";
            case FR_NOT_ENABLED:		 return "(12) The volume has no work area";
            case FR_NO_FILESYSTEM:		 return "(13) There is no valid FAT volume";
            case FR_MKFS_ABORTED:		 return "(14) The f_mkfs() aborted due to any problem";
            case FR_TIMEOUT:			 return "(15) Could not get a grant to access the volume within defined period";
            case FR_LOCKED:				 return "(16) The operation is rejected according to the file sharing policy";
            case FR_NOT_ENOUGH_CORE:	 return "(17) LFN working buffer could not be allocated";
            case FR_TOO_MANY_OPEN_FILES: return "(18) Number of open files > FF_FS_LOCK";
            case FR_INVALID_PARAMETER:	 return "(19) Given parameter is invalid";
        }

        return "Unkown error";
    }
}