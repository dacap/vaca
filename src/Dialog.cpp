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

#include "stdvaca.h"
#include "Vaca/Dialog.h"
#include "Vaca/Debug.h"
#include "Vaca/Application.h"
#include "Vaca/CloseEvent.h"

using namespace Vaca;

/**
 * Creates a dialog using the default DialogClass. Remember that
 * dialogs aren't automatically disposed when them are closed.
 *
 * @see doModal()
 */
Dialog::Dialog(const String &title, Widget *parent, Style style)
  : Frame(NULL, title, parent, style)
{
  create(DialogClass::getClassName(), parent, style);
  setText(title);

  SetWindowLongPtr(getHWND(), DWL_DLGPROC,
		   reinterpret_cast<LONG_PTR>(Dialog::globalDlgProc));

  mState = false;
}

/**
 * Creates a dialog with a custom WNDCLASS. @a className can be NULL
 * if you want to call Widget::create() by your self.
 */
Dialog::Dialog(LPCTSTR className, const String &title, Widget *parent, Style style)
  : Frame(NULL, title, parent, style)
{
  if (className != NULL) {
    create(className, parent, style);
    setText(title);
  }

  mState = false;
}

Dialog::~Dialog()
{
}

/**
 * You can use this to set the doModal()'s return value.
 */
void Dialog::setReturnState(bool state)
{
  mState = state;
}

/**
 * Executes the dialog in a local message loop, disabling the parent
 * widget. The Dialog isn't disposed when it's closed, because
 * generally you want the Dialog data (like a form) to known the user
 * input.
 *
 * @return Should returns true if the user press the OK button, or
 *         false if the user press the Cancel/Close buttons.
 */
bool Dialog::doModal()
{
  setVisible(true);

  Thread *thread = Thread::getCurrent();

  assert(thread != NULL);
  
  thread->doMessageLoopFor(this);

  return mState;
}

/**
 * Calls IsDialogMessage.
 */
bool Dialog::preTranslateMessage(MSG &msg)
{
  if (Frame::preTranslateMessage(msg))
    return true;

  if (IsDialogMessage(getHWND(), &msg))
    return true;

  return false;
}

/**
 * Hides the dialog, but before changes the return state
 * (setReturnState) to true, so doModal() returns true too.
 * You can use this to bind the OK button action (Button::Action).
 * Example:
 * 
 * @code
 *   ...
 *   Dialog dlg(...);
 *   Button okButton("&OK", &dlg);
 *   ...
 *   okButton.Action.connect(Bind(&Dialog::defaultOkAction, &dlg));
 *   ...
 * @endcode
 *
 * @see defaultCancelAction
 */
void Dialog::defaultOkAction()
{
  setReturnState(true);
  setVisible(false);
}

/**
 * Generates a onClose event (WM_CLOSE message) for the dialog, but
 * before changes the return state (setReturnState) to false, so
 * doModal() returns false too.  You can use this to bind the Cancel
 * button action (Button::Action).  Example:
 * 
 * @code
 *   ...
 *   Dialog dlg(...);
 *   Button cancelButton("&Cancel", &dlg);
 *   ...
 *   cancelButton.Action.connect(Bind(&Dialog::defaultCancelAction, &dlg));
 *   ...
 * @endcode
 *
 * @see defaultOkAction, onClose
 */
void Dialog::defaultCancelAction()
{
  setReturnState(false);

  // cancel is like if the user close the window
  sendMessage(WM_CLOSE, 0, 0);
}

/**
 * Consume the close event to avoid dispose the dialog. A dialog is
 * just hidden (not disposed) when it's closed. This is called when
 * the user press the close button of the title-bar or the ESC key.
 *
 * @see defaultCancelAction
 */
void Dialog::onClose(CloseEvent &ev)
{
  // fire Close signal
  Frame::onClose(ev);

  // don't dispose the Dialog...
  ev.cancel();

  // ...just hide it
  setVisible(false);
}

bool Dialog::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
  if (Frame::wndProc(message, wParam, lParam, lResult))
    return true;

  switch (message) {

    case WM_COMMAND:
      switch (LOWORD(wParam)) {

	case IDOK:
	  // not necessary, because the default button should have the IDOK
	  break;

	case IDCANCEL:
	  defaultCancelAction();
	  break;

      }
      break;

  }

  return false;
}

LRESULT Dialog::defWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  return DefDlgProc(getHWND(), message, wParam, lParam);
}

LRESULT CALLBACK Dialog::globalDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  // do default behaviour
  return FALSE;
}