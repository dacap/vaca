#include <gtest/gtest.h>

#include "Vaca/Image.h"
#include "Vaca/Graphics.h"
#include "Vaca/Color.h"
#include "Vaca/Brush.h"

using namespace Vaca;

inline std::ostream& operator<<(std::ostream& os, const Color& color)
{
  return os << "Color(" 
	    << color.getR() << ", "
	    << color.getG() << ", "
	    << color.getB() << ")\n";
}

#define EXPECT_PIXEL(pixels, x, y, r, g, b)			\
  {								\
    ImagePixels::pixel_type color = pixels.getPixel(x, y);	\
    EXPECT_EQ(r, ImagePixels::getR(color));			\
    EXPECT_EQ(g, ImagePixels::getG(color));			\
    EXPECT_EQ(b, ImagePixels::getB(color));			\
  }

TEST(Image, Pointers)
{
  Image img1(32, 32);
  Image img2 = img1;		// img1 and img2 references the same Image
  Image img3 = img1.clone();
  assert(img1 == img2);
  assert(img1 != img3);
}

TEST(Image, GetPixels)
{
  Image img(32, 32);

  Graphics& g = img.getGraphics();
  g.fillRect(Brush(Color(64, 128, 255)), g.getClipBounds());
  ImagePixels pixels = img.getPixels();
  for (int y=0; y<pixels.getHeight(); ++y)
    for (int x=0; x<pixels.getWidth(); ++x)
      EXPECT_PIXEL(pixels, x, y, 64, 128, 255);

  g.setPixel(0, 0, Color(255, 0, 0));
  g.setPixel(1, 0, Color(0, 255, 0));
  g.setPixel(2, 0, Color(0, 0, 255));

  EXPECT_EQ(Color(255, 0, 0), g.getPixel(0, 0));
  EXPECT_EQ(Color(0, 255, 0), g.getPixel(1, 0));
  EXPECT_EQ(Color(0, 0, 255), g.getPixel(2, 0));

  pixels = img.getPixels();
  EXPECT_PIXEL(pixels, 0, 0, 255, 0, 0);
  EXPECT_PIXEL(pixels, 1, 0, 0, 255, 0);
  EXPECT_PIXEL(pixels, 2, 0, 0, 0, 255);
}
