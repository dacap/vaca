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

#include "Vaca/Dialog.h"
#include "Vaca/Debug.h"
#include "Vaca/Application.h"
#include "Vaca/CloseEvent.h"
#include "Vaca/WidgetClass.h"

using namespace Vaca;

/**
 * Creates a dialog using the default DialogClass.
 *
 * @see doModal(), DialogStyle
 */
Dialog::Dialog(const String& title, Widget* parent, Style style)
  : Frame(WidgetClassName::None, title, parent, style)
{
  setDefWndProc(DefDlgProc);

  create(DialogClass::getClassName(), parent, style);
  setText(title);

  SetWindowLongPtr(getHandle(), DWL_DLGPROC,
		   reinterpret_cast<LONG_PTR>(Dialog::globalDlgProc));

  m_state = false;
}

/**
 * Creates a dialog with a custom WNDCLASS. @a className can be NULL
 * if you want to call Widget::create() by your self.
 */
Dialog::Dialog(const WidgetClassName& className, const String& title, Widget* parent, Style style)
  : Frame(WidgetClassName::None, title, parent, style)
{
  setDefWndProc(DefDlgProc);
  
  if (className != WidgetClassName::None) {
    create(className, parent, style);
    setText(title);
  }

  m_state = false;
}

Dialog::~Dialog()
{
}

/**
 * You can use this to set the doModal()'s return value.
 */
void Dialog::setReturnState(bool state)
{
  m_state = state;
}

/**
 * Executes the dialog in a local message loop, disabling the parent
 * widget.
 *
 * @return Should returns true if the user press the OK button, or
 *         false if the user press the Cancel/Close buttons.
 */
bool Dialog::doModal()
{
  setVisible(true);

  Thread thread;
  thread.doMessageLoopFor(this);

  return m_state;
}

/**
 * Calls Win32's IsDialogMessage.
 */
bool Dialog::preTranslateMessage(Message& message)
{
  if (Frame::preTranslateMessage(message))
    return true;

  if (::IsDialogMessage(getHandle(), (LPMSG)message))
    return true;

  return false;
}

/**
 * Hides the dialog, but before changes the return state
 * (setReturnState) to true, so #doModal returns true.
 * 
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
 * doModal() returns false too.  You can use this method to bind
 * the Cancel button action (Button::Action).  Example:
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

Widget* Dialog::getNextFocusableWidget(Widget* widget)
{
  assert(::IsWindow(getHandle()));

  HWND hwnd = GetNextDlgTabItem(getHandle(),
				widget != NULL ? widget->getHandle(): NULL,
				FALSE);

  return hwnd != NULL ? Widget::fromHandle(hwnd): NULL;
}

Widget* Dialog::getPreviousFocusableWidget(Widget* widget)
{
  assert(::IsWindow(getHandle()));

  HWND hwnd = GetNextDlgTabItem(getHandle(),
				widget ? widget->getHandle(): NULL,
				TRUE);

  return hwnd != NULL ? Widget::fromHandle(hwnd): NULL;
}

bool Dialog::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
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

LRESULT CALLBACK Dialog::globalDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  // do default behaviour
  return FALSE;
}
