#ifndef ESP32_SETTINGS_H
#define ESP32_SETTINGS_H

#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#endif

// Main BNO08X I2C address (as reference clock, must be enabled)
uint8_t  i2c_main_addr = 0x4B;

// Extended BNO08X I2C address (optional)
uint8_t  i2c_extend_addr = 0x4A;

// Brownout detector
bool brown_en = true;

// IMU reset pin
const uint8_t reset_pin = 25;

// LED indicator pin
const uint8_t led_pin = 19;

// Battery voltage monitoring pin
const uint8_t batt_monitor_pin = 35;

// WiFi power saving mode
bool wifi_power_save = true;

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
float get_battery_voltage(uint32_t milliVolt) {
  return 2 * milliVolt * 0.001;
}
