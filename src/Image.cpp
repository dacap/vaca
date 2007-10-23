// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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
#include "Vaca/Application.h"
#include "Vaca/ResourceException.h"

using namespace Vaca;

Image::Image()
{
  m_fromHDC = NULL;
  m_size = Size(0, 0);
  m_HBITMAP = NULL;
  m_graphics = NULL;
}

/**
 * Loads an image from the resource specified by @a imageId. This
 * constructor uses Win32 LoadBitmap.
 * 
 */
Image::Image(ResourceId imageId)
{
  m_fromHDC = GetDC(GetDesktopWindow());

  // HBITMAP
  m_HBITMAP = LoadBitmap(Application::getHINSTANCE(), imageId.toLPTSTR());

  if (m_HBITMAP == NULL)
    throw ResourceException("Can't load the image resource " + imageId.toString());

  // get the size from the m_HBITMAP
  BITMAPCOREHEADER bc;
  ZeroMemory(&bc, sizeof(bc));
  bc.bcSize = sizeof(bc);
  GetDIBits(m_fromHDC, m_HBITMAP, 0, 0, NULL,
	    reinterpret_cast<BITMAPINFO *>(&bc), 0);
  m_size = Size(bc.bcWidth, bc.bcHeight);

  // graphics
  m_graphics = NULL;
}

Image::Image(const String &fileName)
{
  m_fromHDC = GetDC(GetDesktopWindow());

  // file name size
  int size = fileName.size()+1;
  LPTSTR lpstr = new TCHAR[size];
  fileName.copyTo(lpstr, size);

  // HBITMAP
  m_HBITMAP = reinterpret_cast<HBITMAP>
    (LoadImage(Application::getHINSTANCE(),
	       lpstr,
	       IMAGE_BITMAP,
	       0, 0,
	       LR_LOADFROMFILE));
  delete[] lpstr;

  if (m_HBITMAP == NULL)
    throw ResourceException("Can't load the image from file " + fileName);

  // get the size from the m_HBITMAP
  BITMAPCOREHEADER bc;
  ZeroMemory(&bc, sizeof(bc));
  bc.bcSize = sizeof(bc);
  GetDIBits(m_fromHDC, m_HBITMAP, 0, 0, NULL,
	    reinterpret_cast<BITMAPINFO *>(&bc), 0);
  m_size = Size(bc.bcWidth, bc.bcHeight);

  // graphics
  m_graphics = NULL;
}

Image::Image(const Size &sz)
{
  assert(sz.w > 0 && sz.h > 0);

  m_fromHDC = GetDC(GetDesktopWindow());
  m_size = sz;
  m_HBITMAP = CreateCompatibleBitmap(m_fromHDC, sz.w, sz.h);
  m_graphics = NULL;
}

Image::Image(const Size &sz, int bpp)
{
  assert(sz.w > 0 && sz.h > 0);

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

  char *bits = NULL;
  m_fromHDC = GetDC(GetDesktopWindow());
  m_size = sz;
  m_HBITMAP = CreateDIBSection(m_fromHDC, &binf, DIB_RGB_COLORS,
			       reinterpret_cast<void**>(&bits),
			       NULL, 0);
  m_graphics = NULL;
}

Image::Image(const Size &sz, Graphics &g)
{
  assert(g.getHDC() != NULL);
  assert(sz.w > 0 && sz.h > 0);

  m_fromHDC = g.getHDC();
  m_size = sz;
  m_HBITMAP = CreateCompatibleBitmap(m_fromHDC, m_size.w, m_size.h);
  m_graphics = NULL;
}

Image::Image(const Image &image)
{
  m_fromHDC = image.m_fromHDC;
  m_size = image.getSize();
  m_HBITMAP = CreateCompatibleBitmap(m_fromHDC, m_size.w, m_size.h);
  m_graphics = NULL;

  image.copyTo(*this);
}

Image::~Image()
{
  destroy();
}  

bool Image::isValid()
{
  return m_HBITMAP != NULL;
}

int Image::getWidth() const
{
  return m_size.w;
}

int Image::getHeight() const
{
  return m_size.h;
}

Size Image::getSize() const
{
  return m_size;
}

/**
 * Returns a Graphics context to draw inside the image.
 *
 * @warning You can't delete the returned pointer.
 */
Graphics *Image::getGraphics()
{
  if (m_graphics == NULL) {
    assert(m_fromHDC != NULL);
    m_graphics = new Graphics(m_fromHDC, *this);
  }
  return m_graphics;
}

HBITMAP Image::getHBITMAP()
{
  return m_HBITMAP;
}

Image &Image::operator=(const Image &image)
{
  destroy();

  m_fromHDC = image.m_fromHDC;
  m_size = image.getSize();
  m_HBITMAP = CreateCompatibleBitmap(m_fromHDC, m_size.w, m_size.h);
  m_graphics = NULL;

  image.copyTo(*this);
  return *this;
}

void Image::destroy()
{
  if (m_HBITMAP != NULL) {
    DeleteObject(m_HBITMAP);
    m_HBITMAP = NULL;
  }

  if (m_graphics != NULL) {
    delete m_graphics;
    m_graphics = NULL;
  }
}

void Image::copyTo(Image &image) const
{
  image.getGraphics()->drawImage(*const_cast<Image *>(this), 0, 0);
}
