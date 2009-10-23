#include <cassert>

#include "Vaca/Image.h"
#include "Vaca/Graphics.h"
#include "Vaca/Color.h"
#include "Vaca/Brush.h"

using namespace Vaca;

#define assert_pixel(pixels, x, y, r, g, b)			\
  {								\
    ImagePixels::pixel_type color = pixels.getPixel(x, y);	\
    assert(ImagePixels::getR(color) == r);			\
    assert(ImagePixels::getG(color) == g);			\
    assert(ImagePixels::getB(color) == b);			\
  }

void test_pointers()
{
  Image img1(32, 32);
  Image img2 = img1;		// img1 and img2 references the same Image
  Image img3 = img1.clone();
  assert(img1 == img2);
  assert(img1 != img3);
}

void test_pixels()
{
  Image img(32, 32);
  Graphics& g = img.getGraphics();
  g.fillRect(Brush(Color(64, 128, 255)), g.getClipBounds());

  ImagePixels pixels = img.getPixels();
  for (int y=0; y<pixels.getHeight(); ++y)
    for (int x=0; x<pixels.getWidth(); ++x)
      assert_pixel(pixels, x, y, 64, 128, 255);
}

int main()
{
  test_pointers();
  test_pixels();
  return 0;
}
