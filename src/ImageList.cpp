// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
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

#include "Vaca/ImageList.h"
#include "Vaca/Application.h"
#include "Vaca/Debug.h"
#include "Vaca/ResourceException.h"
#include "Vaca/String.h"

using namespace Vaca;

#define GdiObj GdiObject<HIMAGELIST, Win32DestroyImageList>

ImageList::ImageList()
  : SmartPtr<GdiObj>(new GdiObj(ImageList_Create(16, 16, ILC_COLOR32, 0, 1)))
{
}

ImageList::ImageList(const Size& sz)
  : SmartPtr<GdiObj>(new GdiObj(ImageList_Create(sz.w, sz.w, ILC_COLOR32, 0, 1)))
{
  if (getHandle() == NULL)
    throw ResourceException(L"Can't create the image-list");
}

ImageList::ImageList(HIMAGELIST hImageList)
  : SmartPtr<GdiObj>(new GdiObj(hImageList))
{
}

/**
 * Loads a ImageList from a @msdn{BITMAP} resource.
 */
ImageList::ImageList(ResourceId bitmapId, int widthPerIcon, Color maskColor)
  : SmartPtr<GdiObj>(new GdiObj())
{
  HIMAGELIST himagelist =
    ImageList_LoadImage(Application::getHandle(),
			bitmapId.toLPTSTR(),
			widthPerIcon,
			1,
			maskColor.getColorRef(),
			IMAGE_BITMAP,
			0);

  if (himagelist == NULL)
    throw ResourceException(format_string(L"Can't create the image-list resource %d",
					  bitmapId.getId()));

  get()->setHandle(himagelist);
}

/**
 * Loads a ImageList from a file.
 */
ImageList::ImageList(const String& fileName, int widthPerIcon, Color maskColor)
  : SmartPtr<GdiObj>(new GdiObj())
{
  HIMAGELIST himagelist =
    ImageList_LoadImage(Application::getHandle(),
			fileName.c_str(), 
			widthPerIcon,
			1,
			maskColor.getColorRef(),
			IMAGE_BITMAP,
			LR_LOADFROMFILE);
  if (himagelist == NULL)
    throw ResourceException(L"Can't load the image-list from file " + fileName);

  get()->setHandle(himagelist);
}

ImageList::~ImageList()
{
}

/**
 * Returns how many images has this image list.
 * 
 * @return
 *   A number from 0 to n that specified the size of the list.
 */
int ImageList::getImageCount()
{
  assert(getHandle());
  return ImageList_GetImageCount(getHandle());
}

/**
 * Returns the @msdn{HIMAGELIST} handle.
 */
HIMAGELIST ImageList::getHandle()
{
  return get()->getHandle();
}
