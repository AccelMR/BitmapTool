#pragma once

#include "Prerequisites.h"

class Color
{
public:
  Color() : r(0), g(0), b(0), a(255) {}
  Color(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 255) : r(_r), g(_g), b(_b), a(_a){}
  ~Color() {}

  /*
   * Convert the color to a 16-bit value
   * @param isRGB565: true if the color should be converted to RGB565, false if it should be converted to RGB555
   * @return: 16-bit value
   */
  uint16
  to16Bit(bool isRGB565 = true) const;

  /*
   * Convert a 16-bit value to a color
   * @param value: 16-bit value
   * @param isRGB565: true if the value is in RGB565 format, false if it is in RGB555 format
   * @return: color
   */
  static Color 
  from16Bit(uint16_t value, bool isRGB565 = true);

  /*
   * Compare two colors
   * @param color: color to compare with
   * @return: true if the colors are equal, false otherwise
   */
  inline bool
  operator==(const Color &color) const
  {
    return r == color.r && g == color.g && b == color.b && a == color.a;
  }

  static const Color Black;
  static const Color White;
  static const Color Red;
  static const Color Green;
  static const Color Blue;
  static const Color Transparent;

  uint8 r;
  uint8 g;
  uint8 b;
  uint8 a;
};


