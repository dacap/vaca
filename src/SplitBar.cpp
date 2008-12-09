// Vaca - Visual Application Components Abstraction
// Copyright (c) 2008, Jie Zhang, David Capello
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
// * Neither the name of the Vaca nor the names of its contributors
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

#include "Vaca/SplitBar.h"
#include "Vaca/Color.h"
#include "Vaca/Brush.h"
#include "Vaca/Cursor.h"
#include "Vaca/MouseEvent.h"
#include "Vaca/System.h"

using namespace Vaca;

#define SPLITBAR_DEFAULT_WIDTH		5

SplitBar::SplitBar(Orientation orientation, Widget* parent, Style style)
  : Widget(SplitBarClass::getClassName(), parent, style)
  , m_orientation(orientation)
  , m_first(NULL)
  , m_second(NULL)
  , m_size(Size(100, 100))
  , m_fullDrag(false)
  , m_trackerIsVisible(false)
  , m_gripperVisible(true)
  , m_barWidth(SPLITBAR_DEFAULT_WIDTH)
{
}

SplitBar::~SplitBar()
{
}

void SplitBar::layout()
{
  Widget::layout();

  Rect rcL, rcR;
  getRects(rcL, rcR);
  if (m_first) m_first->setBounds(rcL);
  if (m_second) m_second->setBounds(rcR);
}

void SplitBar::setFirstWidget(Widget* widget)
{
  m_first = widget;
  updateChildrenVisibility();
  layout();
}

void SplitBar::setSecondWidget(Widget* widget)
{
  m_second = widget;
  updateChildrenVisibility();
  layout();
}

Widget* SplitBar::getFirstWidget() const
{
  return m_first;
}

Widget* SplitBar::getSecondWidget() const
{
  return m_second;
}

void SplitBar::setPaneSize(const Size& sz)
{
  m_size = sz;
  layout();
}

Orientation SplitBar::getOrientation() const
{
  return m_orientation;
}

void SplitBar::setOrientation(Orientation orientation)
{
  m_orientation = orientation;
  layout();
}

void SplitBar::setFullDrag(bool state)
{
  m_fullDrag = state;
}

bool SplitBar::isFullDrag() const
{
  return m_fullDrag;
}

void SplitBar::setGripperVisible(bool state)
{
  if (m_gripperVisible != state) {
    m_gripperVisible = state;
    invalidate(true);
  }
}

bool SplitBar::isGripperVisible() const
{
  return m_gripperVisible;
}

void SplitBar::setBarWidth(int width)
{
  if (m_barWidth != width) {
    m_barWidth = width;
    layout();
  }
}

int SplitBar::getBarWidth() const
{
  return m_barWidth;
}

void SplitBar::onResize(const Size& sz)
{
  invalidate(true);
  layout();
}

void SplitBar::onPaint(Graphics& g)
{
  if (m_gripperVisible) {
    Rect rcBar(getBarRect());

    for (int c=0; c<8; ++c) {
      Rect rc(0, 0, 3, 3);

      if (m_orientation == Orientation::Vertical) {
	rc.x = rcBar.x+rcBar.w/2-rc.w/2;
	rc.y = rcBar.y+rcBar.h/2-8*(rc.h+1)/2+c*(rc.h+1);
      }
      else {
	rc.x = rcBar.x+rcBar.w/2-8*(rc.w+1)/2+c*(rc.w+1);
	rc.y = rcBar.y+rcBar.h/2-rc.h/2;
      }

      g.draw3dRect(rc,
		   System::getColor(COLOR_3DSHADOW),
		   System::getColor(COLOR_3DHIGHLIGHT));
    }
  }
  else {
    Widget::onPaint(g);
  }
}

void SplitBar::onMouseDown(MouseEvent& ev)
{
  Widget::MouseDown(ev);

  // capture the mouse only if the user clicks in the bar
  if (getBarRect().contains(ev.getPoint())) {
    m_oldPoint = ev.getPoint();
    m_oldSize = m_size;

    captureMouse();

    if (!isFullDrag()) {
      ScreenGraphics g;
      drawTracker(g);
    }
  }
}

void SplitBar::onMouseUp(MouseEvent& ev)
{
  Widget::MouseUp(ev);

  releaseMouse();

  if (!isFullDrag()) {
    ScreenGraphics g;
    cleanTracker(g);
    layout();
  }
}

void SplitBar::onMouseMove(MouseEvent& ev)
{
  Widget::MouseMove(ev);

  if (hasCapture()) {
    Rect rcClient(getClientBounds());
    ScreenGraphics g;

    if (!isFullDrag())
      cleanTracker(g);

    if (m_orientation == Orientation::Vertical) {
      m_size.w = m_oldSize.w + (ev.getPoint().x - m_oldPoint.x);
      m_size.w = clamp_value(m_size.w, 0, rcClient.w-m_barWidth);
    }
    else {
      m_size.h = m_oldSize.h + (ev.getPoint().y - m_oldPoint.y);
      m_size.h = clamp_value(m_size.h, 0, rcClient.h-m_barWidth);
    }

    if (!isFullDrag())
      drawTracker(g);
    else
      layout();
  }
}

void SplitBar::onSetCursor(WidgetHitTest hitTest)
{
  bool isVertical = (m_orientation == Orientation::Vertical);
  SysCursor modCursor = isVertical ? SysCursor::SizeE : SysCursor::SizeN;
  SysCursor sysCursor = SysCursor::Arrow;

  if (hasCapture()) {
    sysCursor = modCursor;
  }
  else {
    Point pt = System::getCursorPos() - getAbsoluteBounds().getOrigin();
    if (getBarRect().contains(pt))
      sysCursor = modCursor;
  }

  setCursor(Cursor(sysCursor));
}

void SplitBar::onAddChild(Widget* child)
{
  Widget::onAddChild(child);

  if (m_first == NULL)
    setFirstWidget(child);
  else if (m_second == NULL)
    setSecondWidget(child);
}

void SplitBar::updateChildrenVisibility()
{
  // hide children that aren't first or second
  Container children = getChildren();
  for (Container::iterator
	 it = children.begin(); it != children.end(); ++it) {
    Widget* child = *it;
    child->setVisible(child == m_first ||
		      child == m_second);
  }
}

Rect SplitBar::getBarRect() const
{
  Rect rcBar(getClientBounds());

  if (m_orientation == Orientation::Vertical) {
    rcBar.x = clamp_value(m_size.w, 0, rcBar.w-m_barWidth);
    rcBar.w = m_barWidth;
  }
  else {
    rcBar.y = clamp_value(m_size.h, 0, rcBar.h-m_barWidth);
    rcBar.h = m_barWidth;
  }

  return rcBar;
}

void SplitBar::getRects(Rect& rcFirst, Rect& rcSecond) const
{
  Rect rcClient, rcBar(getBarRect());
	
  rcClient = getClientBounds();
  rcFirst = rcSecond = rcClient;

  if (m_orientation == Orientation::Vertical) {
    rcFirst.w = rcBar.x;
    rcSecond.x = rcBar.x+rcBar.w;
    rcSecond.w -= rcSecond.x - rcClient.x;
  }
  else {
    rcFirst.h = rcBar.y;
    rcSecond.y = rcBar.y+rcBar.h;
    rcSecond.h -= rcSecond.y - rcClient.y;
  }
}

void SplitBar::drawTracker(Graphics& g)
{
  if (!m_trackerIsVisible) {
    g.fillXorFrame(getBarRect().offset(getAbsoluteBounds().getOrigin()));
    m_trackerIsVisible = true;
  }
}

void SplitBar::cleanTracker(Graphics& g)
{
  if (m_trackerIsVisible) {
    g.fillXorFrame(getBarRect().offset(getAbsoluteBounds().getOrigin()));
    m_trackerIsVisible = false;
  }
}
