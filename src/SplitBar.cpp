// Vaca - Visual Application Components Abstraction
// Copyright (c) 2008, 2009, Jie Zhang, David Capello
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

#include "Vaca/SplitBar.h"
#include "Vaca/Color.h"
#include "Vaca/Brush.h"
#include "Vaca/Cursor.h"
#include "Vaca/MouseEvent.h"
#include "Vaca/PaintEvent.h"
#include "Vaca/System.h"
#include "Vaca/SetCursorEvent.h"

using namespace Vaca;

#define SPLITBAR_DEFAULT_SIZE		5
#define SPLITBAR_DEFAULT_POS		50

SplitBar::SplitBar(Orientation orientation, Widget* parent, Style style)
  : Widget(SplitBarClass::getClassName(), parent, style)
  , m_orientation(orientation)
  , m_pane1(NULL)
  , m_pane2(NULL)
  , m_barSize(SPLITBAR_DEFAULT_SIZE)
  , m_barPos(SPLITBAR_DEFAULT_POS)
  , m_fullDrag(true)
  , m_trackerIsVisible(false)
  , m_gripperVisible(true)
{
}

SplitBar::~SplitBar()
{
}

void SplitBar::setPane1(Widget* widget)
{
  m_pane1 = widget;
  updateChildrenVisibility();
  layout();
}

void SplitBar::setPane2(Widget* widget)
{
  m_pane2 = widget;
  updateChildrenVisibility();
  layout();
}

Widget* SplitBar::getPane1() const
{
  return m_pane1;
}

Widget* SplitBar::getPane2() const
{
  return m_pane2;
}

int SplitBar::getBarSize() const
{
  return m_barSize;
}

void SplitBar::setBarSize(int size)
{
  if (m_barSize != size) {
    m_barSize = size;
    layout();
  }
}

double SplitBar::getBarPosition() const
{
  return m_barPos;
}

void SplitBar::setBarPosition(double pos)
{
  m_barPos = pos;
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

void SplitBar::onLayout(LayoutEvent& ev)
{
  Rect rcL, rcR;
  getRects(rcL, rcR);
  if (m_pane1) {
    m_pane1->setBounds(rcL);
    m_pane1->layout();
  }
  if (m_pane2) {
    m_pane2->setBounds(rcR);
    m_pane2->layout();
  }
  Widget::onLayout(ev);
}

void SplitBar::onResize(ResizeEvent& ev)
{
  invalidate(true);
  // layout();
  Widget::onResize(ev);
}

void SplitBar::onPaint(PaintEvent& ev)
{
  Graphics& g = ev.getGraphics();

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
}

void SplitBar::onMouseDown(MouseEvent& ev)
{
  Widget::MouseDown(ev);

  // capture the mouse only if the user clicks in the bar
  if (getBarRect().contains(ev.getPoint())) {
    m_oldPoint = ev.getPoint();
    m_oldBarPos = m_barPos;

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

    bool byPixels = (getStyle() & Styles::ByPixels) == Styles::ByPixels;

    // Bar-position by pixels
    if (byPixels) {
      if (m_orientation == Orientation::Vertical) {
	m_barPos = m_oldBarPos + (ev.getPoint().x - m_oldPoint.x);
	m_barPos = clamp_value(m_barPos, 0.0, (double)rcClient.w-m_barSize);
      }
      else {
	m_barPos = m_oldBarPos + (ev.getPoint().y - m_oldPoint.y);
	m_barPos = clamp_value(m_barPos, 0.0, (double)rcClient.h-m_barSize);
      }
    }
    // Bar-position by percentage
    else {
      if (m_orientation == Orientation::Vertical) {
	m_barPos = m_oldBarPos + 100.0 * (ev.getPoint().x - m_oldPoint.x) / getBounds().w;
      }
      else {
	m_barPos = m_oldBarPos + 100.0 * (ev.getPoint().y - m_oldPoint.y) / getBounds().h;
      }
      m_barPos = clamp_value(m_barPos, 0.0, 100.0);
    }

    if (!isFullDrag())
      drawTracker(g);
    else {
      layout();
      invalidate(true);
    }
  }
}

void SplitBar::onSetCursor(SetCursorEvent& ev)
{
  if (!ev.isConsumed()) {
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
    ev.setCursor(Cursor(sysCursor));
  }
  Widget::onSetCursor(ev);
}

void SplitBar::updateChildrenVisibility()
{
  // Hide children that aren't first or second
  WidgetList children = getChildren();
  for (WidgetList::iterator
	 it = children.begin(); it != children.end(); ++it) {
    Widget* child = *it;
    child->setVisible(child == m_pane1 ||
		      child == m_pane2);
  }
}

Rect SplitBar::getBarRect() const
{
  Rect rcBar(getClientBounds());
  bool byPixels = (getStyle() & Styles::ByPixels) == Styles::ByPixels;

  if (m_orientation == Orientation::Vertical) {
    if (byPixels)
      rcBar.x = (int)clamp_value(m_barPos, 0.0, (double)rcBar.w-m_barSize);
    else
      rcBar.x = (int)clamp_value(rcBar.w * m_barPos / 100, 0.0, (double)rcBar.w-m_barSize);

    rcBar.w = m_barSize;
  }
  else {
    if (byPixels)
      rcBar.y = (int)clamp_value(m_barPos, 0.0, (double)rcBar.h-m_barSize);
    else
      rcBar.y = (int)clamp_value(rcBar.h * m_barPos / 100.0, 0.0, (double)rcBar.h-m_barSize);

    rcBar.h = m_barSize;
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
