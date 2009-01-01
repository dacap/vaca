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

#include "Vaca/Cursor.h"
#include "Vaca/Application.h"
#include "Vaca/Debug.h"
#include "Vaca/ResourceException.h"
#include "Vaca/String.h"

using namespace Vaca;

#define GdiObj GdiObject<HCURSOR, Win32DestroyCursor>

/**
 * Creates the null cursor (NoCursor).
 */
Cursor::Cursor()
  : SmartPtr<GdiObj>(new GdiObj)
{
}

Cursor::Cursor(const Cursor& cursor)
  : SmartPtr<GdiObj>(cursor)
{
}

/**
 * @throw ResourceException
 *   When the resource with ID @a cursorId wasn't found.
 */
Cursor::Cursor(ResourceId cursorId)
  : SmartPtr<GdiObj>(new GdiObj)
{
  HCURSOR handle = ::LoadCursor(Application::getHandle(), cursorId.toLPTSTR());
  if (!handle)
    throw ResourceException(L"Can't load the cursor resource " + cursorId.toString());

  get()->setHandle(handle);
}

/**
 * @throw ResourceException
 *   When the specified system @a cursor couldn't be loaded.
 */
Cursor::Cursor(SysCursor cursor)
  : SmartPtr<GdiObj>(new GdiObj)
{
  LPCTSTR winCursor = IDC_ARROW;

  switch (cursor) {

    // special cursor that has HCURSOR = NULL
    case SysCursor::None:
      return;

    case SysCursor::Arrow:     winCursor = IDC_ARROW; break;
    case SysCursor::Crosshair: winCursor = IDC_CROSS; break;
    case SysCursor::Hand:      winCursor = IDC_HAND; break;
    case SysCursor::Help:      winCursor = IDC_HELP; break;
    case SysCursor::Text:      winCursor = IDC_IBEAM; break;
    case SysCursor::Forbidden: winCursor = IDC_NO; break;
    case SysCursor::Move:      winCursor = IDC_SIZEALL; break;
    case SysCursor::SizeE:     winCursor = IDC_SIZEWE; break;
    case SysCursor::SizeN:     winCursor = IDC_SIZENS; break;
    case SysCursor::SizeNE:    winCursor = IDC_SIZENESW; break;
    case SysCursor::SizeNW:    winCursor = IDC_SIZENWSE; break;
    case SysCursor::SizeS:     winCursor = IDC_SIZENS; break;
    case SysCursor::SizeSE:    winCursor = IDC_SIZENWSE; break;
    case SysCursor::SizeSW:    winCursor = IDC_SIZENESW; break;
    case SysCursor::SizeW:     winCursor = IDC_SIZEWE; break;
    case SysCursor::UpArrow:   winCursor = IDC_UPARROW; break;
    case SysCursor::Wait:      winCursor = IDC_WAIT; break;
    case SysCursor::WaitBg:    winCursor = IDC_APPSTARTING; break;
  }
  
  HCURSOR handle = ::LoadCursor(NULL, winCursor);
  if (handle == NULL)
    throw ResourceException(format_string(L"Can't load the SysCursor %d", int(cursor)));

  get()->setHandle(handle);
}

/**
 * @throw ResourceException
 *   When the cursor couldn't be loaded from the specified @a fileName.
 */
Cursor::Cursor(const String& fileName)
  : SmartPtr<GdiObj>(new GdiObj)
{
  HCURSOR handle = reinterpret_cast<HCURSOR>
    (::LoadImage(Application::getHandle(),
		 fileName.c_str(),
		 IMAGE_CURSOR,
		 0, 0, LR_LOADFROMFILE));

  if (handle == NULL)
    throw ResourceException(L"Can't load cursor from file " + fileName);

  get()->setHandle(handle);
}

Cursor::Cursor(HCURSOR handle)
  : SmartPtr<GdiObj>(new GdiObj(handle))
{
}

Cursor::~Cursor()
{
}

HCURSOR Cursor::getHandle() const
{
  return get()->getHandle();
}
