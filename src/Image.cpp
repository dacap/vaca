// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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
// * Neither the name of the Vaca nor the names of its contributors
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

#include "stdvaca.h"
#include "Vaca/Image.h"
#include "Vaca/Debug.h"
#include "Vaca/Graphics.h"

using namespace Vaca;

Image::Image()
{
  mFromHdc = NULL;
  mSize = Size(0, 0);
  mHbitmap = NULL;
  mGraphics = NULL;
}

Image::Image(const Size &sz)
{
  VACA_ASSERT(sz.w > 0 && sz.h > 0);

  mFromHdc = GetDC(GetDesktopWindow());
  mSize = sz;
  mHbitmap = CreateCompatibleBitmap(mFromHdc, sz.w, sz.h);
  mGraphics = NULL;
}

Image::Image(const Size &sz, int bpp)
{
  VACA_ASSERT(sz.w > 0 && sz.h > 0);

  BITMAPINFOHEADER bhdr;
  bhdr.biSize = sizeof(BITMAPINFOHEADER);
  bhdr.biWidth = sz.w;
  bhdr.biHeight = -sz.h;
  bhdr.biPlanes = 1;
  bhdr.biBitCount = bpp;
  bhdr.biCompression = BI_RGB;
  bhdr.biSizeImage = 0;
  bhdr.biXPelsPerMeter = 0;
  bhdr.biYPelsPerMeter = 0;
  bhdr.biClrUsed = 0;
  bhdr.biClrImportant = 0;

  BITMAPINFO binf;
  RGBQUAD dummy = { 0, 0, 0, 0 };
  binf.bmiColors[0] = dummy;
  binf.bmiHeader = bhdr;

  // TODO use getDesktopGraphics
  char *bits = NULL;

  mFromHdc = GetDC(GetDesktopWindow());
  mSize = sz;
  mHbitmap = CreateDIBSection(mFromHdc, &binf, DIB_RGB_COLORS,
			      reinterpret_cast<void**>(&bits),
			      NULL, 0);
  mGraphics = NULL;
}

Image::Image(Graphics &g, const Size &sz)
{
  VACA_ASSERT(g.getHdc() != NULL);
  VACA_ASSERT(sz.w > 0 && sz.h > 0);

  mFromHdc = g.getHdc();
  mSize = sz;
  mHbitmap = CreateCompatibleBitmap(mFromHdc, mSize.w, mSize.h);
  mGraphics = NULL;
}

Image::Image(const Image &image)
{
  mFromHdc = image.mFromHdc;
  mSize = image.getSize();
  mHbitmap = CreateCompatibleBitmap(mFromHdc, mSize.w, mSize.h);
  mGraphics = NULL;

  image.copyTo(*this);
}

Image::~Image()
{
  destroy();
}  

bool Image::isValid()
{
  return mHbitmap != NULL;
}

int Image::getWidth() const
{
  return mSize.w;
}

int Image::getHeight() const
{
  return mSize.h;
}

Size Image::getSize() const
{
  return mSize;
}

Graphics &Image::getGraphics()
{
  if (mGraphics == NULL) {
    VACA_ASSERT(mFromHdc != NULL);

    mGraphics = new Graphics(mFromHdc, *this);
  }

  return *mGraphics;
}

HBITMAP Image::getHbitmap()
{
  return mHbitmap;
}

Image &Image::operator=(const Image &image)
{
  destroy();

  mFromHdc = image.mFromHdc;
  mSize = image.getSize();
  mHbitmap = CreateCompatibleBitmap(mFromHdc, mSize.w, mSize.h);
  mGraphics = NULL;

  image.copyTo(*this);
  return *this;
}

void Image::destroy()
{
  if (mHbitmap != NULL) {
    DeleteObject(mHbitmap);
    mHbitmap = NULL;
  }

  if (mGraphics != NULL) {
    delete mGraphics;
    mGraphics = NULL;
  }
}

void Image::copyTo(Image &image) const
{
  image.getGraphics().drawImage(*const_cast<Image *>(this), 0, 0);
}
