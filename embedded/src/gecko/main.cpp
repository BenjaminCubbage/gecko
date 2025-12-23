#include "gecko/FS.h"
#include "gecko/FSCert.h"
#include "gecko/Log.h"
#include "gecko/MQTTConn.h"
#include "gecko/MQTTPub.h"
#include "gecko/Wifi.h"

#define TLS_CLIENT_SERVER        "fw-download-alias1.raspberrypi.com"
#define TLS_CLIENT_HTTP_REQUEST  "GET /net_install/boot.sig HTTP/1.1\r\n" \
                                 "Host: " TLS_CLIENT_SERVER "\r\n"        \
                                 "Connection: close\r\n"                  \
                                 "\r\n"
#define TLS_CLIENT_TIMEOUT_SECS  15

extern bool run_tls_client_test(const uint8_t *cert, size_t cert_len, const char *server, const char *request, int timeout);

#define HOLD_VSYS_EN_PIN 2 // Inky Frame “hold power” pin
#define LED_A_PIN 11
#define LED_B_PIN 12
#define LED_C_PIN 13
#define LED_D_PIN 14
#define LED_E_PIN 15

void published(err_t err)
{
}

void connected(mqtt_connection_status_t status)
{
    using namespace Gecko::Embedded;
    MQTTPub::PublishStatus(MQTTPub::Status::Online, &published);
}

int main()
{
    using namespace Gecko::Embedded;

    gpio_init(HOLD_VSYS_EN_PIN);
    gpio_set_dir(HOLD_VSYS_EN_PIN, GPIO_OUT);
    gpio_put(HOLD_VSYS_EN_PIN, 1);

    stdio_init_all();
    gpio_init(LED_A_PIN); gpio_set_dir(LED_A_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN); gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_init(LED_C_PIN); gpio_set_dir(LED_C_PIN, GPIO_OUT);
    gpio_init(LED_D_PIN); gpio_set_dir(LED_D_PIN, GPIO_OUT);
    gpio_init(LED_E_PIN); gpio_set_dir(LED_E_PIN, GPIO_OUT);

    gpio_put(LED_A_PIN, 1);
    gpio_put(LED_B_PIN, 1);
    gpio_put(LED_C_PIN, 1);
    gpio_put(LED_D_PIN, 1);
    gpio_put(LED_E_PIN, 1);
    sleep_ms(1200);

    if (!Wifi::Init() ||
        !Wifi::Connect(WIFI_SSID, WIFI_PASSWORD))
    {
        gpio_put(HOLD_VSYS_EN_PIN, 0);
        return 1;
    }

    std::string_view cert;
    FS::Mount();
    FSCert::ReadCert(&cert);

    char deviceID[37]{};
    char devicePassword[37]{};

    unsigned int deviceIDLen{};
    unsigned int devicePasswordLen{};
    FS::ReadFile("/device_id.txt",       36, deviceID,       &deviceIDLen);
    FS::ReadFile("/device_password.txt", 36, devicePassword, &devicePasswordLen);

    printf("%s\n", deviceID);
    printf("%s\n", devicePassword);

    MQTTConn::Init(MQTT_IP, MQTT_PORT, cert, deviceID, devicePassword);
    MQTTConn::Connect(&connected);

    printf("%s\n", MQTTConn::StatusTopic());
    

    while (1)
    {
        Wifi::Poll();
        sleep_ms(100);
    }

    gpio_put(HOLD_VSYS_EN_PIN, 0);
    return 0;
}