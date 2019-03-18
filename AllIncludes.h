#pragma once
  // Verwendete Bibliotheken   =========================================================================//
  
  #include "ESP8266WiFi.h"        // http://esp8266.github.io/Arduino/versions/2.3.0/doc/installing.html 
  #include "ESP8266WiFiGeneric.h" // http://esp8266.github.io/Arduino/versions/2.3.0/doc/installing.html 
  #include <ESP8266mDNS.h>        // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS
  #include <Dns.h>                // http://esp8266.github.io/Arduino/versions/2.3.0/doc/installing.html 
  
  #include "ESPAsyncUDP.h"        // http://github.com/me-no-dev/ESPAsyncUDP
  #include <ESPAsyncTCP.h>        // http://github.com/me-no-dev/ESPAsyncTCP
  #include <ESPAsyncWebServer.h>  // http://github.com/me-no-dev/ESPAsyncWebServer
  #include <SPIFFSEditor.h>       // http://github.com/me-no-dev/ESPAsyncWebServer
 

  #include <ArduinoJson.h>        // https://arduinojson.org
  #include <Adafruit_NeoPixel.h>  // https://github.com/adafruit/Adafruit_NeoPixel

  
  #include <EEPROM.h>
  #include <memory>
  #include "FS.h"

  extern "C" {
    #include "user_interface.h"
  }
  #include "rgb_hsl.h"
  
  // Basiskonfigurationen ==============================================================================//
  
  // Webserver und WiFi Konfigurationen
  #define CONF_VERSION 2    // 1..255 incremented, each time the configuration struct changes
  #define CONF_MAGIC "flka" // unique identifier for project. Please change for forks of this code
  #define CONF_MDNS_NAME "FLKA-DOOR-SIGN"
  #define CONF_DEFAULT_SSID "FLKA-DOOR-SIGN"
  #define CONF_DEFAULT_PWD "ABCdef123456"
  #define CONF_HOSTNAME "FLKA-DOOR-SIGN"
  #define CONF_ADMIN_USER "admin"
  #define CONF_ADMIN_PWD "admin"
  #define CONF_DEFAULT_CHANNEL 4

  
  // dont use pins 0,2,15, as they are needed for reboot and flash
  
  #define PIN_INSIDE_TEXT  12  // D6
  #define PIN_INSIDE_LOGO  13  // D7
  #define PIN_OUTSIDE_TEXT 14  // D5
  #define PIN_OUTSIDE_LOGO 16  // D0

  #define LED_COUNT_LOGO  10
  #define LED_COUNT_TEXT  31

  #define LOGO_LEAF_COLOR   0x0000F10E
  #define LOGO_BUBBLE_COLOR 0x00DF4000
  #define LOGO_PENCIL_COLOR 0x000044AA
  #define LOGO_WRENCH_COLOR 0x002A4422

  #define TEXT_HUE 110
  #define TEXT_SATURATION 0.15

  #define LOGO_LEAF_OFFSET    0
  #define LOGO_BUBBLE_OFFSET 10
  #define LOGO_PENCIL_OFFSET 20
  #define LOGO_WRENCH_OFFSET 30 



  // Konfiguration für Flash
  struct T_ConfigStruct { 
    char    magic[5] = CONF_MAGIC;
    uint8_t version  = CONF_VERSION;
    char    mySSID[32] = CONF_DEFAULT_SSID;
    char    myPWD[64]  = CONF_DEFAULT_PWD;
    char    myHostname[20] = CONF_HOSTNAME;
    char    myAdminUser[20] = CONF_ADMIN_USER;
    char    myAdminPWD[20] = CONF_ADMIN_PWD;
  
    uint8_t myChannel  = CONF_DEFAULT_CHANNEL;
  
  };
