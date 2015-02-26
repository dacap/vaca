// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_TEXTEDIT_H
#define VACA_TEXTEDIT_H

#include "vaca/base.h"
#include "vaca/Widget.h"

namespace vaca {

/**
   Widget to edit text.

   @see @ref page_tn_008
*/
class VACA_DLL TextEdit : public Widget
{
public:

  struct VACA_DLL Styles {
    static const Style Default;
    static const Style Password;
    static const Style TextArea;
    static const Style RightAligned;
    static const Style ReadOnly;
    static const Style AutoHorizontalScroll;
    static const Style AutoVerticalScroll;
  };

  TextEdit(const String& text, Widget* parent, Style style = Styles::Default);
  virtual ~TextEdit();

  // ============================================================
  // ANY TEXT EDIT
  // ============================================================

  int getTextLength() const;
  int getTextLimit() const;
  void setTextLimit(int textLimit);

  bool isReadOnly() const;
  void setReadOnly(bool readOnly);

  bool canUndo() const;
  void undo();

  void cut();
  void copy();
  void paste();

  void selectAll();
  void selectRange(int start, int end);
  void deselect();

  void getSelection(int& start, int& end);

  // ============================================================
  // PASSWORD
  // ============================================================

  Char getPasswordChar();
  void setPasswordChar(Char passChar);

  // ============================================================
  // TEXT AREA
  // ============================================================

  bool getWantReturnMode();
  void setWantReturnMode(bool wantReturn);

  int getLineCount();
  String getLine(int lineNo);
  int getLineLength(int lineNo);

  void scrollLines(int lines);

  // ============================================================
  // SIGNALS
  // ============================================================

  // Signals
  Signal1<void, Event&> Change; ///< @see onChange

protected:
  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);

  // New events
  virtual void onChange(Event& ev);

  // Reflected notifications
  virtual bool onReflectedCommand(int id, int code, LRESULT& lResult);

};

} // namespace vaca

#endif // VACA_TEXTEDIT_H
