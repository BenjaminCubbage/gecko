#pragma once
#include <array>
#include <string>
#include <tuple>
#include "drivers/fatfs/ff.h"

namespace Gecko::Embedded
{
    /*
        intent(FSTwoPhase): To provide a way to (more) safely write
        to disk without (as much) risking partial-writes / data
        loss.

        Uses A/B files with commit flags to limit the ability of
        partial writes (mainly as a result of power loss) to
        overwrite the previously stored data.

        When writes are completed, a commit flag is set indicating
        the A/B file was successfully written and recent.
    */
    class FSTwoPhase
    {
    private:
#if FF_USE_LFN > 0
        static constexpr size_t MaxTotalFilenameLength{ FF_LFN_BUF };
#else
        static constexpr size_t MaxTotalFilenameLength{ FF_SFN_BUF };
#endif

        using Suffix = std::array<char, 2>;

        static constexpr Suffix ASuffix{ '@', 'A' };
        static constexpr Suffix BSuffix{ '@', 'B' };
        static constexpr Suffix ACommitSuffix{ '$', 'A' };
        static constexpr Suffix BCommitSuffix{ '$', 'B' };

        /*
            intent(FilenameScratchpad): Enable in-place modification
            of a filename's suffix while eliminating unnecessary copies.
        */
        class FilenameScratchpad
        {
        public:
            // Set <filename>
            bool SetFilename(const std::string& filename);

            // Set <suffix>
            void SetSuffix(const Suffix& suffix);

            // Get <filename><suffix>
            inline const char* FilenameWithSuffix() const { return m_filename; };

        private:
            size_t m_preSuffixLen{ 0 };
            char m_filename[MaxTotalFilenameLength + 1];
        };

    public:
        /*
            Maximum filename length before appending the A/B suffix
        */
        static constexpr size_t MaxInputFilenameLength{
            MaxTotalFilenameLength - std::tuple_size_v<Suffix> };

        static_assert(std::tuple_size_v<Suffix> < MaxTotalFilenameLength);

        /*
            Write data to the associated file, then commit the A/B.

            Returns true if the data was successfully written.
            Otherwise, false.
        */
        static bool WriteAndCommit(const std::string& filename,
                                   const char* bytes,
                                   unsigned int numBytes,
                                   bool writeOverExisting);

        /*
            Read the committed A/B file if it exists.

            Returns true if the data was successfully read.
            Otherwise, false.
        */
        static bool ReadCommitted(const std::string& filename,
                                  unsigned int readBytes,
                                  char* outBuffer,
                                  unsigned int* outBytesRead);

    private:
        static bool CommitFlag_Get(const char* filename, bool* outFlag);
        static bool CommitFlag_Set(const char* filename, bool flag);
    };
}