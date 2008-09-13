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

#ifndef VACA_IMAGELIST_H
#define VACA_IMAGELIST_H

#include "Vaca/base.h"
#include "Vaca/Image.h"
#include "Vaca/Color.h"
#include "Vaca/ResourceId.h"
#include "Vaca/GdiObject.h"
#include "Vaca/SmartPtr.h"

namespace Vaca {

class String;
class Size;

struct Win32DestroyImageList
{
  static void destroy(HIMAGELIST handle)
  {
    ::ImageList_Destroy(handle);
  }
};

/**
 * A list of images (Win32's HIMAGELIST wrapper).
 */
class VACA_DLL ImageList : private SmartPtr<GdiObject<HIMAGELIST, Win32DestroyImageList> >
{
public:

  ImageList();
  explicit ImageList(const Size& sz);
  explicit ImageList(HIMAGELIST hImageList);
  ImageList(ResourceId bitmapId, int widthPerIcon, Color maskColor);
  ImageList(const String& fileName, int widthPerIcon, Color maskColor);
  virtual ~ImageList();

  int getImageCount();
  
  HIMAGELIST getHandle();

};

} // namespace Vaca

#endif // VACA_IMAGELIST_H
