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

using namespace Vaca;

/**
 * Loads a ImageList from a BITMAP resource.
 */
ImageList::ImageList(int bitmapId, int widthPerIcon, Color maskColor)
{
  mHimagelist =
    ImageList_LoadImage(Application::getHinstance(),
			MAKEINTRESOURCE(bitmapId),
			widthPerIcon,
			1,
			maskColor.getColorRef(),
			IMAGE_BITMAP,
			0);

  // TODO throw an exception
  if (mHimagelist == NULL)
    Beep(400, 100);
}

/**
 * Loads a ImageList from a file.
 */
ImageList::ImageList(const String &fileName, int widthPerIcon, Color maskColor)
{
  mHimagelist =
    ImageList_LoadImage(Application::getHinstance(),
			fileName.c_str(), 
			widthPerIcon,
			1,
			maskColor.getColorRef(),
			IMAGE_BITMAP,
			LR_LOADFROMFILE);

  // TODO throw an exception
  if (mHimagelist == NULL)
    Beep(400, 100);
}

ImageList::~ImageList()
{
  if (mHimagelist != NULL)
    ImageList_Destroy(mHimagelist);
}

HIMAGELIST ImageList::getHimagelist()
{
  return mHimagelist;
}
