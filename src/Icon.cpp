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

#include "Vaca/Icon.h"
#include "Vaca/Application.h"
#include "Vaca/Debug.h"
#include "Vaca/ResourceException.h"

using namespace Vaca;

#define GdiObj GdiObject<HCURSOR, Win32DestroyIcon>

Icon::Icon()
{
}

Icon::Icon(const Icon& icon)
  : SmartPtr<GdiObj>(icon)
{
}

/**
 * Loads an Icon from a ICON resource.
 *
 * @throw ResourceException
 *   When the resource with ID @a iconId wasn't found.
 */
Icon::Icon(ResourceId iconId, const Size& sz)
  : SmartPtr<GdiObj>(new GdiObj)
{
  HICON handle = reinterpret_cast<HICON>
    (::LoadImage(Application::getHandle(),
		 MAKEINTRESOURCE(iconId.getId()),
		 IMAGE_ICON,
		 sz.w, sz.h, 0));
  if (handle == NULL)
    throw ResourceException("Can't load the icon resource " + String::fromInt(iconId.getId()));

  get()->setHandle(handle);
}

/**
 * Loads an Icon from a file.
 * 
 * @throw ResourceException
 *   When the icon couldn't be loaded from the specified @a fileName.
 */
Icon::Icon(const String& fileName, const Size& sz)
  : SmartPtr<GdiObj>(new GdiObj)
{
  HICON handle = reinterpret_cast<HICON>
    (::LoadImage(Application::getHandle(),
		 fileName.c_str(),
		 IMAGE_ICON,
		 sz.w, sz.h, LR_LOADFROMFILE));
  if (handle == NULL)
    throw ResourceException("Can't load icon from file " + fileName);

  get()->setHandle(handle);
}

Icon::Icon(HICON handle)
  : SmartPtr<GdiObj>(new GdiObj(handle))
{
}

Icon::~Icon()
{
}

HICON Icon::getHandle() const
{
  return get()->getHandle();
}
