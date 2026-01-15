#include "gecko/Wifi.h"
#include "gecko/Log.h"

namespace Gecko::Embedded
{
    bool Wifi::Init()
    {
        int result{};
        if (result = cyw43_arch_init())
        {
            Log_Error("Error: cyw43_arch_init failed: Status code %d", result);
            return false;
        }

        cyw43_arch_enable_sta_mode();
        return true;
    }

    bool Wifi::ConnectSync(const std::string& wifiSSID, const std::string& wifiPassword)
    {
        int result{};
        if (result = cyw43_arch_wifi_connect_timeout_ms(
            wifiSSID.c_str(),
            wifiPassword.c_str(),
            CYW43_AUTH_WPA2_AES_PSK,
            TimeoutMS))
        {
            Log_Error("Wifi: Failed to connect: Status code %d ", result);
            return false;
        }

        return true;
    }

    void Wifi::Poll()
    {
        cyw43_arch_lwip_begin();
        cyw43_arch_poll();
        cyw43_arch_lwip_end();
    }
}