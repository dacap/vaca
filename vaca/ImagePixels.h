// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_IMAGEPIXELS_H
#define VACA_IMAGEPIXELS_H

#include <vector>
#include <algorithm>

#include "Vaca/base.h"
#include "Vaca/Size.h"
#include "Vaca/Point.h"
#include "Vaca/SharedPtr.h"

namespace Vaca {

class ImagePixelsHandle : public Referenceable
{
public:
  typedef UINT32 pixel_type;

private:
  int m_width;
  int m_height;
  int m_scanline;
  std::vector<pixel_type> m_buffer;

public:
  ImagePixelsHandle() { init(0, 0); }
  ImagePixelsHandle(int w, int h) { init(w, h); }
  ImagePixelsHandle(const Size& sz) { init(sz.w, sz.h); }
  virtual ~ImagePixelsHandle() { }

  Size getSize() const { return Size(m_width, m_height); }
  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }
  int getScanlineSize() const { return m_scanline; }

  const pixel_type& operator[](size_t index) const {
    assert(index >= 0 && index < m_buffer.size());
    return m_buffer[index];
  }

  pixel_type& operator[](size_t index) {
    assert(index >= 0 && index < m_buffer.size());
    return m_buffer[index];
  }

  pixel_type getPixel(int x, int y) const {
    assert(x >= 0 && y >= 0 && x < m_width && y < m_height);
    return m_buffer[y*m_scanline + x];
  }

  void setPixel(int x, int y, pixel_type color) {
    assert(x >= 0 && y >= 0 && x < m_width && y < m_height);
    m_buffer[y*m_scanline + x] = color;
  }

  void invertScanlines()
  {
    std::vector<pixel_type> scanline(m_scanline);
    int top = 0;
    int bottom = (m_height-1)*m_scanline;
    for (int y=0; y<m_height/2; ++y) {
      // Top to temporary buffer
      std::copy(m_buffer.begin()+top, m_buffer.begin()+top+m_scanline, scanline.begin());

      // Bottom to top
      std::copy(m_buffer.begin()+bottom, m_buffer.begin()+bottom+m_scanline, m_buffer.begin()+top);

      // Temporary to bottom
      std::copy(scanline.begin(), scanline.end(), m_buffer.begin()+bottom);

      top += m_scanline;
      bottom -= m_scanline;
    }
  }

  void copyTo(ImagePixelsHandle& other) const
  {
    std::copy(m_buffer.begin(), m_buffer.begin(), other.m_buffer.begin());
  }

private:
  void init(int w, int h)
  {
    m_width = w;
    m_scanline = w;
    m_height = h;
    m_buffer.resize(m_scanline * m_height);
  }
};

/**
   A set of pixels obtained from a Image.

   Each pixel has four channels: RGBA. This class is useful to
   manipulate image pixels directly.
*/
class ImagePixels : private SharedPtr<ImagePixelsHandle>
{
public:
  typedef ImagePixelsHandle::pixel_type pixel_type;

  ImagePixels()
    : SharedPtr<ImagePixelsHandle>(new ImagePixelsHandle())
  {
  }

  ImagePixels(int w, int h)
    : SharedPtr<ImagePixelsHandle>(new ImagePixelsHandle(w, h))
  {
  }

  ImagePixels(const Size& sz)
    : SharedPtr<ImagePixelsHandle>(new ImagePixelsHandle(sz))
  {
  }

  virtual ~ImagePixels()
  {
  }

  ImagePixels clone() const
  {
    ImagePixels copy(getSize());
    get()->copyTo(*copy.get());
    return copy;
  }

  Size getSize() const { return get()->getSize(); }
  int getWidth() const { return get()->getWidth(); }
  int getHeight() const { return get()->getHeight(); }
  int getScanlineSize() const { return get()->getScanlineSize(); }

  const pixel_type& operator[](int index) const {
    return get()->operator[](index);
  }

  pixel_type& operator[](int index) {
    return get()->operator[](index);
  }

  pixel_type getPixel(int x, int y) const {
    return get()->getPixel(x, y);
  }

  void setPixel(int x, int y, pixel_type color) {
    get()->setPixel(x, y, color);
  }

  void invertScanlines() {
    get()->invertScanlines();
  }

  static int getR(pixel_type color) { return (color & 0x00ff0000) >> 16; }
  static int getG(pixel_type color) { return (color & 0x0000ff00) >> 8; }
  static int getB(pixel_type color) { return (color & 0x000000ff); }
  static int getA(pixel_type color) { return (color & 0xff000000) >> 24; }

  static pixel_type makePixel(int r, int g, int b, int a) {
    return
      ((a & 0xff) << 24) |
      ((r & 0xff) << 16) |
      ((g & 0xff) << 8) |
      ((b & 0xff));
  }

};

} // namespace Vaca

#endif // VACA_IMAGEPIXELS_H
