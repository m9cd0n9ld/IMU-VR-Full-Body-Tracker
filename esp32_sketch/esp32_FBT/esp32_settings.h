#ifndef ESP32_SETTINGS_H
#define ESP32_SETTINGS_H

#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#endif

// Brownout detector
bool brown_en = false;

// WiFi SSID
const char * networkName = "ssid";

// WiFi password
const char * networkPswd = "password";

// IMU reset pin
const uint8_t reset_pin = 25;

// LED indicator pin
const uint8_t led_pin = 2;

// Battery voltage monitoring pin
const uint8_t batt_monitor_pin = 36;

/* 
 WIFI_POWER_19_5dBm // 19.5dBm
 WIFI_POWER_19dBm // 19dBm
 WIFI_POWER_18_5dBm // 18.5dBm
 WIFI_POWER_17dBm // 17dBm
 WIFI_POWER_15dBm // 15dBm
 WIFI_POWER_13dBm // 13dBm
 WIFI_POWER_11dBm // 11dBm
 WIFI_POWER_8_5dBm // 8.5dBm
 WIFI_POWER_7dBm // 7dBm
 WIFI_POWER_5dBm // 5dBm
 WIFI_POWER_2dBm // 2dBm
 WIFI_POWER_MINUS_1dBm // -1dBm
*/
// WiFi TX power
wifi_power_t txPower = WIFI_POWER_5dBm;

// Battery analog reading to voltage function
float get_battery_voltage(uint16_t analogValue) {
  return 2 * analogValue * 3.9 / 4095;
}
