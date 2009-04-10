// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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
// * Neither the name of the author nor the names of its contributors
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
#include "../resource.h"

using namespace Vaca;

class ColoredButton : public CustomButton
{
public:

  ColoredButton(const String &text, Color color, Widget* parent)
    : CustomButton(text, parent)
  {
    setBgColor(color);
 
    // use double-buffering technique for this Widget when onPaint()
    // event is received
    setDoubleBuffered(true);
  }

  // paint event
  virtual void onPaint(PaintEvent& ev)
  {
    Graphics& g = ev.getGraphics();
    Rect bounds = getClientBounds();
    bool selected = hasSelectedVisualAspect();
    Color color = getBgColor();

    // draw focus
    if (hasFocusVisualAspect()) {
      Pen pen(Color::Black);
      g.drawRect(pen, bounds);
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
		       Orientation::Vertical);

    // draw bottom gradient
    g.fillGradientRect(Rect(bounds.x, bounds.y+h, bounds.w, bounds.h-h),
		       selected ? color*0.9: color/2,
		       selected ? color*1.2: color,
		       Orientation::Vertical);

    // draw the label
    Point point = bounds.getCenter() - Point(g.measureString(getText())/2);
    g.drawString(getText(), Color::Black, selected ? point+1: point);
  }
  
};

class MainFrame : public Frame
{
  ColoredButton m_button1;
  ColoredButton m_button2;
  ColoredButton m_button3;
  ColoredButton m_button4;

public:

  MainFrame()
    : Frame(L"ColoredButton")
    , m_button1(L"Red Button", Color(255, 150, 150), this)
    , m_button2(L"Green Button", Color(150, 255, 150), this)
    , m_button3(L"Blue Button", Color(150, 150, 255), this)
    , m_button4(L"Custom Button", Color(200, 200, 200), this)
  {
    setLayout(new BoxLayout(Orientation::Vertical, true));

    m_button4.Click.connect(&MainFrame::onSelectColor, this);

    setSize(getPreferredSize());
  }

  void onSelectColor(Event& ev)
  {
    ColorDialog dlg(m_button4.getBgColor(), this);
    if (dlg.doModal()) {
      m_button4.setBgColor(dlg.getColor());
      m_button4.invalidate(true);
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
