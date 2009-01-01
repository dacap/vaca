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

#ifndef VACA_CURSOR_H
#define VACA_CURSOR_H

#include "Vaca/base.h"
#include "Vaca/ResourceId.h"
#include "Vaca/SmartPtr.h"
#include "Vaca/GdiObject.h"

namespace Vaca {

/**
 * @see SysCursor
 */
struct SysCursorEnum
{
  enum enumeration {
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
  static const enumeration default_value = Arrow;
};

/**
 * System mouse cursors.
 *
 * @see Cursor#Cursor(SysCursor)
 */
typedef Enum<SysCursorEnum> SysCursor;

struct Win32DestroyCursor
{
  static void destroy(HCURSOR handle)
  {
    ::DestroyCursor(handle);
  }
};

/**
 * A mouse cursor.
 *
 * This is a SmartPtr, so if you copy instances of cursors they will be
 * referencing to the same place. You can't clone cursors because you can't
 * modify them.
 *
 * @win32
 *   This is a @msdn{HCURSOR} wrapper.
 * @endwin32
 */
class VACA_DLL Cursor : private SmartPtr<GdiObject<HCURSOR, Win32DestroyCursor> >
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

} // namespace Vaca

#endif // VACA_CURSOR_H
