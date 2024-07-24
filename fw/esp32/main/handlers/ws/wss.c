/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/
#include "../handlers.h"
static const char *TAG = "WS_WSS";

/**
????
*/
esp_err_t ws_api_gpio(char *msg, void *ctx) {
  httpd_handle_t * server = (httpd_handle_t*)ctx;
  return ESP_OK;
}

esp_err_t ws_route_api(char *api, char *msg, void *ctx){
  httpd_handle_t * server = (httpd_handle_t*)ctx;
  if(strcmp(api, "gpio")==0){
    return ws_api_gpio(msg, server);
  }
  return ESP_FAIL;
}

/**
  ws_handler
  @brief Starts the WebSocket API Server
  @parms req: Request's structure
  @return esp_err_t
*/
esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));

    // First receive the full ws message
    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }

    if (ws_pkt.len)
    {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL)
        {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
    }
    // If it was a PONG, update the keep-alive
    if (ws_pkt.type == HTTPD_WS_TYPE_PONG)
    {
        ESP_LOGD(TAG, "Received PONG message");
        free(buf);
        return wss_keep_alive_client_is_active(httpd_get_global_user_ctx(req->handle), httpd_req_to_sockfd(req));
    }
    else if (ws_pkt.type == HTTPD_WS_TYPE_TEXT || ws_pkt.type == HTTPD_WS_TYPE_PING ||
             ws_pkt.type == HTTPD_WS_TYPE_CLOSE)
    {
        if (ws_pkt.type == HTTPD_WS_TYPE_TEXT)
        {
          cJSON *data = cJSON_ParseWithLength((char *)ws_pkt.payload, ws_pkt.len);

          if (data == NULL) {
            cJSON_Delete(data);
            free(buf);
            return ESP_ERR_INVALID_ARG;
          }

          const cJSON *apiJson = cJSON_GetObjectItemCaseSensitive(data, "api");
          const cJSON *msgJson = cJSON_GetObjectItemCaseSensitive(data, "msg");

          if (!cJSON_IsString(apiJson) || (!cJSON_IsString(msgJson) && !cJSON_IsObject(msgJson))) {
            cJSON_Delete(data);
            free(buf);
            return ESP_ERR_INVALID_ARG;
          }

          char* api = apiJson->valuestring;
          if(cJSON_IsString(msgJson)){
            // MSG IS TEXT
            char* msg = msgJson->valuestring;
            ret = ws_route_api(api, msg, &req->handle);
            if(ret != ESP_OK){
              cJSON_Delete(data);
              free(buf);
              return ret;
            }
          }
          else if (cJSON_IsObject(msgJson))
          {
            // MSG IS JSON
            ESP_LOGI(TAG, "cJSON_IsObject");
          }
          else
          {
            cJSON_Delete(data);
            free(buf);
            return ESP_ERR_INVALID_ARG;
          }
          cJSON_Delete(data);
          free(buf);
          return ESP_OK;
        }
        else if (ws_pkt.type == HTTPD_WS_TYPE_PING)
        {
            // Response PONG packet to peer
            ESP_LOGI(TAG, "Got a WS PING frame, Replying PONG");
            ws_pkt.type = HTTPD_WS_TYPE_PONG;
        }
        else if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE)
        {
            // Response CLOSE packet with no payload to peer
            ws_pkt.len = 0;
            ws_pkt.payload = NULL;
        }
        ret = httpd_ws_send_frame(req, &ws_pkt);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
        }
        ESP_LOGI(TAG, "ws_handler: httpd_handle_t=%p, sockfd=%d, client_info:%d", req->handle, httpd_req_to_sockfd(req),
                 httpd_ws_get_fd_info(req->handle, httpd_req_to_sockfd(req)));
        free(buf);
        return ret;
    }
    free(buf);
    return ESP_OK;
}

esp_err_t wss_open_fd(httpd_handle_t hd, int sockfd)
{
    ESP_LOGI(TAG, "New client connected %d", sockfd);
    wss_keep_alive_t h = httpd_get_global_user_ctx(hd);
    return wss_keep_alive_add_client(h, sockfd);
}

void wss_close_fd(httpd_handle_t hd, int sockfd)
{
    ESP_LOGI(TAG, "Client disconnected %d", sockfd);
    wss_keep_alive_t h = httpd_get_global_user_ctx(hd);
    wss_keep_alive_remove_client(h, sockfd);
    close(sockfd);
}
