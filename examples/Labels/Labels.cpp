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

using namespace Vaca;

class MainFrame : public Frame
{
  Label m_longLabel;
  Label m_simpleLabel;
  Label m_ellipsis1Label;
  Label m_ellipsis2Label;
  Label m_ellipsis3Label;
  Button m_alignButton;

public:

  MainFrame()
    : Frame("Labels", NULL, FrameStyle - MaximizableFrameStyle)
    , m_longLabel("This is a long label to test the word wrap. Also, see\r\n"
		 "how the BoxLayout manager fit all in the right place.", this)
    , m_simpleLabel("A simple label (it's not word wrapped, not text-aligned).",
		   this, LabelStyle + SimpleLabelStyle)
    , m_ellipsis1Label("A label with word-ellipsis style.",
		      this, LabelStyle + WordEllipsisLabelStyle)
    , m_ellipsis2Label("C:\\folder\\subFolder\\subSubFolder\\fileName.txt",
		      this, LabelStyle + EndEllipsisLabelStyle)
    , m_ellipsis3Label("C:\\folder\\subFolder\\subSubFolder\\fileName.txt",
		      this, LabelStyle + PathEllipsisLabelStyle)
    , m_alignButton("Set Alignment", this)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));

    m_alignButton.Action.connect(Bind(&MainFrame::onSetAlignment, this));

    setSize(getPreferredSize());
    center();
  }

protected:

  void onSetAlignment()
  {
    TextAlign align = m_longLabel.getTextAlign();
    switch (align) {
      case TextAlign::Left:   align = TextAlign::Center; break;
      case TextAlign::Center: align = TextAlign::Right;  break;
      case TextAlign::Right:  align = TextAlign::Left;   break;
    }
    m_longLabel.setTextAlign(align);
    m_simpleLabel.setTextAlign(align);
    m_ellipsis1Label.setTextAlign(align);
    m_ellipsis2Label.setTextAlign(align);
    m_ellipsis3Label.setTextAlign(align);
  }

  virtual void onResizing(CardinalDirection dir, Rect &rc)
  {
    rc.y = getBounds().y;
    
    if (dir != CardinalDirection::North &&
	dir != CardinalDirection::South) {
      // the height is calculated through the width that the user wants
      rc.h = getPreferredSize(Size(rc.getSize().w, 0)).h;
    }
    else {
      rc.h = getBounds().h;
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
  Example* app(new Example);
  app->run();
  delete app;
  return 0;
}
