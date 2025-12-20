#define TLS_CLIENT_SERVER        "fw-download-alias1.raspberrypi.com"
#define TLS_CLIENT_HTTP_REQUEST  "GET /net_install/boot.sig HTTP/1.1\r\n" \
                                 "Host: " TLS_CLIENT_SERVER "\r\n" \
                                 "Connection: close\r\n" \
                                 "\r\n"
#define TLS_CLIENT_TIMEOUT_SECS  15

extern bool run_tls_client_test(const uint8_t *cert, size_t cert_len, const char *server, const char *request, int timeout);

#define HOLD_VSYS_EN_PIN 2   // Inky Frame “hold power” pin
#define LED_A_PIN 11
#define LED_B_PIN 12
#define LED_C_PIN 13
#define LED_D_PIN 14
#define LED_E_PIN 15

void FailurePath()
{
    gpio_put(LED_A_PIN, 1);
    while (true)
        sleep_ms(100000);
}

void SuccessPath()
{
    gpio_put(LED_A_PIN, 1);
    for (int v = 0;; sleep_ms(100))
    {
        printf("ass");
        gpio_put(LED_A_PIN, v = !v);
    }
}

int main() {
    gpio_init(HOLD_VSYS_EN_PIN);
    gpio_set_dir(HOLD_VSYS_EN_PIN, GPIO_OUT);
    gpio_put(HOLD_VSYS_EN_PIN, 1);
    
    stdio_init_all();
    gpio_init(LED_A_PIN); gpio_set_dir(LED_A_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN); gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_init(LED_C_PIN); gpio_set_dir(LED_C_PIN, GPIO_OUT);
    gpio_init(LED_D_PIN); gpio_set_dir(LED_D_PIN, GPIO_OUT);
    gpio_init(LED_E_PIN); gpio_set_dir(LED_E_PIN, GPIO_OUT);

    if (cyw43_arch_init())
        FailurePath();

    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
        FailurePath();

    bool pass = run_tls_client_test(NULL, 0, TLS_CLIENT_SERVER, TLS_CLIENT_HTTP_REQUEST, TLS_CLIENT_TIMEOUT_SECS);

    if (pass) {
        SuccessPath();
    } else
        FailurePath();

    cyw43_arch_deinit();
    return pass ? 0 : 1;
}