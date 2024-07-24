/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/
#include "../../coolkids.h"
#include "../../wifi.h"
#include "../handlers.h"
#include <esp_spiffs.h>
static const char *TAG = "GET_UUID";
/**
uuid_get_handler
@brief UUID GET API, returns a buch of informations about the badge
@parms req: Http request
@return esp_err_t
*/
esp_err_t uuid_get_handler(httpd_req_t *req) {
  cJSON *root = cJSON_CreateObject();
  char *jsonResponse;
  char *buffer;
  uint8_t chipid[6];
  int rssi = 0;
  size_t spiffs_total = 0, spiffs_used = 0;
  size_t heap_total = 0, heap_used = 0;
  ESP_LOGI(TAG, "GET UUID REQUEST");
  wifi_config_t sta_config = {0};

  if (wifi_sta_get_saved(&sta_config) != ESP_OK) {
    ESP_LOGE(TAG, "Could not retrieve last STA configuration... :( \n");
  }

  esp_read_mac(chipid, ESP_MAC_BASE);
  esp_spiffs_info(NULL, &spiffs_total, &spiffs_used);
  heap_total = (size_t)heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
  heap_used =
      (size_t)(heap_total - heap_caps_get_free_size(MALLOC_CAP_DEFAULT));

  asprintf(&buffer, "%02x%02x%02x%02x%02x%02x", chipid[0], chipid[1], chipid[2],
           chipid[3], chipid[4], chipid[5]);
  cJSON *APinfo = cJSON_CreateObject();
  cJSON *STAinfo = cJSON_CreateObject();
  cJSON *HEAPinfo = cJSON_CreateObject();
  cJSON *SPIFFSinfo = cJSON_CreateObject();

  cJSON_AddStringToObject(root, "uuid", (char *)buffer);
  cJSON_AddStringToObject(root, "version", CKB_FIRMWARE_VERSION);

  cJSON_AddNumberToObject(HEAPinfo, "total", heap_total);
  cJSON_AddNumberToObject(HEAPinfo, "used", heap_used);

  cJSON_AddNumberToObject(SPIFFSinfo, "total", spiffs_total);
  cJSON_AddNumberToObject(SPIFFSinfo, "used", spiffs_used);
  // AP data
  cJSON_AddStringToObject(APinfo, "ssid", ckb_info.ap_ssid);
  cJSON_AddStringToObject(APinfo, "password", ckb_info.ap_password);
  cJSON_AddStringToObject(APinfo, "ip", ckb_info.ap_ip);
  cJSON_AddNumberToObject(APinfo, "cidr", (int)IP2CIDR(ckb_info.ap_cidr));
  // STA data
  cJSON_AddStringToObject(STAinfo, "ssid", (char *)sta_config.sta.ssid);
  if (ckb_info.sta_connected == true) {
    cJSON_AddStringToObject(STAinfo, "connected", "on");
    ESP_ERROR_CHECK(esp_wifi_sta_get_rssi(&rssi));
    if (ckb_info.sta_ip != NULL) {
      cJSON_AddStringToObject(STAinfo, "ip", ckb_info.sta_ip);
    }
    if (ckb_info.sta_gw != NULL) {
      cJSON_AddStringToObject(STAinfo, "gw", ckb_info.sta_gw);
    }
    cJSON_AddNumberToObject(STAinfo, "cidr", (int)IP2CIDR(ckb_info.sta_cidr));
    cJSON_AddNumberToObject(STAinfo, "rssi", rssi);
  } else {
    cJSON_AddStringToObject(STAinfo, "connected", "off");
  }
  // Add STA and AP data to root
  cJSON_AddItemToObject(root, "sta", STAinfo);
  cJSON_AddItemToObject(root, "ap", APinfo);
  cJSON_AddItemToObject(root, "heap", HEAPinfo);
  cJSON_AddItemToObject(root, "spiffs", SPIFFSinfo);

  jsonResponse = cJSON_Print(root);
  cJSON_Delete(root);
  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req, jsonResponse);
  free(jsonResponse);
  free(buffer);
  return ESP_OK;
}
