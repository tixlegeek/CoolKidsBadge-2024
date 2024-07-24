/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/
#include "../handlers.h"
static const char *TAG = "POST_LED";

esp_err_t staconf_post_handler(httpd_req_t *req)
{
    /* Destination buffer for content of HTTP POST request.
     * httpd_req_recv() accepts char* only, but content could
     * as well be any binary data (needs type casting).
     * In case of string data, null termination will be absent, and
     * content length would give length of string */
    wifi_config_t wifi_config;
    unsigned char essid[64] = {0};
    char b64_essid[64] = {0};
    size_t essid_len;

    unsigned char password[64] = {0};
    char b64_password[64] = {0};
    size_t password_len;

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
    err = httpd_query_key_value((char *)content, "essid", (char *)b64_essid, sizeof(b64_essid));
    if (err != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "C0uld n0t g3t ss1d :/");
        return err;
    }
    mbedtls_base64_decode(essid, sizeof(essid), &essid_len, (unsigned char *)b64_essid, strlen(b64_essid));
    err = httpd_query_key_value((char *)content, "password", (char *)b64_password, sizeof(b64_password));
    if (err != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "C0uld n0t g3t p4ssw0rd :/");
        return err;
    }
    mbedtls_base64_decode(password, sizeof(password), &password_len, (unsigned char *)b64_password,
                          strlen(b64_password));
    /* Send a simple response */

    wifi_config_t sta_config = {0};
    strcpy((char *)sta_config.sta.ssid, (char *)essid);
    strcpy((char *)sta_config.sta.password, (char *)password);
    if (strlen((char *)sta_config.sta.ssid) <= 1)
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "SS1d t00 sh0rt >:(");
        return ESP_FAIL;
    }
    if (strlen((char *)sta_config.sta.password) <= 6)
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "P4ssw0rd t00 sh0rt >:(");
        return ESP_FAIL;
    }
    err = wifi_sta_set_saved(&sta_config);
    if (err != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "W1f1 c0nf1gur4t1On n0t s3t :(");
        return err;
    }

    asprintf(&response, "W1f1 c0nf1gur4t1On s3t f0r \"%s\" s4v3d :D\n Reboot...\n", essid);
    // 205  Reset Content: Reload the page
    httpd_resp_set_status   (req, "205 Reset Content");
    httpd_resp_set_type     (req, HTTPD_TYPE_TEXT);
    httpd_resp_sendstr(req, response);
    free(response);

    vTaskDelay(500 / portTICK_PERIOD_MS);
    esp_restart();

    return ESP_OK;
}

esp_err_t apconf_post_handler(httpd_req_t *req)
{
    /* Destination buffer for content of HTTP POST request.
     * httpd_req_recv() accepts char* only, but content could
     * as well be any binary data (needs type casting).
     * In case of string data, null termination will be absent, and
     * content length would give length of string */
    wifi_config_t wifi_config;
    unsigned char essid[64] = {0};
    char b64_essid[64] = {0};
    size_t essid_len;

    unsigned char password[64] = {0};
    char b64_password[64] = {0};
    size_t password_len;

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
    err = httpd_query_key_value((char *)content, "essid", (char *)b64_essid, sizeof(b64_essid));
    if (err != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "C0uld n0t g3t ss1d :/");
        return err;
    }
    mbedtls_base64_decode(essid, sizeof(essid), &essid_len, (unsigned char *)b64_essid, strlen(b64_essid));
    err = httpd_query_key_value((char *)content, "password", (char *)b64_password, sizeof(b64_password));
    if (err != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "C0uld n0t g3t p4ssw0rd :/");
        return err;
    }
    mbedtls_base64_decode(password, sizeof(password), &password_len, (unsigned char *)b64_password,
                          strlen(b64_password));
    /* Send a simple response */
    wifi_config_t ap_config = {0};
    strcpy((char *)ap_config.ap.ssid, (char *)essid);
    strcpy((char *)ap_config.ap.password, (char *)password);
    if (strlen((char *)ap_config.ap.ssid) <= 1)
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "SS1d t00 sh0rt >:(");
        return ESP_FAIL;
    }
    if (strlen((char *)ap_config.ap.password) <= 1)
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "P4ssw0rd t00 sh0rt >:(");
        return ESP_FAIL;
    }
    err = wifi_ap_set_saved(&ap_config);
    if (err != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "W1f1 c0nf1gur4t1On n0t s3t :(");
        return err;
    }

    asprintf(&response, "W1f1 c0nf1gur4t1On s3t f0r \"%s\" s4v3d :D\n Reboot...\n", essid);
    httpd_resp_sendstr(req, response);
    free(response);

    vTaskDelay(500 / portTICK_PERIOD_MS);
    esp_restart();

    return ESP_OK;
}
