#pragma once
#include <cstring>
#include <string>
#include <string_view>
#include "gecko/Log.h"
#include "lwip/apps/mqtt_priv.h"
#include "lwip/apps/mqtt.h"
#include "lwip/inet.h"
#include "lwip/ip_addr.h"

namespace Gecko::Embedded
{
    class MQTTConn
    {
    public:
        struct Connection
        {
            std::string username;
            std::string password;

            ip_addr_t ip;
            int port;
            mqtt_connect_client_info_t clientInfo;
            mqtt_client_t *client;

            bool connected;
        };

        static bool Init(const std::string& ipStr,
                         int port,
                         std::string_view tlsCert,
                         const std::string& username,
                         const std::string& password)
        {
            static constexpr int LWTQOS             = 1;
            static constexpr int LWTRetain          = 1;
            static constexpr const char* LWTMessage = "off";

            if (!ip4addr_aton(ipStr.c_str(), &s_connection.ip))
            {
                Log_Error("MQTTConn: Could not parse IP from string: %s\n", ipStr);
                return false;
            }

            s_connection.port     = port;
            s_connection.username = username;
            s_connection.password = password;

            s_connection.clientInfo.tls_config = altcp_tls_create_config_client(
                reinterpret_cast<const unsigned char*>(tlsCert.data()),
                tlsCert.size());

            s_connection.clientInfo.client_id   = "";
            s_connection.clientInfo.keep_alive  = KeepAliveSec;
            s_connection.clientInfo.client_user = s_connection.username.c_str();
            s_connection.clientInfo.client_pass = s_connection.password.c_str();

            s_connection.client = mqtt_client_new();

            if (!s_connection.client)
            {
                Log_Error("MQTTConn: mqtt_client_new() failed\n");
                return false;
            }

            return true;
        }

        static bool Connect(void (*cb)(mqtt_connection_status_t))
        {
            cyw43_arch_lwip_begin();
            if (mqtt_client_connect(
                s_connection.client,
                &s_connection.ip,
                s_connection.port,
                ConnectStatusCB,
                reinterpret_cast<void*>(cb),
                &s_connection.clientInfo) != ERR_OK)
            {
                Log_Error("MQTTConn: MQTT broker connection error\n");
                cyw43_arch_lwip_end();
                return false;
            }

            // note(ben): Enable this when doing tls
            // mbedtls_ssl_set_hostname(altcp_tls_context(state->mqtt_client_inst->conn), MQTT_SERVER);

            cyw43_arch_lwip_end();
            return true;
        }

        static void ConnectStatusCB(mqtt_client_t*, void* cb, mqtt_connection_status_t status)
        {
            Log_Info("MQTTConn: %s\n", ConnectionStatusToStr(status));
            s_connection.connected = status == MQTT_CONNECT_ACCEPTED;
            reinterpret_cast<void (*)(mqtt_connection_status_t)>(cb)(status);
        }

        static const Connection* ConnectionState()
        {
            return &s_connection;
        }

    private:
        static const char* ConnectionStatusToStr(mqtt_connection_status_t status)
        {
            switch (status)
            {
                case MQTT_CONNECT_ACCEPTED:                 return "Connected";
                case MQTT_CONNECT_REFUSED_PROTOCOL_VERSION: return "Refused protocol version";
                case MQTT_CONNECT_REFUSED_IDENTIFIER:       return "Refused identifier";
                case MQTT_CONNECT_REFUSED_SERVER:           return "Refused server";
                case MQTT_CONNECT_REFUSED_USERNAME_PASS:    return "Refused username / password";
                case MQTT_CONNECT_REFUSED_NOT_AUTHORIZED_:  return "Authorization failed";
                case MQTT_CONNECT_DISCONNECTED:             return "Connection failed or disconnected";
                case MQTT_CONNECT_TIMEOUT:                  return "Connection timed out";
            }

            return "Unknown error";
        }

        static Connection s_connection;
        static constexpr int KeepAliveSec = 800;
    };
}