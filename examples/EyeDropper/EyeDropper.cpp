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
#include "resource.h"

using namespace Vaca;

class EyeDropper : public Panel
{
  Font mFont;
  Cursor mCursor;
  bool mHexFormat;
  
public:

  EyeDropper(Widget *parent)
    : Panel(parent)
    , mFont("Courier", 10)
    , mCursor(IDC_EYEDROPPER)
    , mHexFormat(false)
  {
    setBgColor(Color::Black);
  }

  void setHexFormat(bool hexFormat)
  {
    mHexFormat = hexFormat;
    invalidate(true);
  }

  virtual Size preferredSize()
  {
    return Size(64, 64);
  }

protected:

  virtual void onPaint(Graphics &g)
  {
    Rect rc = getClientBounds();
    Color bg = getBgColor();

    g.setColor(Color::Black);
    g.drawRect(rc);

    rc.shrink(1);
    g.setColor(Color::White);
    g.drawRect(rc);

    rc.shrink(1);
    g.setColor(Color::Black);
    g.drawRect(rc);

    g.setFont(mFont);

    // draw the text

    String str;

    if (mHexFormat)
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
    acquireCapture();
    setCursor(mCursor);
  }

  virtual void onMouseMove(MouseEvent &ev)
  {
    if (hasCapture()) {
      Graphics g;
      setBgColor(g.getPixel(System::getCursorPos()));
      invalidate(true);
    }
  }

  virtual void onMouseUp(MouseEvent &ev)
  {
    if (hasCapture())
      releaseCapture();
  }

};

class MainFrame : public Frame
{
  Label mLabel;
  EyeDropper mEyeDropper;
  ToggleButton mHex;

public:

  MainFrame()
    : Frame("Eye Dropper", NULL, FrameStyle
				 - ResizableFrameStyle
				 - MaximizableFrameStyle)
    , mLabel("Drag from the box to the pixel\r\n"
	     "that you want to get its color", this)
    , mEyeDropper(this)
    , mHex("Hex", this)
  {
    setLayout(new BoxLayout(Vertical, false));
    mHex.Action.connect(Bind(&MainFrame::onHexToggle, this));
    setSize(preferredSize());
  }

protected:

  void onHexToggle()
  {
    mEyeDropper.setHexFormat(mHex.isSelected());
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
