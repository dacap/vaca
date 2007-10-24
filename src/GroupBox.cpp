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
#include "Vaca/GroupBox.hpp"
#include "Vaca/Point.hpp"
#include "Vaca/Brush.hpp"

using namespace Vaca;

GroupBox::GroupBox(const String &text, Widget *parent, Style style)
  : Widget(WC_BUTTON, parent, style)
{
  setText(text);
}

GroupBox::~GroupBox()
{
}

Rect GroupBox::getLayoutBounds()
{
  ScreenGraphics g;

  g.setFont(getFont());
  Size sz =
    g.measureString(getText());

  Rect rc(getClientBounds());

  return Rect(rc.x+4, rc.y+sz.h, rc.w-8, rc.h-sz.h-4);
}

Size GroupBox::getNonClientSize()
{
  ScreenGraphics g;

  g.setFont(getFont());
  Size sz =
    g.measureString(getText());
  
  return Size(4+4, sz.h+4);
}

void GroupBox::onPreferredSize(Size &sz)
{
  Size ncSize = getNonClientSize();
  
  if (sz.w > 0 || sz.h > 0) {
    sz = Size(VACA_MAX(0, sz.w - ncSize.w),
	      VACA_MAX(0, sz.h - ncSize.h));
  }

  Widget::onPreferredSize(sz);
  sz += ncSize;
}

bool GroupBox::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
  // fix a bug with group-boxes: they don't clear the background
  if (message == WM_ERASEBKGND) {
    HDC hdc = reinterpret_cast<HDC>(wParam);
    Graphics g(hdc);
    Brush brush(getBgColor());
    
    g.fillRect(brush, g.getClipBounds());

    lResult = TRUE;
    return true;
  }
  
  return Widget::wndProc(message, wParam, lParam, lResult);
}
