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
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t f;
  float batt = 0;
  uint8_t footer = (uint8_t)'i';
} ping;

struct __attribute__((packed)) Ack {
  uint8_t header;
  uint8_t reply;
  uint8_t id;
  uint16_t driverPort;
  uint8_t footer;
} ack;

struct __attribute__((packed)) Payload {
  uint8_t header = (uint8_t)'I';
  uint8_t id = 0;
  float x = 0;
  float y = 0;
  float z = 0;
  float w = 1;
  uint8_t footer = (uint8_t)'i';
} payload;

uint32_t broadcast_delay = random(3000, 5001);
uint32_t start_broadcast_time;
uint32_t start_tx_time;
uint32_t recv_watchdog;
uint32_t t_blink;
uint8_t led_state = 0;
BNO080 myIMU;

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
  WiFi.setTxPower(txPower);

  pinMode(reset_pin, OUTPUT);
  digitalWrite(reset_pin, HIGH);
  reset_imu();
  Wire.begin();
  while (!myIMU.begin()) {
    reset_imu();
    delay(500);
  }
  Wire.setClock(100000);
  myIMU.enableARVRStabilizedRotationVector(10);
  DEBUG_PRINT.println("AR VR Stabilized Rotation Vector enabled");

  pinMode(led_pin, OUTPUT);
  
  t_blink = millis();
  start_broadcast_time = millis();
  start_tx_time = millis();
  recv_watchdog = millis();

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
        udpAddress = udp.remoteIP();
        payload.id = ack.id;
        ping.id = ack.id;
        driverPort = ack.driverPort;
        recv_watchdog = millis();
      }
    }
    
    if (udpAddress != IPAddress(0, 0, 0, 0)) {
      if (myIMU.dataAvailable() && payload.id != 0  && driverPort != 0) {
        payload.x = myIMU.getQuatI();
        payload.y = myIMU.getQuatJ();
        payload.z = myIMU.getQuatK();
        payload.w = myIMU.getQuatReal();

        udp.beginPacket(udpAddress,driverPort);
        udp.write((uint8_t*)&payload, sizeof(payload));
        udp.endPacket();
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
      }

      if (millis() - start_tx_time >= 1000) {
        ping.batt = battery_voltage();
        udp.beginPacket(udpAddress,serverPort);
        udp.write((uint8_t*)&ping, sizeof(ping));
        udp.endPacket();
        DEBUG_PRINT.println(ping.batt);
        start_tx_time = millis();
      }
      blink_led(5000);
    }
    
    else {
      if (millis() - start_broadcast_time >= broadcast_delay) {
        Audp.broadcastTo((uint8_t*)&pingbroad, sizeof(pingbroad), serverPort);
        DEBUG_PRINT.println("Pinged");
        start_broadcast_time = millis();
        broadcast_delay = random(3000, 5001);
      } 
      blink_led(2000);
    }
  }
  else {
    blink_led(1000);
  }
  if (millis() - recv_watchdog >= 3000) {
    udpAddress = IPAddress(0, 0, 0, 0);
    driverPort = 0;
  }
}

void connectToWiFi(const char * ssid, const char * pwd){
  DEBUG_PRINT.println("Connecting to WiFi network: " + String(ssid));
  WiFi.disconnect(true);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(ssid, pwd);
  WiFi.setTxPower(txPower);
  DEBUG_PRINT.println("Waiting for WIFI connection...");
}

void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP: 
          DEBUG_PRINT.print("WiFi connected! IP address: ");
          DEBUG_PRINT.println(WiFi.localIP());  
          udp.begin(WiFi.localIP(),serverPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          DEBUG_PRINT.println("WiFi lost connection");
          connected = false;
          WiFi.reconnect();
          break;
      default: break;
    }
}
