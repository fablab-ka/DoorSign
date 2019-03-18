#include <stdint.h>
#pragma once

class RGB {
  public:
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;
    RGB(uint8_t r, uint8_t g, uint8_t b);
    RGB();
    uint32_t NeoColor();
    bool Equals(RGB rgb);
};

class HSL {
  public:
    uint16_t _h;
    float _s;
    float _l;
    HSL(uint16_t h, float s, float l);
    HSL();
    bool Equals(HSL hsl);
};

float HueToRGB(float v1, float v2, float vh);
RGB HSLToRGB(HSL hsl);
float Min(float a, float b);
float Max(float a, float b);
HSL RGBToHSL(RGB rgb);
