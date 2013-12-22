// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/ScrollableWidget.h"
#include "Vaca/ResizeEvent.h"
#include "Vaca/MouseEvent.h"
#include "Vaca/SetCursorEvent.h"
#include "Vaca/Cursor.h"
#include "Vaca/ScrollInfo.h"
#include "Vaca/ScrollEvent.h"

using namespace Vaca;

ScrollableWidget::ScrollableWidget(Widget* parent, Style style)
: Widget(parent, style), m_fullSize(0, 0)
{
}

Size ScrollableWidget::getFullSize() const
{
  return m_fullSize;
}

void ScrollableWidget::setFullSize(const Size& size)
{
  m_fullSize = size;
}

void ScrollableWidget::onResize(ResizeEvent& ev)
{
  Point oldScroll = getScrollPoint();

  // when the widget is resized we have to reconfigure the scroll
  // information (mainly the page size that depends of the widget
  // size ev.getSize())
  ScrollInfo si;
  si.setMinPos(0);
  si.setMaxPos(m_fullSize.w-1);
  si.setPageSize(ev.getSize().w);
  setScrollInfo(Orientation::Horizontal, si);

  si.setMinPos(0);
  si.setMaxPos(m_fullSize.h-1);
  si.setPageSize(ev.getSize().h);
  setScrollInfo(Orientation::Vertical, si);

  // now we get the difference between both scroll position: the old
  // a the new one
  Point scrollDelta = getScrollPoint() - oldScroll;

  // and we move the client area contrary to the scroll's direction
  scrollRect(getClientBounds(), -scrollDelta);

  Widget::onResize(ev);
}

void ScrollableWidget::onMouseEnter(MouseEvent& ev)
{
  Widget::onMouseEnter(ev);

  // to receive onMouseWheel
  requestFocus();
}

// intercepted to scroll with a "drag & drop" behavior
void ScrollableWidget::onMouseDown(MouseEvent& ev)
{
  Widget::onMouseDown(ev);

  // start dragging the viewport?
  if (!hasCapture()) {
    // we capture the mouse
    captureMouse();

    // here we save the current mouse position as a start point of dragging
    m_oldPoint = ev.getPoint();
  }
}

// intercepted to scroll with a "drag & drop" behavior
void ScrollableWidget::onMouseMove(MouseEvent& ev)
{
  Widget::onMouseMove(ev);

  if (hasCapture()) {
    // this "delta" point is the scroll variation
    Point delta = m_oldPoint - ev.getPoint();

    // here we save the current mouse position so the next call of
    // onMouseMove we can calculate the new variation of mouse movement
    m_oldPoint = ev.getPoint();

    // here we change the current scroll position applying the delta
    Point oldScroll = getScrollPoint();
    setScrollPoint(oldScroll + delta);

    Point scrollDelta = getScrollPoint() - oldScroll;
    scrollRect(getClientBounds(), -scrollDelta);
  }
}

void ScrollableWidget::onMouseUp(MouseEvent& ev)
{
  Widget::onMouseUp(ev);

  // we release the mouse
  releaseMouse();
}

// intercepted to scroll with the mouse wheel
void ScrollableWidget::onMouseWheel(MouseEvent& ev)
{
  Widget::onMouseWheel(ev);

  Orientation orien =
    ev.isControlKeyPressed() ? Orientation::Horizontal:
			       Orientation::Vertical;

  int pageSize = getScrollInfo(orien).getPageSize();
  int oldPos = getScrollPos(orien);

  Point oldScroll = getScrollPoint();
  setScrollPos(orien, oldPos - ev.getDelta() * pageSize/3);

  Point scrollDelta = getScrollPoint() - oldScroll;
  scrollRect(getClientBounds(), -scrollDelta);
}

void ScrollableWidget::onSetCursor(SetCursorEvent& ev)
{
  if (!ev.isConsumed() && hasCapture())
    ev.setCursor(Cursor(SysCursor::Move));

  Widget::onSetCursor(ev);
}

// this event is generated when the user touches the scroll bar
void ScrollableWidget::onScroll(ScrollEvent& ev)
{
  Widget::onScroll(ev);

  // "ev.getPosition()" is the tentative position where the scroll could
  // be, is calculated automatically through the scroll information
  // (like min/max pos and page size)
  Point oldScroll = getScrollPoint();
  setScrollPos(ev.getOrientation(),
	       ev.getPosition());

  Point scrollDelta = getScrollPoint() - oldScroll;
  scrollRect(getClientBounds(), -scrollDelta);
}
