// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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
#include "../resource.h"

using namespace Vaca;

static Side side[4] = { Side::Left,
			Side::Top,
			Side::Right,
			Side::Bottom };
static Sides sides[4] = { Sides::Left,
			  Sides::Top,
			  Sides::Right,
			  Sides::Bottom };

class AnchoredWidget : public Widget
{
  Anchor* m_anchor;
  Sides m_hotSides;

public:

  AnchoredWidget(Anchor* anchor, Widget* parent)
    : Widget(parent)
    , m_anchor(anchor)
  {
    setBgColor(Color(200, 200, 255));
    setFgColor(Color(0, 100, 120));

    setConstraint(m_anchor);
    setDoubleBuffered(true);
  }

protected:

  virtual void onResize(ResizeEvent& ev)
  {
    invalidate(true);
    Widget::onResize(ev);
  }

  virtual void onPaint(PaintEvent& ev)
  {
    Graphics& g = ev.getGraphics();
    Rect rc(getClientBounds());

    Pen pen(getFgColor());
    g.drawRoundRect(pen, rc, Size(8, 8));

    for (int c=0; c<4; ++c)
      drawRect(g, getSideRect(side[c]), sides[c]);
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    for (int c=0; c<4; ++c)
      if (getSideRect(side[c]).contains(ev.getPoint())) {
	m_anchor->setSides(m_anchor->getSides() ^ sides[c]);
	getParent()->layout();
	invalidate(true);
	ev.consume();
      }

    Widget::onMouseDown(ev);
  }

  virtual void onMouseMove(MouseEvent &ev)
  {
    for (int c=0; c<4; ++c) {
      if (getSideRect(side[c]).contains(ev.getPoint())) {
	if ((m_hotSides & sides[c]) == 0) {
	  m_hotSides |= sides[c];
	  invalidate(true);
	}
      }
      else if ((m_hotSides & sides[c]) == sides[c]) {
	m_hotSides &= ~sides[c];
	invalidate(true);
      }
    }
    Widget::onMouseDown(ev);
  }

  virtual void onMouseLeave(MouseEvent& ev)
  {
    if (m_hotSides != 0) {
      m_hotSides = 0;
      invalidate(true);
    }
    Widget::onMouseLeave(ev);
  }

private:

  void drawRect(Graphics &g, const Rect &rc, Sides b)
  {
    Color color((m_hotSides & b) == b ? Color::Yellow:
					  getFgColor());
    
    if ((m_anchor->getSides() & b) == b) {
      Brush brush(color);
      g.fillRoundRect(brush, rc, Size(8, 8));
    }
    else {
      Pen pen(color);
      g.drawRoundRect(pen, rc, Size(8, 8));
    }
  }

  Rect getSideRect(Side side)
  {
    Rect rc(getClientBounds());
    
    switch (side) {
      case Side::Left:   return Rect(rc.x,        rc.y,        8, rc.h);
      case Side::Top:    return Rect(rc.x,        rc.y,        rc.w, 8);
      case Side::Right:  return Rect(rc.x+rc.w-8, rc.y,        8, rc.h);
      case Side::Bottom: return Rect(rc.x,        rc.y+rc.h-8, rc.w, 8);
    }

    throw;
  }

};

class MainFrame : public Frame
{
  Size m_refSize;
  Size m_minSize;
  AnchoredWidget m_allWidget;
  AnchoredWidget m_leftWidget;
  AnchoredWidget m_topWidget;
  AnchoredWidget m_rightWidget;
  AnchoredWidget m_bottomWidget;

public:

  MainFrame()
    : Frame(L"AnchorLayouts")
    , m_allWidget   (new Anchor(Rect( 48,  48, 160, 160), Sides::All), this)
    , m_leftWidget  (new Anchor(Rect(  8,  48,  32, 160), Sides::Left   | Sides::Top  | Sides::Bottom), this)
    , m_topWidget   (new Anchor(Rect( 48,   8, 160,  32), Sides::Top    | Sides::Left | Sides::Right), this)
    , m_rightWidget (new Anchor(Rect(216,  48,  32, 160), Sides::Right  | Sides::Top  | Sides::Bottom), this)
    , m_bottomWidget(new Anchor(Rect( 48, 216, 160,  32), Sides::Bottom | Sides::Left | Sides::Right), this)
  {
    m_refSize = Size(256, 256);
    setLayout(new AnchorLayout(m_refSize));
    setSize(m_minSize = getPreferredSize() + m_refSize);
  }

protected:

  virtual void onResize(ResizeEvent& ev)
  {
    invalidate(true);
    Frame::onResize(ev);
  }

  virtual void onResizing(CardinalDirection dir, Rect &rc)
  {
    if (rc.w < m_minSize.w) {
      if (dir == CardinalDirection::West ||
	  dir == CardinalDirection::Northwest ||
	  dir == CardinalDirection::Southwest)
	rc.x = rc.x+rc.w-m_minSize.w;
      rc.w = m_minSize.w;
    }

    if (rc.h < m_minSize.h) {
      if (dir == CardinalDirection::North ||
	  dir == CardinalDirection::Northeast ||
	  dir == CardinalDirection::Northwest)
	rc.y = rc.y+rc.h-m_minSize.h;
      rc.h = m_minSize.h;
    }
  }

  virtual void onPaint(PaintEvent& ev)
  {
    Graphics& g = ev.getGraphics();
    Rect rc(getClientBounds());
    Pen pen(getBgColor()*1.2);

    g.drawRect(pen, Rect(rc.getCenter() - Point(m_refSize/2), m_refSize));
  }

private:
  
};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
