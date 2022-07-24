// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_CURSOR_H
#define VACA_CURSOR_H

#include "vaca/base.h"
#include "vaca/ResourceId.h"
#include "vaca/SharedPtr.h"
#include "vaca/GdiObject.h"

namespace vaca {

/**
   System mouse cursors.

   @see Cursor#Cursor(SysCursor)
*/
enum class SysCursor
{
  None,
  Arrow,
  Crosshair,
  Hand,
  Help,
  Text,
  Forbidden,
  Move,
  SizeE,
  SizeN,
  SizeNE,
  SizeNW,
  SizeS,
  SizeSE,
  SizeSW,
  SizeW,
  UpArrow,
  Wait,
  WaitBg
};

/**
   Used to destroy the HCURSOR handle from GdiObject.

   @internal
*/
struct Win32DestroyCursor
{
  /**
     @win32
     Calls @msdn{DestroyCursor}.
     @endwin32
  */
  static void destroy(HCURSOR handle)
  {
    ::DestroyCursor(handle);
  }
};

/**
   A mouse cursor.

   This is a SharedPtr, so if you copy instances of cursors they will be
   referencing to the same place. You can't clone cursors because you can't
   modify them.

   @win32
     This is a @msdn{HCURSOR} wrapper.
   @endwin32
*/
class VACA_DLL Cursor : private SharedPtr<GdiObject<HCURSOR, Win32DestroyCursor> >
{
public:
  Cursor();
  Cursor(const Cursor& cursor);
  explicit Cursor(ResourceId cursorId);
  explicit Cursor(SysCursor cursor);
  explicit Cursor(const String& fileName);
  explicit Cursor(HCURSOR handle);
  virtual ~Cursor();

  Cursor& operator=(const Cursor& cursor);

  HCURSOR getHandle() const;

};

} // namespace vaca

#endif // VACA_CURSOR_H
