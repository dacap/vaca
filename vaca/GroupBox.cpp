// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/GroupBox.h"
#include "vaca/Point.h"
#include "vaca/Brush.h"
#include "vaca/WidgetClass.h"
#include "vaca/PreferredSizeEvent.h"
#include "vaca/LayoutEvent.h"

using namespace vaca;

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
