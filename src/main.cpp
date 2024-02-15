#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <EEManager.h>
#include <TimerMs.h>
#include <ArduinoJson.h>

#include <log.h>


struct Data
{
  // Data
  char device_name[32] = "Hydra";
  bool wifiAP = true;
  byte wifiConnectTry = 0;
  // WiFi
  char wifiDataString[1024];
};
  double pressure=0; 
  double temp=0; 
  double humidity=0;
  double mmHg=133.3; //величина для перевода паскалей в мм.рт.ст

  bool showIpEnded = false;
  bool isShowIp = false;

byte customChar_mm[] = {
  0x1A,
  0x15,
  0x11,
  0x00,
  0x1A,
  0x15,
  0x11,
  0x00
};
byte customCharPressure[] = {
  0x04,
  0x0A,
  0x0A,
  0x0A,
  0x0E,
  0x1F,
  0x0E,
  0x04
};
byte customCharHumidity[] = {
  0x00,
  0x04,
  0x0E,
  0x1F,
  0x1F,
  0x1F,
  0x0E,
  0x00
};

Data data;
EEManager memory(data);
GPlog glog("log");
TimerMs bmeTimer, scanWifiTimer;

WiFiEventHandler onSoftAPModeStationConnected, onSoftAPModeStationDisconnected, onStationModeConnected;

ESP8266WiFiMulti wifiMulti;

ESP8266WebServer server(80);

LiquidCrystal_I2C lcd(0x27,20,4); 

#define BME280__ADDRESS (0x76) //адрес для подключения датчиков bmp280 через I2C

Adafruit_BME280 bme;

void serve();
#include <function.h>
#include <httpFunc.h>

void setup()
{
  // println((String)data.wifiAP);
  startup();
}

void loop()
{
  server.handleClient();
  if (bmeTimer.tick()) readBME();
  if (scanWifiTimer.tick()) stopScanWifi();
}