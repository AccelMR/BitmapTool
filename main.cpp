#include <iostream>

#include "Image.h"
#include "Color.h"

int main()
{
  std::cout << "Sizeof Bitmap = " << sizeof(BitmapImage) << std::endl;

  BitmapImage source;
  source.decode("/home/accelmr/Pictures/test.bmp");

  BitmapImage dest;
  uint32 destWidth = source.getWidth() * 2;
  uint32 destHeight = source.getHeight() * 2;
  dest.create(destWidth, destHeight, BPP::BPP_32);

  Rect srcRect(0, 0, source.getWidth(), source.getHeight());
  Rect dstRect(0, 0, destWidth, destHeight);

  dest.bitBlt(source, srcRect, dstRect, TextureMode::NONE);
  dest.encode("/home/accelmr/Pictures/destNone");
  dest.clear(Color::Transparent);

  dest.bitBlt(source, srcRect, dstRect, TextureMode::REPEAT);
  dest.encode("/home/accelmr/Pictures/destRepeat");
  dest.clear(Color::Transparent);

  dest.bitBlt(source, srcRect, dstRect, TextureMode::CLAMP);
  dest.encode("/home/accelmr/Pictures/destClamp");
  dest.clear(Color::Transparent);

  dest.bitBlt(source, srcRect, dstRect, TextureMode::MIRROR);
  dest.encode("/home/accelmr/Pictures/destMirror");
  dest.clear(Color::Transparent);

  dest.bitBlt(source, srcRect, dstRect, TextureMode::STRETCH);
  dest.encode("/home/accelmr/Pictures/destStretch");
  dest.clear(Color::Transparent);

  return 0;
}