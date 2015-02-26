// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_WIDGETLIST_H
#define VACA_WIDGETLIST_H

#include "vaca/base.h"

#include <vector>

namespace vaca {

/**
   Collection of widgets.

   Used to handle the list of children of each widget.
*/
typedef std::vector<Widget*> WidgetList;

} // namespace vaca

#endif // VACA_WIDGETLIST_H
