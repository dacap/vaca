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

// To control a progress bar that can be paused, it's a little tricky:
// We need a state-machine and to use a custom loop (see "The Loop"
// below) to pump the message queue while we do the "real work"
class MainFrame : public Dialog
{
  // possible states
  enum State {
    WaitingToWork,
    Working,
    Paused,
    WorkDone,
    Aborting
  };

  ProgressBar mProgressBar1;	// normal progress bar
  ProgressBar mProgressBar2;	// smooth progress bar
  Button mStart;
  Button mClose;
  State mState;			// current state

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

    // the application is waiting to work (the user should press the
    // "Start" button)
    mState = WaitingToWork;

    // set the size of the Frame
    setSize(Size(256, preferredSize().h));
    center();
  }

protected:

  // when the "Start/Pause/Continue/Restart" button is pressed...
  void onStart()
  {
    switch (mState) {

      case WaitingToWork:
      case Paused:
	mState = Working;
	mStart.setText("Pause"); // convert the button to "Pause"...

	// this is "The Loop", where the real work is done
	do {
	  // when we pump the message queue, we can get events like onClose()
	  Thread::getCurrent()->pumpMessageQueue();

	  // work done
	  if (mProgressBar1.getValue() == mProgressBar1.getMaximum()) {
	    mState = WorkDone;
	    mStart.setText("Restart"); // convert the button to "Restart"
	  }
	  else {
	    mProgressBar1.addValue(1);
	    mProgressBar2.addValue(1);

	    // in our case, the "real work" is sleep :) ...but for
	    // your application this could be "loading a file"...
	    Sleep(10);
	  }

	  // still working?
	} while (mState == Working);
	// aborting work? dispose the frame...
	if (mState == Aborting)
	  dispose();
	break;

      case Working:
	mState = Paused;
	mStart.setText("Continue"); // convert the button to "Continue"
	break;

      // the work is done? the user press the "Restart" button
      case WorkDone:
	// restart progress bars
	mProgressBar1.setValue(mProgressBar1.getMinimum());
	mProgressBar2.setValue(mProgressBar2.getMinimum());
	mStart.setText("Start"); // convert the button to "Start"
	mState = WaitingToWork;
	break;

      case Aborting:
	assert(false);		// impossible
	break;
    }
  }

  virtual void onClose(CloseEvent &ev)
  {
    // work in progress? (we are in "The Loop")
    if (mState == Working) {
      // display a warning message
      if (msgBox("The application is working.\r\n"
		 "Do you really want to close it?",
		 "Warning",
		 MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES) {
	// the user really want to close the window, abort the work
	mState = Aborting;
      }

      // cancel the close event, don't hide the Dialog
      ev.cancel();
    }
    // need more time?
    else if (mState == Paused) {
      // display a warning message
      if (msgBox("The application is paused, but doesn't finish its work.\r\n"
		 "Do you really want to close it?",
		 "Warning",
		 MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES) {
	// the dialog implementation just hide the window
	Dialog::onClose(ev);

	// so we should dispose the Frame
	dispose();
      }
      else
	// cancel the close event, don't hide the Dialog
	ev.cancel();
    }
    // the work is done (or never start), close the Dialog...
    else {
      // the dialog implementation just hide the window
      Dialog::onClose(ev);

      // so we should dispose the Frame
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
