#pragma once
#include <cstring>
#include <string>
#include "gecko/FS.h"
#include "gecko/Log.h"

namespace Gecko::Embedded
{
    class FSSafeReadWrite
    {
    public:
#if FF_USE_LFN > 0
        static constexpr size_t MaxFilenameLength{ FF_LFN_BUF };
#else
        static constexpr size_t MaxFilenameLength{ FF_SFN_BUF };
#endif

    private:
        static constexpr size_t SuffixLength{ 2 };
        static constexpr char ASuffix[SuffixLength]{ '@', 'A' };
        static constexpr char BSuffix[SuffixLength]{ '@', 'B' };
        static constexpr char ACommitSuffix[SuffixLength]{ '$', 'A' };
        static constexpr char BCommitSuffix[SuffixLength]{ '$', 'B' };

    public:
        static bool WriteFile(const std::string& filename,
                              const char* bytes,
                              unsigned int numBytes,
                              bool writeOverExisting)
        {
            // Contains null-terminated text like "<filename>@a\0"
            static char filenameScratch[MaxFilenameLength];
            if (!AssertPreSuffixFilenameLength(filename))
                return false;

            // Paste in filename and null terminator
            std::memcpy(filenameScratch, filename.c_str(), filename.size());
            filenameScratch[filename.size() + SuffixLength] = '\0';

            // Utility
            auto setScratchSuffix = [&filename](const char* suffix) {
                std::memcpy(filenameScratch + filename.size(), suffix, SuffixLength);
            };

            // Let's check the commit flag for file A first
            bool writingToB{};
            setScratchSuffix(ACommitSuffix);

            CommitFlag_Get(filenameScratch, &writingToB);

            if (!CommitFlag_Get(filenameScratch, &writingToB))
                Log_Warn("FSSafeReadWrite: Could not get commit flag at %s\n",
                         filenameScratch);

            if (writingToB) Log_Debug("FSSafeReadWrite: Selected side B\n");
            else            Log_Debug("FSSafeReadWrite: Selected side A\n");

            const char* suffixToWrite = writingToB ? BSuffix : ASuffix;
            const char* commitToSet   = writingToB ? BCommitSuffix : ACommitSuffix;
            const char* commitToUnset = writingToB ? ACommitSuffix : BCommitSuffix;

            // Write to selected file
            setScratchSuffix(suffixToWrite);
            if (!FS::WriteFile(filenameScratch, bytes, numBytes, writeOverExisting))
            {
                Log_Error("FSSafeReadWrite: Couldn't write to file %s\n", filenameScratch);
                return false;
            }

            // Set written commit flag
            setScratchSuffix(commitToSet);
            if (!CommitFlag_Set(filenameScratch, true))
            {
                Log_Error("FSSafeReadWrite: Could not set commit flag on %s\n",
                          filenameScratch);
                return false;
            }

            // Unset the other commit flag
            setScratchSuffix(commitToUnset);
            if (!CommitFlag_Set(filenameScratch, false))
            {
                Log_Error("FSSafeReadWrite: Could not unset commit flag on %s\n",
                          filenameScratch);
                return false;
            }

            Log_Debug("FSSafeReadWrite: Committed %u bytes to %s\n",
                      numBytes, filename.c_str());
            return true;
        }

        static bool ReadFile(const std::string& filename, 
                             unsigned int readBytes, 
                             char* outBuffer, 
                             unsigned int* outBytesRead)
        {
            // Contains null-terminated text like "<filename>@a\0"
            static char filenameScratch[MaxFilenameLength];
            
            if (!AssertPreSuffixFilenameLength(filename))
                return false;

            // Paste in filename and null terminator
            std::memcpy(filenameScratch, filename.c_str(), filename.size());
            filenameScratch[filename.size() + SuffixLength] = '\0';

            // Utility
            auto setScratchSuffix = [&filename](const char* suffix) {
                std::memcpy(filenameScratch + filename.size(), suffix, SuffixLength);
            };

            // Let's try for A, first
            bool commitASet{};
            bool commitBSet{};

            setScratchSuffix(ACommitSuffix);
            if (!CommitFlag_Get(filenameScratch, &commitASet))
            {
                Log_Warn("FSSafeReadWrite: Could not get commit flag at %s\n",
                         filenameScratch);
                return false;
            }

            setScratchSuffix(BCommitSuffix);
            if (!commitASet && !CommitFlag_Get(filenameScratch, &commitBSet))
            {
                Log_Warn("FSSafeReadWrite: Could not get commit flag at %s\n",
                         filenameScratch);
                return false;
            }

            if (!commitASet && !commitBSet)
            {
                Log_Error("FSSafeReadWrite: Could not get "
                          "either commit flag for filename %s\n", 
                          filename.c_str());
                return false;
            }

            if (!(commitASet ^ commitBSet))
            {
                Log_Error("FSSafeReadWrite: Logic error: commitASet and "
                          "commitBSet should be mutually exclusive\n");
                return false;
            }

            setScratchSuffix(commitASet ? ASuffix : BSuffix);
            if (!FS::ReadFile(filenameScratch, readBytes, outBuffer, outBytesRead))
            {
                Log_Error("FSSafeReadWrite: Couldn't read from file %s\n",
                          filenameScratch);
                return false;
            }

            return true;
        }

    private:
        static bool CommitFlag_Get(const char* filename, bool* outFlag)
        {
            char c{};
            unsigned int bytesRead{};

            if (!FS::ReadFile(filename, 1, &c, &bytesRead))
                return false;

            *outFlag = bytesRead == 1 && c == '1';
            return true;
        }

        static bool CommitFlag_Set(const char* filename, bool flag)
        {
            char c = flag ? '1' : '0';
            return FS::WriteFile(filename, &c, 1, true);
        }

        static bool AssertPreSuffixFilenameLength(const std::string& filename)
        {
            if (filename.size() + SuffixLength + 1 > MaxFilenameLength)
            {
                Log_Error("FSSafeReadWrite: Filename exceeded max allowable length "
                          "after appending suffix: %s "
                          "(length: %zu, suffix: %zu, total: %zu, allowed: %zu)\n",
                          filename.c_str(),
                          filename.size(),
                          SuffixLength,
                          filename.size() + SuffixLength,
                          MaxFilenameLength - 1);
                return false;
            }

            return true;
        }
    };
}