#include "rgb_hsl.h"

RGB::RGB(uint8_t r, uint8_t g, uint8_t b):_r(r),_g(g),_b(b) {
};
RGB::RGB():_r(0),_g(0),_b(0) {
};

bool RGB::Equals(RGB rgb)  {
    return (_r == rgb._r) && (_g == rgb._g) && (_b == rgb._b);
};

uint32_t RGB::NeoColor() {
  return ((uint32_t)_r << 16) | ((uint32_t)_g << 8) | _b;
}

HSL::HSL(uint16_t h, float s, float l):_h(h),_s(s),_l(l) {
};

HSL::HSL():_h(0),_s(0),_l(0) {
}

bool HSL::Equals(HSL hsl)  {
    return (_h == hsl._h) && (_s == hsl._s) && (_l == hsl._l);
};

float HueToRGB(float v1, float v2, float vh) {
  if (vh < 0)
    vh += 1;

  if (vh > 1)
    vh -= 1;

  if ((6 * vh) < 1)
    return (v1 + (v2 - v1) * 6 * vh);

  if ((2 * vh) < 1)
    return v2;

  if ((3 * vh) < 2)
    return (v1 + (v2 - v1) * ((2.0f / 3) - vh) * 6);

  return v1;
}

RGB HSLToRGB(HSL hsl) {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;

  if (hsl._s == 0)
  {
    r = g = b = (uint8_t)(hsl._l * 255);
  }
  else
  {
    float v1, v2;
    float hue = (float)hsl._h / 360;

    v2 = (hsl._l < 0.5) ? (hsl._l * (1 + hsl._s)) : ((hsl._l + hsl._s) - (hsl._l * hsl._s));
    v1 = 2 * hsl._l - v2;

    r = (uint8_t)(255 * HueToRGB(v1, v2, hue + (1.0f / 3)));
    g = (uint8_t)(255 * HueToRGB(v1, v2, hue));
    b = (uint8_t)(255 * HueToRGB(v1, v2, hue - (1.0f / 3)));
  }

  return RGB(r, g, b);
}


float Min(float a, float b) {
  return a <= b ? a : b;
}

float Max(float a, float b) {
  return a >= b ? a : b;
}

HSL RGBToHSL(RGB rgb) {
  HSL hsl = HSL(0, 0, 0);

  float r = (rgb._r / 255.0f);
  float g = (rgb._g/ 255.0f);
  float b = (rgb._b / 255.0f);

  float min = Min(Min(r, g), b);
  float max = Max(Max(r, g), b);
  float delta = max - min;

  hsl._l = (max + min) / 2;

  if (delta <= 0.001)
  {
    hsl._h = 0;
    hsl._s = 0.0f;
  }
  else
  {
    hsl._s = (hsl._l <= 0.5) ? (delta / (max + min)) : (delta / (2 - max - min));

    float hue;

    if (r == max)
    {
      hue = ((g - b) / 6) / delta;
    }
    else if (g == max)
    {
      hue = (1.0f / 3) + ((b - r) / 6) / delta;
    }
    else
    {
      hue = (2.0f / 3) + ((r - g) / 6) / delta;
    }

    if (hue < 0)
      hue += 1;
    if (hue > 1)
      hue -= 1;

    hsl._h = (int)(hue * 360);
  }

  return hsl;
}
