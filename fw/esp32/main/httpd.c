/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/

#include "httpd.h"
static const char *TAG = "HTTPD";
#if !CONFIG_HTTPD_WS_SUPPORT
#error This example cannot be used unless HTTPD_WS_SUPPORT is enabled in esp-http-server component configuration
#endif

/*
 * HTTP Server endpoints and callbacks
 */
httpd_uri_t index_get = {.uri = "/",
                         .method = HTTP_GET,
                         .handler = index_get_handler,
                         .user_ctx = NULL};
httpd_uri_t wifi_get = {.uri = "/wifi",
                        .method = HTTP_GET,
                        .handler = wifi_get_handler,
                        .user_ctx = NULL};
httpd_uri_t led_get = {.uri = "/led",
                       .method = HTTP_GET,
                       .handler = led_get_handler,
                       .user_ctx = NULL};
httpd_uri_t ledcb_get = {.uri = "/ledcb",
                         .method = HTTP_GET,
                         .handler = ledcb_get_handler,
                         .user_ctx = NULL};

httpd_uri_t gpio_get = {.uri = "/gpio",
                        .method = HTTP_GET,
                        .handler = gpio_get_handler,
                        .user_ctx = NULL};

httpd_uri_t gpioconf_post = {.uri = "/gpioconf",
                             .method = HTTP_POST,
                             .handler = gpioconf_post_handler,
                             .user_ctx = NULL};

httpd_uri_t i2c_get = {.uri = "/i2c",
                       .method = HTTP_GET,
                       .handler = i2c_get_handler,
                       .user_ctx = NULL};
httpd_uri_t i2cscan_get = {.uri = "/i2cscan",
                           .method = HTTP_GET,
                           .handler = i2cscan_get_handler,
                           .user_ctx = NULL};
httpd_uri_t i2cread_get = {.uri = "/i2cread",
                           .method = HTTP_GET,
                           .handler = i2cread_get_handler,
                           .user_ctx = NULL};
httpd_uri_t i2cwrite_get = {.uri = "/i2cwrite",
                            .method = HTTP_GET,
                            .handler = i2cwrite_get_handler,
                            .user_ctx = NULL};
httpd_uri_t landing_get = {.uri = "/landing",
                           .method = HTTP_GET,
                           .handler = landing_get_handler,
                           .user_ctx = NULL};
httpd_uri_t css_get = {.uri = "/css.css",
                       .method = HTTP_GET,
                       .handler = css_get_handler,
                       .user_ctx = NULL};
httpd_uri_t js_get = {.uri = "/js.js",
                      .method = HTTP_GET,
                      .handler = js_get_handler,
                      .user_ctx = NULL};
httpd_uri_t uuid_get = {.uri = "/uuid",
                        .method = HTTP_GET,
                        .handler = uuid_get_handler,
                        .user_ctx = NULL};
httpd_uri_t update_post = {.uri = "/update",
                           .method = HTTP_POST,
                           .handler = update_post_handler,
                           .user_ctx = NULL};
httpd_uri_t ledconf_post = {.uri = "/ledconf",
                            .method = HTTP_POST,
                            .handler = ledconf_post_handler,
                            .user_ctx = NULL};
httpd_uri_t staconf_post = {.uri = "/staconf",
                            .method = HTTP_POST,
                            .handler = staconf_post_handler,
                            .user_ctx = NULL};
httpd_uri_t apconf_post = {.uri = "/apconf",
                           .method = HTTP_POST,
                           .handler = apconf_post_handler,
                           .user_ctx = NULL};
httpd_uri_t scan_get = {.uri = "/scan",
                        .method = HTTP_GET,
                        .handler = scan_get_handler,
                        .user_ctx = NULL};

httpd_uri_t ws = {.uri = "/ws",
                  .method = HTTP_GET,
                  .handler = ws_handler,
                  .user_ctx = NULL,
                  .is_websocket = true,
                  .handle_ws_control_frames = true};

/**
send_hello
@brief
@parms
@return
*/
void send_hello(void *arg) {
  char *data = get_current_date();
  struct async_resp_arg *resp_arg = arg;
  httpd_handle_t hd = resp_arg->hd;
  int fd = resp_arg->fd;
  httpd_ws_frame_t ws_pkt;
  memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
  ws_pkt.payload = (uint8_t *)data;
  ws_pkt.len = strlen(data);
  ws_pkt.type = HTTPD_WS_TYPE_TEXT;

  httpd_ws_send_frame_async(hd, fd, &ws_pkt);
  free(resp_arg);
}

/**
send_ping
@brief
@parms
@return
*/
void send_ping(void *arg) {
  struct async_resp_arg *resp_arg = arg;
  httpd_handle_t hd = resp_arg->hd;
  int fd = resp_arg->fd;
  httpd_ws_frame_t ws_pkt;
  memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
  ws_pkt.payload = NULL;
  ws_pkt.len = 0;
  ws_pkt.type = HTTPD_WS_TYPE_PING;

  httpd_ws_send_frame_async(hd, fd, &ws_pkt);
  free(resp_arg);
}

/**
client_not_alive_cb
@brief
@parms
@return
*/
bool client_not_alive_cb(wss_keep_alive_t h, int fd) {
  httpd_sess_trigger_close(wss_keep_alive_get_user_ctx(h), fd);
  return true;
}

/**
check_client_alive_cb
@brief
@parms
@return
*/
bool check_client_alive_cb(wss_keep_alive_t h, int fd) {
  struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
  resp_arg->hd = wss_keep_alive_get_user_ctx(h);
  resp_arg->fd = fd;

  if (httpd_queue_work(resp_arg->hd, send_ping, resp_arg) == ESP_OK) {
    return true;
  }
  return false;
}

/**
http_server_init
@brief Sets up the HTTPD with endpoints
@parms
@return httpd_handle_t
*/
httpd_handle_t http_server_init(void) {
  // Prepare keep-alive engine
  wss_keep_alive_config_t keep_alive_config = KEEP_ALIVE_CONFIG_DEFAULT();
  keep_alive_config.max_clients = max_clients;
  keep_alive_config.client_not_alive_cb = client_not_alive_cb;
  keep_alive_config.check_client_alive_cb = check_client_alive_cb;
  wss_keep_alive_t keep_alive = wss_keep_alive_start(&keep_alive_config);

  // Start the httpd server
  httpd_handle_t server = NULL;
  httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();
  conf.httpd.max_open_sockets = max_clients;
  conf.httpd.global_user_ctx = keep_alive;
  conf.httpd.open_fn = wss_open_fd;
  conf.httpd.close_fn = wss_close_fd;
  conf.httpd.max_uri_handlers = 20;

  extern const unsigned char servercert_start[] asm(
      "_binary_servercert_pem_start");
  extern const unsigned char servercert_end[] asm("_binary_servercert_pem_end");
  conf.servercert = servercert_start;
  conf.servercert_len = servercert_end - servercert_start;

  extern const unsigned char prvtkey_pem_start[] asm(
      "_binary_prvtkey_pem_start");
  extern const unsigned char prvtkey_pem_end[] asm("_binary_prvtkey_pem_end");
  conf.prvtkey_pem = prvtkey_pem_start;
  conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

  esp_err_t ret = httpd_ssl_start(&server, &conf);
  if (ESP_OK != ret) {
    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
  }

  // httpd_register_uri_handler(server, &ws);
  httpd_register_uri_handler(server, &index_get);
  httpd_register_uri_handler(server, &wifi_get);
  httpd_register_uri_handler(server, &led_get);
  httpd_register_uri_handler(server, &ledcb_get);
  httpd_register_uri_handler(server, &gpio_get);
  httpd_register_uri_handler(server, &i2c_get);
  httpd_register_uri_handler(server, &i2cscan_get);
  httpd_register_uri_handler(server, &i2cread_get);
  httpd_register_uri_handler(server, &i2cwrite_get);
  httpd_register_uri_handler(server, &landing_get);
  httpd_register_uri_handler(server, &css_get);
  httpd_register_uri_handler(server, &js_get);
  httpd_register_uri_handler(server, &uuid_get);
  httpd_register_uri_handler(server, &update_post);
  httpd_register_uri_handler(server, &ledconf_post);
  httpd_register_uri_handler(server, &gpioconf_post);
  httpd_register_uri_handler(server, &staconf_post);
  httpd_register_uri_handler(server, &apconf_post);
  httpd_register_uri_handler(server, &scan_get);
  httpd_register_uri_handler(server, &ws);

  wss_keep_alive_set_user_ctx(keep_alive, server);

  return server;
}

/**
http_server_deinit
@brief
@parms
@return
*/
esp_err_t http_server_deinit(httpd_handle_t server) {
  // Stop keep alive thread
  wss_keep_alive_stop(httpd_get_global_user_ctx(server));
  // Stop the httpd server
  return httpd_ssl_stop(server);
}

/**
disconnect_handler
@brief
@parms
@return
*/
void disconnect_handler(httpd_handle_t *server) {
  ESP_LOGE(TAG, "Disconnect handler");

  if (*server != NULL) {
    if (http_server_deinit(*server) == ESP_OK) {
      *server = NULL;
    }
  } else {
    ESP_LOGE(TAG, "No HTTPD server to connect");
  }
}

/**
connect_handler
@brief
@parms
@return
*/
void connect_handler(httpd_handle_t *server) {
  if (*server == NULL) {
    *server = http_server_init();
  }
}

// Get all clients and send async message
/**
wss_server_send_messages
@brief
@parms
@return
*/
void wss_server_send_messages(httpd_handle_t *server) {
  bool send_messages = true;

  // Send async message to all connected clients that use websocket protocol
  // every 10 seconds
  while (send_messages) {
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    if (!*server) { // httpd might not have been created by now
      continue;
    }
    size_t clients = max_clients;
    int client_fds[max_clients];
    if (httpd_get_client_list(*server, &clients, client_fds) == ESP_OK) {
      for (size_t i = 0; i < clients; ++i) {
        int sock = client_fds[i];
        if (httpd_ws_get_fd_info(*server, sock) == HTTPD_WS_CLIENT_WEBSOCKET) {
          ESP_LOGI(TAG, "[-] Active client (fd=%d) -> sending async message",
                   sock);
          struct async_resp_arg *resp_arg =
              malloc(sizeof(struct async_resp_arg));
          resp_arg->hd = *server;
          resp_arg->fd = sock;
          if (httpd_queue_work(resp_arg->hd, send_hello, resp_arg) != ESP_OK) {
            ESP_LOGE(TAG, "[-] httpd_queue_work failed!");
            send_messages = false;
            break;
          }
        }
      }
    } else {
      ESP_LOGE(TAG, "[-] httpd_get_client_list failed!");
      return;
    }
  }
}
