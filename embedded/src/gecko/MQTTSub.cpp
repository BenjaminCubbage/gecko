#include "gecko/MQTTSub.h"

namespace Gecko::Embedded
{
    MQTTSub::Status MQTTSub::s_status{ MQTTSub::Status::Uninitialized };

    char MQTTSub::s_latestImageIDTopic[100];
    char MQTTSub::s_latestImageTopic[100];

    MQTTSub::SubStatus MQTTSub::s_latestImageIDSubStatus{ SubStatus::Uninitialized };
    MQTTSub::SubStatus MQTTSub::s_latestImageSubStatus{ SubStatus::Uninitialized };
    
    char MQTTSub::s_latestImageIDBuffer[128];
    char MQTTSub::s_latestImageBuffer[1024*10];

    MQTTSub::Inbox* MQTTSub::s_incoming{ nullptr };

    MQTTSub::Inbox MQTTSub::s_latestImageID{ s_latestImageIDBuffer, sizeof(s_latestImageIDBuffer) };
    MQTTSub::Inbox MQTTSub::s_latestImage  { s_latestImageBuffer,   sizeof(s_latestImageBuffer)   };
}