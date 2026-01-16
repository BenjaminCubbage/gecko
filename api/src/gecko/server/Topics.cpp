#include "gecko/server/Topics.h"
#include "gecko/logging/Logger.h"

namespace Gecko::API::Server
{
    bool Topics::Start()
    {
        using Logging::Logger;

        if (!m_mqttClient.ConnectSync())
        {
            Logger::Error() << "[Topics.Start]: Couldn't connect to the MQTT server";
            Logger::Error() << "[Topics.Start]: ~ On port: " + std::to_string(m_env->mosquittoPort);
            return false;
        }

        Logging::Logger::Info() << "[Topics.Start]: Successfully connected to MQTT server";
        Heartbeat().Start();

        return true;
    }
}