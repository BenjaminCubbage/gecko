#pragma once
#include <string_view>
#include "drivers/fatfs/ff.h"
#include "gecko/FS.h"
#include "gecko/Log.h"

namespace Gecko::Embedded
{
    class FSCert
    {
    public:
        static bool ReadCert(std::string_view* outCertificate)
        {
            // note(ben): TLS cert needs space to have null terminator at the end

            static constexpr const char* CertPath = "/cert.pem";
            Log_Debug("FSCert: Reading certificate from %s\n", CertPath);

            FRESULT r{};
            if (!FS::ReadFile(CertPath, AllocCertSize - 1, s_certificate, &s_certificateLen))
            {
                Log_Error("FSCert: Failed to read certificate.");
                return false;
            }

            if (s_certificateLen >= AllocCertSize - 1)
            {
                Log_Error("FSCert: Certificate file was too large. "
                          "Bytes allocated: %d\n", AllocCertSize - 1);
                return false;
            }

            Log_Info("FSCert: Successfully read certificate. "
                     "Bytes: %d, Allocated: %d\n", s_certificateLen, AllocCertSize);

            s_certificate[s_certificateLen++] = '\0';
            *outCertificate = std::string_view{ s_certificate, s_certificateLen };
            return true;
        }

    private:
        // note(ben): 4kB max certificate length
        static constexpr int AllocCertSize = 1024 * 4;

        static char s_certificate[AllocCertSize];
        static unsigned int s_certificateLen;
    };
}