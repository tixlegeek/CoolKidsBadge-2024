#include "sntp.h"

void initialize_sntp(void) {
  esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
  esp_sntp_setservername(0, "pool.ntp.org"); // Use the NTP pool project server
  esp_sntp_init();
}

void deinitialize_sntp(void) {
  if (esp_sntp_enabled()) {
    esp_sntp_stop();
  }
}

char *get_current_date() {
  static char datetime_str[36]; // Buffer to hold the date string. Adjust size
                                // if format changes.
  time_t now;
  struct tm timeinfo;

  // Get current time
  time(&now);
  // Convert to local time
  localtime_r(&now, &timeinfo);

  // Format the date and time, and store it in the buffer
  snprintf(
      datetime_str, sizeof(datetime_str), "%04d-%02d-%02d %02d:%02d:%02d",
      timeinfo.tm_year + 1900, // Year
      timeinfo.tm_mon + 1,     // Month, range 0 to 11
      timeinfo.tm_mday,        // Day of the month
      timeinfo.tm_hour,        // Hours since midnight, 0-23
      timeinfo.tm_min,         // Minutes after the hour, 0-59
      timeinfo.tm_sec); // Seconds after the minute, 0-60 (60 for leap second)
  return datetime_str;  // Return the buffer containing date and time
}
