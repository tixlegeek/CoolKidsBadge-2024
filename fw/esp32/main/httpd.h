/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/

#ifndef _HTTPD_H_
#define _HTTPD_H_

#include "handlers/handlers.h"
#include "sntp.h"
#include <esp_err.h>
#include <esp_http_server.h>
#include <esp_https_server.h>
#include <stdbool.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <unistd.h> // Include for close()

extern httpd_handle_t https_server;

void send_hello(void *arg);
void send_ping(void *arg);
bool client_not_alive_cb(wss_keep_alive_t h, int fd);
bool check_client_alive_cb(wss_keep_alive_t h, int fd);
void wss_server_send_messages(httpd_handle_t *server);

void connect_handler(httpd_handle_t *server);
httpd_handle_t http_server_init(void);
void disconnect_handler(httpd_handle_t *server);
esp_err_t http_server_deinit(httpd_handle_t server);

#endif /* end of include guard: _HTTPD_H_ */
