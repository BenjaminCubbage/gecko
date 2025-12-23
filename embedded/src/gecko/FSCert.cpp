#include "gecko/FSCert.h"

namespace Gecko::Embedded
{
    char FSCert::s_certificate[FSCert::AllocCertSize];
    unsigned int FSCert::s_certificateLen;
}