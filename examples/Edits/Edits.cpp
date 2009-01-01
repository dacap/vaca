// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
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

using namespace Vaca;

class MainFrame : public Frame
{
  TextEdit m_edit1;
  TextEdit m_edit2;
  TextEdit m_edit3;
  TextEdit m_edit4;

public:

  MainFrame()
    : Frame(L"Edits")
    , m_edit1(L"Default TextEdit widget with some text that you can edit.", this)
    , m_edit2(L"An TextEdit widget without client-edge and right-aligned.", this,
	      TextEdit::Styles::Default +
	      TextEdit::Styles::RightAligned
	      - Widget::Styles::ClientEdge)
    , m_edit3(L"A read-only TextEdit widget without client-edge and with modified bgColor", this,
	      TextEdit::Styles::Default
	      - Widget::Styles::ClientEdge)
    , m_edit4(L"A TextEdit widget with TextArea style.\r\nIt has multiple lines\r\nof text...", this,
	      TextEdit::Styles::TextArea +
	      Widget::Styles::Scroll)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_edit4.setConstraint(new BoxConstraint(true));

    m_edit3.setReadOnly(true);
    m_edit3.setBgColor(System::getColor(COLOR_3DFACE));

    setSize(getPreferredSize());
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;

  virtual void main() {
    m_mainFrame.setVisible(true);
  }
};

int VACA_MAIN()
{
  Example app;
  app.run();
  return 0;
}
