#include "Color.h"

const Color Color::Black(0, 0, 0);
const Color Color::Transparent(0, 0, 0, 0);
const Color Color::White(255, 255, 255);
const Color Color::Red(255, 0, 0);
const Color Color::Green(0, 255, 0);
const Color Color::Blue(0, 0, 255);

uint16
Color::to16Bit(bool isRGB565) const
{
  const float RGB555 =  31 / 255;
  const float RGB565 =  63 / 255;
  if (isRGB565)
  {
    uint8 r = r * RGB555;
    uint8 g = g * RGB565;
    uint8 b = b * RGB555;
    return (r << 11) | (g << 5) | b;
  }
  else
  {
    uint8 r = r * RGB555;
    uint8 g = g * RGB555;
    uint8 b = b * RGB555;
    return (r << 10) | (g << 5) | b;
  }
}

Color 
Color::from16Bit(uint16_t value, bool isRGB565)
{
  Color color;
  const float RGB555 =  255 / 31;
  const float RGB565 =  255 / 63;

  if (isRGB565)
  {
    color.r = ((value >> 11) & 0x1F) * RGB555;
    color.g = ((value >> 5) & 0x3F) * RGB565;
    color.b = (value & 0x1F) * RGB555;
  }
  else
  {
    color.r = ((value >> 10) & 0x1F) * RGB555;
    color.g = ((value >> 5) & 0x1F) * RGB555;
    color.b = (value & 0x1F) * RGB555;
  }

  color.a = 255;
  return color;
}
