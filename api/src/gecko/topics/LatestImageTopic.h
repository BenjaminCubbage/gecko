#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "gecko/mqtt/MQTTClient.h"

namespace Gecko::API::Topics
{
    class LatestImageTopic
    {
    public:
        LatestImageTopic(MQTT::MQTTClient* mqttClient)
            : m_mqttClient(mqttClient) {}

        /*
            This is a no-op for now.
        */
        inline void Start() {}

        /*
            Push the latest image and latest image ID to the MQTT server
            as a retained message.
        */
        void PublishLatestImage(int deviceID,
                                int imageID,
                                const std::vector<uint8_t>& image);

    private:
        void MessageReceivedHandler(std::string_view topic, std::span<uint8_t> payload);

        MQTT::MQTTClient* m_mqttClient;
    };
}