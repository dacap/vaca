// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
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

#include "Vaca/GroupBox.h"
#include "Vaca/Point.h"
#include "Vaca/Brush.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/PreferredSizeEvent.h"
#include "Vaca/LayoutEvent.h"

using namespace Vaca;

GroupBox::GroupBox(const String& text, Widget* parent, Style style)
  : Widget(WidgetClassName(WC_BUTTON), parent, style)
{
  setText(text);
}

GroupBox::~GroupBox()
{
}

Size GroupBox::getNonClientSize()
{
  ScreenGraphics g;

  g.setFont(getFont());
  Size sz =
    g.measureString(getText());
  
  return Size(4+4, sz.h+4);
}

void GroupBox::onPreferredSize(PreferredSizeEvent& ev)
{
  Size ncSize = getNonClientSize();
  
  if (ev.fitInWidth() || ev.fitInHeight()) {
    ev.setPreferredSize(max_value(0, ev.fitInWidth() - ncSize.w),
			max_value(0, ev.fitInHeight() - ncSize.h));
  }

  Widget::onPreferredSize(ev);
  ev.setPreferredSize(ev.getPreferredSize() + ncSize);
}

void GroupBox::onLayout(LayoutEvent& ev)
{
  ScreenGraphics g;

  g.setFont(getFont());
  Size sz = g.measureString(getText());

  Rect rc(ev.getBounds());
  ev.setBounds(Rect(rc.x+4, rc.y+sz.h, rc.w-8, rc.h-sz.h-4));

  Widget::onLayout(ev);
}

bool GroupBox::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
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
