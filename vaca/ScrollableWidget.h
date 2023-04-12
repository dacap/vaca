// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SCROLLABLEWIDGET_H
#define VACA_SCROLLABLEWIDGET_H

#include "vaca/base.h"
#include "vaca/Widget.h"
#include "vaca/Size.h"
#include "vaca/Point.h"

namespace vaca {

/**
   A wrapper ready-to-use to create scrollable widgets.
*/
class VACA_DLL ScrollableWidget : public Widget
{
  /**
     The full size area that can be viewed (using scrolling).
  */
  Size m_fullSize;

  /**
     This point is used to save the old mouse's position (on
     successive onMouseMove events).
  */
  Point m_oldPoint;

public:

  struct Styles {
    static constexpr Style Default =
      Widget::Styles::Default |
      Widget::Styles::Scroll |     // to show both scroll bars
      Widget::Styles::ClientEdge | // to show an edge arround the client area
      Widget::Styles::Focusable;   // to receive onMouseWheel
  };

  ScrollableWidget(Widget* parent, Style style = Styles::Default);

  Size getFullSize() const;
  void setFullSize(const Size& sz);

  // Events
protected:
  virtual void onResize(ResizeEvent& ev);
  virtual void onMouseEnter(MouseEvent& ev);
  virtual void onMouseDown(MouseEvent& ev);
  virtual void onMouseMove(MouseEvent& ev);
  virtual void onMouseUp(MouseEvent& ev);
  virtual void onMouseWheel(MouseEvent& ev);
  virtual void onSetCursor(SetCursorEvent& ev);
  virtual void onScroll(ScrollEvent& ev);

};

} // namespace vaca

#endif // VACA_SCROLLABLEWIDGET_H
