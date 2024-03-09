void info_wifi(){
              String json = "{\"success\": false}";
            if (checkScanWifi()){
             json = printScannedNetworks();
            }
            server.send(200, "application/json", json);
      }

void info_device(){
              JsonDocument doc;
              doc["device_name"] = data.device_name;
              doc["wifiAP"] = data.wifiAP;
              doc["temp"] = temp;
              doc["pressure"] = pressure;
              doc["humidity"] = humidity;

            
              Serial.print(F("Sending: "));
  serializeJson(doc, Serial);
  Serial.println();

  String json;
  // Write JSON document
  serializeJsonPretty(doc, json);

  server.send(200, "application/json", json);
      }

void webReset(){
    if (server.arg("reset") == "1") {
      server.send(200, "text/html", "<html><head></head><body>RESET</body></html>");
      factoryReset();
    } else if (server.arg("showIp") == "0") {
      showIpEnded = true;
      isShowIp = false;
      server.send(200, "text/html", "<html><head></head><body>Ok</body></html>");
    }else if (server.arg("restart") == "1") {
      server.send(200, "text/html", "<html><head></head><body>Ok</body></html>");
      restart();
    } else if (server.arg("log") == "1") {
      server.send(200, "text/html", "<html><head></head><body><textarea rows='5' cols='33'>" + glog.read() + "</textarea></body></html>");
     }else server.send(200, "text/html", "<html><head></head><body><a href='https://github.com/zazacaca32/HydraMF/releases/download/html/settings.html'>download</a><br><textarea rows='5' cols='33'>" + glog.read() + "</textarea></body></html>");
      }


void handleNotFound() {
  String message = "Page Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void set_wifi(){
  String json_text = server.arg("plain");
  setWifiNetworks(json_text);
  data.wifiAP = false;
  memory.updateNow();
  server.send(200, "text/html", json_text);
  println((String)data.wifiAP);
  restart();
}


void serve()
{
  server.begin();
  server.enableCORS(true);

  server.on("/info/wifi", HTTP_GET, info_wifi);

  server.on("/info/device", HTTP_GET, info_device);

  server.on("/", HTTP_GET, webReset);

  server.on("/set/wifi", HTTP_POST, set_wifi);

  server.onNotFound(handleNotFound);
}
