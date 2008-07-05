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
#include "Vaca/ImageList.hpp"
#include "Vaca/Application.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/ResourceException.hpp"

using namespace Vaca;

ImageList::ImageList()
{
  m_HIMAGELIST = NULL;
  m_selfDestruction = false;
}

ImageList::ImageList(HIMAGELIST hImageList, SelfDestruction selfDestruction)
{
  m_HIMAGELIST = hImageList;
  m_selfDestruction = selfDestruction.isEnabled();
}

/**
 * Loads a ImageList from a BITMAP resource.
 */
ImageList::ImageList(ResourceId bitmapId, int widthPerIcon, Color maskColor)
{
  m_HIMAGELIST =
    ImageList_LoadImage(Application::getHINSTANCE(),
			bitmapId.toLPTSTR(),
			widthPerIcon,
			1,
			maskColor.getColorRef(),
			IMAGE_BITMAP,
			0);

  if (m_HIMAGELIST == NULL)
    throw ResourceException("Can't create the image-list resource " + bitmapId.toString());

  m_selfDestruction = true;
}

/**
 * Loads a ImageList from a file.
 */
ImageList::ImageList(const String& fileName, int widthPerIcon, Color maskColor)
{
  m_HIMAGELIST =
    ImageList_LoadImage(Application::getHINSTANCE(),
			fileName.c_str(), 
			widthPerIcon,
			1,
			maskColor.getColorRef(),
			IMAGE_BITMAP,
			LR_LOADFROMFILE);

  if (m_HIMAGELIST == NULL)
    throw ResourceException("Can't load the image-list from file " + fileName);

  m_selfDestruction = true;
}

ImageList::~ImageList()
{
  destroy();
}

/**
 * Returns true if the image list is valid and can be used in
 * drawing routines like Graphics#drawImageList.
 */
bool ImageList::isValid()
{
  return m_HIMAGELIST != NULL;
}

/**
 * Returns how many images has this image list.
 * 
 * @return
 *     A number from 0 to n that specified the size of the list.
 */
int ImageList::getImageCount()
{
  assert(m_HIMAGELIST != NULL);
  
  return ImageList_GetImageCount(m_HIMAGELIST);
}

void ImageList::assign(HIMAGELIST hImageList, SelfDestruction selfDestruction)
{
  destroy();

  m_HIMAGELIST = hImageList;
  m_selfDestruction = selfDestruction.isEnabled();
}

HIMAGELIST ImageList::getHIMAGELIST()
{
  return m_HIMAGELIST;
}

void ImageList::destroy()
{
  if (m_HIMAGELIST != NULL && m_selfDestruction) {
    ImageList_Destroy(m_HIMAGELIST);

    m_HIMAGELIST = NULL;
  }
}
