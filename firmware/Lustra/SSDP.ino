/*
void SSDP_init(void) {
  // SSDP дескриптор
  server.on("/description.xml", HTTP_GET, []() {
  SSDP.schema(server.client());
  });
  //Если версия  2.0.0 закаментируйте следующую строчку
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(cfg.mqttID);
  SSDP.setSerialNumber(ESP.getChipId());
  SSDP.setURL("/");
  SSDP.setModelName("FireLamp2 Lustra");
  SSDP.setModelNumber("0.23.02b");
 // SSDP.setModelURL("https://alexgyver.ru/gyverlamp/");
  SSDP.setManufacturer("Svetlaya");
  SSDP.setManufacturerURL("https://www.v-elite.ru");
  SSDP.begin();
}*/
void setupOTA() {
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
//    MDNS.begin(AP_NameChar);
    Serial.println("WiFi start");
    server.on("/", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/html", serverIndex);
    });
    server.on("/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
  // mqtt передача состояния update
        topicz = String(cfg.mqttID) + "/update";
        dataz = "[" + String(now.hour) + ":" + String(now.min) + ":" + String(now.sec) + "] ";
        dataz += "Update: ";
        dataz += upload.filename.c_str();
        sendToMqtt();
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace)) { //start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          topicz = String(cfg.mqttID) + "/update";
          dataz = "[" + String(now.hour) + ":" + String(now.min) + ":" + String(now.sec) + "] ";
          dataz += "Update Success: ";
          dataz += upload.totalSize;
//          String(tmp) = "[" + String(now.hour) + ":" + String(now.min) + ":" + String(now.sec) + "] " + dataz;          
          sendToMqtt();
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });
    server.begin();
//    MDNS.addService("http", "tcp", 80);
 //   char sss[32] = WiFi.localIP();
    Serial.print("Ready! Open http://");
    Serial.print(WiFi.localIP());
    Serial.printf(" in your browser\n", AP_NameChar);
  } else {
    Serial.println("WiFi Failed");
  }
  
}
void tmpmqtt (void) {
        char top[50];
        String(tmp);
        tmp.toCharArray(top, tmp.length() + 1);
  
}