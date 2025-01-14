#include "Image.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <math.h>

/*
 */
BitmapImage::~BitmapImage()
{
  delete[] m_pixels;
  m_pixels = nullptr;
}

/*
 */
void
BitmapImage::create(uint32 width, uint32 height, BPP bpp)
{
  if (width <= 0 || height <= 0)
  {
    std::cerr << "BitmapImage::create() " << "Error: Invalid dimensions (" << width << ", " << height << ")" << std::endl;
    return;
  }

  if (m_pixels)
  {
    delete[] m_pixels;
  }

  m_width = width;
  m_height = height;
  m_bpp = bpp;

  m_bytesPerPixel = static_cast<int32>(m_bpp) / 8;
  m_pitch = m_width * m_bytesPerPixel;

  m_pixels = new uint8[m_pitch * m_height];
}

/*
 */
void 
BitmapImage::clear(const Color &color)
{
  std::vector<uint8> rowBuffer(m_pitch, 0);

  for (int32 x = 0; x < m_width; ++x)
  {
    uint8 *buffer = rowBuffer.data() + x * m_bytesPerPixel;
    writePixel(buffer, color);
  }

  for (int32 y = 0; y < m_height; ++y)
  {
    std::memcpy(m_pixels + y * m_pitch, rowBuffer.data(), m_pitch);
  }
}

/*
 */
Color 
BitmapImage::getPixel(uint32 x, uint32 y) const
{
  if (x >= m_width || y >= m_height)
  {
    std::cerr << "BitmapImage::getPixel() Error: Invalid pixel coordinates (" << x << ", " << y << ")" << std::endl;
    return Color();
  }

  const uint8 *buffer = m_pixels + y * m_pitch + x * m_bytesPerPixel;
  return readPixel(buffer);
}

/*
 */
void 
BitmapImage::setPixel(uint32 x, uint32 y, const Color &color)
{
  if (x < 0 || x >= m_width || y < 0 || y >= m_height)
  {
    std::cerr << "BitmapImage::setPixel() " << "Error: Invalid pixel coordinates (" << x << ", " << y << ")" << std::endl;
    return;
  }

  uint8 *buffer = m_pixels + y * m_pitch + x * m_bytesPerPixel;
  writePixel(buffer, color);
}

/*
 */
Color 
BitmapImage::getColor(float u, float v) const
{
  int32 x = static_cast<int32>(u * (m_width - 1));
  int32 y = static_cast<int32>(v * (m_height - 1));

  return getPixel(x, y);
}

/*
 */
void 
BitmapImage::setColor(float u, float v, const Color &color)
{
  int32 x = static_cast<int32>(u * (m_width - 1));
  int32 y = static_cast<int32>(v * (m_height - 1));

  setPixel(x, y, color);
}

/*
 */
bool 
BitmapImage::decode(const std::string &bmpPath)
{
  std::fstream file(bmpPath, std::ios::in | std::ios::binary);
  if (!file.is_open())
  {
    std::cerr << "Error: Unable to open file " << bmpPath << std::endl;
    return false;
  }

  BMPHeader header;
  file.read(reinterpret_cast<char *>(&header), sizeof(BMPHeader));

  if (header.signature[0] != 'B' || header.signature[1] != 'M')
  {
    std::cerr << "BitmapImage::decode() " << "Error: Invalid BMP file format." << std::endl;
    file.close();
    return false;
  }

  BMPInfoHeaderCore infoHeader;
  file.read(reinterpret_cast<char *>(&infoHeader), sizeof(BMPInfoHeaderCore));

  create(infoHeader.width, infoHeader.height, static_cast<BPP>(infoHeader.bpp));

  int32 padding = m_pitch % 4;
  int32 lineMemoryWidth = m_pitch;
  if (padding != 0)
  {
    lineMemoryWidth += 4 - padding;
  }

  for (int32 y = m_height - 1; y >= 0; --y)
  {
    file.seekp(header.dataOffset + y * lineMemoryWidth);
    file.read(reinterpret_cast<char *>(m_pixels + m_pitch * (m_height - 1 - y)), m_pitch);
  }

  file.close();
  return true;
}

/*
 */
void 
BitmapImage::encode(const std::string &filename) const
{
  std::fstream file(filename + ".bmp", std::ios::out | std::ios::binary);
  if (!file.is_open())
  {
    std::cerr << "BitmapImage::encode() " << "Error: Unable to open file " << filename << std::endl;
    return;
  }

  int32 padding = m_pitch % 4;
  int32 lineMemoryWidth = m_pitch;
  if (padding != 0)
  {
    padding = 4 - padding;
    lineMemoryWidth += padding;
  }

  BMPHeader header;
  header.signature[0] = 'B';
  header.signature[1] = 'M';
  header.fileSize = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + lineMemoryWidth * m_height;
  header.reserved = 0;
  header.dataOffset = sizeof(BMPHeader) + sizeof(BMPInfoHeader);

  file.write(reinterpret_cast<const char *>(&header), sizeof(BMPHeader));

  BMPInfoHeader infoHeader;
  infoHeader.core.headerSize = sizeof(BMPInfoHeader);
  infoHeader.core.width = m_width;
  infoHeader.core.height = m_height;
  infoHeader.core.planes = 1;
  infoHeader.core.bpp = static_cast<int>(m_bpp);

  infoHeader.compression = 0;
  infoHeader.imageSize = 0;
  infoHeader.xPixelsPerMeter = 3780; // 96 dpi
  infoHeader.yPixelsPerMeter = 3780;
  infoHeader.colorsUsed = 0;
  infoHeader.importantColors = 0;

  file.write(reinterpret_cast<const char *>(&infoHeader), sizeof(BMPInfoHeader));

  const char paddBuffer[3] = {0, 0, 0};
  for (int y = m_height - 1; y >= 0; --y)
  {
    file.write(reinterpret_cast<const char *>(m_pixels + y * m_pitch), m_pitch);

    if (padding != 0)
    {
      file.write(paddBuffer, padding);
    }
  }

  file.close();
}

/*
 */
void 
BitmapImage::bitBlt(const BitmapImage &src,
                    const Rect &srcRect,
                    const Rect &dstRect,
                    const TextureMode mode,
                    const Color &colorKey)
{
  Rect srcRectClamped = srcRect;
  srcRectClamped.clamp(Rect(0, 0, src.m_width, src.m_height));

  Rect dstRectClamped = dstRect;
  dstRectClamped.clamp(Rect(0, 0, m_width, m_height));

  for (int32 y = 0; y < dstRectClamped.height; ++y)
  {
    for (int32 x = 0; x < dstRectClamped.width; ++x)
    {
      int32 srcX = 0, srcY = 0;
      if (!calculateSourceCoordinates(x, y, srcRectClamped, dstRectClamped, mode, srcX, srcY))
      {
        continue;
      }

      Color color = src.getPixel(srcX, srcY);
      if (color == colorKey)
      {
        continue;
      }

      setPixel(dstRectClamped.x + x, dstRectClamped.y + y, color);
    }
  }
}

/*
 */
bool 
BitmapImage::calculateSourceCoordinates(int32 x, int32 y,
                                        const Rect &srcRect, 
                                        const Rect &dstRect,
                                        TextureMode mode,
                                        int32 &srcX, int32 &srcY)
{
  switch (mode)
  {
  case TextureMode::NONE:
    srcX = srcRect.x + x;
    srcY = srcRect.y + y;

    return srcX >= 0 && srcX < static_cast<int32>(srcRect.width) &&
          srcY >= 0 && srcY < static_cast<int32>(srcRect.height);

  case TextureMode::REPEAT:
    return calculateRepeatCoordinates(x, y, srcRect, srcX, srcY);

  case TextureMode::CLAMP:
    return calculateClampCoordinates(x, y, srcRect, srcX, srcY);

  case TextureMode::MIRROR:
    return calculateMirrorCoordinates(x, y, srcRect, srcX, srcY);

  case TextureMode::STRETCH:
    return calculateStretchCoordinates(x, y, srcRect, dstRect, srcX, srcY);

  default:
    std::cerr << "BitmapImage::calculateSourceCoordinates() Error: Unsupported TextureMode" << std::endl;
    return false;
  }
}

/*
*/
bool 
BitmapImage::calculateRepeatCoordinates(int32 x, int32 y, 
                                        const Rect &srcRect,
                                        int32 &srcX, int32 &srcY)
{
  srcX = (srcRect.x + x) % srcRect.width;
  if (srcX < 0)
  {
    srcX += srcRect.width;
  }

  srcY = (srcRect.y + y) % srcRect.height;
  if (srcY < 0)
  {
    srcY += srcRect.height;
  }

  return true;
}

/*
*/
bool 
BitmapImage::calculateClampCoordinates(int32 x, int32 y, 
                                       const Rect &srcRect,
                                       int32 &srcX, int32 &srcY)
{
  srcX = std::max(0, std::min(static_cast<int32>(srcRect.x + x), static_cast<int32>(srcRect.width - 1)));
  srcY = std::max(0, std::min(static_cast<int32>(srcRect.y + y), static_cast<int32>(srcRect.height - 1)));

  return true;
}

/*
*/
bool 
BitmapImage::calculateMirrorCoordinates(int32 x, int32 y, 
                                        const Rect &srcRect,
                                        int32 &srcX, int32 &srcY)
{
    srcX = static_cast<int32>(srcRect.x) + x;
    srcY = static_cast<int32>(srcRect.y) + y;

    if (srcX < 0)
    {
        srcX = -srcX;
    }
    if (srcX >= static_cast<int32>(srcRect.width))
    {
        srcX %= (2 * static_cast<int32>(srcRect.width));
        if (srcX >= static_cast<int32>(srcRect.width))
        {
            srcX = 2 * static_cast<int32>(srcRect.width) - srcX - 1;
        }
    }

    if (srcY < 0)
    {
        srcY = -srcY;
    }
    if (srcY >= static_cast<int32>(srcRect.height))
    {
        srcY %= (2 * static_cast<int32>(srcRect.height));
        if (srcY >= static_cast<int32>(srcRect.height))
        {
            srcY = 2 * static_cast<int32>(srcRect.height) - srcY - 1;
        }
    }

    if (srcX < 0 || srcX >= static_cast<int32>(srcRect.width) ||
        srcY < 0 || srcY >= static_cast<int32>(srcRect.height))
    {
        std::cerr << "calculateMirrorCoordinates() Error: Coordenadas fuera de rango después del cálculo." << std::endl;
        return false;
    }

    return true;
}

/*
*/
bool 
BitmapImage::calculateStretchCoordinates(int32 x, int32 y, 
                                         const Rect &srcRect, 
                                         const Rect &dstRect,
                                         int32 &srcX, int32 &srcY)
{
  const float scaleX = static_cast<float>(srcRect.width) / dstRect.width;
  const float scaleY = static_cast<float>(srcRect.height) / dstRect.height;

  srcX = static_cast<int32>(x * scaleX) + srcRect.x;
  srcY = static_cast<int32>(y * scaleY) + srcRect.y;

  return (srcX >= 0 && srcX < srcRect.width && srcY >= 0 && srcY < srcRect.height);
}

/*
 */
void
BitmapImage::resize(uint32 width, uint32 height)
{
  if ((width == m_width && height == m_height) || width <= 0 || height <= 0)
  {
    return;
  }

  const float scaleX = 1 / static_cast<float>(width - 1);
  const float scaleY = 1 / static_cast<float>(height - 1);

  BitmapImage temp;
  temp.create(width, height, m_bpp);

  // #pragma omp parallel for
  for (int32 y = 0; y < height; ++y)
  {
    float v = y * scaleY;
    for (int32 x = 0; x < width; ++x)
    {
      float u = x * scaleX;
      temp.setPixel(x, y, getColor(u, v));
    }
  }

  std::swap(m_pixels, temp.m_pixels);
  std::swap(m_width, temp.m_width);
  std::swap(m_height, temp.m_height);
  std::swap(m_bpp, temp.m_bpp);
  std::swap(m_bytesPerPixel, temp.m_bytesPerPixel);
  std::swap(m_pitch, temp.m_pitch);
}

/*
 */
void 
BitmapImage::writePixel(uint8 *buffer, const Color &color)
{
  switch (m_bpp)
  {
  case BPP::BPP_16:
  {
    uint16 pixel = color.to16Bit(true);
    buffer[0] = pixel & 0xFF;
    buffer[1] = (pixel >> 8) & 0xFF;
    break;
  }
  case BPP::BPP_24:
    buffer[0] = color.b;
    buffer[1] = color.g;
    buffer[2] = color.r;
    break;
  case BPP::BPP_32:
    buffer[0] = color.b;
    buffer[1] = color.g;
    buffer[2] = color.r;
    buffer[3] = color.a;
    break;
  default:
    std::cerr << "BitmapImage::writePixel() " << "Error: Unsupported BPP format in writePixel." << std::endl;
    break;
  }
}

/*
 */
Color 
BitmapImage::readPixel(const uint8 *buffer) const
{
  switch (m_bpp)
  {
  case BPP::BPP_16:
    return Color::from16Bit(*reinterpret_cast<const uint16 *>(buffer), true);
    break;
  case BPP::BPP_24:
  {
    Color color;
    color.b = buffer[0];
    color.g = buffer[1];
    color.r = buffer[2];
    return color;
    break;
  }
  case BPP::BPP_32:
  {
    Color color;
    color.b = buffer[0];
    color.g = buffer[1];
    color.r = buffer[2];
    color.a = buffer[3];
    return color;
    break;
  }
  default:
    std::cerr << "BitmapImage::readPixel() " << "Error: Unsupported BPP format in readPixel." << std::endl;
    return Color();
    break;
  }
}