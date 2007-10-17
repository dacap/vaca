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

class ColoredButton : public CustomButton
{
public:

  ColoredButton(const String &text, Color color, Widget *parent)
    : CustomButton(text, parent)
  {
    setBgColor(color);
 
    // use double-buffering technique for this Widget when onPaint()
    // event is received
    setDoubleBuffering(true);
  }

  // paint event
  virtual void onPaint(Graphics &g)
  {
    Rect bounds = getClientBounds();
    bool selected = hasSelectedVisualAspect();
    Color color = getBgColor();

    // draw focus
    if (hasFocusVisualAspect()) {
      g.setColor(Color::Black);
      g.drawRect(bounds);
      bounds.shrink(1);
    }

    // draw 3d edge
    g.drawGradientRect(bounds,
		       selected ? color/3:   color*3,
		       selected ? color*0.8: color,
		       selected ? color:     color*0.8,
		       selected ? color:     color/3);
    bounds.shrink(1);

    // where split gradients
    int h = bounds.h*3/4;

    // draw top gradient
    g.fillGradientRect(Rect(bounds.x, bounds.y, bounds.w, h),
		       selected ? color*0.8: color,
		       selected ? color*1.1: color*2,
		       Vertical);

    // draw bottom gradient
    g.fillGradientRect(Rect(bounds.x, bounds.y+h, bounds.w, bounds.h-h),
		       selected ? color*0.9: color/2,
		       selected ? color*1.2: color,
		       Vertical);

    // draw the label
    Point point = bounds.getCenter() - Point(g.measureString(getText())/2);
    g.setColor(Color::Black);
    g.drawString(getText(), selected ? point+1: point);
  }
  
};

class MainFrame : public Frame
{
  ColoredButton mButton1;
  ColoredButton mButton2;
  ColoredButton mButton3;
  ColoredButton mButton4;

public:

  MainFrame()
    : Frame("ColoredButton")
    , mButton1("Red Button", Color(255, 150, 150), this)
    , mButton2("Green Button", Color(150, 255, 150), this)
    , mButton3("Blue Button", Color(150, 150, 255), this)
    , mButton4("Custom Button", Color(200, 200, 200), this)
  {
    setLayout(new BoxLayout(Vertical, true));

    mButton4.Action.connect(Bind(&MainFrame::onSelectColor, this));

    setSize(preferredSize());
  }

  void onSelectColor()
  {
    ColorDialog dlg(mButton4.getBgColor(), this);
    if (dlg.doModal()) {
      mButton4.setBgColor(dlg.getColor());
      mButton4.invalidate(true);
    }
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
