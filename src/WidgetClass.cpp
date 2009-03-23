// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

#include "Vaca/WidgetClass.h"
#include "Vaca/Widget.h"
#include "Vaca/Debug.h"

using namespace Vaca;

const WidgetClassName WidgetClassName::None;

//////////////////////////////////////////////////////////////////////
// WidgetClassName

WidgetClassName::WidgetClassName()
  : m_className(L"")
{
}

WidgetClassName::WidgetClassName(const String& className)
  : m_className(className)
{
}

WidgetClassName::WidgetClassName(const WidgetClassName& className)
  : m_className(className.m_className)
{
}

WidgetClassName::~WidgetClassName()
{
}

WidgetClassName& WidgetClassName::operator=(const WidgetClassName& className)
{
  m_className = className.m_className;
  return *this;
}

bool WidgetClassName::operator==(const WidgetClassName& className) const
{
  return m_className == className.m_className;
}

bool WidgetClassName::operator!=(const WidgetClassName& className) const
{
  return m_className != className.m_className;
}

//////////////////////////////////////////////////////////////////////
// WidgetClass

/// Returns the class name.
/// 
/// @win32
///   The class name is used in the lpszClassName field of
///   the @msdn{WNDCLASSEX} structure inside the @link Register#Register Register constructor@endlink.
/// @endwin32
/// 
WidgetClassName WidgetClass::getClassName()
{
  return WidgetClassName(L"Vaca.Widget");
}

/// Returns the set of CS_ Win32 flags.
/// 
/// All Widgets by default received double-clicks, so it returns
/// CS_DBLCLKS.
/// 
int WidgetClass::getStyle()
{
  return CS_DBLCLKS;
}

int WidgetClass::getColor()
{
  return COLOR_3DFACE;
}

int WidgetClass::getWndExtra()
{
  return 0;
}

WNDPROC WidgetClass::getWndProc()
{
  return Widget::getGlobalWndProc();
}
