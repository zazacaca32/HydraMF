void println(const String &text)
{
  Serial.println(text);
  glog.println(text);
}

void print(const String &text)
{
  Serial.print(text);
  glog.print(text);
}

// void startWiFiScan() {
//   Serial.println("Scan start");
//   // WiFi.mode(WIFI_STA);
//   // WiFi.disconnect();
//   // // WiFi.scanNetworks will return immediately in Async Mode.
//   WiFi.scanNetworks(true); // 'true' turns Async Mode ON
// }

String printScannedNetworks() {
  int16_t networksFound = WiFi.scanComplete();
      String json;
  if (networksFound == 0) {
    Serial.println("no networks found");
      json = "no networks found";
  } else {
    Serial.print(networksFound);
    Serial.println(" networks found");
    JsonDocument mainDoc;
    JsonDocument docArr;
    JsonDocument docObj;

     JsonArray data = docArr.to<JsonArray>();

      JsonObject object = docObj.to<JsonObject>();
     

    for (int i = 0; i < networksFound; ++i) {

      object["ssid"] = WiFi.SSID(i);
      object["rssi"] = WiFi.RSSI(i);
      object["channel"] = WiFi.channel(i);
      object["encType"] = WiFi.encryptionType(i);

      data.add(object);
      object.clear();
    }
    mainDoc["success"] = true;
    mainDoc["list"] = data;
      serializeJson(mainDoc, json);
  }
      return json;
  }
  


void stopScanWifi(){
  println("Stop Scan  WIFI");
  WiFi.scanNetworks(false);
  WiFi.scanDelete();
  showIpEnded = true;
  isShowIp = false;
}

bool checkScanWifi(){
    int16_t WiFiScanStatus = WiFi.scanComplete();
  if (WiFiScanStatus < 0) { // it is busy scanning or got an error
      Serial.println("WiFi Scan has failed. ");
      Serial.println(WiFiScanStatus);
     return false;
  } else { 
    return true;
  }
} 

void wifiApStaTimerHandler()
{
  WiFi.mode(WIFI_STA);
}

template <class T>
String type_name(const T &)
{
  String s = __PRETTY_FUNCTION__;

  int start = s.indexOf("[with T = ") + 10;
  int stop = s.lastIndexOf(']');

  return s.substring(start, stop);
}

IPAddress getDeviceIp (){
  if (data.wifiAP) return WiFi.softAPIP();
  return WiFi.localIP();
}

void updateLcd() {
  if (isShowIp){
    String ip = getDeviceIp().toString();
    lcd.setCursor(0, 0);
    String s;
    int calc = 16-ip.length();
   for (int i = 0; i < calc; i++) { 
        s+= ' ';
    } 
    lcd.print(ip+s); 
  }else {
  // lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print(data.device_name); 
  lcd.print(" ");
  lcd.write(6); 
  lcd.print(pressure); 
  lcd.write(4);
  lcd.print("Hg");

  lcd.setCursor(0, 1); 
  lcd.print(temp); 
  lcd.write(67);
  lcd.write(223);
  lcd.print("  ");

  lcd.write(1);  
  lcd.print(humidity); 
  lcd.print("%");
  lcd.print(" ");

  // println((String)WiFi.RSSI());
  // println((String)temp);
  // println((String)humidity);
  // println((String)pressure);
  }
  if (!showIpEnded) {
    isShowIp = !isShowIp;
  }
}

void initBME(){
      if (!bme.begin(BME280__ADDRESS)) 
      {
        println("Error BME280"); //если датчик не найден - выводим ошибку 
         println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        print("SensorID was: 0x");println((String) bme.sensorID());
        print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
       print("   ID of 0x56-0x58 represents a BMP 280,\n");
        print("        ID of 0x60 represents a BME 280.\n");
        print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
      }
    bme.setSampling(Adafruit_BME280::MODE_NORMAL,   //режим работы датчика bmp280
                  Adafruit_BME280::SAMPLING_X2,   //точность изм. температуры  
                  Adafruit_BME280::SAMPLING_X16,  //точность изм. давления 
                  Adafruit_BME280::SAMPLING_X1, // влажность
                  Adafruit_BME280::FILTER_X16,    //уровень фильтрации
                  Adafruit_BME280::STANDBY_MS_0_5); //период просыпания, мСек
}

 void readBME(){
    pressure=bme.readPressure()/mmHg;  //сумма значений атмосферного давления 
    temp=bme.readTemperature();      //сумма значений температуры
    humidity=bme.readHumidity();
    updateLcd();
 }

 void restart()
{
  println("Rebooting...");
  println("-------------------------------");
  ESP.restart();
}

void wifiAp()
{

  println("Create AP");
  
  String ssid = data.device_name;
  ssid += "_AP";
  WiFi.mode(WIFI_AP);
  onSoftAPModeStationConnected = WiFi.onSoftAPModeStationConnected([](const WiFiEventSoftAPModeStationConnected &event)
                                                                   { println("Client connected to wifi"); });

  onSoftAPModeStationDisconnected = WiFi.onSoftAPModeStationDisconnected([](const WiFiEventSoftAPModeStationDisconnected &event)
                                                                         { println("Client disconnected from wifi"); });

  WiFi.softAP(ssid);
  IPAddress ip = WiFi.softAPIP();
  print("AP IP address: ");
  println(ip.toString());

  data.wifiConnectTry = 0;
  data.wifiAP = true;
}

void wifiConnect()
{
  WiFi.mode(WIFI_STA);
  WiFi.hostname(data.device_name);

  // onStationModeConnected = WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &event)
  //                                                      {
  //   println("StationModeConnected"); });

  println("Wifi connecting...");
  lcd.setCursor(0, 0);

  lcd.print("Wifi connecting..."); 
  uint32_t notConnectedCounter = 0;
  lcd.setCursor(0, 1);
  while (wifiMulti.run(7000) != WL_CONNECTED)
  {
    notConnectedCounter++;
    println((String)notConnectedCounter);
    lcd.print(notConnectedCounter); 
    if (notConnectedCounter > 5)
    { // Reset board if not connected after 5s
      println("Resetting due to Wifi not connecting...");

      data.wifiConnectTry += 1;
      println("wifiConnectTry: " + String(data.wifiConnectTry));
      if (data.wifiConnectTry >= 3)
      {
        data.wifiAP = true;
      }
      memory.updateNow();

      restart();
    }
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  print("Wifi connected, IP address: ");
  println(WiFi.localIP().toString());
}

void factoryReset()
{
  println("Factory reset");
  memory.reset();
  memory.updateNow();
  restart();
}

void setWifiNetworks(String str){
  strcpy(data.wifiDataString, str.c_str());
  JsonDocument doc;
  deserializeJson(doc, str);
  JsonArray arr = doc["list"].as<JsonArray>();

  for (JsonVariant value : arr) {
    Serial.println(value["ssid"].as<const char*>());
    Serial.println(value["password"].as<const char*>());
    wifiMulti.addAP(value["ssid"].as<const char*>(), value["password"].as<const char*>());
}
}

void startup()
{
  Serial.begin(9600);
  // Log
  glog.start(1000);
  glog.autoClear(false);

  println("-------------------------------");
  println("Booting...");
  println("Initialize EEPROM");
  EEPROM.begin(sizeof(data) + 1); // +1 key
  memory.begin(0, '0');

  println("Initialize BME");
  initBME();

  println("Initialize Timers");
  bmeTimer.setTime(4000);
  bmeTimer.start();

  telemetryTimer.setTime(1*60*1000);
  telemetryTimer.start();

  scanWifiTimer.setTimerMode();
  scanWifiTimer.setTime(5*60*1000);
  scanWifiTimer.start();

  println("Initialize lcd");
  lcd.init();                      
  lcd.backlight();

  lcd.createChar(1, customCharHumidity);
  lcd.createChar(4, customChar_mm);
  lcd.createChar(6, customCharPressure);


  // Connecting WiFi
  println("Initialize WiFi ");
  println((String)data.wifiAP);
  if (data.wifiAP)
  {
    wifiAp();
  }
  else
  {
    setWifiNetworks(data.wifiDataString);
    wifiConnect();
  }

  memory.updateNow();

  if (data.wifiAP) WiFi.scanNetworks(true);

  println("Boot complete");
  serve();
  println("-------------------------------");
}


