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
#include <cmath>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// 2-D point with floating-point coordinates

struct Point_f
{
  float x, y;

  Point_f(float u, float v) : x(u), y(v) { }
  Point_f(const Point_f &pt) : x(pt.x), y(pt.y) { }
  Point_f(const Point &pt) : x(pt.x), y(pt.y) { }
  Point_f& operator=(const Point_f& pt) { x=pt.x; y=pt.y; return *this; }
  bool operator==(const Point_f& pt) { return x==pt.x && y==pt.y; }

};

//////////////////////////////////////////////////////////////////////
// Draws axis and has methods to convert model <-> view coordinates

class AxisPage : public TabPage
{
public:

  AxisPage(const String& text, Tab* parent)
    : TabPage(text, parent, TabPageStyle + ClientEdgeStyle)
  {
    setBgColor(Color::White);

    // use double-buffering technique for this Widget when onPaint()
    // event is received
    setDoubleBuffered(true);
  }

protected:

  virtual void onResize(const Size &sz)
  {
    TabPage::onResize(sz);
    invalidate(true);
  }

  virtual void onPaint(Graphics &g)
  {
    Rect rc = getClientBounds();
    Point origin = rc.getCenter();

    // draw axis
    Pen pen(Color::Blue, 1, PenStyle::Dot);
    g.drawLine(pen, rc.x, rc.y+rc.h/2, rc.x+rc.w, rc.y+rc.h/2);
    g.drawLine(pen, rc.x+rc.w/2, rc.y, rc.x+rc.w/2, rc.y+rc.h);
  }

  Point_f viewToModel(const Point &_pt)
  {
    Point pt = _pt - getClientBounds().getCenter();
    return Point_f(pt.x/32.0f, -pt.y/32.0f);
  }

  Point modelToView(const Point_f &pt)
  {
    return getClientBounds().getCenter() + Point(static_cast<int>(pt.x*32),
						 static_cast<int>(-pt.y*32));
  }
  
};

//////////////////////////////////////////////////////////////////////
// Controls points with the mouse

class EditablePointsPage : public AxisPage
{
  int mHotPoint;
protected:
  std::vector<Point_f> mPoints;
  
public:

  EditablePointsPage(const String& text, Tab* parent)
    : AxisPage(text, parent)
  {
    mPoints.push_back(Point_f(-2,  0));
    mPoints.push_back(Point_f(-1, -1));
    mPoints.push_back(Point_f( 1,  1));
    mPoints.push_back(Point_f( 2,  0));

    mHotPoint = -1;
  }

protected:

  // when a mouse's button is pressed
  virtual void onMouseDown(MouseEvent &ev)
  {
    if (mHotPoint >= 0) {
      // drag the point with left button
      if (ev.getButton() == MouseButtons::Left)
	captureMouse();
      // ...or remove the point with the right one
      else {
	// remove point
	remove_from_container(mPoints, mPoints[mHotPoint]);

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
      releaseMouse();
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

  virtual void onPaint(Graphics &g)
  {
    AxisPage::onPaint(g);
    
    Rect rc = getClientBounds();
    Point origin = rc.getCenter();

    // number of points
    int n = mPoints.size();
    
    // draw points
    Pen pen(Color::Blue);
    Brush brush(Color::Blue);

    for (int i=0; i<n; ++i) {
      Size sz(8, 8);

      g.fillEllipse(brush, Rect(modelToView(mPoints[i]), sz).offset(-Point(sz/2)));

      if (i == mHotPoint) {
	sz = Size(16, 16);
	g.drawEllipse(pen, Rect(modelToView(mPoints[i]), sz).offset(-Point(sz/2)));
      }
    }
  }
  
};

//////////////////////////////////////////////////////////////////////
// Functions

float func_x(float x) { return x; }
float func_xx(float x) { return x*x; }
float func_xxx(float x) { return x*x*x; }
float func_sin(float x) { return sin(x); }
float func_cos(float x) { return cos(x); }
float func_tan(float x) { return tan(x); }
float func_exp(float x) { return exp(x); }
float func_1_exp_x(float x) { return 1-exp(-x); }
float func_asin(float x) { return asin(x); }
float func_acos(float x) { return acos(x); }
float func_atan(float x) { return atan(x); }
float func_log(float x) { return log(x); }

struct function
{
  float (*f)(float x);
  const char* expr;
} functions[] = {
  { func_x, "x" },
  { func_xx, "x^2" },
  { func_xxx, "x^3" },
  { func_sin, "sin(x)" },
  { func_cos, "cos(x)" },
  { func_tan, "tan(x)" },
  { func_exp, "exp(x)" },
  { func_1_exp_x, "1-exp(-x)" },
  { func_asin, "asin(x)" },
  { func_acos, "acos(x)" },
  { func_atan, "atan(x)" },
  { func_log, "log(x)" },
};

class FunctionsPage : public AxisPage
{
  Timer m_timer;
  int m_currentFunction;
  Font m_font;
  
public:

  FunctionsPage(Tab* parent)
    : AxisPage("2-D functions y=f(x)", parent)
    , m_timer(1500)
    , m_currentFunction(0)
    , m_font("Courier", 10)
  {
    m_timer.Action.connect(Bind(&FunctionsPage::nextFunction, this));
    m_timer.start();

    setFont(m_font);
  }

protected:

  virtual void onPaint(Graphics &g)
  {
    AxisPage::onPaint(g);

    Rect rc = getClientBounds();

    // draw the f(x) = x
    for (int viewX=rc.x; viewX<rc.x+rc.w; ++viewX) {
      float x = viewToModel(Point(viewX, 0)).x;
      float y = functions[m_currentFunction].f(x);

      g.setPixel(modelToView(Point_f(x, y)), Color(0, 192, 0));
    }

    g.drawString(functions[m_currentFunction].expr, 0, 0);
  }

private:

  void nextFunction()
  {
    m_currentFunction =
      (m_currentFunction+1) %
      (sizeof(functions)/sizeof(function));

    invalidate(true);
  }
  
};

//////////////////////////////////////////////////////////////////////
// Lagrange polynomial

class Lagrange : public EditablePointsPage
{
public:

  Lagrange(Tab* parent)
    : EditablePointsPage("Lagrange polynomial", parent)
  {
  }

protected:

  virtual void onPaint(Graphics &g)
  {
    EditablePointsPage::onPaint(g);

    Rect rc = getClientBounds();

    // number of points
    int n = mPoints.size();
    
    // draw the f(x) (Lagrange polynomial)
    for (int viewX=rc.x; viewX<rc.x+rc.w; ++viewX) {
      Point_f pt = viewToModel(Point(viewX, 0));
      pt.y = 0;

      for (int i=0; i<n; ++i) {
	float y_i = mPoints[i].y;

	for (int j=0; j<n; ++j) {
	  if (i == j)
	    continue;

	  y_i *=
	    (pt.x - mPoints[j].x) /
	    (mPoints[i].x - mPoints[j].x);
	}

	pt.y += y_i;
      }

      g.setPixel(modelToView(pt), Color(0, 192, 0));
    }
  }
  
};

//////////////////////////////////////////////////////////////////////
// Bezier

class Bezier : public EditablePointsPage
{
public:

  Bezier(Tab* parent)
    : EditablePointsPage("Bézier curve", parent)
  {
  }

protected:

  virtual void onPaint(Graphics &g)
  {
    EditablePointsPage::onPaint(g);

    // pens
    Pen greenPen(Color(0, 192, 0), 2);
    Pen dashPen(Color::Blue, 1, PenStyle::Dash);

    // number of points
    int n = mPoints.size();
    
    // draw control lines
    if (n > 0) {
      for (int i=1; i<n; ++i)
	g.drawLine(dashPen,
		   modelToView(mPoints[i-1]),
		   modelToView(mPoints[i]));
    }

    // draw the spline
    if (n >= 4) {
      std::vector<Point> points;
      for (int i=0; i<n; ++i)
	points.push_back(modelToView(mPoints[i]));

      g.drawBezier(greenPen, points);
    }
  }
  
};

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  Tab m_tab;
  FunctionsPage m_functionsPage;
  Lagrange m_lagrangePage;
  Bezier m_bezierPage;

public:

  MainFrame()
    : Frame("Maths")
    , m_tab(this)
    , m_functionsPage(&m_tab)
    , m_lagrangePage(&m_tab)
    , m_bezierPage(&m_tab)
  {
    setLayout(new ClientLayout());
  }

protected:

  virtual void onKeyDown(KeyEvent &ev)
  {
    Frame::onKeyDown(ev);

    // with ESC key you can close this application
    if (ev.getKeyCode() == Keys::Escape) {
      setVisible(false);
      ev.consume();
    }
  }
  
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

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example app;
  app.run();
  return 0;
}
