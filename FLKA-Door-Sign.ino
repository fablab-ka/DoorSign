 #include <Adafruit_NeoPixel.h>  // https://github.com/adafruit/Adafruit_NeoPixel

  extern "C" {
    #include "user_interface.h"
  }
  #include "rgb_hsl.h"
  
  // Basiskonfigurationen ==============================================================================//
  
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



void setup(){
  pinMode( 0,OUTPUT);
  digitalWrite(0, HIGH);
  Serial.begin(115200);
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


void loop() {
  static uint8_t  rotation = 0;
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
