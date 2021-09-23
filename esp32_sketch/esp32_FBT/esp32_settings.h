#ifndef ESP32_SETTINGS_H
#define ESP32_SETTINGS_H

#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#endif

// Main BNO08X I2C address (as reference clock, must be enabled)
const uint8_t  i2c_main_addr = 0x4B;

// Extended BNO08X I2C address (optional)
const uint8_t  i2c_extend_addr = 0x4A;

// Brownout detector
const bool brown_en = true;

// IMU reset pin
const uint8_t reset_pin = 25;

// LED indicator pin
const uint8_t led_pin = 19;

// Battery voltage monitoring pin
const uint8_t batt_monitor_pin = 35;

// Battery analog reading to voltage function
float get_battery_voltage(uint32_t milliVolt) {
  return 2 * milliVolt * 0.001;
}
