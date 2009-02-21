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

#ifndef VACA_TEXTEDIT_H
#define VACA_TEXTEDIT_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"

namespace Vaca {

/// Widget to edit text.
/// 
/// @see @ref page_tn_008
/// 
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

  // signals
  Signal1<void, Event&> Change; ///< @see onChange

protected:
  // events
  virtual void onPreferredSize(Size& sz);

  // new events
  virtual void onChange(Event& ev);

  // reflection
  virtual bool onReflectedCommand(int id, int code, LRESULT& lResult);

};

} // namespace Vaca

#endif // VACA_TEXTEDIT_H
