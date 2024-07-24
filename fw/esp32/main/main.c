/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/

#include "cJSON.h"
#include <driver/i2s.h>

#include <esp_spiffs.h>

#include "nvs_db.h"
#include <nvs.h>
#include <nvs_flash.h>

#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <unistd.h> // Include for close()

#include <esp_event.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <esp_system.h>

#include "config.h"
#include "keep_alive.h"
#include "main.h"
#include "mdns.h"
#include "sdkconfig.h"

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "board.h"
#include "coolkids.h"
#include "driver/i2c_master.h"
#include "handlers/handlers.h"
#include "httpd.h"
#include "led.h"
#include "wifi.h"

static const char *TAG = "COOLKIDSBADGE";

static uint8_t led_strip_pixels[BOARD_LED_NUMBERS * 3];
EventGroupHandle_t wifi_event_group;
httpd_handle_t https_server = NULL;
rgb_led_ctx_t rgb_led_ctx;

const int CONNECTED_BIT = (0x01) << 0;

ckb_info_t ckb_info = {
    .sta_connected = false,
    .sta_ip = NULL,
    .sta_gw = NULL,
    .sta_netmask = NULL,
};

void app_main(void) {
  esp_err_t err = nvs_flash_init();

  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  led_init(&rgb_led_ctx);

  const esp_partition_t *partition = esp_ota_get_running_partition();
  esp_ota_img_states_t ota_state;

  if (esp_ota_get_state_partition(partition, &ota_state) == ESP_OK) {
    if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
      esp_ota_mark_app_valid_cancel_rollback();
    }
  }

  ESP_LOGI(TAG, "[+] Starting led task");
  xTaskCreate(&led_task, "led_task", 3000, &rgb_led_ctx, tskIDLE_PRIORITY + 2,
              NULL);

  ESP_LOGI(TAG, "[+] Init WiFi");
  wifi_init();
  if (wifi_apsta(5000) == CKB_WIFI_MODE_APSTA) {
    ESP_LOGI(TAG, "[.] WiFi mode set to APSTA");
  } else {
    ESP_LOGI(TAG, "[.] WiFi mode set to AP");
  }

  ESP_LOGI(TAG, "[+] Starting gpio web task");
  xTaskCreate(&gpio_web_task, "gpio_web_task", 4096, &https_server,
              tskIDLE_PRIORITY, NULL);

  ESP_LOGI(TAG, "[+] Starting main loop");
  while (1) {
    ESP_LOGI(TAG, ".");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  //wss_server_send_messages(&https_server);
}
