/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/

#ifndef NVS_DB_H_
#define NVS_DB_H_
#include <nvs.h>

#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

esp_err_t led_set_saved(uint8_t shader);
esp_err_t led_get_saved(uint8_t *shader);
esp_err_t wifi_sta_set_saved(wifi_config_t *config);
esp_err_t wifi_sta_get_saved(wifi_config_t *config);
esp_err_t wifi_ap_set_saved(wifi_config_t *config);
esp_err_t wifi_ap_get_saved(wifi_config_t *config);

#endif /* end of include guard: NVS_DB_H_ */
