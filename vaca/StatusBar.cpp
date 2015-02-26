// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/StatusBar.h"
#include "vaca/WidgetClass.h"
#include "vaca/PreferredSizeEvent.h"
#include "vaca/LayoutEvent.h"

using namespace vaca;

StatusBar::StatusBar(Widget* parent, Style style)
  : Widget(WidgetClassName(STATUSCLASSNAME), parent, style)
{
}

StatusBar::~StatusBar()
{
}

/**
   A status bar is arranged by a Frame, but doesn't depend of the
   current Layout manager in that Frame.
*/
bool StatusBar::isLayoutFree() const
{
  return true;
}

void StatusBar::onPreferredSize(PreferredSizeEvent& ev)
{
  ev.setPreferredSize(0, 24);
}

void StatusBar::onLayout(LayoutEvent& ev)
{
  Rect rc = ev.getBounds();
  Size pref = getPreferredSize();

  setBounds(Rect(rc.x, rc.y+rc.h-pref.h, rc.w, pref.h));

  rc.h -= pref.h;
  ev.setBounds(rc);

  Widget::onLayout(ev);
}
