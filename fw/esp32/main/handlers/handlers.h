/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/
#ifndef PAGE_HANDLERS_H
#define PAGE_HANDLERS_H
#include "cJSON.h"
#include <errno.h>
#include <esp_http_server.h>
#include <esp_ota_ops.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_system.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <mbedtls/base64.h>
#include <lwip/sockets.h>
#include "nvs_db.h"
#include "../config.h"
#include "../keep_alive.h"


#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

struct async_resp_arg
{
    httpd_handle_t hd;
    int fd;
};

static const size_t max_clients = 4;

typedef struct
{
    bool sta_connected;
    char *sta_ip;
    char *sta_gw;
    char *sta_netmask;
    char ap_ssid[32];
    char ap_password[64];
    uint32_t sta_cidr;
    char *ap_ip;
    uint32_t *ap_cidr;

} ckb_info_t;

extern ckb_info_t ckb_info;

/*
 * Serve OTA update portal (index.html)
 */
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

extern const uint8_t css_css_start[] asm("_binary_css_css_start");
extern const uint8_t css_css_end[] asm("_binary_css_css_end");

extern const uint8_t js_js_start[] asm("_binary_js_js_start");
extern const uint8_t js_js_end[] asm("_binary_js_js_end");

extern const uint8_t i2c_html_start[] asm("_binary_i2c_html_start");
extern const uint8_t i2c_html_end[] asm("_binary_i2c_html_end");

extern const uint8_t led_html_start[] asm("_binary_led_html_start");
extern const uint8_t led_html_end[] asm("_binary_led_html_end");

extern const uint8_t landing_html_start[] asm("_binary_landing_html_start");
extern const uint8_t landing_html_end[] asm("_binary_landing_html_end");

extern const uint8_t gpio_html_start[] asm("_binary_gpio_html_start");
extern const uint8_t gpio_html_end[] asm("_binary_gpio_html_end");

extern const uint8_t wifi_html_start[] asm("_binary_wifi_html_start");
extern const uint8_t wifi_html_end[] asm("_binary_wifi_html_end");

void gpio_web_task(void *ctx);
void send_gpio(void *arg);

esp_err_t scan_get_handler(httpd_req_t *req);
esp_err_t uuid_get_handler(httpd_req_t *req);
esp_err_t index_get_handler(httpd_req_t *req);

esp_err_t led_get_handler(httpd_req_t *req);
esp_err_t ledcb_get_handler(httpd_req_t *req);

esp_err_t gpio_get_handler(httpd_req_t *req);
esp_err_t gpioconf_post_handler(httpd_req_t *req);

esp_err_t i2c_get_handler(httpd_req_t *req);
esp_err_t i2cscan_get_handler(httpd_req_t *req);
esp_err_t i2cread_get_handler(httpd_req_t *req);
esp_err_t i2cwrite_get_handler(httpd_req_t *req);

esp_err_t landing_get_handler(httpd_req_t *req);
esp_err_t wifi_get_handler(httpd_req_t *req);
esp_err_t css_get_handler(httpd_req_t *req);
esp_err_t js_get_handler(httpd_req_t *req);

esp_err_t ledconf_post_handler(httpd_req_t *req);
esp_err_t staconf_post_handler(httpd_req_t *req);
esp_err_t apconf_post_handler(httpd_req_t *req);
esp_err_t update_post_handler(httpd_req_t *req);

esp_err_t ws_route_api(char *api, char *msg, void *ctx);
esp_err_t ws_api_gpio(char *msg, void *ctx);

//esp_err_t ws_handler(httpd_req_t *req);
esp_err_t ws_handler(httpd_req_t *req);
esp_err_t wss_open_fd(httpd_handle_t hd, int sockfd);
void wss_close_fd(httpd_handle_t hd, int sockfd);

#endif /* end of include guard: PAGE_HANDLERS_H */
