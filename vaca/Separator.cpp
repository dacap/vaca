// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Separator.h"
#include "vaca/WidgetClass.h"
#include "vaca/PreferredSizeEvent.h"

using namespace vaca;

Separator::Separator(Widget* parent, Style style)
  : Widget(WidgetClassName(WC_STATIC), parent, style)
{
}

Separator::~Separator()
{
}

/**
   The preferred size of a separator is a 2x2 box. It is your
   responsability to make the Separator widget a horizontal or
   vertical line (through the Layout of the @link Widget#getParent parent Widget@endlink).
*/
void Separator::onPreferredSize(PreferredSizeEvent& ev)
{
  ev.setPreferredSize(2, 2);
}
