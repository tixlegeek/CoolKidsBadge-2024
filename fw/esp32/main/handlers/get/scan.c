/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/
#include "../handlers.h"
#include "../../coolkids.h"
#include "main.h"

static const char *TAG = "GET_SCAN";
/**
  authtoa
  @brief Return a string representing the auth type
  @parms authstr: Pointer to an allocated buffer
  @parms authmode: authentication code (Related to ESP-IDF inners)
  @return -
*/
static void authtoa(char *authstr, int authmode)
{
    switch (authmode) {

      case WIFI_AUTH_OPEN:
        snprintf(authstr, 32, "OPEN");
        break;
      case WIFI_AUTH_WEP:
        snprintf(authstr, 32, "WEP");
        break;
      case WIFI_AUTH_WPA_PSK:
        snprintf(authstr, 32, "WPA_PSK");
        break;
      case WIFI_AUTH_WPA2_PSK:
        snprintf(authstr, 32, "WPA2_PSK");
        break;
      case WIFI_AUTH_WPA_WPA2_PSK:
        snprintf(authstr, 32, "WPA_WPA2_PSK");
        break;
      case WIFI_AUTH_WPA2_ENTERPRISE:
        snprintf(authstr, 32, "WPA2_ENTERPRISE");
        break;
      case WIFI_AUTH_WPA3_PSK:
        snprintf(authstr, 32, "WPA3_PSK");
        break;
      case WIFI_AUTH_WPA2_WPA3_PSK:
        snprintf(authstr, 32, "WPA2_WPA3_PSK");
        break;
      case WIFI_AUTH_WAPI_PSK:
        snprintf(authstr, 32, "WAPI_PSK");
        break;
      case WIFI_AUTH_OWE:
        snprintf(authstr, 32, "OWE");
        break;
      case WIFI_AUTH_WPA3_ENT_192:
        snprintf(authstr, 32, "WPA3_ENT_192");
        break;
      case WIFI_AUTH_MAX:
        snprintf(authstr, 32, "MAX");
        break;
    }
}
/**
  rssi_to_percentage
  @brief returns a percentage from a RSSI Value
  @parms rssi: RSSI Value (dBm)
  @return A number between 0 and 100
*/
int rssi_to_percentage(int rssi) {
    // Define the maximum and minimum RSSI values for ESP32
    const int MAX_RSSI = -20; // Maximum RSSI value (excellent signal)
    const int MIN_RSSI = -100; // Minimum RSSI value (poor signal)

    // Normalize RSSI value within the defined range
    if (rssi > MAX_RSSI) {
        rssi = MAX_RSSI;
    } else if (rssi < MIN_RSSI) {
        rssi = MIN_RSSI;
    }

    // Convert the RSSI value to a percentage
    int percentage = ((rssi - MIN_RSSI) * 100) / (MAX_RSSI - MIN_RSSI);

    // Invert the percentage since the RSSI scale is negative
    return 100 - percentage;
}

/**
  scan_get_handler
  @brief WIFI Scan API
  @parms req: HTTP Request
  @return esp_err_t
*/
esp_err_t scan_get_handler(httpd_req_t *req)
{
    esp_err_t err;
    uint16_t number = CKB_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[CKB_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;
    char authmode[32] = {0};
    cJSON *root = 0;
    root = cJSON_CreateArray();
    if (!root)
        return ESP_FAIL;
    char *jsonResponse;
    memset(ap_info, 0, sizeof(ap_info));
    ESP_LOGI(TAG, "GET SCAN REQUEST");

    wifi_mode_t wifi_mode;

    ESP_ERROR_CHECK(esp_wifi_get_mode(&wifi_mode));
    if( wifi_mode == WIFI_MODE_APSTA){
        ESP_LOGI(TAG, "Scanning while APSTA");
      }
      else if( wifi_mode == WIFI_MODE_AP)
      {
        ESP_LOGI(TAG, "Scanning while AP");
    }
    else if( wifi_mode == WIFI_MODE_STA)
    {
        ESP_LOGI(TAG, "Scanning while STA");
    }
    else
    {
      ESP_LOGE(TAG, "Scanning while mode unknown");

    }
    // Ensure STA is enabled
    /*
    if (wifi_mode != WIFI_MODE_APSTA && wifi_mode != WIFI_MODE_STA) {
        esp_wifi_set_mode(wifi_mode == WIFI_MODE_AP ? WIFI_MODE_APSTA : WIFI_MODE_STA);
        ESP_ERROR_CHECK(esp_wifi_start());
    }*/

    err = esp_wifi_scan_start(NULL, true);
    if (err != ESP_OK)
    {
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "C0uld n0t St4rt Sc4n :'(");
      ESP_ERROR_CHECK(err);
      return err;
    }

    err = esp_wifi_scan_get_ap_records(&number, ap_info);
    if (err != ESP_OK)
    {
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "W1f1 Sc4n F41l3d :(");
      ESP_ERROR_CHECK(err);
      return err;
    }

    err = esp_wifi_scan_get_ap_num(&ap_count);
    if (err != ESP_OK)
    {
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "W1f1 Sc4n F41l3d :(");
      ESP_ERROR_CHECK(err);
      return err;
    }

    ESP_LOGI(TAG, "FOUND %d APs", ap_count);
    for (int i = 0; (i < number); i++)
    {
        cJSON *jsonAp = cJSON_CreateObject();
        if (!jsonAp)
            return ESP_FAIL;
        cJSON_AddStringToObject(jsonAp, "ssid", (char *)ap_info[i].ssid);
        cJSON_AddNumberToObject(jsonAp, "rssi_dbm", ap_info[i].rssi);
        cJSON_AddNumberToObject(jsonAp, "rssi_%", rssi_to_percentage(ap_info[i].rssi));
        cJSON_AddNumberToObject(jsonAp, "channel", ap_info[i].primary);
        authtoa(authmode, ap_info[i].authmode);
        cJSON_AddStringToObject(jsonAp, "authmode", authmode);
        cJSON_AddNumberToObject(jsonAp, "authmode_code", ap_info[i].authmode);
        cJSON_AddItemToArray(root, jsonAp);
    }
    jsonResponse = cJSON_Print(root);
    cJSON_Delete(root);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, jsonResponse);
    free(jsonResponse);
    return ESP_OK;
}
