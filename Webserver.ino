
// Bestandteile der Webseite ==============================================================================//


volatile boolean scanForWLANS = true;
volatile uint8_t semaWLAN = 0;

uint8_t setWLANsema() {
  uint8_t result=0;
  noInterrupts();
    if (0 == semaWLAN) {
      semaWLAN=1;
      result=1;
    }
  interrupts();
  return( result);  
}

uint8_t clearWLANsema() {
  uint8_t result=1;
  noInterrupts();
    if (1 == semaWLAN) {
      semaWLAN=0;
      result=0;
    }
  interrupts();
  return( result);  
}



// Webserver
AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws



void setupWebserver() { 
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.addHandler(new SPIFFSEditor(GlobalConfig.myAdminUser,GlobalConfig.myAdminPWD));
  server.on("/saveConfig", HTTP_POST|HTTP_GET, handleSaveConfig);
  server.on("/getConfigData", HTTP_POST|HTTP_GET, handleGetConfigData);
  server.on("/wifiRestartAP", HTTP_POST, handleWiFiRestartAP);
  server.on("/wifiRestartSTA", HTTP_GET, handleWiFiRestartSTA);
  server.on("/wpsconfig", HTTP_GET, handleWPSConfig);
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.begin();
}


void handleGetConfigData( AsyncWebServerRequest *request){
  if(!request->authenticate( GlobalConfig.myAdminUser, GlobalConfig.myAdminPWD))
    return request->requestAuthentication();

  struct T_ConfigStruct tempConf;
  char myAdminPWD2[20];
  char buffer[200];
  snprintf(buffer, sizeof(buffer), "{\"SSID\": \"%s\", \"WLANPWD\": \"%s\", \"CHANNEL\": \"%u\", \"ADMPWD\": \"%s\"}\n", 
           GlobalConfig.mySSID, GlobalConfig.myPWD, GlobalConfig.myChannel, GlobalConfig.myAdminPWD);
  request->send(200, "text/html", buffer);
}





void handleSaveConfig(AsyncWebServerRequest *request){
  if(!request->authenticate(GlobalConfig.myAdminUser, GlobalConfig.myAdminPWD))
    return request->requestAuthentication();
  Serial.println("save config");

  struct T_ConfigStruct tempConf;
  char myAdminPWD2[20];
  
  int params = request->params();
  for(int i=0;i<params;i++){
    AsyncWebParameter* p = request->getParam(i);
    if(! p->isFile()) {  // ONLY for GET and POST
      if (! strcmp(p->name().c_str(), "SSID")) {
        strlcpy( tempConf.mySSID, p->value().c_str(), sizeof(tempConf.mySSID));
        Serial.print("SSID: "); Serial.println(tempConf.mySSID);
      } 
      if (! strcmp(p->name().c_str(), "WLANPWD")) {
        strlcpy( tempConf.myPWD, p->value().c_str(), sizeof(tempConf.myPWD));
        Serial.print("WLAN-PWD: "); Serial.println(tempConf.myPWD);
      } 
      if (! strcmp(p->name().c_str(), "Kanal")) {
         tempConf.myChannel = atoi(p->value().c_str());
        Serial.print("Channel: "); Serial.println(tempConf.myChannel);
      } 
      if (! strcmp(p->name().c_str(), "ADMPWD")) {
        strlcpy( tempConf.myAdminPWD, p->value().c_str(), sizeof(tempConf.myAdminPWD));
        Serial.print("ADM: "); Serial.println(tempConf.myAdminPWD);
      } 
      if (! strcmp(p->name().c_str(), "ADMPWD2")) {
        strlcpy( myAdminPWD2, p->value().c_str(), sizeof(myAdminPWD2));
        Serial.print("ADMPWD2: "); Serial.println(myAdminPWD2);
      } 
    }
  }
  if (params >2)  {
    Serial.println("Configuration complete");
    if((strlen(tempConf.myAdminPWD) > 0) and 
       (strlen(myAdminPWD2) >0) and
       (0 == strncmp(tempConf.myAdminPWD, myAdminPWD2, sizeof(myAdminPWD2))))  {
      strlcpy(tempConf.magic, CONF_MAGIC, sizeof(tempConf.magic));
      tempConf.version=CONF_VERSION;
      ConfigSave(&tempConf);
      LoadAndCheckConfiguration();
      request->send(204, "text/plain", "OK");
      return;
    } else {
      request->send(204, "text/plain", "ERROR");
    }
  } else {
    request->send(204, "text/plain", "ERROR");
  }
}




void handleWiFiRestartAP(AsyncWebServerRequest *request){
  handleWiFiRestart(request, true);
}
void handleWiFiRestartSTA(AsyncWebServerRequest *request){
  handleWiFiRestart(request, false);
}

void handleWiFiRestart(AsyncWebServerRequest *request, boolean asAP){
  char  clientSSID[32] = "";
  char  clientPWD[64]  = "";  
  Serial.println("WiFiSave start");
  int params = request->params();
  String page = "";
  if (asAP) {
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(! p->isFile()) {  // ONLY for GET and POST
        if (! strcmp(p->name().c_str(), "s")) {
          strlcpy(  clientSSID, p->value().c_str(), sizeof( clientSSID));
          Serial.println("SSID received!");
        }
        if (! strcmp(p->name().c_str(), "p")) {
          strlcpy(  clientPWD, p->value().c_str(), sizeof( clientPWD));
          Serial.println("PWD received!");
        }
      }
    }
  }
  File menuFile = SPIFFS.open("/decodeIP.html", "r");
  while (menuFile.available()){
    Serial.println("  reading line...");
    page += menuFile.readStringUntil('\n');
  }
  request->send(200, "text/html", page);
  Serial.println("Request done");
  struct station_config newConf;  memset(&newConf, 0, sizeof(newConf));
  strlcpy((char *)newConf.ssid, clientSSID, strlen(clientSSID)+1);
  strlcpy((char *)newConf.password, clientPWD, strlen(clientPWD)+1);
  ETS_UART_INTR_DISABLE();
  wifi_station_set_config(&newConf);
  ETS_UART_INTR_ENABLE();
  Serial.println("New config is written");
  pending_reboot=true; //make sure, ESP gets rebooted
}

void handleWPSConfig(AsyncWebServerRequest *request){
  Serial.println("wps config done");
  String message="<html><body><h1>WPSCONFIG NOT YET IMPLEMENTED</h1></body></html>";
  request->send(200, "text/html", message);
  
}

void handleApConfig(AsyncWebServerRequest *request){
  Serial.println("AP config done");
  String message="<html><body><h1>AP CONFIG NOT YET IMPLEMENTED</h1></body></html>";
  request->send(200, "text/html", message);
}

void sendWLANscanResult( AsyncWebSocketClient * client) {
  if ( 1 == setWLANsema()) {
    int16_t numWLANs=  WiFi.scanComplete();
    if (0 < numWLANs) {
      for (uint16_t i=0; i<numWLANs; i++) {
        char buffer[200];
        snprintf(buffer, sizeof(buffer), "JSONDATA{\"SSID\": \"%s\", \"encType\": %u, \"RSSI\": \"%d\", \"BSSID\": \"%s\", \"Channel\": %u, \"Hidden\": \"%s\"}",
                WiFi.SSID(i).c_str(), WiFi.encryptionType(i), WiFi.RSSI(i), WiFi.BSSIDstr(i).c_str(), WiFi.channel(i), WiFi.isHidden(i) ? "true" : "false");
        client->text(buffer);
      }
    } else {
      client->text("[\"no WLAN found\"]\n");
    }
    clearWLANsema();
  } else {
    client->text("[\"no WLAN found\"]\n");
  }
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  StaticJsonBuffer<300> jsonBuffer;
  Serial.println("Websockets fired");
  if(type == WS_EVT_CONNECT){
    //client connected
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
  } else if(type == WS_EVT_ERROR){
  } else if(type == WS_EVT_PONG){
    // pong message was received (in response to a ping request maybe)
    // os_printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    //data packet
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      if(info->opcode == WS_TEXT) {
        data[len] = 0;
        JsonObject &root = jsonBuffer.parseObject((char*)data);
       if ( !root.success() ) {
        } else {
          const char* magic = root["magic"];
          const char* cmd = root["cmd"];
          if (0 == strncmp("FLKA", magic, 4)) {
            if(0 == strncmp("LISTWLANS", cmd, 9)) {
              sendWLANscanResult(client);
              scanForWLANS = true;
              Serial.println("scan4wlan true!");
            }
          }
        }
      } else {
        // we currently dont react on binary messages
      }  
    } else {
      // we don't expect fragmentation either
    }
  }
}









void showFablabLogo(uint8_t pin, uint8_t rotate = 0, bool show=true) {
  uint8_t rotationOffset = (rotate % 4) * 10;
  Adafruit_NeoPixel strip = Adafruit_NeoPixel(4*LED_COUNT_LOGO, pin, NEO_GRB + NEO_KHZ800);  
  strip.begin();
  if (show) {
    for (uint16_t i =0; i< (LED_COUNT_LOGO); i++) {
      strip.setPixelColor(((LOGO_LEAF_OFFSET+rotationOffset)   % 40) + i, LOGO_LEAF_COLOR);
      strip.setPixelColor(((LOGO_BUBBLE_OFFSET+rotationOffset) % 40) + i, LOGO_BUBBLE_COLOR);
      strip.setPixelColor(((LOGO_PENCIL_OFFSET+rotationOffset) % 40) + i, LOGO_PENCIL_COLOR);
      strip.setPixelColor(((LOGO_WRENCH_OFFSET+rotationOffset) % 40) + i, LOGO_WRENCH_COLOR);
    }
    
  } else {
    for (uint16_t i =0; i< (4*LED_COUNT_LOGO); i++) {
      strip.setPixelColor(i,0);
    }
  }
  strip.show();
}

void showFablabText(uint8_t pin, uint8_t brightness) {
  Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT_TEXT, pin, NEO_GRB + NEO_KHZ800);  
  strip.begin();
  for (uint16_t i =0; i< (LED_COUNT_TEXT); i++) {
    RGB colorVal = HSLToRGB(HSL(TEXT_HUE, TEXT_SATURATION, brightness/100.0));
    strip.setPixelColor( i, colorVal.NeoColor() );
  }
  strip.show();
}





void do_pending_Actions(void){
  static uint8_t  rotation = 0;
  static uint32_t firstLoop = millis();

  // do all things, which need to be done after an asynchronous request
  if (pending_reboot) {
    Serial.println("Restarting system, hold on");
    delay(4000);
    // Sicherstellen, dass die GPIO-Pins dem Bootloader das erneute Starten des Programmes erlauben!
    pinMode( 0,OUTPUT);
    pinMode( 2,OUTPUT);
    pinMode(15,OUTPUT);
    WiFi.forceSleepBegin(); 
    wdt_reset();  
    digitalWrite(15, LOW);
    digitalWrite( 0,HIGH);
    digitalWrite( 2,HIGH);
    ESP.restart();
  }  
  if (scanForWLANS) {
    if ( 1==setWLANsema()) {
      int16_t noWlans= WiFi.scanComplete();
      if ( noWlans != -1 ) {
        WiFi.scanNetworks(true, true);
        scanForWLANS = false;
      }  
      clearWLANsema();
    }  
  }
  showFablabText(PIN_OUTSIDE_TEXT, 50);
  showFablabText(PIN_INSIDE_TEXT, 50);
  uint32_t delta = millis() % 60000;
  if (delta < 500) {
    rotation = 1;
  }  else
  if (delta < 1000) {
    rotation = 2;
  } else
  if (delta < 1500) {
    rotation = 3;
  } else
  if (delta < 2000) {
    rotation = 0;
  }
  delay(100);
  showFablabLogo(PIN_INSIDE_LOGO, rotation);
  showFablabLogo(PIN_OUTSIDE_LOGO, rotation);
}
