#pragma once
#include <cstring>
#include <string>
#include <string_view>
#include "gecko/Log.h"
#include "gecko/Wifi.h"
#include "lwip/apps/mqtt_priv.h"
#include "lwip/apps/mqtt.h"
#include "lwip/inet.h"
#include "lwip/ip_addr.h"

namespace Gecko::Embedded
{
    class MQTTConn
    {
    public:
        enum class Status
        {
            Uninitialized, NotConnected, Connecting, Connected, Error
        };

        struct Connection
        {
            std::string username;
            std::string password;

            ip_addr_t ip;
            int port;
            mqtt_connect_client_info_t clientInfo;
            mqtt_client_t *client;

            Status status{ Status::Uninitialized };
        };

        static bool Init(const std::string& ipStr,
                         int port,
                         std::string_view tlsCert,
                         const std::string& username,
                         const std::string& password)
        {
            if (s_connection.status != Status::Uninitialized)
            {
                Log_Error("MQTTConn: Tried to initialize more than once\n");
                return false;
            }

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

            s_connection.status = Status::NotConnected;
            return true;
        }

        static bool Connect()
        {
            cyw43_arch_lwip_begin();
            if (s_connection.status == Status::Uninitialized)
            {
                Log_Error("MQTTConn: Tried to connect but not yet initialized\n");
                goto fail;
            }

            if (s_connection.status == Status::Connected ||
                s_connection.status == Status::Connecting)
            {
                Log_Error("MQTTConn: Tried to connect but already connected or trying to connect\n");
                goto fail;
            }

            if (mqtt_client_connect(
                s_connection.client,
                &s_connection.ip,
                s_connection.port,
                ConnectStatusCB,
                nullptr,
                &s_connection.clientInfo) != ERR_OK)
            {
                Log_Error("MQTTConn: MQTT broker connection error\n");
                s_connection.status = Status::Error;
                goto fail;
            }

            s_connection.status = Status::Connecting;
            cyw43_arch_lwip_end();
            return true;

        fail:
            cyw43_arch_lwip_end();
            return false;
        }

        static bool ConnectSync()
        {
            if (!Connect())
                return false;

            while (s_connection.status == MQTTConn::Status::Connecting)
            {
                Wifi::Poll();
                sleep_ms(90);
            }

            return s_connection.status == MQTTConn::Status::Connected;
        }

        static void Disconnect()
        {
            cyw43_arch_lwip_begin();
            mqtt_disconnect(s_connection.client);
            s_connection.status = Status::NotConnected;
            cyw43_arch_lwip_end();
        }

        static void ConnectStatusCB(mqtt_client_t*, void* cb, mqtt_connection_status_t status)
        {
            if (status == MQTT_CONNECT_ACCEPTED)
            {
                Log_Info("MQTTConn: Connected\n");
                s_connection.status = Status::Connected;
            }
            else
            {
                Log_Error("MQTTConn: Couldn't connect: %s\n", ConnectionStatusToStr(status));
                s_connection.status = Status::Error;
            }
        }

        static inline const Connection* ConnectionState()
        {
            return &s_connection;
        }

        static inline Status ConnectionStatus()
        {
            return s_connection.status;
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