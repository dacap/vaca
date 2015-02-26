// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <time.h>

#include <vaca/vaca.h>
#include "resource.h"

using namespace vaca;

class MainFrame : public Frame
{
  Image m_vacaImage;
  Point m_vacaOrigin;
  Image m_mouseImage;
  Point m_mousePoint;

public:

  MainFrame()
    : Frame(L"Images")
    , m_vacaImage(ResourceId(IDB_VACA))
    , m_mouseImage(Size(64, 64))
  {
    setDoubleBuffered(true);

    // set the Frame's size to the m_vacaImage's size (plus the
    // non-client size needed by the Frame's borders)
    setSize(getNonClientSize() + m_vacaImage.getSize());

    // the background color is white
    setBgColor(Color::White);

    // draw the image controlled by the mouse
    Graphics& g = m_mouseImage.getGraphics();
    Size sz = m_mouseImage.getSize();

    // ...clear the background
    Brush brush(Color::Black);
    g.fillRect(brush, Rect(sz));

    // ...draw the scope
    Pen pen3(Color::Red, 3);
    g.drawEllipse(pen3, Rect(sz).shrink(4));

    Pen pen1(Color::Red, 1);
    g.drawLine(pen1, 0, sz.h/2, sz.w, sz.h/2);
    g.drawLine(pen1, sz.w/2, 0, sz.w/2, sz.h);

    // center the frame
    center();
  }

protected:

  virtual void onMouseMove(MouseEvent &ev)
  {
    // invalidate the current area under by the m_mouseImage
    invalidate(Rect(m_mousePoint -
		    Point(m_mouseImage.getSize()/2),
		    m_mouseImage.getSize()), false);

    // new mouse position
    m_mousePoint = ev.getPoint();

    // invalidate the new area under by the m_mouseImage
    invalidate(Rect(m_mousePoint -
		    Point(m_mouseImage.getSize()/2),
		    m_mouseImage.getSize()), false);

    Frame::onMouseMove(ev);
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    Graphics& g = m_vacaImage.getGraphics();
    int d = 8+(rand()%13);
    Size sz = Size(d, d);
    Point pt = ev.getPoint() - m_vacaOrigin;

    Brush redBrush(Color::Red);
    g.fillEllipse(redBrush, Rect(pt - Point(sz/2), sz));

    for (int c=0; c<5; c++) {
      GraphicsPath path;
      path.lineTo((rand()%5)-2, (rand()%5)-2);
      path.lineTo((rand()%65)-32, (rand()%65)-32);
      path.closeFigure();

      g.fillPath(path, redBrush, pt);
    }

    invalidate(false);
    Frame::onMouseDown(ev);
  }

  virtual void onMouseEnter(MouseEvent &ev)
  {
    invalidate(false);
    Frame::onMouseEnter(ev);
  }

  virtual void onMouseLeave(MouseEvent& ev)
  {
    invalidate(false);
    Frame::onMouseLeave(ev);
  }

  virtual void onSetCursor(SetCursorEvent& ev)
  {
    if (!ev.isConsumed()) {
      if (ev.getWidgetHit() == WidgetHit::Client)
	ev.setCursor(Cursor(SysCursor::None));
    }
    Frame::onSetCursor(ev);
  }

  virtual void onPaint(PaintEvent& ev)
  {
    Graphics& g = ev.getGraphics();
    m_vacaOrigin =
      Point(getClientBounds().getCenter()) -
      Point(m_vacaImage.getSize()/2);

    g.drawImage(m_vacaImage, m_vacaOrigin);

    // Widget::hasMouse is a method to known if the widget has the
    // mouse inside
    if (hasMouse())
      g.drawImage(m_mouseImage,
		  m_mousePoint -
		  Point(m_mouseImage.getSize()/2),
		  // the "background color" or "mask color" is Black
		  Color::Black);
  }

  virtual void onResize(ResizeEvent& ev)
  {
    invalidate(false);
    Widget::onResize(ev);
  }

};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  srand(static_cast<unsigned int>(time(NULL)));

  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
