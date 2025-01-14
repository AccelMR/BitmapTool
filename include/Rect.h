#pragma once 

#include "Prerequisites.h"

struct Rect
{
  Rect() : x(0), y(0), width(0), height(0) {}
  Rect(uint32 _x, uint32 _y, uint32 _width, uint32 _height) 
    : x(_x), y(_y), width(_width), height(_height) {}
  ~Rect() {}

  inline void 
  clamp(const Rect& rect)
  {
      if (x < rect.x) { x = rect.x; }
      if (y < rect.y) { y = rect.y; }
      if (x + width > rect.x + rect.width) { width = rect.x + rect.width - x; }
      if (y + height > rect.y + rect.height) { height = rect.y + rect.height - y; }
  }

  uint32 x;
  uint32 y;
  uint32 width;
  uint32 height;
};