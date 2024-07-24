/** -------------------------------------------------------
 *  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
 *  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
 *  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
 * Copyright© 2024 - Cyberpunk.Company - GPLv3
 * This program is free software
 * See LICENSE.txt - https://cyberpunk.company/tixlegeek
 * -------------------------------------------------------**/

#include "wifi.h"
#include "dnsd.h"
static const char *TAG = "WIFI";
esp_err_t configure_mac_wifi(esp_netif_t *ap_netif, esp_netif_t *sta_netif) {
  uint8_t mac_ap[6];
  uint8_t mac_sta[6];

  assert(ap_netif);
  esp_read_mac(mac_ap, ESP_MAC_WIFI_SOFTAP);
  mac_ap[0] = 0xFC;
  mac_ap[1] = 0xD4;
  mac_ap[2] = 0xF2;

  assert(sta_netif);
  esp_read_mac(mac_sta, ESP_MAC_WIFI_STA);
  mac_sta[0] = 0xFC;
  mac_sta[1] = 0xD4;
  mac_sta[2] = 0xF2;

  ESP_LOGI(TAG, "[*] Changing AP MAC to %02X:%02X:%02X:%02X:%02X:%02X",
           mac_ap[0], mac_ap[1], mac_ap[2], mac_ap[3], mac_ap[4], mac_ap[5]);
  ESP_ERROR_CHECK(esp_iface_mac_addr_set(mac_ap, ESP_MAC_WIFI_SOFTAP));
  ESP_ERROR_CHECK(esp_netif_set_mac(ap_netif, mac_ap));

  ESP_LOGI(TAG, "[*] Changing STA MAC to %02X:%02X:%02X:%02X:%02X:%02X",
           mac_sta[0], mac_sta[1], mac_sta[2], mac_sta[3], mac_sta[4],
           mac_sta[5]);
  ESP_ERROR_CHECK(esp_iface_mac_addr_set(mac_sta, ESP_MAC_WIFI_STA));
  ESP_ERROR_CHECK(esp_netif_set_mac(sta_netif, mac_sta));

  return ESP_OK;
}

void wifi_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data) {
  switch (event_id) {
  case WIFI_EVENT_AP_STACONNECTED:
    ESP_LOGI(TAG, "WIFI AP STA CONNECTED, STARTING SERVER.");
    connect_handler(&https_server);
    break;
  case WIFI_EVENT_AP_STADISCONNECTED:
    // connect_handler(&https_server);
    break;
  case WIFI_EVENT_STA_CONNECTED:
    initialize_sntp();

    break;
  case WIFI_EVENT_STA_DISCONNECTED:

    break;
  default:
    break;
  }
}
/**
  Converts an IPv4 into CIDR
*/
uint8_t IP2CIDR(uint32_t ip) {
  uint8_t i = 0;
  uint8_t cidr = 0;
  for (i = 0; i < 32; i++) {
    cidr += ((ip >> i) & 0x01);
  }
  return cidr;
}

void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                      void *event_data) {
  switch (event_id) {
  case IP_EVENT_STA_GOT_IP:
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    // ckb_info.ip_sta.ip = event->ip_info.ip;
    ckb_info.sta_connected = true;
    asprintf(&ckb_info.sta_ip, IPSTR, IP2STR(&event->ip_info.ip));
    asprintf(&ckb_info.sta_gw, IPSTR, IP2STR(&event->ip_info.gw));
    asprintf(&ckb_info.sta_netmask, IPSTR, IP2STR(&event->ip_info.netmask));
    ckb_info.sta_cidr = event->ip_info.netmask.addr;
    ESP_LOGI(TAG, "[!] got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    ESP_LOGI(TAG, "[+] Connect_handler...");
    connect_handler(&https_server);
    start_dns_server();
    break;
  case IP_EVENT_STA_LOST_IP:
    ckb_info.sta_connected = false;
    free(ckb_info.sta_ip);
    free(ckb_info.sta_gw);
    free(ckb_info.sta_netmask);
    ESP_LOGI(TAG, "[x] disconnect_handler...");
    disconnect_handler(&https_server);
    break;
  default:
    break;
  }
}

void wifi_get_ssid(char *ssid, size_t len) {
  uint8_t chipid[6];
  esp_read_mac(chipid, ESP_MAC_BASE);
  snprintf(ssid, len, "CoolKidsBadge%02X%02X%02X", chipid[3], chipid[4],
           chipid[5]);
  ESP_LOGI(TAG, "Getting ssid: %s", ssid);
}

char *wordz[] = {"Zort",  "Blerp", "Flib", "Wum",  "Jig",  "Splat",
                 "Ploof", "Gag",   "Fizz", "Whim", "Drip", "Zing",
                 "Flip",  "Snap",  "Crap", "Wiz",  "Fop",  "Pup",
                 "Zip",   "Zap",   "Bop",  "Pop",  "Fad",  "Vat",
                 "Jam",   "Gum",   "Hum",  "Wham", "Chum", "Prout"};
/**
  Creates a procedural password.
*/
void wifi_get_password(char *password, size_t len) {
  uint8_t chipid[6];
  esp_read_mac(chipid, ESP_MAC_BASE);
  uint8_t m1 = ((chipid[0] + chipid[3]) % 97);
  uint8_t m2 = ((chipid[1] + chipid[4]) % 97);
  uint8_t m3 = ((chipid[2] + chipid[5]) % 97);
  snprintf(password, len, "%s%02x%s%02x%s%02x", wordz[m1 % 29], m1,
           wordz[m2 % 29], m2, wordz[m3 % 29], m3);
  ESP_LOGI(TAG, "Getting password: %s", password);
}

void wifi_get_conf(char *ssid, size_t ssidlen, char *password,
                   size_t passwordlen) {
  wifi_ap_config_t ap_config = {0};
  if (wifi_ap_get_saved(&ap_config) == ESP_OK) {
    if (strlen((char *)ap_config.ssid) <= 5) {
      ESP_LOGE(TAG, "Wrong length for ssid");
      wifi_get_ssid((char *)ssid, ssidlen);
      wifi_get_password((char *)password, passwordlen);
      return;
    } else if (strlen((char *)ap_config.password) <= 8) {
      ESP_LOGE(TAG, "Wrong length for password");
      wifi_get_ssid((char *)ssid, ssidlen);
      wifi_get_password((char *)password, passwordlen);
      return;
    } else {
      snprintf((char *)ssid, ssidlen, (char *)ap_config.ssid);
      snprintf((char *)password, passwordlen, (char *)ap_config.password);
      return;
    }
  }
  ESP_LOGE(TAG, "Could not retrieve last AP configuration.");
  ESP_LOGE(TAG, "Fallback to default.");
  wifi_get_ssid((char *)ssid, ssidlen);
  wifi_get_password((char *)password, passwordlen);
}

wifi_ckb_mode_t wifi_apsta(int timeout_ms) {

  wifi_ap_config_t ap_config = {0};

  // Creates and set the AP iface's SSID and password
  /**/
  wifi_get_conf((char *)ap_config.ssid, 32, (char *)ap_config.password, 64);
  wifi_get_conf((char *)ckb_info.ap_ssid, 32, (char *)ap_config.password, 64);

  ap_config.authmode = WIFI_AUTH_WPA_WPA2_PSK;
  ap_config.ssid_len = strlen((char *)ap_config.ssid);
  ap_config.max_connection = CKB_AP_MAX_STA_CONN;
  ap_config.channel = CKB_AP_WIFI_CHANNEL;

  if (strlen(CKB_AP_PASSWORD) == 0) {
    ap_config.authmode = WIFI_AUTH_OPEN;
  }

  wifi_sta_config_t sta_config = {0};

  if (wifi_sta_get_saved(&sta_config) != ESP_OK) {
    ESP_LOGE(TAG, "[x] Could not retrieve last STA configuration... :( \n");
  }
  // If SSID's len is lesser than 6char long, it's not an SSID,
  // So the configuration failed.
  if (strlen((char *)sta_config.ssid) <= 5) {
    strcpy((char *)sta_config.ssid, (char *)"n0p3_f41l3d");
  }

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "[>] WIFI_MODE_AP started. SSID:%s password: ***  channel:%d",
           ap_config.ssid, CKB_AP_WIFI_CHANNEL);

  ESP_ERROR_CHECK(esp_wifi_connect());
  int bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, pdFALSE,
                                 pdTRUE, timeout_ms / portTICK_PERIOD_MS);

  return ((bits & CONNECTED_BIT) != 0) ? CKB_WIFI_MODE_APSTA : CKB_WIFI_MODE_AP;
}

esp_err_t wifi_init(void) {
  esp_log_level_set("wifi", ESP_LOG_ERROR);

  ESP_ERROR_CHECK(esp_netif_init());
  wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_event_loop_create_default());

  esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
  assert(sta_netif);
  // Makes fingerprinting a bit more fun
  ESP_LOGI(TAG, "[*] Changing STA hostname to %s", CKB_STA_HOSTNAME);
  ESP_ERROR_CHECK(esp_netif_set_hostname(sta_netif, CKB_STA_HOSTNAME));

  esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
  assert(ap_netif);
  // Makes fingerprinting a bit more fun
  ESP_LOGI(TAG, "[*] Changing AP hostname to %s", CKB_AP_HOSTNAME);
  ESP_ERROR_CHECK(esp_netif_set_hostname(ap_netif, CKB_AP_HOSTNAME));

  configure_mac_wifi(ap_netif, sta_netif);

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // AP mode client connected
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, &wifi_event_handler, NULL));
  // AP mode client disconnected
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, &wifi_event_handler, NULL));
  // Station mode connect
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &wifi_event_handler, NULL));
  // Station mode disconnect
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &wifi_event_handler, NULL));

  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                             &ip_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_LOST_IP,
                                             &ip_event_handler, NULL));

  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));

  ESP_ERROR_CHECK(esp_wifi_start());

  esp_netif_ip_info_t ap_ip;
  esp_netif_get_ip_info(ap_netif, &ap_ip);

  asprintf(&ckb_info.ap_ip, IPSTR, IP2STR(&ap_ip.ip));
  ckb_info.ap_cidr = ap_ip.netmask.addr;

  return ESP_OK;
}
