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

#ifndef VACA_DIALOG_H
#define VACA_DIALOG_H

#include "Vaca/base.h"
#include "Vaca/Frame.h"
#include "Vaca/Event.h"

namespace Vaca {

/// Represents the Win32 class used by Dialog.
/// 
class DialogClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName(L"Vaca.Dialog"); }
  static int getWndExtra() { return DLGWINDOWEXTRA; }
};

/// A dialog box.
/// 
/// There are some standard considerations for a dialog:
/// 
/// @li When the user press ESC, the #onClose event is generated.
/// @li If the dialog has a "Cancel" button is a good idea to bind its
///     Button#Action to #onCancel.
/// @li If the dialog has an "OK" button is a good idea to bind its
///     Button#Action to #onOk.
/// 
/// If the Dialog have buttons with the special ids IDOK and/or IDCANCEL,
/// the #onOk and #onCancel will be called automatically.
/// 
/// Example:
/// @code
/// class MyDialog : public Dialog
/// {
///   Button ok, cnl;
/// public:
///   MyDialog()
///     : Dialog("My dialog")
///     , ok("OK", this)
///     , cnl("Cancel", this)
///   {
///     ok.Action.connect(Bind(&MyDialog::onOk, this));
///     cnl.Action.connect(Bind(&MyDialog::onCancel, this));
///     // ...
///   }
/// };
/// @endcode
/// 
/// How to customize the close action? Just use your own #onClose.
/// 
/// If you want to use a MenuBar, you should use a Frame instead of a
/// Dialog, because the #preTranslateMessage "eat" all
/// Alt+Letter keyboard messages (used as mnemonics for MenuItems).
/// 
class VACA_DLL Dialog : public Register<DialogClass>, public Frame
{
  bool m_state;

public:

  struct VACA_DLL Styles {
    static const Style Default;
    static const Style Modal;
  };

  Dialog(const String& title, Widget* parent = NULL, Style style = Styles::Default);
  Dialog(const WidgetClassName& className, const String& title, Widget* parent = NULL, Style style = Styles::Default);
  explicit Dialog(ResourceId dialogId, Widget* parent = NULL);
  explicit Dialog(HWND handle);
  virtual ~Dialog();

  void setReturnState(bool state);

  virtual bool doModal();
  virtual bool preTranslateMessage(Message& message);

  /// @deprecated Use #onOk instead.
  /// 
  void defaultOkAction() { onOk(); }

  /// @deprecated Use #onCancel instead.
  /// 
  void defaultCancelAction() { onCancel(); }

  Widget* getNextFocusableWidget(Widget* widget);
  Widget* getPreviousFocusableWidget(Widget* widget);

  Signal0<void> Ok;       ///< @see onOk
  Signal0<void> Cancel;   ///< @see onCancel

protected:
  virtual void onOk();
  virtual void onCancel();
  virtual bool onCommand(CommandId id);

private:
  static BOOL CALLBACK globalDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

};

} // namespace Vaca

#endif // VACA_DIALOG_H
