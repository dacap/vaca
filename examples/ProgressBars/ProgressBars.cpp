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

class MainFrame : public Dialog
{
  ProgressBar mProgressBar1;
  ProgressBar mProgressBar2;
  Button mStart;
  Button mClose;
  bool mWorking;
  bool mBreak;
  bool mClosing;

public:

  MainFrame()
    : Dialog("ProgressBars")
    , mProgressBar1(this)
    , mProgressBar2(this, ProgressBarStyle + SmoothProgressBarStyle)
    , mStart("Start", this)
    , mClose("Close", this)
  {
    // a box layout manager with vertical orientation and no-homogeneous
    setLayout(new BoxLayout(Vertical, false));

    // set the ranges of the progress bars
    mProgressBar1.setRange(0, 100);
    mProgressBar2.setRange(0, 100);

    // the "Start" button is the default one
    mStart.setDefault(true);

    // call "onStart" when the "Start" button is pressed
    mStart.Action.connect(Bind(&MainFrame::onStart, this));

    // the defaultCancelAction of dialogs generates an "WM_CLOSE"
    // message that is converted to the "onClose" event
    mClose.Action.connect(Bind(&MainFrame::defaultCancelAction, this));

    // the application is not working now
    mWorking = false;

    // set the size of the Frame
    setSize(Size(256, preferredSize().h));
    center();
  }

protected:

  // when the "Start" button is pressed...
  void onStart()
  {
    // the work is done? the user press the "Restart" button
    if (mProgressBar1.getPosition() == mProgressBar1.getMaximum()) {
      mProgressBar1.setPosition(mProgressBar1.getMinimum());
      mProgressBar2.setPosition(mProgressBar2.getMinimum());
      mStart.setText("Start");
      mWorking = false;
      return;
    }

    // we are working...
    if (mWorking) {
      // the user press "Pause"?
      if (mBreak == false) {
	mBreak = true;
	return;
      }
      // the user press "Continue"...
      else {
	// do nothing
      }
    }
    else {
      // ...the user really press "Start" button
      mWorking = true;
    }

    mBreak = false;
    mClosing = false;

    // convert the button to "Pause"...
    mStart.setText("Pause");

    do {
      Thread::getCurrent()->pumpMessageQueue();

      if (mClosing) {
	dispose();
	return;
      }
      
      if (mBreak)
	break;

      mProgressBar1.advancePosition(1);
      mProgressBar2.advancePosition(1);
      Sleep(10);

      // we don't finish?
    } while (mProgressBar1.getPosition() < mProgressBar1.getMaximum());

    if (mBreak) {
      // convert the button to "Continue"
      mStart.setText("Continue");
    }
    else {
      // convert the button to "Restart"
      mStart.setText("Restart");
      mWorking = false;
    }
  }

  void onCancel()
  {
    mBreak = true;
  }

  virtual void onClose(CloseEvent &ev)
  {
    if (mWorking) {
      if (msgBox("The application is working.\r\n"
		 "Do you really want to stop it?",
		 "Warning",
		 MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES) {
	// the user really want to close the window
	mClosing = true;
      }
      else {
	// cancel the close event, don't dispose the Frame
	ev.cancel();
      }
    }
    // close the Dialog...
    else {
      // the dialog implementation just hide the window
      Dialog::onClose(ev);

      // now we can dispose the Frame
      dispose();
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
