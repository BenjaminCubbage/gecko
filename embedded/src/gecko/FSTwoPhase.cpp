#include "gecko/FSTwoPhase.h"
#include <cstring>
#include "gecko/FS.h"
#include "gecko/Log.h"

namespace Gecko::Embedded
{
    bool FSTwoPhase::FilenameScratchpad::SetFilename(const std::string& filename)
    {
        if (filename.size() > MaxInputFilenameLength)
        {
            Log_Error("FSTwoPhase::FilenameScratchpad: Filename exceeded "
                      "max allowable length: %s (length: %zu, allowed: %zu)\n",
                      filename.c_str(),
                      filename.size(),
                      MaxInputFilenameLength);
            return false;
        }

        m_preSuffixLen = filename.size();
        std::memcpy(m_filename, filename.c_str(), m_preSuffixLen);
        m_filename[m_preSuffixLen] = '\0';

        return true;
    }

    void FSTwoPhase::FilenameScratchpad::SetSuffix(const Suffix& suffix)
    {
        std::memcpy(&m_filename[m_preSuffixLen], suffix.data(), suffix.size());
        m_filename[m_preSuffixLen + suffix.size()] = '\0';
    }

    bool FSTwoPhase::WriteAndCommit(const std::string& filename,
                                    const char* bytes,
                                    unsigned int numBytes,
                                    bool writeOverExisting)
    {
        FilenameScratchpad scratch;

        if (!scratch.SetFilename(filename))
            return false;

        bool writingToB{};

        scratch.SetSuffix(ACommitSuffix);
        if (!CommitFlag_Get(scratch.FilenameWithSuffix(), &writingToB))
        {
            Log_Warn("FSTwoPhase: Could not get commit flag at %s\n",
                     scratch.FilenameWithSuffix());
        }

        if (writingToB) Log_Debug("FSTwoPhase: Selected side B\n");
        else            Log_Debug("FSTwoPhase: Selected side A\n");

        auto suffixToWrite    = writingToB ? BSuffix : ASuffix;
        auto suffixToCommit   = writingToB ? BCommitSuffix : ACommitSuffix;
        auto suffixToUncommit = writingToB ? ACommitSuffix : BCommitSuffix;

        // Write to the selected file
        scratch.SetSuffix(writingToB ? BSuffix : ASuffix);
        if (!FS::WriteFile(scratch.FilenameWithSuffix(), bytes, numBytes, writeOverExisting))
        {
            Log_Error("FSTwoPhase: Couldn't write to file %s\n",
                      scratch.FilenameWithSuffix());
            return false;
        }

        // Set written commit flag
        scratch.SetSuffix(writingToB ? BCommitSuffix : ACommitSuffix);
        if (!CommitFlag_Set(scratch.FilenameWithSuffix(), true))
        {
            Log_Error("FSTwoPhase: Could not set commit flag on %s\n",
                       scratch.FilenameWithSuffix());
            return false;
        }

        // Unset the other commit flag
        scratch.SetSuffix(writingToB ? ACommitSuffix : BCommitSuffix);
        if (!CommitFlag_Set(scratch.FilenameWithSuffix(), false))
        {
            Log_Error("FSTwoPhase: Could not unset commit flag on %s\n",
                       scratch.FilenameWithSuffix());
            return false;
        }

        Log_Debug("FSTwoPhase: Committed %u bytes to %s\n",
                  numBytes, filename.c_str());
        return true;
    }

    bool FSTwoPhase::ReadCommitted(const std::string& filename,
                                   unsigned int readBytes,
                                   char* outBuffer,
                                   unsigned int* outBytesRead)
    {
        FilenameScratchpad scratch;

        if (!scratch.SetFilename(filename))
            return false;

        bool aCommitted{ false };
        bool bCommitted{ false };

        scratch.SetSuffix(ACommitSuffix);
        if (!CommitFlag_Get(scratch.FilenameWithSuffix(), &aCommitted))
            Log_Warn("FSTwoPhase: Could not get commit flag at %s\n", scratch.FilenameWithSuffix());

        scratch.SetSuffix(BCommitSuffix);
        if (!CommitFlag_Get(scratch.FilenameWithSuffix(), &bCommitted))
            Log_Warn("FSTwoPhase: Could not get commit flag at %s\n", scratch.FilenameWithSuffix());

        if (!aCommitted && !bCommitted) 
            return false;

        if (aCommitted && bCommitted)
            Log_Warn("FSTwoPhase: Both A and B committed for %s; choosing A\n", filename.c_str());

        auto tryRead = [&](const Suffix& suf) {
            scratch.SetSuffix(suf);
            return FS::ReadFile(scratch.FilenameWithSuffix(), readBytes, outBuffer, outBytesRead);
        };

        // Try preferred, then fallback
        if (aCommitted && tryRead(ASuffix)) return true;
        if (bCommitted && tryRead(BSuffix)) return true;

        Log_Error("FSTwoPhase: Couldn't read committed copy for %s\n", filename.c_str());
        return false;
    }

    bool FSTwoPhase::CommitFlag_Get(const char* filename, bool* outFlag)
    {
        char c{};
        unsigned int bytesRead{};

        if (!FS::ReadFile(filename, 1, &c, &bytesRead))
            return false;

        *outFlag = bytesRead == 1 && c == '1';
        return true;
    }

    bool FSTwoPhase::CommitFlag_Set(const char* filename, bool flag)
    {
        char c = flag ? '1' : '0';
        return FS::WriteFile(filename, &c, 1, true);
    }
}