#include "gecko/MQTTSub.h"

namespace Gecko::Embedded
{
    MQTTSub::Status MQTTSub::s_status{ MQTTSub::Status::Uninitialized };

    MQTTSub::Inbox* MQTTSub::s_incoming{ nullptr };
    MQTTSub::Inbox MQTTSub::s_latestImageID{ s_latestImageIDBuffer, sizeof(s_latestImageIDBuffer) };
    MQTTSub::Inbox MQTTSub::s_latestImage  { s_latestImageBuffer,   sizeof(s_latestImageBuffer)   };

    char MQTTSub::s_latestImageIDBuffer[128];
    char MQTTSub::s_latestImageBuffer[1024*10];
}