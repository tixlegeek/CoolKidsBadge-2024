/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/
#include "../handlers.h"
#include "../../led.h"
static const char *TAG = "POST_LED";
//rgb_led_ctx_t rgb_led_ctx;

esp_err_t ledconf_post_handler(httpd_req_t *req)
{
    /* Destination buffer for content of HTTP POST request.
     * httpd_req_recv() accepts char* only, but content could
     * as well be any binary data (needs type casting).
     * In case of string data, null termination will be absent, and
     * content length would give length of string */
    unsigned char shader[16] = {0};
    size_t shader_len;

    char content[128];
    esp_err_t err;
    char *response;

    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, sizeof(content));

    err = httpd_req_recv(req, content, recv_size);
    if (err <= 0)
    { /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (err == HTTPD_SOCK_ERR_TIMEOUT)
        {
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
    err = httpd_query_key_value((char *)content, "shader", (char *)shader, sizeof(shader));
    if (err != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "C0uld n0t g3t sh4d3r :/");
        return err;
    }

    char *endPtr;
    uint8_t shaderId = (uint8_t)strtoul((char*)shader, &endPtr, 10);

    /* Send a simple response */
    if(shaderId>=LED_CB_NUMBER){
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "L3d sh4d3r d03s n0t 3x1st :(");
      return ESP_FAIL;
    }

    err = led_set_saved(shaderId);
    if (err != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "L3d c0nf1gur4t1On n0t s3t :(");
        return err;
    }
    rgb_led_ctx.led_cb = led_cb_array[shaderId];
    asprintf(&response, "L3d c0nf1gur4t1On s3t :D\n Reboot...\n");
    httpd_resp_sendstr(req, response);
    free(response);

    vTaskDelay(500 / portTICK_PERIOD_MS);
    //esp_restart();

    return ESP_OK;
}
