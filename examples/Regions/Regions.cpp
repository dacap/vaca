// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include <cmath>
#include "../resource.h"

using namespace vaca;

class Regions : public Widget
{
  Region m_region;
  Point m_startPoint;
  Point m_oldPoint;
  bool m_readOnly;
  bool m_erasing;
  bool m_ellipses;

public:

  Regions(Widget* parent)
    : Widget(parent, Widget::Styles::Default +
		     Widget::Styles::ClientEdge)
    , m_readOnly(true)
  {
    setBgColor(Color::White);
  }

  void setRegion(Region& region, bool readOnly, bool ellipses = false)
  {
    m_region = region;
    m_readOnly = readOnly;
    m_ellipses = ellipses;

    invalidate(true);
  }

protected:

  virtual void onPaint(PaintEvent& ev)
  {
    Graphics& g = ev.getGraphics();
    Brush brush(m_readOnly ? Color::Gray: Color::Red);
    g.fillRegion(brush, m_region);
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    if (!m_readOnly && !hasCapture()) {
      captureMouse();

      m_startPoint =
	m_oldPoint = System::getCursorPos();

      ScreenGraphics g;
      xorRect(g, m_oldPoint);

      // with right mouse button we erase
      m_erasing = ev.getButton() == MouseButton::Right;
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
      releaseMouse();

      // clean the feedback rectangle
      ScreenGraphics g;
      xorRect(g, m_oldPoint);

      // new rectangle (absolute coordinates)
      Rect newRect(min_value(m_startPoint.x, m_oldPoint.x),
		   min_value(m_startPoint.y, m_oldPoint.y),
		   abs(m_startPoint.x-m_oldPoint.x)+1,
		   abs(m_startPoint.y-m_oldPoint.y)+1);

      // translate to client area (relative)
      newRect.offset(-getAbsoluteClientBounds().getOrigin());

      Region newRegion =
	m_ellipses ? Region::fromEllipse(newRect):
		     Region::fromRect(newRect);

      // update the region
      if (m_erasing)
	m_region -= newRegion; // substract
      else
	m_region |= newRegion; // union

      invalidate(true);
    }
  }

private:

  void xorRect(Graphics &g, const Point &pt)
  {
    Rect rc(min_value(m_startPoint.x, pt.x),
	    min_value(m_startPoint.y, pt.y),
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
  Label m_label;
  Regions m_regions;
  Region m_regionA;
  Region m_regionB;
  Region m_region;

public:

  MainFrame()
    : Frame(L"Regions")
    , m_tab(this)
    , m_label(L"Use right/left mouse buttons to draw/erase regions", this)
    , m_regions(&m_tab)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_tab.setConstraint(new BoxConstraint(true));
    m_tab.setLayout(new BoxLayout(Orientation::Vertical, true));

    m_tab.addPage(L"Region A");
    m_tab.addPage(L"Region B");
    m_tab.addPage(L"Union: A | B");
    m_tab.addPage(L"Intersect: A && B");
    m_tab.addPage(L"Substract: A - B");
    m_tab.addPage(L"Substract: B - A");
    m_tab.addPage(L"Xor: A ^ B");

    m_tab.PageChange.connect([this]{ onPageChange(); });

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
	m_regions.setRegion(m_regionA, false);
	break;
      // Region B
      case 1:
	m_regions.setRegion(m_regionB, false, true);
	break;
      // A | B
      case 2:
	m_region = m_regionA | m_regionB;
	m_regions.setRegion(m_region, true);
	break;
      // A & B
      case 3:
	m_region = m_regionA & m_regionB;
	m_regions.setRegion(m_region, true);
	break;
      // A - B
      case 4:
	m_region = m_regionA - m_regionB;
	m_regions.setRegion(m_region, true);
	break;
      // B - A
      case 5:
	m_region = m_regionB - m_regionA;
	m_regions.setRegion(m_region, true);
	break;
      // A ^ B
      case 6:
	m_region = m_regionA ^ m_regionB;
	m_regions.setRegion(m_region, true);
	break;
    }
  }

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
