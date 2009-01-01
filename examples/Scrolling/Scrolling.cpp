// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the author nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include <Vaca/Vaca.h>

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// Almost every widget that want to be scrollable should implement
// something like this

class Scrollable : public Panel
{
  // the full size area that can be viewed (using scrolling)
  Size m_fullSize;

  // this point is used to save the old mouse's position (on
  // successive onMouseMove events)
  Point m_oldPoint;

  // true if we have to wait some milliseconds after update the
  // invalidated area
  bool m_sleep;

public:
  Scrollable(Widget* parent)
    : Panel(parent, Panel::Styles::Default +
		    Widget::Styles::Scroll +     // to show both scroll bars
		    Widget::Styles::ClientEdge + // to show an edge arround the client area
		    Widget::Styles::Focusable)   // to receive onMouseWheel
    , m_fullSize(2000, 1500)
    , m_sleep(false)
  {
    // the red color will be used to erase the background of
    // invalidated area, but in onPaint the red will be re-erased with
    // a white brush
    setBgColor(Color::Red);
  }

  void setSleep(bool sleep)
  {
    m_sleep = sleep;
  }

protected:

  virtual void onPaint(Graphics& g)
  {
    // this sleep have the purpose to show the invalidated areas 100
    // milliseconds (the background filled by default with the
    // getBgColor, that in this case is Color::Red)
    if (m_sleep)
      Thread::sleep(100);

    // draw the (white) background
    Brush brush(Color::White);
    g.fillRect(brush, getClientBounds());

    // draw the shapes (ellipses and round-rectangles)
    Pen pen(Color::Blue);
    Point offset = -getScrollPoint();
    for (int r=0; r<10; ++r) {
      Rect rc(offset, m_fullSize);

      rc.shrink(50 * r);

      g.drawEllipse(pen, rc);
      g.drawRoundRect(pen, rc, Size(32, 32));
    }
  }

  virtual void onResize(const Size& sz)
  {
    Panel::onResize(sz);

    Point oldScroll = getScrollPoint();

    // when the widget is resized we have to reconfigure the scroll
    // information (mainly the page size that depends of the widget
    // size "sz")
    ScrollInfo si;
    si.setMinPos(0);
    si.setMaxPos(m_fullSize.w-1);
    si.setPageSize(sz.w);
    setScrollInfo(Orientation::Horizontal, si);

    si.setMinPos(0);
    si.setMaxPos(m_fullSize.h-1);
    si.setPageSize(sz.h);
    setScrollInfo(Orientation::Vertical, si);

    // now we get the difference between both scroll position: the old
    // a the new one
    Point scrollDelta = getScrollPoint() - oldScroll;

    // and we move the client area contrary to the scroll's direction
    scrollRect(getClientBounds(), -scrollDelta);
  }

  virtual void onMouseEnter(MouseEvent& ev)
  {
    Panel::onMouseEnter(ev);

    // to receive onMouseWheel
    requestFocus();
  }

  // intercepted to scroll with a "drag & drop" behavior
  virtual void onMouseDown(MouseEvent& ev)
  {
    Panel::onMouseDown(ev);

    // start dragging the viewport?
    if (!hasCapture()) {
      // we capture the mouse
      captureMouse();

      // when we capture the mouse the onSetCursor event isn't
      // generated, so we can setup the mouse cursor here
      setCursor(Cursor(SysCursor::Move));

      // here we save the current mouse position as a start point of dragging
      m_oldPoint = ev.getPoint();
    }
  }

  // intercepted to scroll with a "drag & drop" behavior
  virtual void onMouseMove(MouseEvent& ev)
  {
    Panel::onMouseMove(ev);

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

  virtual void onMouseUp(MouseEvent& ev)
  {
    Panel::onMouseUp(ev);

    // we release the mouse
    releaseMouse();
  }

  // intercepted to scroll with the mouse wheel
  virtual void onMouseWheel(MouseEvent& ev)
  {
    Panel::onMouseWheel(ev);

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

  // this event is generated when the user touches the scroll bar
  virtual void onScroll(ScrollEvent& ev)
  {
    Panel::onScroll(ev);

    // "ev.getPosition()" is the tentative position where the scroll could
    // be, is calculated automatically through the scroll information
    // (like min/max pos and page size)
    Point oldScroll = getScrollPoint();
    setScrollPos(ev.getOrientation(),
		 ev.getPosition());

    Point scrollDelta = getScrollPoint() - oldScroll;
    scrollRect(getClientBounds(), -scrollDelta);
  }
  
};

//////////////////////////////////////////////////////////////////////

void updateSleep(Scrollable* s, CheckBox* cb)
{
  s->setSleep(cb->isSelected());
}

int VACA_MAIN()
{
  Application app;
  Frame frm(L"Scrolling");
  Scrollable wgt(&frm);
  CheckBox sleep(L"Show invalidated areas for some milliseconds", &frm);

  sleep.Action.connect(Bind<void>(&updateSleep, &wgt, &sleep));

  frm.setLayout(new BoxLayout(Orientation::Vertical, false));
  wgt.setConstraint(new BoxConstraint(true));
  frm.setVisible(true);

  app.run();
  return 0;
}
