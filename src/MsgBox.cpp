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

#include "Vaca/MsgBox.h"
#include "Vaca/System.h"
#include "Vaca/Widget.h"

using namespace Vaca;

/// Shows a message box, locking the specified @a parent widget.
/// 
/// The next code is a typical example where is displayed a message box with
/// the "Yes" and "No" buttons, an icon to indicate a warning, and
/// by default the "No" button will be focused:
/// 
/// @code
/// if (MsgBox::show(NULL,
///                  "Warning",
///                  "Are you sure?",
///                  MsgBox::Type::YesNo,
///                  MsgBox::Icon::Warning, -1) == MsgBox::Result::Yes) {
///   // the user press Yes, do it!
/// }
/// else {
///   // the user press No (or ESC, in which case MsgBox::Result::Cancel is returned)
/// }
/// @endcode
/// 
/// @win32
///   It is @msdn{MessageBox} wrapper.
/// @endwin32
/// 
/// @param parent
///     The widget to be locked by this message-box. When the message-box is shown
///     this widget will be temporarily disabled. Can be NULL.
/// 	
/// @param title
///     Title for the dialog box.
/// 
/// @param text
///     Text shown in the center of the dialog box. It can contains "\n" to break lines.
/// 	
/// @param type
///     What set of buttons we have to show. One of these values:
///     @li @c MsgBox::Type::Ok
///     @li @c MsgBox::Type::OkCancel
///     @li @c MsgBox::Type::YesNo
///     @li @c MsgBox::Type::YesNoCancel
///     @li @c MsgBox::Type::RetryCancel
///     @li @c MsgBox::Type::CancelRetryContinue
/// 
/// @param icon
///     The icon to be shown. One of these ones:
///     @li @c MsgBox::Icon::Error
///     @li @c MsgBox::Icon::Warning
///     @li @c MsgBox::Icon::Question
///     @li @c MsgBox::Icon::Information
/// 
/// @param default_button
///     The default button to be focused by default.
///     @li 1 is the first button,
///     @li 2 the second,
///     @li 3 the third,
///     @li -1 the last one.
/// 
/// @return
///     It returns the button that was clicked. One of these values:
///     @li @c MsgBox::Result::Ok
///     @li @c MsgBox::Result::Cancel (if the user pressed the Cancel button or press @c ESC key)
///     @li @c MsgBox::Result::Yes
///     @li @c MsgBox::Result::No
///     @li @c MsgBox::Result::Retry
///     @li @c MsgBox::Result::Continue
/// 
MsgBox::Result MsgBox::show(Widget* parent,
			    const String& title,
			    const String& text,
			    MsgBox::Type type,
			    MsgBox::Icon icon,
			    int default_button)
{
  int flags = 0;
  int buttons = 0;

  switch (type) {
    case MsgBox::Type::Ok:
      flags |= MB_OK;
      buttons = 1;
      break;
    case MsgBox::Type::OkCancel:
      flags |= MB_OKCANCEL;
      buttons = 2;
      break;
    case MsgBox::Type::YesNo:
      flags |= MB_YESNO;
      buttons = 2;
      break;
    case MsgBox::Type::YesNoCancel:
      flags |= MB_YESNOCANCEL;
      buttons = 3;
      break;
    case MsgBox::Type::RetryCancel:
      flags |= MB_RETRYCANCEL;
      buttons = 2;
      break;
    case MsgBox::Type::CancelRetryContinue:
      if (System::isWin2K_XP())
	flags |= MB_CANCELTRYCONTINUE;
      else
	flags |= MB_ABORTRETRYIGNORE;
      buttons = 3;
      break;
  }

  if (default_button != 0) {
    default_button = clamp_value(default_button, -buttons, buttons);
    if (default_button < 0)
      default_button = buttons + default_button + 1;
    switch (default_button) {
      case 1: flags |= MB_DEFBUTTON1; break;
      case 2: flags |= MB_DEFBUTTON2; break;
      case 3: flags |= MB_DEFBUTTON3; break;
    }
  }

  switch (icon) {
    case MsgBox::Icon::None:
      // do nothing
      break;
    case MsgBox::Icon::Error:
      flags |= MB_ICONERROR;
      break;
    case MsgBox::Icon::Question:
      flags |= MB_ICONQUESTION;
      break;
    case MsgBox::Icon::Warning:
      flags |= MB_ICONWARNING;
      break;
    case MsgBox::Icon::Information:
      flags |= MB_ICONINFORMATION;
      break;
  }

  int res = ::MessageBox(parent ? parent->getHandle(): NULL,
			 text.c_str(),
			 title.c_str(),
			 flags);
  Result result;

  switch (res) {
    case IDOK: result = Result::Ok; break;
    case IDCANCEL: result = Result::Cancel; break;
    case IDYES: result = Result::Yes; break;
    case IDNO: result = Result::No; break;
    case IDABORT: result = Result::Cancel; break;
    case IDRETRY: result = Result::Retry; break;
    case IDTRYAGAIN: result = Result::Retry; break;
    case IDCONTINUE: result = Result::Continue; break;
    case IDIGNORE: result = Result::Continue; break;
  }

  return result;
}
