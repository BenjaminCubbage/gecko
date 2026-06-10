#pragma once
#include <string>
#include "gecko/env/Env.h"
#include "gecko/mqtt/MQTTClient.h"
#include "gecko/topics/DevicesHeartbeatTopic.h"
#include "gecko/topics/LatestImageTopic.h"

namespace Gecko::API::Server
{
    class Topics
    {
      public:
        Topics(Env::Env* env)
            : m_env{ env },
              m_mqttClient{
                  "ssl://localhost:" + std::to_string(m_env->mosquittoPort),
                  m_env->mosquittoCertPath, "root",
                  m_env->mosquittoPassword },
              m_devicesHeartbeatTopic{ &m_mqttClient },
              m_latestImageTopic     { &m_mqttClient } {}

        Topics           (const Topics&) = delete;
        Topics& operator=(const Topics&) = delete;
        Topics           (Topics&&) = delete;
        Topics& operator=(Topics&&) = delete;

        inline API::Topics::DevicesHeartbeatTopic& Heartbeat()   { return m_devicesHeartbeatTopic; }
        inline API::Topics::LatestImageTopic&      LatestImage() { return m_latestImageTopic; }

        bool Start();

      private:
        Env::Env*        m_env;
        MQTT::MQTTClient m_mqttClient;
        
        API::Topics::DevicesHeartbeatTopic m_devicesHeartbeatTopic;
        API::Topics::LatestImageTopic      m_latestImageTopic;
    };
}