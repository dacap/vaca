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
#include "Vaca/ImageList.h"
#include "Vaca/Application.h"
#include "Vaca/Debug.h"
#include "Vaca/ResourceException.h"

using namespace Vaca;

ImageList::ImageList()
{
  mHIMAGELIST = NULL;
  mAutoDelete = false;
}

ImageList::ImageList(HIMAGELIST hImageList, bool autoDelete)
{
  mHIMAGELIST = hImageList;
  mAutoDelete = autoDelete;
}

/**
 * Loads a ImageList from a BITMAP resource.
 */
ImageList::ImageList(int bitmapId, int widthPerIcon, Color maskColor)
{
  mHIMAGELIST =
    ImageList_LoadImage(Application::getHINSTANCE(),
			MAKEINTRESOURCE(bitmapId),
			widthPerIcon,
			1,
			maskColor.getColorRef(),
			IMAGE_BITMAP,
			0);

  if (mHIMAGELIST == NULL)
    throw ResourceException();

  mAutoDelete = true;
}

/**
 * Loads a ImageList from a file.
 */
ImageList::ImageList(const String &fileName, int widthPerIcon, Color maskColor)
{
  mHIMAGELIST =
    ImageList_LoadImage(Application::getHINSTANCE(),
			fileName.c_str(), 
			widthPerIcon,
			1,
			maskColor.getColorRef(),
			IMAGE_BITMAP,
			LR_LOADFROMFILE);

  if (mHIMAGELIST == NULL)
    throw ResourceException();

  mAutoDelete = true;
}

ImageList::~ImageList()
{
  if (mAutoDelete && mHIMAGELIST != NULL)
    ImageList_Destroy(mHIMAGELIST);
}

bool ImageList::isValid()
{
  return mHIMAGELIST != NULL;
}

int ImageList::getImageCount()
{
  assert(mHIMAGELIST != NULL);
  
  return ImageList_GetImageCount(mHIMAGELIST);
}

void ImageList::assign(HIMAGELIST hImageList, bool autoDelete)
{
  if (mAutoDelete && mHIMAGELIST != NULL)
    ImageList_Destroy(mHIMAGELIST);

  mHIMAGELIST = hImageList;
  mAutoDelete = autoDelete;
}

HIMAGELIST ImageList::getHIMAGELIST()
{
  return mHIMAGELIST;
}
