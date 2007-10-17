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

#include "Vaca/Vaca.h"
#include "resource.h"

using namespace Vaca;

class MainFrame : public Frame
{
  Image mVacaImage;
  Point mVacaOrigin;
  Image mMouseImage;
  Point mMousePoint;

public:

  MainFrame()
    : Frame("Images")
    , mVacaImage(IDB_VACA)
    , mMouseImage(Size(64, 64))
  {
    // set the Frame's size to the mVacaImage's size (plus the
    // non-client size needed by the Frame's borders)
    setSize(getNonClientSize() + mVacaImage.getSize());

    // the background color is white
    setBgColor(Color::White);

    // draw the image controlled by the mouse
    Graphics &g = mMouseImage.getGraphics();
    Size sz = mMouseImage.getSize();

    // ...clear the background
    g.setColor(Color::Black);
    g.fillRect(Rect(sz));

    // ...draw the scope
    g.setColor(Color::Red);
    g.setPenSize(3);
    g.drawEllipse(Rect(sz).shrink(4));

    g.setPenSize(1);
    g.drawLine(0, sz.h/2, sz.w, sz.h/2);
    g.drawLine(sz.w/2, 0, sz.w/2, sz.h);

    // center the frame
    center();
  }

protected:

  virtual void onMouseMove(MouseEvent &ev)
  {
    // invalidate the current area under by the mMouseImage
    invalidate(Rect(mMousePoint -
		    Point(mMouseImage.getSize()/2),
		    mMouseImage.getSize()), false);

    // new mouse position
    mMousePoint = ev.getPoint();

    // invalidate the new area under by the mMouseImage
    invalidate(Rect(mMousePoint -
		    Point(mMouseImage.getSize()/2),
		    mMouseImage.getSize()), false);
  }

  virtual void onDoubleClick(MouseEvent &ev)
  {
    onMouseDown(ev);
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    Graphics &g = mVacaImage.getGraphics();
    int d = 8+(rand()%13);
    Size sz = Size(d, d);
    Point pt = ev.getPoint() - mVacaOrigin;

    g.setColor(Color::Red);
    g.fillEllipse(Rect(pt - Point(sz/2), sz));

    for (int c=0; c<5; c++) {
      g.beginPath();
      g.moveTo(pt);
      g.lineTo(pt + Point((rand()%5)-2, (rand()%5)-2));
      g.lineTo(pt + Point((rand()%65)-32, (rand()%65)-32));
      g.endPath();
      g.fillPath();
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

  virtual void onSetCursor(int hitTest)
  {
    if (hitTest == HTCLIENT)
      setCursor(Cursor(NoCursor));
    else
      Frame::onSetCursor(hitTest);
  }

  virtual void onPaint(Graphics &gDest)
  {
    Size size = getClientBounds().getSize();
    Image image(gDest, size); // Create the image for double-buffering
    Graphics &g(image.getGraphics()); // Get the Graphics to draw in the image
    Rect bounds = Rect(size);	      // Area where we can draw

    g.setColor(getBgColor());
    g.fillRect(bounds);

    mVacaOrigin =
      Point(bounds.getCenter()) -
      Point(mVacaImage.getSize()/2);

    g.drawImage(mVacaImage, mVacaOrigin);

    // Widget::hasMouse is a method to known if the widget has the
    // mouse inside
    if (hasMouse())
      g.drawImage(mMouseImage,
		  mMousePoint -
		  Point(mMouseImage.getSize()/2),
		  // the "background color" or "mask color" is Black
		  Color::Black);

    // draw the image on "gDest"
    gDest.drawImage(image, getClientBounds().getOrigin());
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
  MainFrame mMainWnd;

  virtual void main(std::vector<String> args) {
    mMainWnd.setVisible(true);
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
