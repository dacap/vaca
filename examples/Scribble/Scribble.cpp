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

#include "Vaca/Vaca.h"

#include <math.h>

using namespace Vaca;

class Scribble : public Panel
{
  Image mImage;
  Point mOldPoint;
  bool mErasing;

public:

  Scribble(Widget *parent)
    : Panel(parent)
  {
  }

protected:

  virtual void onResize(const Size &size)
  {
    Size imageSize = getClientBounds().getSize();
    if (imageSize.w == 0 || imageSize.h == 0)
      return;
    
    Image newImage(imageSize);

    Graphics &g(newImage.getGraphics());

    // clear the new image with a white background
    g.setColor(Color::White);
    g.fillRect(0, 0, imageSize.w, imageSize.h);

    // draw the old image in the center of the new image
    if (mImage.isValid()) {
      g.drawImage(mImage, Point(imageSize/2 - mImage.getSize()/2));
    }

    // clone the image
    mImage = newImage;

    // redraw all the image
    invalidate(false);
  }

  virtual void onPaint(Graphics &g)
  {
    // draw only the clip area
    Rect rc = g.getClipBounds();
    g.drawImage(mImage, rc.getOrigin(), rc);
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    if (!hasCapture()) {
      acquireCapture();
      mOldPoint = ev.getPoint();

      // with right mouse button we erase 
      mErasing = ev.getButton() == MouseButtons::Right;
    }
  }

  virtual void onMouseMove(MouseEvent &ev)
  {
    if (hasCapture()) {
      Graphics &g(mImage.getGraphics());
      // we must to use "System::getCursorPos()" (absolute cursor
      // position), because "ev.getPoint()" is useful only inside the
      // client-bounds, but we adquire the mouse capture, so we need
      // to know the position of the mouse outside the client bounds
      Point newPoint =
	System::getCursorPos()
	- getAbsoluteClientBounds().getOrigin();

      // pen size
      int penSize = 0;

      // erase the line
      if (mErasing) {
	g.setColor(Color::White);
	// the eraser has a constant pen size
	penSize = 128;
      }
      // draw the line
      else {
	g.setColor(Color::Black);
	// the pen size is directly proportional to the length of the
	// drawn line
	penSize = (int)sqrt(pow(newPoint.x-mOldPoint.x, 2) +
			    pow(newPoint.y-mOldPoint.y, 2));
      }
      
      g.setPenSize(penSize);
      g.drawLine(mOldPoint, newPoint);

      // invalidate the drawn region
      Point origin = Point(VACA_MIN(mOldPoint.x, newPoint.x),
			   VACA_MIN(mOldPoint.y, newPoint.y));
      Size size = Size(VACA_MAX(mOldPoint.x, newPoint.x) - origin.x + 1,
		       VACA_MAX(mOldPoint.y, newPoint.y) - origin.y + 1);
      invalidate(Rect(origin - penSize/2,
		      size + penSize), false);

      // the old point is now the new point
      mOldPoint = newPoint;
    }
  }

  virtual void onMouseUp(MouseEvent &ev)
  {
    if (hasCapture()) {
      // we can release the capture (remember to check hasCapture()
      // before to release the mouse capture)
      releaseCapture();
    }
  }

};

class MainFrame : public Frame
{
  Scribble mScrible;

public:
  
  MainFrame()
    : Frame("Scribble")
    , mScrible(this)
  {
    setLayout(new ClientLayout);
  }
  
};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame mMainWnd;
public:
  virtual void main(std::vector<String> args) {
    mMainWnd.setVisible(true);
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
