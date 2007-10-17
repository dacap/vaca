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

class MainFrame : public Frame
{
  SpinButton mSpinButton1;
  SpinButton mSpinButton2;
  SpinButton mSpinButton3;
  Spinner mSpinner1;
  Spinner mSpinner2;

public:

  MainFrame()
    : Frame("Spinners")
    , mSpinButton1(this)
    , mSpinButton2(this, SpinButtonStyle + HorizontalSpinButtonStyle)
    , mSpinButton3(this, SpinButtonStyle + HotTrackSpinButtonStyle)
    , mSpinner1(0, 10, 0, this)
    , mSpinner2(-5, 5, 0, this)
  {
    setLayout(new BoxLayout(Vertical, false));
    mSpinButton1.setConstraint(new BoxConstraint(true));
    mSpinButton2.setConstraint(new BoxConstraint(true));
    mSpinButton3.setConstraint(new BoxConstraint(true));

    mSpinButton1.Change.connect(Bind(&MainFrame::onSpinButtonChange, this,
				     boost::arg<1>(), &mSpinner1));
    
    mSpinButton2.Change.connect(Bind(&MainFrame::onSpinButtonChange, this,
				     boost::arg<1>(), &mSpinner2));

    setSize(Size(256, 256));
    center();
  }

protected:
  
  void onSpinButtonChange(SpinButtonEvent &ev, Spinner *spinner)
  {
    spinner->setPosition(spinner->getPosition() + ev.getDelta());
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
