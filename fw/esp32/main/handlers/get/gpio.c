/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/
#include "driver/gpio.h"
#include "../../coolkids.h"
#include "../handlers.h"

static const char *TAG = "GET_GPIO";
#define CKB_GPIO_NUMBER 10

esp_err_t gpio_get_init() {
  gpio_config_t io_conf = {};

  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;

  gpio_num_t gpio;
  for(gpio = 1;gpio <=CKB_GPIO_NUMBER;gpio ++){
    io_conf.pin_bit_mask = (1ULL<<gpio);
    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK) {
      return ESP_FAIL;
    }
    err = gpio_set_level(gpio, 0);
    if (err != ESP_OK) {
      return ESP_FAIL;
    }
  }

  return ESP_OK;
}

esp_err_t gpio_get_handler(httpd_req_t *req) {
  esp_err_t err = ESP_OK;
  ESP_LOGI(TAG, "GET GPIO FILE");
  httpd_resp_set_type(req, "text/html");
  err = gpio_get_init();
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
                        "1 br0k3 a l3g :'(");
    return err;
  }
  httpd_resp_sendstr(req, (const char *)gpio_html_start);
  return ESP_OK;
}

/**
  gpiocb_get_handler
  @brief DUMMY
  @parms req: HTTP Request
  @return esp_err_t
*/
esp_err_t gpiocb_get_handler(httpd_req_t *req) {
  cJSON *root = cJSON_CreateObject();
  char *jsonResponse;

  jsonResponse = cJSON_Print(root);
  cJSON_Delete(root);

  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req, jsonResponse);

  free(jsonResponse);
  return ESP_OK;
}

/**
  send_gpio
  @brief WebSocket GPIO status
  @parms arg ->async_resp_arg (ws server)
  @return esp_err_t
*/
void send_gpio(void *arg) {
  struct async_resp_arg *resp_arg = arg;
  httpd_handle_t hd = resp_arg->hd;
  int fd = resp_arg->fd;
  httpd_ws_frame_t ws_pkt;
  memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));

  cJSON *root = cJSON_CreateObject();
  cJSON *gpioArray = cJSON_CreateArray();
  char *jsonResponse;
  gpio_num_t gpio;

  for(gpio = 1; gpio <= CKB_GPIO_NUMBER; gpio++){

    cJSON *gpioJson = cJSON_CreateObject();
    cJSON_AddNumberToObject(gpioJson, "id", (int)gpio);
    cJSON_AddNumberToObject(gpioJson, "state", gpio_get_level(gpio));
    cJSON_AddItemToArray(gpioArray, gpioJson);
  }
  cJSON_AddItemToObject(root, "states", gpioArray);

  jsonResponse = cJSON_Print(root);
  cJSON_Delete(root);
  ws_pkt.payload = (uint8_t *)jsonResponse;
  ws_pkt.len = strlen(jsonResponse);
  ws_pkt.type = HTTPD_WS_TYPE_TEXT;

  httpd_ws_send_frame_async(hd, fd, &ws_pkt);
  free(jsonResponse);
  free(resp_arg);
}

/**
  gpio_web_task
  @brief WebSocket GPIO task
  @parms ctx ->httpd_handle_t (Ws server handle)
  @return esp_err_t
*/
void gpio_web_task(void *ctx) {
  httpd_handle_t *server = (httpd_handle_t *)ctx;
  esp_err_t err = ESP_OK;
  int len = 0;
  while (err == ESP_OK) {
    if (!*server) { // httpd might not have been created by now
      continue;
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
    size_t clients = max_clients;
    int client_fds[max_clients];
    if (httpd_get_client_list(*server, &clients, client_fds) == ESP_OK) {
      for (size_t i = 0; i < clients; ++i) {
        int sock = client_fds[i];
        if (httpd_ws_get_fd_info(*server, sock) == HTTPD_WS_CLIENT_WEBSOCKET) {
          ESP_LOGI(TAG, "Active client (fd=%d) -> sending async message", sock);
          struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
          resp_arg->hd = *server;
          resp_arg->fd = sock;
          err = httpd_queue_work(resp_arg->hd, send_gpio, resp_arg);
          if (err != ESP_OK) {
            ESP_LOGE(TAG, "httpd_queue_work failed!");
            break;
          }
        }
      }
    } else {
      ESP_LOGE(TAG, "httpd_get_client_list failed!");
      vTaskDelete(NULL);
    }
  }
  ESP_LOGE(TAG,"EXITING GPIO TASK");
  vTaskDelete(NULL);
}
