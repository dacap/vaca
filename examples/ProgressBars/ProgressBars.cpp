// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

#include <Vaca/Vaca.hpp>

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

  ProgressBar m_progressBar1;	// normal progress bar
  ProgressBar m_progressBar2;	// smooth progress bar
  Button m_start;
  Button m_close;
  State m_state;			// current state

public:

  MainFrame()
    : Dialog("ProgressBars")
    , m_progressBar1(this)
    , m_progressBar2(this, ProgressBarStyle + SmoothProgressBarStyle)
    , m_start("Start", this)
    , m_close("Close", this)
  {
    // a box layout manager with vertical orientation and no-homogeneous
    setLayout(new BoxLayout(Orientation::Vertical, false));

    // set the ranges of the progress bars
    m_progressBar1.setRange(0, 100);
    m_progressBar2.setRange(0, 100);

    // the "Start" button is the default one
    m_start.setDefault(true);

    // call "onStart" when the "Start" button is pressed
    m_start.Action.connect(Bind(&MainFrame::onStart, this));

    // the defaultCancelAction of dialogs generates an "WM_CLOSE"
    // message that is converted to the "onClose" event
    m_close.Action.connect(Bind(&MainFrame::defaultCancelAction, this));

    // the application is waiting to work (the user should press the
    // "Start" button)
    m_state = WaitingToWork;

    // set the size of the Frame
    setSize(Size(256, getPreferredSize().h));
    center();
  }

protected:

  // when the "Start/Pause/Continue/Restart" button is pressed...
  void onStart()
  {
    switch (m_state) {

      case WaitingToWork:
      case Paused: {
	m_state = Working;
	m_start.setText("Pause"); // convert the button to "Pause"...

	// this is "The Loop", where the real work is done
	do {
	  // when we pump the message queue, we can get events like onClose()
	  Thread::pumpMessageQueue();

	  // work done
	  if (m_progressBar1.getValue() == m_progressBar1.getMaximum()) {
	    m_state = WorkDone;
	    m_start.setText("Restart"); // convert the button to "Restart"
	  }
	  else {
	    m_progressBar1.addValue(1);
	    m_progressBar2.addValue(1);

	    // in our case, the "real work" is sleep :) ...but for
	    // your application this could be "loading a file"...
	    Sleep(10);
	  }

	  // still working?
	} while (m_state == Working);
	// aborting work? hide the frame...
	if (m_state == Aborting)
	  setVisible(false);
	break;
      }

      case Working:
	m_state = Paused;
	m_start.setText("Continue"); // convert the button to "Continue"
	break;

      // the work is done? the user press the "Restart" button
      case WorkDone:
	// restart progress bars
	m_progressBar1.setValue(m_progressBar1.getMinimum());
	m_progressBar2.setValue(m_progressBar2.getMinimum());
	m_start.setText("Start"); // convert the button to "Start"
	m_state = WaitingToWork;
	break;

      case Aborting:
	assert(false);		// impossible
	break;
    }
  }

  virtual void onClose(CloseEvent &ev)
  {
    // work in progress? (we are in "The Loop")
    if (m_state == Working) {
      // display a warning message
      if (msgBox("The application is working.\r\n"
		 "Do you really want to close it?",
		 "Warning",
		 MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES) {
	// the user really want to close the window, abort the work
	m_state = Aborting;
      }

      // cancel the close event, don't hide the Dialog
      ev.cancel();
    }
    // need more time?
    else if (m_state == Paused) {
      // display a warning message
      if (msgBox("The application is paused, but doesn't finish its work.\r\n"
		 "Do you really want to close it?",
		 "Warning",
		 MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES) {
	// hide the dialog
	Dialog::onClose(ev);
      }
      else
	// cancel the close event, don't hide the Dialog
	ev.cancel();
    }
    // the work is done (or never start), hide the Dialog...
    else {
      Dialog::onClose(ev);
    }
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main(std::vector<String> args) {
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
