/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/
#include "../../coolkids.h"
#include "../../led.h"
#include "../handlers.h"

static const char *TAG = "GET_LED";

esp_err_t led_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "GET LED FILE");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_sendstr(req, (const char *)led_html_start);
    return ESP_OK;
}
/**
  ledcb_get_handler
  @brief LED API (gets the current configured Shader)
  @parms req: HTTP Request
  @return esp_err_t
*/
esp_err_t ledcb_get_handler(httpd_req_t *req) {
  cJSON *root = cJSON_CreateObject();
  cJSON *led_callbacks = cJSON_CreateArray();
  char *jsonResponse;
  uint8_t led_callbacks_number = (uint8_t)LED_CB_NUMBER;
  uint8_t i = 0;

  for(i = 0; i < led_callbacks_number; i++){
    cJSON *led_callback = cJSON_CreateObject();
    cJSON_AddNumberToObject(led_callback, "id", (int)i);
    cJSON_AddStringToObject(led_callback, "name", led_cb_name[i]);
    cJSON_AddItemToArray(led_callbacks, led_callback);
  }

  cJSON_AddItemToObject(root, "cb", led_callbacks);

  jsonResponse = cJSON_Print(root);
  cJSON_Delete(root);

  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req, jsonResponse);

  free(jsonResponse);
  return ESP_OK;
}
