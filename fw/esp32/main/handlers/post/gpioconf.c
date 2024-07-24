/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/
#include "../handlers.h"
#include "driver/gpio.h"

static const char *TAG = "POST_GPIO";

esp_err_t gpio_apply(uint8_t id, uint8_t mode, uint8_t state) {
  esp_err_t err = ESP_OK;
  // zero-initialize the config structure.
  gpio_config_t io_conf = {};
  // disable interrupt
  io_conf.intr_type = GPIO_INTR_DISABLE;
  // set as output mode
  io_conf.mode = (mode) ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT;
  // bit mask of the pins that you want to set,e.g.GPIO18/19
  io_conf.pin_bit_mask = 0x01 << id;
  // disable pull-down mode
  io_conf.pull_down_en = 0;
  // disable pull-up mode
  io_conf.pull_up_en = 0;
  // configure GPIO with the given settings
  err = gpio_config(&io_conf);
  if (err != ESP_OK) {
    return ESP_FAIL;
  }
  err = gpio_set_level((gpio_num_t)id, ((state) ? 1 : 0));
  if (err != ESP_OK) {
    return ESP_FAIL;
  }
  return ESP_OK;
}

esp_err_t gpioconf_post_handler(httpd_req_t *req) {
  /* Destination buffer for content of HTTP POST request.
   * httpd_req_recv() accepts char* only, but content could
   * as well be any binary data (needs type casting).
   * In case of string data, null termination will be absent, and
   * content length would give length of string */
  unsigned char jsonStr[35] = {0};
  size_t jsonStr_len;

  char content[128];
  esp_err_t err;
  char *response;

  /* Truncate if content length larger than the buffer */
  size_t recv_size = MIN(req->content_len, sizeof(content));

  err = httpd_req_recv(req, content, recv_size);
  if (err <= 0) { /* 0 return value indicates connection closed */
    /* Check if timeout occurred */
    if (err == HTTPD_SOCK_ERR_TIMEOUT) {
      /* In case of timeout one can choose to retry calling
       * httpd_req_recv(), but to keep it simple, here we
       * respond with an HTTP 408 (Request Timeout) error */
      httpd_resp_send_408(req);
    }
    /* In case of error, returning ESP_FAIL will
     * ensure that the underlying socket is closed */
    return ESP_FAIL;
  }
  // End the response, because it's not 0 terminated.
  content[recv_size] = 0;
  /* Retrieve the request from POST query  */
  err = httpd_query_key_value((char *)content, "data", (char *)jsonStr,
                              sizeof(jsonStr));
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
                        "C0uld n0t g3t sh4d3r :/");
    return err;
  }

  int status = 0;
  cJSON *data = cJSON_ParseWithLength((char *)jsonStr, 35);

  if (data == NULL) {
    cJSON_Delete(data);
    return ESP_ERR_INVALID_ARG;
  }

  const cJSON *idJson = cJSON_GetObjectItemCaseSensitive(data, "id");
  const cJSON *modeJson = cJSON_GetObjectItemCaseSensitive(data, "mode");
  const cJSON *stateJson = cJSON_GetObjectItemCaseSensitive(data, "state");

  if (!cJSON_IsNumber(idJson) || !cJSON_IsNumber(modeJson) ||
      !cJSON_IsNumber(stateJson)) {
    cJSON_Delete(data);
    return ESP_ERR_INVALID_ARG;
  }

  uint8_t id = (uint8_t)idJson->valueint;
  uint8_t mode = (uint8_t)modeJson->valueint;
  uint8_t state = (uint8_t)stateJson->valueint;
  ESP_ERROR_CHECK(gpio_apply(id, mode, state));

  asprintf(&response, "I got Set mode %d to GPIO %d; state %d\n", mode, id,
           state);
  httpd_resp_sendstr(req, response);
  cJSON_Delete(data);
  free(response);
  return ESP_OK;
}
