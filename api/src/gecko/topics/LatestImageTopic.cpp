#include "gecko/topics/LatestImageTopic.h"

namespace Gecko::API::Topics
{
    void LatestImageTopic::PublishLatestImage(int deviceID,
                                              int imageID,
                                              const std::vector<uint8_t>& image)
    {
        const std::string topic
            = "devices/" + std::to_string(deviceID) + "/in/latest-image";

        std::string imageIDStr = std::to_string(imageID);

        m_mqttClient->PublishMessage(
            topic, std::span<const uint8_t>{ 
                reinterpret_cast<const uint8_t*>(imageIDStr.c_str()),
                imageIDStr.size() },
            true, nullptr, nullptr);

        m_mqttClient->PublishMessage(
            topic, image, true, nullptr, nullptr);
    }
}