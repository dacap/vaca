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

class MainFrame : public Frame
{
  std::vector<Point> mPoints;
  int mHotPoint;
  
public:

  MainFrame()
    : Frame("Curves (Lagrange polynomial)")
  {
    setBgColor(Color::Black);
    setFgColor(Color(0, 220, 0));

    mPoints.push_back(Point(-128, 0));
    mPoints.push_back(Point(0, -64));
    mPoints.push_back(Point(64, -128));

    mHotPoint = -1;

    // use double-buffering technique for this Widget when onPaint()
    // event is received
    setDoubleBuffering(true);
  }

protected:

  virtual void onDoubleClick(MouseEvent &ev)
  {
    // convert a double-click in a single-click
    onMouseDown(ev);
  }

  // when a mouse's button is pressed
  virtual void onMouseDown(MouseEvent &ev)
  {
    if (mHotPoint >= 0) {
      // drag the point with left button
      if (ev.getButton() == MouseButtons::Left)
	acquireCapture();
      // ...or remove the point with the right one
      else {
	// remove point
	remove_element_from_container(mPoints,
				      mPoints[mHotPoint]);

	// update hot-tracking point
	mHotPoint = -1;

	// redraw
	invalidate(true);
      }
    }
    else {
      // add a new point
      mPoints.push_back(viewToModel(ev.getPoint()));
      
      // update hot-tracking point
      onMouseMove(ev);

      // redraw
      invalidate(true);
    }
  }

  // when a mouse's button is unpressed
  virtual void onMouseUp(MouseEvent &ev)
  {
    if (hasCapture())
      releaseCapture();
  }

  // when the mouse is moved
  virtual void onMouseMove(MouseEvent &ev)
  {
    // dragging a point
    if (hasCapture()) {
      mPoints[mHotPoint] = viewToModel(ev.getPoint());
      invalidate(true);
    }
    // change hot-tracking point
    else {
      int oldHotTracking = mHotPoint;

      mHotPoint = -1;

      // number of points
      int n = mPoints.size();

      // check points
      for (int i=0; i<n; ++i) {
	Size sz(16, 16);
	Rect pointBounds = Rect(modelToView(mPoints[i]), sz).offset(-Point(sz/2));

	if (pointBounds.contains(ev.getPoint())) {
	  mHotPoint = i;
	  break;
	}
      }

      if (oldHotTracking != mHotPoint)
	invalidate(true);
    }
  }

  virtual void onResize(const Size &sz)
  {
    Frame::onResize(sz);
    invalidate(true);
  }

  virtual void onPaint(Graphics &g)
  {
    Rect rc = getClientBounds();
    Point origin = rc.getCenter();

    g.setColor(getFgColor());

    // draw axis
    g.drawLine(rc.x, rc.y+rc.h/2, rc.x+rc.w, rc.y+rc.h/2);
    g.drawLine(rc.x+rc.w/2, rc.y, rc.x+rc.w/2, rc.y+rc.h);

    // number of points
    int n = mPoints.size();
    
    // draw points
    for (int i=0; i<n; ++i) {
      Size sz(8, 8);

      g.fillEllipse(Rect(modelToView(mPoints[i]), sz).offset(-Point(sz/2)));

      if (i == mHotPoint) {
	sz = Size(16, 16);
	g.drawEllipse(Rect(modelToView(mPoints[i]), sz).offset(-Point(sz/2)));
      }
    }

    // draw the f(x) (Lagrange polynomial)
    for (int deviceX=rc.x; deviceX<rc.x+rc.w; ++deviceX) {
      int x = viewToModel(Point(deviceX, 0)).x;
      double y = 0;

      for (int i=0; i<n; ++i) {
	double y_i = mPoints[i].y;

	for (int j=0; j<n; ++j) {
	  if (i == j)
	    continue;

	  y_i *=
	    static_cast<double>(x - mPoints[j].x) /
	    static_cast<double>(mPoints[i].x - mPoints[j].x);
	}

	y += y_i;
      }

      g.setPixel(modelToView(Point(x, static_cast<int>(y))),
		 Color::White);
    }
  }

  Point viewToModel(const Point &_pt)
  {
    Point pt = _pt - getClientBounds().getCenter();
    return Point(pt.x, -pt.y);
  }

  Point modelToView(const Point &pt)
  {
    return getClientBounds().getCenter() + Point(pt.x, -pt.y);
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
