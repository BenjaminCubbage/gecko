#include "gecko/MQTTConn.h"

namespace Gecko::Embedded
{
    MQTTConn::Connection MQTTConn::s_connection;
    char MQTTConn::s_statusTopic[56];
}