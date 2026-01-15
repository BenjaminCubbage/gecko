#pragma once
#include <string>

namespace Gecko::Embedded
{
    class Wifi
    {
    public:
        Wifi() = delete;
        static bool Init();
        static bool ConnectSync(const std::string& wifiSSID, const std::string& wifiPassword);
        static void Poll();

    private:
        static constexpr int TimeoutMS = 10 * 1000;
    };
}