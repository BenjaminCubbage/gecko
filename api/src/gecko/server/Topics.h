#pragma once
#include <ostream>
#include "gecko/env/Env.h"
#include "gecko/mqtt/MQTTClient.h"
#include "gecko/topics/DevicesHeartbeatTopic.h"

namespace Gecko::API::Server
{
    class Topics
    {
    public:
        Topics(Env::Env* env, std::ostream* log)
            : m_env{ env },
              m_log{ log },
              m_mqttClient{
                  "ssl://localhost:" + std::to_string(m_env->mosquittoPort),
                  m_env->mosquittoCertPath, "root",
                  m_env->mosquittoPassword },
              m_devicesHeartbeatTopic{ &m_mqttClient } {}
        
        Topics           (const Topics&) = delete;
        Topics& operator=(const Topics&) = delete;
        Topics           (Topics&&) = delete;
        Topics& operator=(Topics&&) = delete;

        inline API::Topics::DevicesHeartbeatTopic& Heartbeat() { return m_devicesHeartbeatTopic; }

        bool Start();

    private:
        Env::Env* m_env;
        std::ostream* m_log;

        MQTT::MQTTClient m_mqttClient;
        API::Topics::DevicesHeartbeatTopic m_devicesHeartbeatTopic;
    };
}