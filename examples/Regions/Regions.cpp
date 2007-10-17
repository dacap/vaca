// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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

#include <cmath>

using namespace Vaca;

class Regions : public Panel
{
  Region *m_region;
  Point m_startPoint;
  Point m_oldPoint;
  bool m_readOnly;
  bool m_erasing;
  bool m_ellipses;

public:

  Regions(Widget *parent)
    : Panel(parent, PanelStyle + ClientEdgeStyle)
    , m_region(NULL)
    , m_readOnly(true)
  {
    setBgColor(Color::White);
  }

  void setRegion(Region *region, bool readOnly, bool ellipses = false)
  {
    m_region = region;
    m_readOnly = readOnly;
    m_ellipses = ellipses;

    invalidate(true);
  }

protected:
    
  virtual void onPaint(Graphics &g)
  {
    if (m_region != NULL) {
      Brush brush(m_readOnly ? Color::Gray: Color::Red);
      g.fillRegion(brush, *m_region);
    }
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    if (m_region != NULL &&
	!m_readOnly &&
	!hasCapture()) {
      acquireCapture();

      m_startPoint =
	m_oldPoint = System::getCursorPos();

      ScreenGraphics g;
      xorRect(g, m_oldPoint);

      // with right mouse button we erase 
      m_erasing = ev.getButton() == MouseButtons::Right;
    }
  }

  virtual void onMouseMove(MouseEvent &ev)
  {
    if (hasCapture()) {
      Point newPoint = System::getCursorPos();

      // clean the feedback rectangle
      ScreenGraphics g;
      xorRect(g, m_oldPoint);

      // draw the feedback rectangle in new position
      xorRect(g, newPoint);
      
      // the old point is now the new point
      m_oldPoint = newPoint;
    }
  }

  virtual void onMouseUp(MouseEvent &ev)
  {
    if (hasCapture()) {
      // we can release the capture (remember to check hasCapture()
      // before to release the mouse capture)
      releaseCapture();

      // clean the feedback rectangle
      ScreenGraphics g;
      xorRect(g, m_oldPoint);

      // new rectangle (absolute coordinates)
      Rect newRect(VACA_MIN(m_startPoint.x, m_oldPoint.x),
		   VACA_MIN(m_startPoint.y, m_oldPoint.y),
		   abs(m_startPoint.x-m_oldPoint.x)+1,
		   abs(m_startPoint.y-m_oldPoint.y)+1);

      // translate to client area (relative)
      newRect.offset(-getAbsoluteClientBounds().getOrigin());

      Region newRegion =
	m_ellipses ? Region::fromEllipse(newRect):
		     Region::fromRect(newRect);

      // update the region
      if (m_erasing)
	*m_region = (*m_region) - newRegion; // substract
      else
	*m_region = (*m_region) | newRegion; // union

      invalidate(true);
    }
  }

private:

  void xorRect(Graphics &g, const Point &pt)
  {
    Rect rc(VACA_MIN(m_startPoint.x, pt.x),
	    VACA_MIN(m_startPoint.y, pt.y),
	    abs(m_startPoint.x-pt.x)+1,
	    abs(m_startPoint.y-pt.y)+1);
    
    g.setRop2(R2_NOTXORPEN);
    g.intersectClipRect(getAbsoluteClientBounds());

    Pen pen(Color::Black);			// TODO XorPen?

    if (m_ellipses)
      g.drawEllipse(pen, rc);
    else
      g.drawRect(pen, rc);

    g.setRop2(R2_COPYPEN);
  }

};

class MainFrame : public Frame
{
  TabBase m_tab;
  Regions m_regions;
  Region m_regionA;
  Region m_regionB;
  Region m_region;

public:
  
  MainFrame()
    : Frame("Regions")
    , m_tab(this)
    , m_regions(&m_tab)
  {
    setLayout(new ClientLayout);
    m_tab.setLayout(new BoxLayout(Vertical, true)); // as ClientLayout,
						   // but with borders

    m_tab.addPage("Region A");
    m_tab.addPage("Region B");
    m_tab.addPage("Union: A | B");
    m_tab.addPage("Intersect: A && B");
    m_tab.addPage("Substract: A - B");
    m_tab.addPage("Substract: B - A");
    m_tab.addPage("Xor: A ^ B");

    m_tab.PageChange.connect(Bind(&MainFrame::onPageChange, this));

    m_regionA = Region::fromRect(Rect(32, 32, 64, 64));
    m_regionB = Region::fromEllipse(Rect(48, 48, 64, 64));

    onPageChange();
  }

private:

  void onPageChange()
  {
    int pageIndex = m_tab.getActivePage();

    switch (pageIndex) {
      // Region A
      case 0:
	m_regions.setRegion(&m_regionA, false);
	break;
      // Region B
      case 1:
	m_regions.setRegion(&m_regionB, false, true);
	break;
      // A | B
      case 2:
	m_region = m_regionA | m_regionB;
	m_regions.setRegion(&m_region, true);
	break;
      // A & B
      case 3: 
	m_region = m_regionA & m_regionB;
	m_regions.setRegion(&m_region, true);
	break;
      // A - B
      case 4: 
	m_region = m_regionA - m_regionB;
	m_regions.setRegion(&m_region, true);
	break;
      // B - A
      case 5: 
	m_region = m_regionB - m_regionA;
	m_regions.setRegion(&m_region, true);
	break;
      // A ^ B
      case 6:
	m_region = m_regionA ^ m_regionB;
	m_regions.setRegion(&m_region, true);
	break;
    }
  }
  
};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main(std::vector<String> args) {
    m_mainFrame.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
