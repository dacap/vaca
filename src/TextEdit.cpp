// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
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

#include "Vaca/TextEdit.h"
#include "Vaca/Event.h"
#include "Vaca/System.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/PreferredSizeEvent.h"

using namespace Vaca;

TextEdit::TextEdit(const String& text, Widget* parent, Style style)
  : Widget(WidgetClassName(WC_EDIT), parent, style)
{
  setText(text);
  setBgColor(System::getColor(COLOR_WINDOW));
}

TextEdit::~TextEdit()
{
}

int TextEdit::getTextLength() const
{
  return static_cast<int>(const_cast<TextEdit*>(this)->sendMessage(WM_GETTEXTLENGTH, 0, 0));
}

/**
   Returns the current text limit (EM_GETLIMITTEXT).
*/
int TextEdit::getTextLimit() const
{
  return static_cast<int>(const_cast<TextEdit*>(this)->sendMessage(EM_GETLIMITTEXT, 0, 0));
}

/**
   Sets the text limit (EM_SETLIMITTEXT). The user can't write more of
   @a textLimit characters inside the Edit widget.
*/
void TextEdit::setTextLimit(int textLimit)
{
  assert(textLimit > 0);

  sendMessage(EM_SETLIMITTEXT, textLimit, 0);
}

/**
   Returns true if this Edit widget has read-only mode activated.

*/
bool TextEdit::isReadOnly() const
{
  return (getStyle().regular & ES_READONLY) != 0 ? true: false;
}

/**
   Changes the read-only mode to @a readOnly. A read-only Edit widget
   can't be modified. It's a wrapper for the EM_SETREADONLY message.
*/
void TextEdit::setReadOnly(bool readOnly)
{
  sendMessage(EM_SETREADONLY, readOnly, 0);
}

/**
   Returns true if the user can undo the last operation.

   @win32
     Sends the @msdn{EM_CANUNDO} message to the widget.
   @endwin32
*/
bool TextEdit::canUndo() const
{
  return const_cast<TextEdit*>(this)->sendMessage(EM_CANUNDO, 0, 0) ? true: false;
}

/**
   Undoes the last user's operation in the Edit widget.

   @win32
     Sends the @msdn{EM_UNDO} message to the widget.
   @endwin32
*/
void TextEdit::undo()
{
  sendMessage(EM_UNDO, 0, 0);
}

void TextEdit::cut()
{
  sendMessage(WM_CUT, 0, 0);
}

void TextEdit::copy()
{
  sendMessage(WM_COPY, 0, 0);
}

void TextEdit::paste()
{
  sendMessage(WM_PASTE, 0, 0);
}

/**
   Selects all the text in the Edit widget.
*/
void TextEdit::selectAll()
{
  sendMessage(EM_SETSEL, 0, -1);
}

/**
   Selects a range of text. The range is specified by the indexes
   startIndex and endIndex.
*/
void TextEdit::selectRange(int startIndex, int endIndex)
{
  // TODO
  //assert(startIndex >= 0 && startIndex <= endIndex && endIndex < getTextLength());

  sendMessage(EM_SETSEL, startIndex, endIndex);
}

/**
   Removes the text's selection.
*/
void TextEdit::deselect()
{
  sendMessage(EM_SETSEL, static_cast<WPARAM>(-1), 0);
}

/**
   Returns the current selected range in the text-box.
*/
void TextEdit::getSelection(int& start, int& end)
{
  DWORD s, e;
  sendMessage(EM_GETSEL,
	      reinterpret_cast<WPARAM>(&s),
	      reinterpret_cast<LPARAM>(&e));
  start = s;
  end = e;
}

void TextEdit::onPreferredSize(PreferredSizeEvent& ev)
{
  Size textSize;
  Style style = getStyle();
  Size border(0, 0);

  // client-edge borders
  if ((style.extended & WS_EX_CLIENTEDGE) != 0)
    border +=
      Size(4, 4)+
      (Size(GetSystemMetrics(SM_CXEDGE),
	    GetSystemMetrics(SM_CYEDGE))*2);

  // scroll bars
  if (style.regular & WS_HSCROLL) border.h += GetSystemMetrics(SM_CYHSCROLL);
  if (style.regular & WS_VSCROLL) border.w += GetSystemMetrics(SM_CXVSCROLL);

  {
    ScreenGraphics g;
    g.setFont(getFont());
//     textSize = g.measureString(getText(), fitIn.w == 0 ? 32767: fitIn.w-border.w,
// 			       ((style.regular & ES_AUTOHSCROLL) != 0) ? 0: DT_WORDBREAK);
//     textSize = g.measureString(getText(), 32767, 0);
//     textSize.h *= sendMessage(EM_GETLINECOUNT, 0, 0);

//     textSize = g.measureString(getText(), fitIn.w == 0 ? 32767: fitIn.w-border.w,
// 			       ((style.regular & ES_AUTOHSCROLL) != 0) ? 0: DT_WORDBREAK);

    Size realSize(g.measureString(getText(), 32767, 0));
    int lines = sendMessage(EM_GETLINECOUNT, 0, 0);

    textSize.w = realSize.w;
    textSize.h = lines * g.measureString(L"", 32767, 0).h;
  }

  ev.setPreferredSize(textSize + border);
}

void TextEdit::onChange(Event& ev)
{
  Change(ev);
}

/**
   Converts the EN_CHANGE in onChange.
*/
bool TextEdit::onReflectedCommand(int id, int code, LRESULT& lResult)
{
  if (Widget::onReflectedCommand(id, code, lResult))
    return true;

  switch (code) {

    case EN_CHANGE: {
      Event ev(this);
      onChange(ev);
      return true;
    }

  }

  return false;
}

Char TextEdit::getPasswordChar()
{
  return static_cast<Char>(sendMessage(EM_GETPASSWORDCHAR, 0, 0));
}

void TextEdit::setPasswordChar(Char passChar)
{
  sendMessage(EM_SETPASSWORDCHAR, passChar, 0);
}

bool TextEdit::getWantReturnMode()
{
  HWND hwnd = getHandle();
  assert(::IsWindow(hwnd));
  return (GetWindowLong(hwnd, GWL_STYLE) & ES_WANTRETURN) != 0 ? true: false;
}

void TextEdit::setWantReturnMode(bool wantReturn)
{
  HWND hwnd = getHandle();
  assert(::IsWindow(hwnd));
  int style = GetWindowLong(hwnd, GWL_STYLE) & ~ES_WANTRETURN;
  SetWindowLong(hwnd, GWL_STYLE, style | (wantReturn ? ES_WANTRETURN: 0));
}

int TextEdit::getLineCount()
{
  return static_cast<int>(sendMessage(EM_GETLINECOUNT, 0, 0));
}

String TextEdit::getLine(int lineNo)
{
  size_t length = getLineLength(lineNo);
  if (length > 0) {
    Char* pstr = new Char[length];

    sendMessage(EM_GETLINE, lineNo, reinterpret_cast<LPARAM>(pstr));

    String str;
    str.reserve(length);

    Char* p = pstr;
    for (size_t c=0; c<length; ++c, ++p)
      str.push_back(*p);

    delete pstr;
    return str;
  }
  else
    return String();
}

int TextEdit::getLineLength(int lineNo)
{
  return static_cast<int>(sendMessage(EM_LINELENGTH, lineNo, 0));
}

void TextEdit::scrollLines(int lines)
{
  sendMessage(EM_LINESCROLL, 0, lines);
}
