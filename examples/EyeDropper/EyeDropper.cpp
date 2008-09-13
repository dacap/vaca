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
#include "resource.h"

using namespace Vaca;

// the eye-dropper widget
class EyeDropper : public Panel
{
  Font m_font;
  Cursor m_cursor;
  bool m_isHexFormat;
  
public:

  EyeDropper(Widget* parent)
    : Panel(parent)
    , m_font("Courier", 10)
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

  virtual void onPaint(Graphics &g)
  {
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
      str = "#" +
	String::fromInt(bg.getR(), 16, 2) +
	String::fromInt(bg.getG(), 16, 2) +
	String::fromInt(bg.getB(), 16, 2);
    else
      str = "(" +
	String::fromInt(bg.getR(), 10) + ", " +
	String::fromInt(bg.getG(), 10) + ", " +
	String::fromInt(bg.getB(), 10) + ")";

    g.setColor(bg.toBlackAndWhite().negative());
    g.drawString(str, rc.getCenter() - Point(g.measureString(str)/2));

    // copy the string to the clipboard
    Clipboard(this).setString(str);
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    captureMouse();
    setCursor(m_cursor);
  }

  virtual void onMouseMove(MouseEvent &ev)
  {
    if (hasCapture()) {
      ScreenGraphics g;
      setBgColor(g.getPixel(System::getCursorPos()));
      invalidate(true);
    }
  }

  virtual void onMouseUp(MouseEvent &ev)
  {
    if (hasCapture())
      releaseMouse();
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
    : Frame("Eye Dropper", NULL, FrameStyle
				 - ResizableFrameStyle
				 - MaximizableFrameStyle)
    , m_label("Drag from the box to the pixel\r\n"
	     "that you want to get its color", this)
    , m_eyeDropper(this)
    , m_hexFormat("Hex", this)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_hexFormat.Action.connect(&MainFrame::onHexToggle, this);
    setSize(getPreferredSize());
  }

protected:

  void onHexToggle(Event& ev)
  {
    m_eyeDropper.setHexFormat(m_hexFormat.isSelected());
  }

};

// entry point of the program
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Application app;
  MainFrame mainFrame;
  mainFrame.setVisible(true);
  app.run();
  return 0;
}
