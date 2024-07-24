/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/
#include "../../config.h"
#include "../handlers.h"
#include "board.h"
#include "driver/i2c_master.h"
#include "protocol_examples_utils.h"

static const char *TAG = "GET_I2C";

esp_err_t i2c_get_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "GET I2C FILE");
  httpd_resp_set_type(req, "text/html");
  httpd_resp_sendstr(req, (const char *)i2c_html_start);
  return ESP_OK;
}

/**
  i2cscan_get_handler
  @brief I2C API (scan)
  @parms req: HTTP Request
  @return esp_err_t
*/
esp_err_t i2cscan_get_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "GET I2C SCAN API");
  esp_err_t err = ESP_OK;
  cJSON *root = cJSON_CreateObject();
  cJSON *devices = cJSON_CreateArray();
  cJSON *addr = NULL;

  if (!root)
    return ESP_FAIL;
  char *jsonResponse;
  i2c_master_bus_handle_t i2c_handle;
  i2c_master_bus_config_t i2c_bus_config = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .i2c_port = I2C_MASTER_NUM,
      .sda_io_num = BOARD_I2C_SDA,
      .scl_io_num = BOARD_I2C_SCL,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true,
  };
  err = i2c_new_master_bus(&i2c_bus_config, &i2c_handle);
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "I sl1pp3d.");
    return err;
  }

  for (uint8_t i = 0; i < 127; i++) {

    esp_err_t ret = i2c_master_probe(i2c_handle, i, I2C_MASTER_TIMEOUT_MS);
    if (ret == ESP_OK) {
      addr = cJSON_CreateNumber((int)i);
      cJSON_AddItemToArray(devices, addr);
    }
  }
  err = i2c_del_master_bus(i2c_handle);
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "I sl1pp3d.");
    return err;
  }
  cJSON_AddItemToObject(root, "devices", devices);
  jsonResponse = cJSON_Print(root);
  cJSON_Delete(root);
  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req, jsonResponse);
  free(jsonResponse);
  return ESP_OK;
}

/**
  i2cwrite_get_handler
  @brief I2C API (write)
  @parms req: HTTP Request
  @return esp_err_t
*/
esp_err_t i2cwrite_get_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "GET I2C WRITE API");
  esp_err_t err = ESP_OK;

  char *buf;
  char *endptr;
  size_t buf_len = httpd_req_get_url_query_len(req) + 1;
  uint8_t reg = 0;
  uint8_t addr = 0;
  uint8_t byte = 0;

  if (buf_len > 1) {
    buf = malloc(buf_len);
    err = httpd_req_get_url_query_str(req, buf, buf_len);
    if (err != ESP_OK) {
      ESP_LOGI(TAG, "Could not retrieve query string.");
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "I sl1pp3d.");
      return err;
    }
  } else {
    ESP_LOGI(TAG, "Could not retrieve any GET parameter.");
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "B4d 4rgum3ntz :'(");
    return ESP_FAIL;
  }

  char param[CKB_HTTP_QUERY_KEY_MAX_LEN],
      dec_param[CKB_HTTP_QUERY_KEY_MAX_LEN] = {0};

  // Get REG parameter
  err = httpd_query_key_value(buf, "reg", param, sizeof(param));
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "n33d \"reg\" address");
    return err;
  }
  // Convert to int
  example_uri_decode(dec_param, param,
                     strnlen(param, CKB_HTTP_QUERY_KEY_MAX_LEN));
  reg = (uint8_t)strtoul(dec_param, &endptr, 16);
  if (endptr == dec_param) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "B4d f0rm4t");
    return err;
  }

  // Get REG parameter
  err = httpd_query_key_value(buf, "addr", param, sizeof(param));
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "n33d \"addr\"");
    return err;
  }
  // Convert to int
  example_uri_decode(dec_param, param,
                     strnlen(param, CKB_HTTP_QUERY_KEY_MAX_LEN));
  addr = (uint8_t)strtoul(dec_param, &endptr, 16);
  if (endptr == dec_param) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "B4d f0rm4t");
    return err;
  }

  // Get REG parameter
  err = httpd_query_key_value(buf, "byte", param, sizeof(param));
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "n33d \"byte\" address");
    return err;
  }
  // Convert to int
  example_uri_decode(dec_param, param,
                     strnlen(param, CKB_HTTP_QUERY_KEY_MAX_LEN));
  byte = (uint8_t)strtoul(dec_param, &endptr, 16);
  if (endptr == dec_param) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "B4d f0rm4t");
    return err;
  }

  i2c_master_bus_handle_t i2c_handle;
  i2c_master_bus_config_t i2c_bus_config = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .i2c_port = I2C_MASTER_NUM,
      .sda_io_num = BOARD_I2C_SDA,
      .scl_io_num = BOARD_I2C_SCL,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true,
  };
  err = i2c_new_master_bus(&i2c_bus_config, &i2c_handle);
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "I sl1pp3d.");
    return err;
  }

  err = i2c_del_master_bus(i2c_handle);
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "I sl1pp3d.");
    return err;
  }

  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req, "[]");
  return ESP_OK;
}

/**
  i2cread_get_handler
  @brief I2C API (read)
  @parms req: HTTP Request
  @return esp_err_t
*/
esp_err_t i2cread_get_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "GET I2C READ API");
  esp_err_t err = ESP_OK;

  char *buf;
  char *endptr;
  size_t buf_len = httpd_req_get_url_query_len(req) + 1;
  uint8_t reg = 0;
  uint8_t addr = 0;
  uint16_t len = 0;

  if (buf_len > 1) {
    buf = malloc(buf_len);
    err = httpd_req_get_url_query_str(req, buf, buf_len);
    if (err != ESP_OK) {
      ESP_LOGI(TAG, "Could not retrieve query string.");
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "I sl1pp3d.");
      return err;
    }
  } else {
    ESP_LOGI(TAG, "Could not retrieve any GET parameter.");
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "B4d 4rgum3ntz :'(");
    return ESP_FAIL;
  }

  char param[CKB_HTTP_QUERY_KEY_MAX_LEN],
      dec_param[CKB_HTTP_QUERY_KEY_MAX_LEN] = {0};

  // Get REG parameter
  err = httpd_query_key_value(buf, "reg", param, sizeof(param));
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "n33d \"reg\" address");
    return err;
  }
  // Convert to int
  example_uri_decode(dec_param, param,
                     strnlen(param, CKB_HTTP_QUERY_KEY_MAX_LEN));
  reg = (uint8_t)strtoul(dec_param, &endptr, 16);
  if (endptr == dec_param) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "B4d f0rm4t");
    return err;
  }

  // Get ADDR parameter
  err = httpd_query_key_value(buf, "addr", param, sizeof(param));
  if (err != ESP_OK) {
    ESP_LOGI(TAG, "Could not retrieve any GET parameter.");
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "n33d \"addr\" address");
    return err;
  }

  // Convert to int
  example_uri_decode(dec_param, param,
                     strnlen(param, CKB_HTTP_QUERY_KEY_MAX_LEN));
  addr = (uint8_t)strtoul(dec_param, &endptr, 16);

  if (endptr == dec_param) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "B4d f0rm4t");
    return err;
  }

  // Get LEN parameter
  err = httpd_query_key_value(buf, "len", param, sizeof(param));
  if (err == ESP_OK) {
    // Convert to int
    example_uri_decode(dec_param, param,
                       strnlen(param, CKB_HTTP_QUERY_KEY_MAX_LEN));
    len = (uint16_t)strtoul(dec_param, &endptr, 10);
    if (endptr == dec_param) {
      httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "B4d f0rm4t");
      return err;
    }
  } else {
    len = 1;
  }

  if (len == 0) {
    len = 1;
  } else if (len > I2C_MAX_REG_READ) {
    len = I2C_MAX_REG_READ;
  }
  free(buf);

  ESP_LOGI(TAG, "Get Params: REG = %02x ADDR = %02x, LEN = %d", reg, addr, len);
  i2c_master_bus_handle_t i2c_handle;
  i2c_master_bus_config_t i2c_bus_config = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .i2c_port = I2C_MASTER_NUM,
      .sda_io_num = BOARD_I2C_SDA,
      .scl_io_num = BOARD_I2C_SCL,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true,
  };

  if (i2c_new_master_bus(&i2c_bus_config, &i2c_handle) != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "I sl1pp3d.");
    return err;
  }

  i2c_device_config_t i2c_dev_conf = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = addr,
      .scl_speed_hz = 400000,
  };

  i2c_master_dev_handle_t i2c_dev;

  err = i2c_master_bus_add_device(i2c_handle, &i2c_dev_conf, &i2c_dev);
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "i2c Br0k3n");
    return err;
  }

  char *jsonResponse;
  cJSON *root = cJSON_CreateObject();
  cJSON *data = cJSON_CreateArray();
  cJSON *seg = NULL;
  uint8_t *segbuf = calloc(len, sizeof(uint8_t));

  if (segbuf == NULL) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "0ut 0f M3m0rY");
    return err;
  }

  cJSON_AddNumberToObject(root, "addr", (int)addr);
  cJSON_AddNumberToObject(root, "reg", (int)reg);
  cJSON_AddNumberToObject(root, "len", (int)len);

  err = i2c_master_transmit_receive(i2c_dev, &reg, 1, segbuf, len,
                                    I2C_MASTER_TIMEOUT_MS);
  if (err == ESP_OK) {
    for (uint16_t byte = 0; byte < len; byte++) {
      seg = cJSON_CreateNumber((int)segbuf[byte]);
      cJSON_AddItemToArray(data, seg);
    }
  }

  free(segbuf);
  ESP_LOGE(TAG, "I2C MASTER BUS RM DEVICE");
  if (i2c_master_bus_rm_device(i2c_dev) != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "i2c Br0k3n");
    return err;
  }
  err = i2c_del_master_bus(i2c_handle);
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "I sl1pp3d.");
    return err;
  }
  cJSON_AddItemToObject(root, "data", data);

  jsonResponse = cJSON_Print(root);
  cJSON_Delete(root);

  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req, jsonResponse);
  free(jsonResponse);
  return ESP_OK;
}
