#pragma once
#include <string_view>
#include "fatfs/ff.h"
#include "gecko/FS.h"
#include "gecko/Log.h"

namespace Gecko::Embedded
{
    class FSCert
    {
    public:
        enum class Result
        {
            Success,
            CouldNotOpen,
            CertDotPEMNotThere,
            CertTooBig
        };

        static Result ReadCert(std::string_view* outCertificate)
        {
            static constexpr const char* CertPath = "/cert.pem";
            Log_Debug("FSCert: Reading certificate from %s\n", CertPath);

            // note(ben): TLS cert needs space to have null terminator at the end

            FRESULT r{};
            if (r = FS::ReadFile(CertPath, AllocCertSize - 1, s_certificate, &s_certificateLen))
            {
                Log_Error("FSCert: Failed to read certificate.");
                return r == FR_NO_FILE
                    ? Result::CertDotPEMNotThere
                    : Result::CouldNotOpen;
            }

            if (s_certificateLen >= AllocCertSize - 1)
            {
                Log_Error("FSCert: Certificate file was too large. "
                          "Bytes allocated: %d\n", AllocCertSize - 1);
                return Result::CertTooBig;
            }

            Log_Info("FSCert: Successfully read certificate. "
                     "Bytes: %d, Allocated: %d\n", s_certificateLen, AllocCertSize);

            s_certificate[s_certificateLen++] = '\0';
            *outCertificate = std::string_view{ s_certificate, s_certificateLen };

            return Result::Success;
        }

    private:
        // note(ben): 4kB max certificate length
        static constexpr int AllocCertSize = 1024 * 4;
        static char s_certificate[AllocCertSize];
        static unsigned int s_certificateLen;
    };
}