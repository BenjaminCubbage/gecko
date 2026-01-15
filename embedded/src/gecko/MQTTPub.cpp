#include "gecko/MQTTPub.h"

namespace Gecko::Embedded
{
    char MQTTPub::s_heartbeatTopic[100];
    MQTTPub::PubStatus MQTTPub::s_heartbeatPubStatus{ PubStatus::Uninitialized };
}