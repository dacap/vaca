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
#include "Vaca/Icon.h"
#include "Vaca/Application.h"
#include "Vaca/Debug.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// Icon


Icon::Icon()
{
  mHicon = NULL;
}

/**
 * Loads an Icon from a ICON resource.
 */
Icon::Icon(int iconId, const Size &sz)
{
  mHicon = reinterpret_cast<HICON>(LoadImage(Application::getHinstance(),
					     MAKEINTRESOURCE(iconId),
					     IMAGE_ICON,
					     sz.w, sz.h, 0));

  // TODO throw an exception
  if (mHicon == NULL)
    Beep(400, 100);
}

/**
 * Loads an Icon from a file.
 */
Icon::Icon(const String &fileName, const Size &sz)
{
  mHicon = reinterpret_cast<HICON>(LoadImage(Application::getHinstance(),
					     fileName.c_str(),
					     IMAGE_ICON,
					     sz.w, sz.h, LR_LOADFROMFILE));

  // TODO throw an exception
  if (mHicon == NULL)
    Beep(400, 100);
}

Icon::~Icon()
{
  if (mHicon != NULL)
    DestroyIcon(mHicon);
}

HICON Icon::getHicon()
{
  return mHicon;
}

void Icon::setHicon(HICON hicon)
{
  mHicon = hicon;
}

//////////////////////////////////////////////////////////////////////
// SharedIcon


SharedIcon::SharedIcon(int iconId, const Size &sz)
{
  HICON hicon = reinterpret_cast<HICON>(LoadImage(Application::getHinstance(),
						  MAKEINTRESOURCE(iconId),
						  IMAGE_ICON,
						  sz.w, sz.h, LR_SHARED));

  // TODO hicon == NULL throw
  if (hicon == NULL)
    Beep(400, 100);

  setHicon(hicon);
}

SharedIcon::SharedIcon(const String &fileName, const Size &sz)
{
  HICON hicon = reinterpret_cast<HICON>(LoadImage(Application::getHinstance(),
						  fileName.c_str(),
						  IMAGE_ICON,
						  sz.w, sz.h,
						  LR_LOADFROMFILE | LR_SHARED));

  // TODO hicon == NULL throw
  if (hicon == NULL)
    Beep(400, 100);

  setHicon(hicon);
}

SharedIcon::~SharedIcon()
{
  // don't call DestroyIcon
  setHicon(NULL);
}
