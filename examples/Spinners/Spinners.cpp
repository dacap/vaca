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
  SpinButton m_spinButton1;
  SpinButton m_spinButton2;
  SpinButton m_spinButton3;
  Spinner m_spinner1;
  Spinner m_spinner2;

public:

  MainFrame()
    : Frame("Spinners")
    , m_spinButton1(this)
    , m_spinButton2(this, SpinButtonStyle + HorizontalSpinButtonStyle)
    , m_spinButton3(this, SpinButtonStyle + HotTrackSpinButtonStyle)
    , m_spinner1(0, 10, 0, this)
    , m_spinner2(-5, 5, 0, this)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_spinButton1.setConstraint(new BoxConstraint(true));
    m_spinButton2.setConstraint(new BoxConstraint(true));
    m_spinButton3.setConstraint(new BoxConstraint(true));

    m_spinButton1.Change.connect(&MainFrame::onSpinButtonChange, this);
    m_spinButton2.Change.connect(&MainFrame::onSpinButtonChange, this);

    setSize(Size(200, 160));
    center();
  }

protected:
  
  void onSpinButtonChange(SpinButtonEvent& ev)
  {
    if (Spinner* spinner = dynamic_cast<Spinner*>(ev.getSource())) {
      spinner->setValue(spinner->getValue() + ev.getDelta());
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

int VACA_MAIN()
{
  Example app;
  app.run();
  return 0;
}
