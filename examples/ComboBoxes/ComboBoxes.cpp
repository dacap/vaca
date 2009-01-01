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
  Label m_label1; ComboBox m_combo1;
  Label m_label2; ComboBox m_combo2;
  Label m_label3; ComboBox m_combo3;
  
public:

  MainFrame()
    : Frame(L"ComboBoxes")
    , m_label1(L"ComboBox::Styles::Default", this)
    , m_combo1(this)
    , m_label2(L"ComboBox::Styles::Simple", this)
    , m_combo2(this, ComboBox::Styles::Simple)
    , m_label3(L"ComboBox::Styles::Editable", this)
    , m_combo3(this, ComboBox::Styles::Editable)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));

    for (int c=0; c<8; ++c) {
      String text = format_string(L"Item %d", c);
      m_combo1.addItem(text);
      m_combo2.addItem(text);
      m_combo3.addItem(text);
    }

    m_combo1.SelChange.connect(&MainFrame::onComboChange, this);
    m_combo2.EditChange.connect(&MainFrame::onEditChange, this);

    setSize(getPreferredSize());
    center();
  }

protected:

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

private:
  
  void onComboChange(Event& ev)
  {
    m_combo2.setCurrentItem(m_combo1.getCurrentItem());
  }

  void onEditChange(Event& ev)
  {
    m_combo3.setText(m_combo2.getText());
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
