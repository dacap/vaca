// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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
#include "Vaca/CommandEvent.h"

using namespace Vaca;

/// Creates a dialog using the default DialogClass.
/// 
/// @see doModal(), DialogStyle
/// 
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

/// Creates a dialog with a custom WNDCLASS. @a className can be NULL
/// if you want to call Widget::create() by your self.
/// 
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

Dialog::Dialog(ResourceId dialogId, Widget* parent)
  : Frame(::CreateDialog(Application::getHandle(),
			 dialogId.toLPTSTR(),
			 parent ? parent->getHandle() : NULL,
			 Dialog::globalDlgProc))
{
  m_state = false;
}

Dialog::Dialog(HWND handle)
  : Frame(handle)
{
  m_state = false;
}

Dialog::~Dialog()
{
}

/// You can use this to set the doModal()'s return value.
/// 
void Dialog::setReturnState(bool state)
{
  m_state = state;
}

/// Executes the dialog in a local message loop, disabling the parent
/// widget.
/// 
/// @return Should returns true if the user press the OK button, or
///         false if the user press the Cancel/Close buttons.
/// 
bool Dialog::doModal()
{
  setVisible(true);

  CurrentThread::doMessageLoopFor(this);

  return m_state;
}

/// Calls Win32's IsDialogMessage.
/// 
bool Dialog::preTranslateMessage(Message& message)
{
  if (Frame::preTranslateMessage(message))
    return true;

  if (::IsDialogMessage(getHandle(), (LPMSG)message))
    return true;

  return false;
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

/// Hides the dialog, but before changes the return state
/// (#setReturnState) to true, so #doModal returns true.
/// 
/// You can use this to bind the OK button action (Button#Click).
/// Also, this function is automatically called when the Id#Ok command
/// is processed by the dialog through #onCommand event.
/// 
/// Example:
/// @code
///   Dialog dlg(...);
///   Button ok("&OK", &dlg);
///   ok.Click.connect(Bind(&Dialog::onOk, &dlg));
/// @endcode
/// Or:
/// @code
///   Dialog dlg(...);
///   Button ok("&OK", Dialog::Id::Ok, &dlg);
/// @endcode
/// 
/// @see #onCancel
/// 
void Dialog::onOk()
{
  Ok();
  setReturnState(true);
  setVisible(false);
}

/// Generates a #onClose event for the dialog, but before changes the
/// return state (setReturnState) to false, so #doModal returns false
/// too.
/// 
/// You can use this method to bind the Cancel button action
/// (Button#Click). Anyway this function is automatically
/// called when the Id#Cancel command is processed by the dialog
/// through #onCommand event.
/// 
/// Example:
/// @code
///   Dialog dlg(...);
///   Button cancel("&Cancel", &dlg);
///   cancel.Click.connect(Bind(&Dialog::onCancel, &dlg));
/// @endcode
/// Or:
/// @code
///   Dialog dlg(...);
///   Button cancel("&Cancel", Dialog::Id::Cancel, &dlg);
/// @endcode
/// 
/// @see #onOk, #onClose
/// 
void Dialog::onCancel()
{
  Cancel();
  setReturnState(false);
  sendMessage(WM_CLOSE, 0, 0); // cancel is like if the user close the window
}

void Dialog::onCommand(CommandEvent& ev)
{
  if (!ev.isConsumed()) {
    switch (ev.getCommandId()) {

      case Dialog::Id::Ok:
	onOk();
	ev.consume();
	break;

      case Dialog::Id::Cancel:
	onCancel();
	ev.consume();
	break;
    }
  }
  Frame::onCommand(ev);
}

BOOL CALLBACK Dialog::globalDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  // do default behaviour
  return FALSE;
}
