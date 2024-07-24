/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/

#ifndef _LED_H_
#define _LED_H_

#include "board.h"
#include "nvs_db.h"
#include <driver/rmt_tx.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <led_strip_encoder.h>

typedef void (*led_cb_t)(uint16_t t, uint8_t ledindex, void *rgb_led_ctx);

typedef struct {
  rmt_channel_handle_t led_chan;
  rmt_tx_channel_config_t tx_chan_config;
  rmt_transmit_config_t tx_config;
  rmt_encoder_handle_t led_encoder;
  uint8_t led_array[BOARD_LED_NUMBERS * 3];
  led_cb_t led_cb;
} rgb_led_ctx_t;

// Code labels
typedef enum {
  ledCbRainbow,
  ledCbGreen,
  ledCbSparkle,
  ledCbSecret,
  ledCbSpin2Win,
  ledCbFire,
  LED_CB_NUMBER
} led_cb_index;

// Text labels
extern const char *const led_cb_name[];

void led_init(rgb_led_ctx_t *rgb_led_ctx);
void led_commit(rgb_led_ctx_t *rgb_led_ctx, uint8_t *led_strip_pixels);
void led_task(void *pvParameter);
void led_update(rgb_led_ctx_t *rgb_led_ctx);
void led_rainbow_cb(uint16_t t, uint8_t ledindex, void *ctx);
void led_sparkle_cb(uint16_t t, uint8_t ledindex, void *ctx);
void led_s3cre3t_cb(uint16_t t, uint8_t ledindex, void *ctx);
void led_sp1n2W1n_cb(uint16_t t, uint8_t ledindex, void *ctx);
void led_fire_cb(uint16_t t, uint8_t ledindex, void *ctx);
void led_green_cb(uint16_t t, uint8_t ledindex, void *ctx);
void led_shader(uint16_t t, rgb_led_ctx_t *rgb_led_ctx);
void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r,
                       uint32_t *g, uint32_t *b);

extern rgb_led_ctx_t rgb_led_ctx;
extern void *led_cb_array[];
#endif /* end of include guard: _LED_H_ */
