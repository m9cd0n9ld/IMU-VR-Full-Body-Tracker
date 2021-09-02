#include <WiFi.h>
#include <WiFiUdp.h>
#include <AsyncUDP.h>
#include "esp32_settings.h"
#include <Wire.h>
#include "SparkFun_BNO080_Arduino_Library.h"

#define DEBUG false
#define DEBUG_PRINT if(DEBUG)Serial

char header[128];
char networkSSID[128];
char networkPassword[128];
int serialLength;

IPAddress udpAddress(0, 0, 0, 0);
const int serverPort = 6969;
int driverPort = 0;

boolean connected = false;

WiFiUDP udp;
AsyncUDP Audp;

struct __attribute__((packed)) PingBroad {
  uint8_t header = (uint8_t)'I';
  uint8_t id = 77;
  uint8_t footer = (uint8_t)'i';
} pingbroad;

struct __attribute__((packed)) Ping {
  uint8_t header = (uint8_t)'I';
  uint8_t id = 0;
  uint8_t id_ext = 0;
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t f;
  float batt = 0;
  bool extend = false;
  uint8_t footer = (uint8_t)'i';
} ping;

struct __attribute__((packed)) Ack {
  uint8_t header;
  uint8_t reply;
  uint8_t id;
  uint8_t id_ext;
  uint16_t driverPort;
  uint8_t footer;
} ack;

struct __attribute__((packed)) PayloadExt {
  uint8_t header = (uint8_t)'I';
  uint8_t id = 0;
  int16_t x = 0;
  int16_t y = 0;
  int16_t z = 0;
  int16_t w = 32767;
  uint8_t id_ext = 0;
  int16_t x_ext = 0;
  int16_t y_ext = 0;
  int16_t z_ext = 0;
  int16_t w_ext = 32767;
  uint8_t footer = (uint8_t)'i';
} payloadext;

struct __attribute__((packed)) Payload {
  uint8_t header = (uint8_t)'I';
  uint8_t id = 0;
  int16_t x = 0;
  int16_t y = 0;
  int16_t z = 0;
  int16_t w = 32767;
  uint8_t footer = (uint8_t)'i';
} payload;

uint32_t last_broadcast;
uint32_t last_ping;
uint32_t last_main_imu_check;
uint32_t last_extend_imu_check;
uint32_t last_udp_check;
uint32_t recv_watchdog;
uint32_t t_blink;
uint8_t led_state = 0;
BNO080 myIMU;
BNO080 myIMU2;
bool extended_imu_found = false;

uint8_t main_id = 0;
uint8_t extended_id = 0;

bool streaming_udp = false;
bool imu_start = false;

float battery_voltage() {
  return get_battery_voltage(analogReadMilliVolts(batt_monitor_pin));
}

void reset_imu() {
  digitalWrite(reset_pin, HIGH);
  delay(100);
  digitalWrite(reset_pin, LOW);
  delay(100);
  digitalWrite(reset_pin, HIGH);
}

void blink_led(uint16_t interval) {
  if (!led_state) {
    if (millis() - t_blink >= interval) {
      led_state = !led_state;
      digitalWrite(led_pin, led_state);
      t_blink = millis();
    }
  }
  else {
    if (millis() - t_blink >= 100) {
      led_state = !led_state;
      digitalWrite(led_pin, led_state);
      t_blink = millis();
    }
  }
}

int16_t map(float x, float in_min = -1, float in_max = 1, float out_min = -32767, float out_max = 32767) {
  return (int16_t)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void sendUDP(bool extend) {
  if ((driverPort != 0) && ((main_id != 0) || (extended_id != 0))) {
    udp.beginPacket(udpAddress, driverPort);
    if (extend) {
      udp.write((uint8_t*)&payloadext, sizeof(payloadext));
    }
    else {
      udp.write((uint8_t*)&payload, sizeof(payload));
    }
    
    if (udp.endPacket()) {
      last_udp_check = millis();
    }

    DEBUG_PRINT.print("main,");
    DEBUG_PRINT.print(payload.id);
    DEBUG_PRINT.print(",");
    DEBUG_PRINT.print(payload.x);
    DEBUG_PRINT.print(",");
    DEBUG_PRINT.print(payload.y);
    DEBUG_PRINT.print(",");
    DEBUG_PRINT.print(payload.z);
    DEBUG_PRINT.print(",");
    DEBUG_PRINT.print(payload.w);
    DEBUG_PRINT.println();

    DEBUG_PRINT.print("extended,");
    DEBUG_PRINT.print(payloadext.id_ext);
    DEBUG_PRINT.print(",");
    DEBUG_PRINT.print(payloadext.x_ext);
    DEBUG_PRINT.print(",");
    DEBUG_PRINT.print(payloadext.y_ext);
    DEBUG_PRINT.print(",");
    DEBUG_PRINT.print(payloadext.z_ext);
    DEBUG_PRINT.print(",");
    DEBUG_PRINT.print(payloadext.w_ext);
    DEBUG_PRINT.println();
  }
}

void setup(){
  if (!brown_en) {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  }
  
  setCpuFrequencyMhz(80);

  uint8_t mac[6];
  WiFi.macAddress(mac);
  ping.a = mac[0];
  ping.b = mac[1];
  ping.c = mac[2];
  ping.d = mac[3];
  ping.e = mac[4];
  ping.f = mac[5];
  
  Serial.begin(115200);
  
  WiFi.onEvent(WiFiEvent);
  WiFi.begin();
  WiFi.setSleep(wifi_power_save);
  WiFi.setTxPower(txPower);

  pinMode(reset_pin, OUTPUT);
  digitalWrite(reset_pin, HIGH);
  reset_imu();
  Wire.begin();
  uint32_t imu_init_time = millis();
  while (!myIMU.begin(i2c_main_addr)) {
    reset_imu();
    delay(500);
    if (millis() - imu_init_time >= 3000) {
      ESP.deepSleep(1000);
    }
  }
  Wire.setClock(400000);
  myIMU.enableARVRStabilizedRotationVector(10);
  DEBUG_PRINT.println("AR VR Stabilized Rotation Vector enabled on main IMU");

  if (myIMU2.begin(i2c_extend_addr)) {
    myIMU2.enableARVRStabilizedRotationVector(10);
    extended_imu_found = true;
    ping.extend = true;
    DEBUG_PRINT.println("AR VR Stabilized Rotation Vector enabled on extended IMU");
  }
  else {
    extended_imu_found = false;
    ping.extend = false;
    DEBUG_PRINT.println("Extended IMU not found");
  }

  pinMode(led_pin, OUTPUT);
  
  t_blink = millis();

  if (!brown_en) {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1);
  }
}

void loop(){
  serialLength = Serial.available();
  if (serialLength) {
    Serial.readBytesUntil('\n', header, sizeof(header));
    if (String(header) == "111") {
      Serial.readBytesUntil('\n', networkSSID, sizeof(networkSSID));
      Serial.readBytesUntil('\n', networkPassword, sizeof(networkPassword));
      connectToWiFi(networkSSID, networkPassword);
      Serial.write(110);
    }
    else {
      char junk[serialLength];
      Serial.readBytes(junk, sizeof(junk));
    }
    memset(header, 0, sizeof(header));
    memset(networkSSID, 0, sizeof(networkSSID));
    memset(networkPassword, 0, sizeof(networkPassword));
  }
  
  if(connected){
    
    udp.parsePacket();
    if (udp.read((uint8_t*)&ack, sizeof(ack)) > 0) {
      if (ack.reply == 200 && ack.header == (uint8_t)'I' && ack.footer == (uint8_t)'i') {
        recv_watchdog = millis();
        udpAddress = udp.remoteIP();
        main_id = ack.id;
        extended_id = ack.id_ext;
        driverPort = ack.driverPort;
        if ((driverPort != 0) && ((main_id != 0) || (extended_id != 0))) {
          if (!streaming_udp) {
            streaming_udp = true;
            last_udp_check = millis();
          }
        }
        else {
          streaming_udp = false;
        }
      }
    }
    
    if (udpAddress != IPAddress(0, 0, 0, 0)) {

      if (!imu_start) {
        imu_start = true;
        last_main_imu_check = millis();
        last_extend_imu_check = millis();
      }
      
      if (myIMU.dataAvailable()) {
        last_main_imu_check = millis();
        payload.id = main_id;
        payload.x = map(myIMU.getQuatI());
        payload.y = map(myIMU.getQuatJ());
        payload.z = map(myIMU.getQuatK());
        payload.w = map(myIMU.getQuatReal());

        payloadext.id = main_id;
        payloadext.x = map(myIMU.getQuatI());
        payloadext.y = map(myIMU.getQuatJ());
        payloadext.z = map(myIMU.getQuatK());
        payloadext.w = map(myIMU.getQuatReal());

        sendUDP(extended_imu_found);
      }

      if (extended_imu_found && myIMU2.dataAvailable()) {
        last_extend_imu_check = millis();
        payloadext.id_ext = extended_id;
        payloadext.x_ext = map(myIMU2.getQuatI());
        payloadext.y_ext = map(myIMU2.getQuatJ());
        payloadext.z_ext = map(myIMU2.getQuatK());
        payloadext.w_ext = map(myIMU2.getQuatReal());
      }

      if (millis() - last_ping >= 1000) {
        last_ping = millis();
        ping.id = main_id;
        ping.id_ext = extended_id;
        ping.batt = battery_voltage();
        udp.beginPacket(udpAddress,serverPort);
        udp.write((uint8_t*)&ping, sizeof(ping));
        udp.endPacket();
        DEBUG_PRINT.println(ping.batt);
      }

      if (millis() - last_main_imu_check >= 500) {
        ESP.deepSleep(1000);
      }

      if (extended_imu_found && (millis() - last_extend_imu_check >= 500)) {
        ESP.deepSleep(1000);
      }

      if (streaming_udp) {
        if (millis() - last_udp_check >= 500) {
          ESP.deepSleep(1000);
        }
      }
                  
      blink_led(5000);
    }
    
    else {
      if (millis() - last_broadcast >= 1000) {
        last_broadcast = millis();
        Audp.broadcastTo((uint8_t*)&pingbroad, sizeof(pingbroad), serverPort);
        DEBUG_PRINT.println("Pinged");
      } 
      blink_led(2000);
    }
  }
  else {
    blink_led(1000);
  }
  if (millis() - recv_watchdog >= 5000) {
    udpAddress = IPAddress(0, 0, 0, 0);
    driverPort = 0;
    streaming_udp = false;
    imu_start = false;
  }
}

void connectToWiFi(const char * ssid, const char * pwd){
  DEBUG_PRINT.println("Connecting to WiFi network: " + String(ssid));
  WiFi.disconnect(true);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(ssid, pwd);
  WiFi.setSleep(wifi_power_save);
  WiFi.setTxPower(txPower);
  DEBUG_PRINT.println("Waiting for WIFI connection...");
}

void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP: 
          DEBUG_PRINT.print("WiFi connected! IP address: ");
          DEBUG_PRINT.println(WiFi.localIP());  
          udp.begin(WiFi.localIP(),serverPort);
          last_broadcast = millis();
          last_ping = millis();
          last_main_imu_check = millis();
          last_extend_imu_check = millis();
          last_udp_check = millis();
          recv_watchdog = millis();
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          DEBUG_PRINT.println("WiFi lost connection");
          connected = false;
          udpAddress = IPAddress(0, 0, 0, 0);
          driverPort = 0;
          streaming_udp = false;
          imu_start = false;
          WiFi.reconnect();
          break;
      default: break;
    }
}
