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

#ifndef VACA_DIALOG_HPP
#define VACA_DIALOG_HPP

#include "Vaca/base.hpp"
#include "Vaca/Frame.hpp"
#include "Vaca/Event.hpp"

namespace Vaca {

/**
 * This style hides the icon of the Dialog frame. You can remove this
 * style if you want to see an icon in the title bar of the Dialog.
 *
 * @see DialogStyle
 */
#define ModalDialogStyle	Style(0, WS_EX_DLGMODALFRAME)

/**
 * This is the default style for the dialogs. It includes
 * ModalDialogStyle.
 * 
 * @see ModalDialogStyle
 */
#define DialogStyle		(WithCaptionFrameStyle +		\
				 WithSystemMenuFrameStyle +		\
				 ModalDialogStyle +			\
				 Style(WS_POPUP | DS_CONTROL, 0))

/**
 * Win32 class used by the Dialog class.
 */
class DialogClass : public WidgetClass
{
public:
  static LPCTSTR getClassName() { return _T("Vaca.Dialog"); }
  static int getWndExtra() { return DLGWINDOWEXTRA; }
};

/**
 * A dialog box.
 *
 * There are some standard considerations for a dialog:
 *
 * - When the user press ESC, the onClose() event is generated.
 * - If the dialog has a "Cancel" button is a good idea to bind its
 *   Button::Action to defaultCancelAction().
 * - If the dialog has an "OK" button is a good idea to bind its
 *   Button::Action to defaultOkAction().
 *
 * How to customize the close action? Just use your own onClose().
 *
 * If you want to use a MenuBar, you should use a Frame instead of a
 * Dialog, because the Dialog::preTranslateMessage "eat" all
 * Alt+Letter keyboard messages (used as mnemonics for MenuItems).
 */
class VACA_DLL Dialog : public Register<DialogClass>, public Frame
{
  bool m_state;

public:

  Dialog(const String &title, Widget *parent = NULL, Style style = DialogStyle);
  Dialog(LPCTSTR className, const String &title, Widget *parent = NULL, Style style = DialogStyle);
  virtual ~Dialog();

  void setReturnState(bool state);

  virtual bool doModal();

  virtual bool preTranslateMessage(MSG &msg);

  void defaultOkAction();
  void defaultCancelAction();

  Widget *getNextFocusableWidget(Widget *widget);
  Widget *getPreviousFocusableWidget(Widget *widget);

protected:
  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult);
//   virtual LRESULT defWndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
  static LRESULT CALLBACK globalDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

};

} // namespace Vaca

#endif
