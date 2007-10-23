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

#include <time.h>

#include <Vaca/Vaca.h>
#include "resource.h"

using namespace Vaca;

class MainFrame : public Frame
{
  Image m_vacaImage;
  Point m_vacaOrigin;
  Image m_mouseImage;
  Point m_mousePoint;

public:

  MainFrame()
    : Frame("Images")
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
    Graphics &g = *m_mouseImage.getGraphics();
    Size sz = m_mouseImage.getSize();

    // ...clear the background
    Brush brush(Color::Black);
    g.fillRect(brush, Rect(sz));

    // ...draw the scope
    Pen pen(Color::Red, 3);
    g.drawEllipse(pen, Rect(sz).shrink(4));

    pen.setWidth(1);
    g.drawLine(pen, 0, sz.h/2, sz.w, sz.h/2);
    g.drawLine(pen, sz.w/2, 0, sz.w/2, sz.h);

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
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    Graphics &g = *m_vacaImage.getGraphics();
    int d = 8+(rand()%13);
    Size sz = Size(d, d);
    Point pt = ev.getPoint() - m_vacaOrigin;

    Brush redBrush(Color::Red);
    g.fillEllipse(redBrush, Rect(pt - Point(sz/2), sz));

    Pen redPen(Color::Red);	// TODO remove this, use GraphicsPath

    for (int c=0; c<5; c++) {
      g.beginPath();
      g.moveTo(pt);
      g.lineTo(redPen, pt + Point((rand()%5)-2, (rand()%5)-2));
      g.lineTo(redPen, pt + Point((rand()%65)-32, (rand()%65)-32));
      g.endPath();
      g.fillPath(redBrush);
    }

    invalidate(false);
  }

  virtual void onMouseEnter(MouseEvent &ev)
  {
    invalidate(false);
  }

  virtual void onMouseLeave()
  {
    invalidate(false);
  }

  virtual void onSetCursor(WidgetHitTest hitTest)
  {
    if (hitTest == WidgetHitTest::Client)
      setCursor(Cursor(SysCursor::None));
    else
      Frame::onSetCursor(hitTest);
  }

  virtual void onPaint(Graphics &g)
  {
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

  virtual void onResize(const Size &sz)
  {
    Widget::onResize(sz);
    invalidate(false);
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;

  virtual void main(std::vector<String> args) {
    m_mainFrame.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  srand(static_cast<unsigned int>(time(NULL)));

  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
