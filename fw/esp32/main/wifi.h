/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/

#ifndef _WIFI_H_
#define _WIFI_H_

#include "coolkids.h"
#include "httpd.h"
#include "lwip/sockets.h"
#include "nvs_db.h"
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_wifi.h>
#include <stdint.h>

#include "sntp.h"

typedef enum { CKB_WIFI_MODE_AP, CKB_WIFI_MODE_APSTA } wifi_ckb_mode_t;

extern EventGroupHandle_t wifi_event_group;
extern const int CONNECTED_BIT;

uint8_t IP2CIDR(uint32_t ip);
esp_err_t configure_mac_wifi(esp_netif_t *ap_netif, esp_netif_t *sta_netif);
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
static void ip_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data);
void wifi_get_ssid(char *ssid, size_t len);
void wifi_get_password(char *password, size_t len);
void wifi_get_conf(char *ssid, size_t ssidlen, char *password,
                   size_t passwordlen);
wifi_ckb_mode_t wifi_apsta(int timeout_ms);
esp_err_t wifi_init(void);

#endif /* end of include guard: _WIFI_H_ */
