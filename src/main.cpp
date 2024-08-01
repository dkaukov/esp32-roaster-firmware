#define HW_INFO "Default"

#include "secrets.h"
#include <Arduino.h>
#include "ArduinoJson.h"
#include <FS.h>
#include <WiFi.h>
#include <PID_v1.h>
#include <debug.h>
#include "core/Component.h"
#include "sensor/Sensor.h"
#include "acuators/Heater.h"
#include "sensor/BTProbe.h"

#include <ArduinoOTA.h>
#include <esp_task_wdt.h>
#include <esp32-hal-sigmadelta.h>
#include <SlowPWM.h>
#include <PWMrelay.h>

#define S_P 91.0
#define S_I 0.26
#define S_D 7950.0
#define S_aP 100.0
#define S_aI 0.0
#define S_aD 0.0
#define S_TSET 32.0
#define S_TBAND 1.5

#define BUF_SIZE 1024


#define CLIENT_ID_TEMPLATE "RST-%06X"
#define CLIENT_ID_SIZE (sizeof(CLIENT_ID_TEMPLATE) + 5)


double currentTemp = 0;
double gOutputPwr=0.0;
double gTargetTemp=0.0;
double gP = S_P, gI = S_I, gD = S_D;
double gaP = S_aP, gaI = S_aI, gaD = S_aD;
int inputPWM;

PID ESPPID(&currentTemp, &gOutputPwr, &gTargetTemp, gP, gI, gD, DIRECT);


String deviceId;
Core::ComponentManager mgr;
PWMrelay heaterRelay(2, true, 5000);
Actuators::Heater heater("heater", 0.0, 1.0, &heaterRelay);
Sensor::BTProbe bt;

void initDeviceId() {
#if defined(DEVICE_ID)
  deviceId = DEVICE_ID;
#else
  char clientId[CLIENT_ID_SIZE];
  uint32_t nic = ESP.getEfuseMac() >> 40;
  uint32_t chipid = nic & 0xFF;
  chipid = chipid << 8;
  nic = nic >> 8;
  chipid = chipid | (nic & 0xFF);
  chipid = chipid << 8;
  nic = nic >> 8;
  chipid = chipid | (nic & 0xFF);
  snprintf(clientId, CLIENT_ID_SIZE, CLIENT_ID_TEMPLATE, chipid);
  deviceId = String(clientId);
#endif
}

const char *getDeviceId() {
  return deviceId.c_str();
}

#include "network/WEB.h"

void printEnvironment() {
  _LOGI("main", "---");
  _LOGI("main", "Hostname: %s", getDeviceId());
  _LOGI("main", "MAC: %s", WiFi.macAddress().c_str());
  _LOGI("main", "Heap Size: %d", ESP.getHeapSize());
  _LOGI("main", "SDK Version: %s", ESP.getSdkVersion());
  _LOGI("main", "CPU Freq: %d", ESP.getCpuFreqMHz());
  _LOGI("main", "Sketch MD5: %s", ESP.getSketchMD5().c_str());
  _LOGI("main", "Chip model: %s", ESP.getChipModel());
  _LOGI("main", "PSRAM size: %d", ESP.getPsramSize());
  _LOGI("main", "FLASH size: %d", ESP.getFlashChipSize());
  _LOGI("main", "EFUSE mac: 0x%llx", ESP.getEfuseMac());
  _LOGI("main", "Hardware: %s", HW_INFO);
  _LOGI("main", "---");
}

void WiFiSTAConnect() {
  //WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.persistent(true);
  WiFi.setSleep(false);
#if (!defined(IP_CONFIGURATION_TYPE) || (IP_CONFIGURATION_TYPE == IP_CONFIGURATION_TYPE_DHCP))
  //WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
#elif (defined(IP_CONFIGURATION_TYPE) && (IP_CONFIGURATION_TYPE == IP_CONFIGURATION_TYPE_STATIC))
#define IP(name, value) IPAddress name(value);
  IP(local_ip, IP_CONFIGURATION_ADDRESS);
  IP(local_mask, IP_CONFIGURATION_MASK);
  IP(gw, IP_CONFIGURATION_GW);
  WiFi.config(local_ip, gw, local_mask);
#else
#error "Incorrect IP_CONFIGURATION_TYPE."
#endif
  WiFi.setHostname(getDeviceId());
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setHostname(getDeviceId());
  WiFi.setAutoReconnect(true);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  _LOGI("main", "Connected to AP successfully!");
}

void wiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  WiFiSTAConnect();
}

void wiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  _LOGI("main", "WiFi connected!");
  _LOGI("main", "IP address: %s", WiFi.localIP().toString().c_str());
  _LOGI("main", "IP hostname: %s", WiFi.getHostname());
}

void setupWiFi() {
  btStop();
  WiFiSTAConnect();
  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(wiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.onEvent(wiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
#if defined(NTP_SERVER)
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, (char *)NTP_SERVER);
  sntp_init();
#endif
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    _LOGE("main", "Wifi connection failed!");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  initDeviceId();
  WiFi.setHostname(getDeviceId());
  WiFi.begin("", "");
  printEnvironment();
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  setupWiFi();
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  debugInit();
  ArduinoOTA.setMdnsEnabled(true);
  ArduinoOTA.setHostname(getDeviceId());
  ArduinoOTA.begin();
  Net::web.init();
  heater.init();
  bt.init();
  mgr.init();
  heater.setValue(30);
}

void loop() {
  //esp_task_wdt_reset();
  debugLoop();
  ArduinoOTA.handle();
  mgr.loop();
  if(ESPPID.Compute()==true) {      
  }
}