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

using namespace Vaca;

class Scribble : public Panel
{
  Image mImage;
  Point mPoint[3];
  bool mErasing;

public:

  Scribble(Widget *parent)
    : Panel(parent)
  {
    setBgColor(Color::White);
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

      for (int c=0; c<3; ++c)
	mPoint[c] = ev.getPoint();

      // with right mouse button we erase 
      mErasing = ev.getButton() == MouseButtons::Right;
    }
  }

  virtual void onMouseMove(MouseEvent &ev)
  {
    if (hasCapture()) {
      // get the graphics from the to draw into the image
      Graphics &g(mImage.getGraphics());

      // we must to use "System::getCursorPos()" (absolute cursor
      // position), because "ev.getPoint()" is useful only inside the
      // client-bounds, but we adquire the mouse capture, so we need
      // to know the position of the mouse outside the client bounds
      Point newPoint =
	System::getCursorPos()
	- getAbsoluteClientBounds().getOrigin();

      // rotate points
      mPoint[0] = mPoint[1];
      mPoint[1] = mPoint[2];
      mPoint[2] = newPoint;

      // pen size
      int penSize;

      // erase
      if (mErasing) {
	g.setColor(Color::White);
	g.setPenSize(penSize = 64);
	g.drawLine(mPoint[1], mPoint[2]);
      }
      // draw
      else {
	g.setColor(Color::Black);
	g.setPenSize(penSize = 1);
	g.drawLine(mPoint[1], mPoint[2]);

	// this add an extra style to the trace (more realistic)
	g.beginPath();
	g.moveTo(mPoint[0]);
	g.lineTo(mPoint[1]);
	g.lineTo(mPoint[2]);
	g.endPath();
	g.fillPath();
      }

      // invalidate the drawn area
      Point minPoint = mPoint[0];
      Point maxPoint = mPoint[0];
      for (int c = 1; c < 3; ++c) {
	if (minPoint.x > mPoint[c].x) minPoint.x = mPoint[c].x;
	if (minPoint.y > mPoint[c].y) minPoint.y = mPoint[c].y;
	if (maxPoint.x < mPoint[c].x) maxPoint.x = mPoint[c].x;
	if (maxPoint.y < mPoint[c].y) maxPoint.y = mPoint[c].y;
      }
      invalidate(Rect(minPoint-penSize, maxPoint+penSize), false);
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
