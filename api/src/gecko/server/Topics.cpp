#include "gecko/server/Topics.h"

namespace Gecko::API::Server
{
    bool Topics::Start()
    {
        if (!m_mqttClient.ConnectSync())
        {
            *m_log << "[Topics]: Couldn't connect to the MQTT server\n";
            *m_log << "[Topics]: Note: On port " << m_env->mosquittoPort << std::endl;
            return false;
        }

        *m_log << "[Topics]: Successfully connected to MQTT server" << std::endl;
        Heartbeat().Start();
        return true;
    }
}