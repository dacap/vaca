// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Layout.h"
#include "vaca/Debug.h"
#include "vaca/Widget.h"

using namespace vaca;

Layout::Layout()
{
}

Layout::~Layout()
{
}

Size Layout::getPreferredSize(Widget* parent, WidgetList& widgets, const Size& fitIn)
{
  return Size(0, 0);
}

//////////////////////////////////////////////////////////////////////
// WidgetsMovement

#include "win32/WidgetsMovementImpl.h"

WidgetsMovement::WidgetsMovement(const WidgetList& widgets)
  : m_impl(new WidgetsMovementImpl(widgets))
{
}

WidgetsMovement::~WidgetsMovement()
{
  delete m_impl;
}

void WidgetsMovement::moveWidget(Widget* widget, const Rect& rc)
{
  m_impl->moveWidget(widget, rc);
}
