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
#include "resource.h"

using namespace Vaca;

// the eye-dropper widget
class EyeDropper : public Widget
{
  Font m_font;
  Cursor m_cursor;
  bool m_isHexFormat;
  
public:

  EyeDropper(Widget* parent)
    : Widget(parent)
    , m_font(L"Courier", 10)
    , m_cursor(ResourceId(IDC_EYEDROPPER))
    , m_isHexFormat(false)
  {
    setBgColor(Color::Black);
    setPreferredSize(Size(64, 64));
  }

  void setHexFormat(bool hexFormat)
  {
    m_isHexFormat = hexFormat;
    invalidate(true);
  }

protected:

  virtual void onPaint(PaintEvent& ev)
  {
    // remember not to call the base implementation of onPaint (Widget::onPaint)

    Graphics& g = ev.getGraphics();
    Rect rc = getClientBounds();
    Color bg = getBgColor();
    Pen blackPen(Color::Black);
    Pen whitePen(Color::White);
    
    g.drawRect(blackPen, rc);

    rc.shrink(1);
    g.drawRect(whitePen, rc);

    rc.shrink(1);
    g.drawRect(blackPen, rc);

    g.setFont(m_font);

    // draw the text

    String str;

    if (m_isHexFormat)
      str = format_string(L"#%02x%02x%02x", bg.getR(), bg.getG(), bg.getB());
    else
      str = format_string(L"(%d, %d, %d)", bg.getR(), bg.getG(), bg.getB());

    g.drawString(str, bg.toBlackAndWhite().negative(),
		 rc.getCenter() - Point(g.measureString(str)/2));

    // copy the string to the clipboard
    Clipboard(this).setString(str);
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    // remember to call the base implementation to generated the MouseDown signal
    Widget::onMouseDown(ev);

    captureMouse();
  }

  virtual void onMouseMove(MouseEvent &ev)
  {
    // remember to call the base implementation to generated the MouseMove signal
    Widget::onMouseMove(ev);

    if (hasCapture()) {
      ScreenGraphics g;
      setBgColor(g.getPixel(System::getCursorPos()));
      invalidate(true);
    }
  }

  virtual void onMouseUp(MouseEvent &ev)
  {
    // remember to call the base implementation to generated the MouseUp signal
    Widget::onMouseUp(ev);

    if (hasCapture())
      releaseMouse();
  }

  virtual void onSetCursor(SetCursorEvent &ev)
  {
    if (!ev.isConsumed() && hasCapture())
      ev.setCursor(m_cursor);
    Widget::onSetCursor(ev);
  }

};

// the main window
class MainFrame : public Frame
{
  Label m_label;
  EyeDropper m_eyeDropper;
  ToggleButton m_hexFormat;

public:

  MainFrame()
    : Frame(L"Eye Dropper", NULL, Frame::Styles::Default
				  - Frame::Styles::Resizable
				  - Frame::Styles::Maximizable)
    , m_label(L"Drag from the box to the pixel\r\n"
	      L"that you want to get its color", this)
    , m_eyeDropper(this)
    , m_hexFormat(L"Hex", this)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_hexFormat.Click.connect(&MainFrame::onHexToggle, this);
    setSize(getPreferredSize());
  }

protected:

  void onHexToggle(Event& ev)
  {
    m_eyeDropper.setHexFormat(m_hexFormat.isSelected());
  }

};

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
