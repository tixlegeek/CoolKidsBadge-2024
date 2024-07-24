/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/

#include "nvs_db.h"

static const char *TAG = "NVS_DB";

esp_err_t led_set_saved(uint8_t shader) {
  esp_err_t err;
  nvs_handle_t nvs_hndl;
  ESP_LOGI(TAG, "Retrieving Led configuration...");
  err = nvs_open("storage", NVS_READWRITE, &nvs_hndl);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    return err;
  }
  err = nvs_set_u8(nvs_hndl, "shader", shader);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) saving \"shader\"\n", esp_err_to_name(err));
    return err;
  }

  err = nvs_commit(nvs_hndl);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) saving Led conf!\n", esp_err_to_name(err));
    return err;
  }
  return ESP_OK;
}

esp_err_t led_get_saved(uint8_t *shader) {
  esp_err_t err;
  nvs_handle_t nvs_hndl;

  ESP_LOGI(TAG, "Retrieving Led configuration...");
  err = nvs_open("storage", NVS_READWRITE, &nvs_hndl);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    return err;
  }
  ESP_LOGI(TAG, "Getting shader from NVS...");

  err = nvs_get_u8(nvs_hndl, "shader", shader);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) getting \"shader\" param  handle!\n",
             esp_err_to_name(err));
    return ESP_FAIL;
  }

  nvs_close(nvs_hndl);
  return ESP_OK;
}

esp_err_t wifi_sta_set_saved(wifi_config_t *config) {
  esp_err_t err;
  nvs_handle_t nvs_hndl;
  ESP_LOGI(TAG, "Retrieving WiFi configuration...");
  err = nvs_open("storage", NVS_READWRITE, &nvs_hndl);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    return err;
  }
  err = nvs_set_str(nvs_hndl, "staessid", (char *)config->sta.ssid);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) saving \"stassid\"\n", esp_err_to_name(err));
    return err;
  }

  err = nvs_set_str(nvs_hndl, "stapassword", (char *)config->sta.password);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) saving \"stapassword\"\n", esp_err_to_name(err));
    return err;
  }

  err = nvs_commit(nvs_hndl);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) saving WiFi conf!\n", esp_err_to_name(err));
    return err;
  }
  return ESP_OK;
}

esp_err_t wifi_ap_set_saved(wifi_config_t *config) {
  esp_err_t err;
  nvs_handle_t nvs_hndl;
  ESP_LOGI(TAG, "Retrieving WiFi configuration...");
  err = nvs_open("storage", NVS_READWRITE, &nvs_hndl);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    return err;
  }
  err = nvs_set_str(nvs_hndl, "apessid", (char *)config->sta.ssid);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) saving \"apssid\"\n", esp_err_to_name(err));
    return err;
  }

  err = nvs_set_str(nvs_hndl, "appassword", (char *)config->sta.password);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) saving \"appassword\"\n", esp_err_to_name(err));
    return err;
  }

  err = nvs_commit(nvs_hndl);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) saving WiFi conf!\n", esp_err_to_name(err));
    return err;
  }
  return ESP_OK;
}

esp_err_t wifi_sta_get_saved(wifi_config_t *config) {
  esp_err_t err;
  char *essid;
  size_t essid_len;
  char *password;
  size_t password_len;

  nvs_handle_t nvs_hndl;
  ESP_LOGI(TAG, "Retrieving WiFi configuration...");
  err = nvs_open("storage", NVS_READWRITE, &nvs_hndl);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    return err;
  }
  ESP_LOGI(TAG, "Getting password from NVS...");
  err = nvs_get_str(nvs_hndl, "stapassword", NULL, &password_len);
  password = malloc(password_len);
  err = nvs_get_str(nvs_hndl, "stapassword", password, &password_len);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) getting \"password\" param  handle!\n",
             esp_err_to_name(err));
    return ESP_FAIL;
  }
  err = nvs_get_str(nvs_hndl, "staessid", NULL, &essid_len);
  essid = malloc(essid_len);
  err = nvs_get_str(nvs_hndl, "staessid", essid, &essid_len);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) getting \"essid\" param  handle!\n",
             esp_err_to_name(err));
    return ESP_FAIL;
  }

  nvs_close(nvs_hndl);
  strcpy((char *)config->sta.ssid, essid);
  strcpy((char *)config->sta.password, password);
  free(password);
  free(essid);
  return ESP_OK;
}

esp_err_t wifi_ap_get_saved(wifi_config_t *config) {
  esp_err_t err;
  char *essid;
  size_t essid_len;
  char *password;
  size_t password_len;

  nvs_handle_t nvs_hndl;
  ESP_LOGI(TAG, "Retrieving WiFi configuration...");
  err = nvs_open("storage", NVS_READWRITE, &nvs_hndl);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    return err;
  }
  ESP_LOGI(TAG, "Getting password from NVS...");
  err = nvs_get_str(nvs_hndl, "appassword", NULL, &password_len);
  password = malloc(password_len);
  err = nvs_get_str(nvs_hndl, "appassword", password, &password_len);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) getting \"password\" param  handle!\n",
             esp_err_to_name(err));
    return ESP_FAIL;
  }
  err = nvs_get_str(nvs_hndl, "apessid", NULL, &essid_len);
  essid = malloc(essid_len);
  err = nvs_get_str(nvs_hndl, "apessid", essid, &essid_len);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) getting \"essid\" param  handle!\n",
             esp_err_to_name(err));
    return ESP_FAIL;
  }

  nvs_close(nvs_hndl);
  strcpy((char *)config->ap.ssid, essid);
  strcpy((char *)config->ap.password, password);
  free(password);
  free(essid);
  return ESP_OK;
}
