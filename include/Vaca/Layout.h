// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_LAYOUT_H
#define VACA_LAYOUT_H

#include "Vaca/base.h"
#include "Vaca/Size.h"
#include "Vaca/Referenceable.h"
#include "Vaca/WidgetList.h"

namespace Vaca {

/**
   A Layout setups the position of a collection of widgets. It's used
   to arrange the widgets quickly without worrying about the exact
   position of them.

   Each widget can have a layout manager, but it's only useful when
   the widget has children.

   @warning If the parent widget doesn't have a layout manager
	    specified, the children bounds aren't modified (see the
	    @c FreeOfLayout example).

   @see @ref page_tn_011
*/
class VACA_DLL Layout : public Referenceable
{
public:
  Layout();
  virtual ~Layout();

  virtual Size getPreferredSize(Widget* parent, WidgetList& widgets, const Size& fitIn);
  virtual void layout(Widget* parent, WidgetList& widgets, const Rect& rc) = 0;
};

/**
   Auxiliary class to move widgets inside onLayout() method.
 */
class VACA_DLL WidgetsMovement
{
public:
  WidgetsMovement(const WidgetList& widgets);
  ~WidgetsMovement();

  void moveWidget(Widget* widget, const Rect& rc);

private:
  class WidgetsMovementImpl;
  WidgetsMovementImpl* m_impl;
};

} // namespace Vaca

#endif // VACA_LAYOUT_H
