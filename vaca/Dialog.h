// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_DIALOG_H
#define VACA_DIALOG_H

#include "vaca/base.h"
#include "vaca/Frame.h"
#include "vaca/Event.h"

namespace vaca {

/**
   Represents the Win32 class used by Dialog.
*/
class DialogClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName(L"Vaca.Dialog"); }
  static int getWndExtra() { return DLGWINDOWEXTRA; }
};

/**
   A dialog box.

   There are some standard considerations for a dialog:

   @li When the user press ESC, the #onClose event is generated.
   @li If the dialog has a "Cancel" button is a good idea to bind its
       Button#Click to #onCancel.
   @li If the dialog has an "OK" button is a good idea to bind its
       Button#Click to #onOk.

   If the Dialog have buttons with the special ids Id#Ok and/or Id#Cancel,
   the #onOk and #onCancel will be called automatically.

   Example:
   @code
   class MyDialog : public Dialog
   {
     Button ok, cnl;
   public:
     MyDialog()
       : Dialog("My dialog")
       , ok("OK", this)
       , cnl("Cancel", this)
     {
       ok.Click.connect(Bind(&MyDialog::onOk, this));
       cnl.Click.connect(Bind(&MyDialog::onCancel, this));
       // ...
     }
   };
   @endcode

   How to customize the close action? Just use your own #onClose.

   If you want to use a MenuBar, you should use a Frame instead of a
   Dialog, because the #preTranslateMessage "eat" all
   Alt+Letter keyboard messages (used as mnemonics for MenuItems).
*/
class VACA_DLL Dialog : public Register<DialogClass>, public Frame
{
  bool m_state;

public:

  struct VACA_DLL Styles {
    static const Style Default;
    static const Style Modal;
  };

  struct VACA_DLL Id {		// it Id "namespace"

    /**
       It is a CommandId that represents the OK buttons.

       A Dialog has as default behavior for Ok command: closes the
       window and doModal returns true.

       @win32
	 It is equal to IDOK.
       @endwin32
    */
    static const CommandId Ok = 1;

    /**
       It is a CommandId that represents the Cancel buttons.

       A Dialog has as default behavior for Cancel command: closes
       the window and doModal returns false.

       @win32
         It is equal to IDCANCEL.
       @endwin32
    */
    static const CommandId Cancel = 2;

  };

  Dialog(const String& title, Widget* parent = NULL, Style style = Styles::Default);
  Dialog(const WidgetClassName& className, const String& title, Widget* parent = NULL, Style style = Styles::Default);
  explicit Dialog(ResourceId dialogId, Widget* parent = NULL);
  explicit Dialog(HWND handle);
  virtual ~Dialog();

  void setReturnState(bool state);

  virtual bool doModal();
  virtual bool preTranslateMessage(Message& message);

  Widget* getNextFocusableWidget(Widget* widget);
  Widget* getPreviousFocusableWidget(Widget* widget);

  Signal0<void> Ok;       ///< @see onOk
  Signal0<void> Cancel;   ///< @see onCancel

protected:
  virtual void onOk();
  virtual void onCancel();
  virtual void onCommand(CommandEvent& ev);

private:
  static INT_PTR CALLBACK globalDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

};

} // namespace vaca

#endif // VACA_DIALOG_H
