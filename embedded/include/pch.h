#include "lwipopts.h"
#include "mbedtls_config.h"

#include <string.h>
#include <time.h>

#include "pico/printf.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"

#include "lwip/pbuf.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/dns.h"