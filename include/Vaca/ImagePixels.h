// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the author nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

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

  const pixel_type& operator[](int index) const {
    assert(index >= 0 && index < m_buffer.size());
    return m_buffer[index];
  }

  pixel_type& operator[](int index) {
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
