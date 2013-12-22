// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Vaca::WidgetsMovement::WidgetsMovementImpl
{
  HDWP m_hdwp;
  WidgetList m_relayoutWidgets;

public:

  WidgetsMovementImpl(const WidgetList& widgets)
  {
    m_hdwp = BeginDeferWindowPos(widgets.size());
  }

  ~WidgetsMovementImpl()
  {
    EndDeferWindowPos(m_hdwp);
    m_hdwp = NULL;

    for (WidgetList::iterator it=m_relayoutWidgets.begin();
	 it!=m_relayoutWidgets.end(); ++it) {
      (*it)->layout();
    }
  }

  void moveWidget(Widget* widget, const Rect& rc)
  {
    m_hdwp = DeferWindowPos(m_hdwp, widget->getHandle(), NULL,
			    rc.x, rc.y, rc.w, rc.h, 
			    SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

    m_relayoutWidgets.push_back(widget);
  }

};
