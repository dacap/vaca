// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#include <Vaca/Vaca.h>

using namespace Vaca;

static Side side[4] = { Side::Left,
			Side::Top,
			Side::Right,
			Side::Bottom };
static Sides sides[4] = { Sides::Left,
			  Sides::Top,
			  Sides::Right,
			  Sides::Bottom };

class AnchoredWidget : public Panel
{
  Anchor* m_anchor;
  Sides m_hotSides;
  
public:

  AnchoredWidget(Anchor* anchor, Widget* parent)
    : Panel(parent)
    , m_anchor(anchor)
  {
    setBgColor(Color::White);
    setFgColor(Color::Black);

    setConstraint(m_anchor);
    setDoubleBuffered(true);
  }

protected:

  virtual void onResize(const Size &sz)
  {
    Panel::onResize(sz);
    invalidate(true);
  }

  virtual void onPaint(Graphics &g)
  {
    Rect rc(getClientBounds());

    Pen pen(getFgColor());
    g.drawRect(pen, rc);

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
      }
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
  }

  virtual void onMouseLeave()
  {
    if (m_hotSides != 0) {
      m_hotSides = 0;
      invalidate(true);
    }
  }
  
private:

  void drawRect(Graphics &g, const Rect &rc, Sides b)
  {
    Color color((m_hotSides & b) == b ? Color::Yellow:
					  getFgColor());
    
    if ((m_anchor->getSides() & b) == b) {
      Brush brush(color);
      g.fillRect(brush, rc);
    }
    else {
      Pen pen(color);
      g.drawRect(pen, rc);
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
    : Frame("AnchorLayouts")
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

  virtual void onResize(const Size &sz)
  {
    Frame::onResize(sz);
    invalidate(true);
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

  virtual void onPaint(Graphics &g)
  {
    Rect rc(getClientBounds());
    Pen pen(getBgColor()*1.2);

    g.drawRect(pen, Rect(rc.getCenter() - Point(m_refSize/2), m_refSize));
  }

private:
  
};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main() {
    m_mainFrame.setVisible(true);
  }
};

int VACA_MAIN()
{
  Example app;
  app.run();
  return 0;
}
