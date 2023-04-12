// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_BASICDOCKAREA_H
#define VACA_BASICDOCKAREA_H

#include "vaca/base.h"
#include "vaca/DockArea.h"

namespace vaca {

/**
   The most basic dock area management.
*/
class VACA_DLL BasicDockArea : public DockArea
{
public:

  struct Styles {
    /**
       Default style for BasicDockArea widget.
    */
    static constexpr Style Default = Widget::Styles::Visible;
  };

  BasicDockArea(Side side, Widget* parent, Style style = Styles::Default);
  virtual ~BasicDockArea();

  virtual bool hitTest(DockBar* bar, const Point& cursor, const Point& anchor, bool fromInside);
  virtual DockInfo* createDefaultDockInfo(DockBar* bar);
  virtual DockInfo* createDockInfo(DockBar* bar, const Point& cursor, const Point& anchor);
  virtual void drawXorTracker(Graphics& g, DockInfo* dockInfo);

  virtual void layout();

protected:
  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);

};

} // namespace vaca

#endif // VACA_BASICDOCKAREA_H
