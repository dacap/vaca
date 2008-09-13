// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#include "Vaca/Image.h"
#include "Vaca/Debug.h"
#include "Vaca/Graphics.h"
#include "Vaca/Application.h"
#include "Vaca/ResourceException.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

ImageHandle::ImageHandle()
{
  m_hdc = NULL;
  m_graphics = NULL;
}

ImageHandle::ImageHandle(HBITMAP handle)
  : GdiObject<HBITMAP>(handle)
{
  m_hdc = NULL;
  m_graphics = NULL;
}

ImageHandle::~ImageHandle()
{
  delete m_graphics;
}

//////////////////////////////////////////////////////////////////////

Image::Image()
  : SmartPtr<ImageHandle>(new ImageHandle())
{
}

/**
 * Loads an image from the resource specified by @a imageId. This
 * constructor uses Win32 LoadBitmap.
 */
Image::Image(ResourceId imageId)
  : SmartPtr<ImageHandle>(new ImageHandle())
{
  // HBITMAP
  HBITMAP hbmp = LoadBitmap(Application::getHandle(), imageId.toLPTSTR());
  if (hbmp == NULL)
    throw ResourceException("Can't load the image resource " + imageId.toString());

  get()->m_hdc = GetDC(GetDesktopWindow());
  get()->setHandle(hbmp);
}

Image::Image(const String& fileName)
  : SmartPtr<ImageHandle>(new ImageHandle())
{
  // file name size
  int size = fileName.size()+1;
  LPTSTR lpstr = new TCHAR[size];
  fileName.copyTo(lpstr, size);

  // HBITMAP
  HBITMAP hbmp = reinterpret_cast<HBITMAP>
    (LoadImage(Application::getHandle(),
	       lpstr,
	       IMAGE_BITMAP,
	       0, 0,
	       LR_LOADFROMFILE));
  delete[] lpstr;

  if (hbmp == NULL)
    throw ResourceException("Can't load the image from file " + fileName);

  get()->m_hdc = GetDC(GetDesktopWindow());
  get()->setHandle(hbmp);
}

Image::Image(const Size& sz)
  : SmartPtr<ImageHandle>(new ImageHandle())
{
  assert(sz.w > 0 && sz.h > 0);

  get()->m_hdc = GetDC(GetDesktopWindow());
  get()->setHandle(CreateCompatibleBitmap(get()->m_hdc, sz.w, sz.h));

  // TODO handle error
}

Image::Image(const Size& sz, int depth)
  : SmartPtr<ImageHandle>(new ImageHandle())
{
  assert(sz.w > 0 && sz.h > 0);

  BITMAPINFOHEADER bhdr;
  bhdr.biSize = sizeof(BITMAPINFOHEADER);
  bhdr.biWidth = sz.w;
  bhdr.biHeight = -sz.h;
  bhdr.biPlanes = 1;
  bhdr.biBitCount = depth;
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

  char* bits = NULL;

  get()->m_hdc = GetDC(GetDesktopWindow());
  get()->setHandle(CreateDIBSection(get()->m_hdc, &binf, DIB_RGB_COLORS,
				    reinterpret_cast<void**>(&bits),
				    NULL, 0));

  // TODO handle error
}

Image::Image(const Size& sz, Graphics& g)
  : SmartPtr<ImageHandle>(new ImageHandle())
{
  assert(g.getHandle());
  assert(sz.w > 0 && sz.h > 0);

  get()->m_hdc = g.getHandle();
  get()->setHandle(CreateCompatibleBitmap(get()->m_hdc, sz.w, sz.h));

}

Image::Image(const Image& image)
  : SmartPtr<ImageHandle>(image)
{
}

Image::~Image()
{
}  

int Image::getWidth() const
{
  return getSize().w;
}

int Image::getHeight() const
{
  return getSize().h;
}

Size Image::getSize() const
{
  BITMAPCOREHEADER bc;
  ZeroMemory(&bc, sizeof(bc));
  bc.bcSize = sizeof(bc);
  GetDIBits(get()->m_hdc, getHandle(), 0, 0, NULL,
	    reinterpret_cast<BITMAPINFO*>(&bc), 0);
  return Size(bc.bcWidth, bc.bcHeight);
}

int Image::getDepth() const
{
  BITMAPCOREHEADER bc;
  ZeroMemory(&bc, sizeof(bc));
  bc.bcSize = sizeof(bc);
  GetDIBits(get()->m_hdc, getHandle(), 0, 0, NULL,
	    reinterpret_cast<BITMAPINFO*>(&bc), 0);
  return bc.bcBitCount;
}

/**
 * Returns a Graphics context to draw inside the image.
 *
 * @warning You can't delete the returned pointer.
 */
Graphics* Image::getGraphics()
{
  ImageHandle* ptr = get();

  if (ptr->m_graphics == NULL) {
    assert(ptr->m_hdc != NULL);
    ptr->m_graphics = new Graphics(ptr->m_hdc, *this);
  }
  return ptr->m_graphics;
}

HBITMAP Image::getHandle() const
{
  return get()->getHandle();
}

Image& Image::operator=(const Image& image)
{
  SmartPtr<ImageHandle>::operator=(image);
  return *this;
}

Image Image::clone() const
{
  Image image(getSize(), getDepth());
  this->copyTo(image);
  return image;
}

void Image::copyTo(Image& image) const
{
  image.getGraphics()->drawImage(*const_cast<Image*>(this), 0, 0);
}
