// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/Cursor.h"
#include "Vaca/Application.h"
#include "Vaca/Debug.h"
#include "Vaca/ResourceException.h"
#include "Vaca/String.h"

using namespace Vaca;

#define GdiObj GdiObject<HCURSOR, Win32DestroyCursor>

/**
   Creates the null cursor (NoCursor).
*/
Cursor::Cursor()
  : SharedPtr<GdiObj>(new GdiObj)
{
}

Cursor::Cursor(const Cursor& cursor)
  : SharedPtr<GdiObj>(cursor)
{
}

/**
   @throw ResourceException
     When the resource with ID @a cursorId wasn't found.
*/
Cursor::Cursor(ResourceId cursorId)
  : SharedPtr<GdiObj>(new GdiObj)
{
  HCURSOR handle = ::LoadCursor(Application::getHandle(), cursorId.toLPTSTR());
  if (!handle)
    throw ResourceException(L"Can't load the cursor resource " + cursorId.toString());

  get()->setHandle(handle);
}

/**
   @throw ResourceException
     When the specified system @a cursor couldn't be loaded.
*/
Cursor::Cursor(SysCursor cursor)
  : SharedPtr<GdiObj>(new GdiObj)
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
   @throw ResourceException
     When the cursor couldn't be loaded from the specified @a fileName.
*/
Cursor::Cursor(const String& fileName)
  : SharedPtr<GdiObj>(new GdiObj)
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
  : SharedPtr<GdiObj>(new GdiObj(handle))
{
}

Cursor::~Cursor()
{
}

HCURSOR Cursor::getHandle() const
{
  return get()->getHandle();
}
