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

#include "stdvaca.h"
#include "Vaca/Edit.hpp"
#include "Vaca/Event.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/System.hpp"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// Edit

Edit::Edit(const String &text, Widget *parent, Style style)
  : Widget(WC_EDIT, parent, style)
{
  setText(text);
  setBgColor(System::getColor(COLOR_WINDOW));
}

Edit::~Edit()
{
}

/**
 * Returns the current text limit (EM_GETLIMITTEXT).
 */
int Edit::getTextLimit()
{
  return static_cast<int>(sendMessage(EM_GETLIMITTEXT, 0, 0));
}

/**
 * Sets the text limit (EM_SETLIMITTEXT). The user can't write more of
 * @a textLimit characters inside the Edit widget.
 */
void Edit::setTextLimit(int textLimit)
{
  assert(textLimit > 0);

  sendMessage(EM_SETLIMITTEXT, textLimit, 0);
}

/**
 * Returns true if this Edit widget has read-only mode activated.
 * 
 */
bool Edit::isReadOnly()
{
  return (getStyle().regular & ES_READONLY) != 0 ? true: false;
}

/**
 * Changes the read-only mode to @a readOnly. A read-only Edit widget
 * can't be modified. It's a wrapper for the EM_SETREADONLY message.
 */
void Edit::setReadOnly(bool readOnly)
{
  sendMessage(EM_SETREADONLY, readOnly, 0);
}

/**
 * Returns true if the user can undo the last operation (EM_CANUNDO).
 * 
 */
bool Edit::canUndo()
{
  return sendMessage(EM_CANUNDO, 0, 0) ? true: false;
}

/**
 * Undoes the last user's operation in the Edit widget.
 * 
 */
void Edit::undo()
{
  sendMessage(EM_UNDO, 0, 0);
}

/**
 * Selects all the text in the Edit widget.
 */
void Edit::selectAll()
{
  sendMessage(EM_SETSEL, 0, -1);
}

/**
 * Selects a range of text. The range is specified by the indexes
 * startIndex and endIndex.
 */
void Edit::selectRange(int startIndex, int endIndex)
{
  // TODO
  //assert(startIndex >= 0 && startIndex <= endIndex && endIndex < getTextLength());

  sendMessage(EM_SETSEL, startIndex, endIndex);
}

/**
 * Removes the text's selection.
 */
void Edit::deselect()
{
  sendMessage(EM_SETSEL, static_cast<WPARAM>(-1), 0);
}

void Edit::onPreferredSize(Size &sz)
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
    textSize.h = lines * g.measureString("", 32767, 0).h;
  }

  sz = textSize+border;
}

void Edit::onChange(Event &ev)
{
  Change(ev);
}

/**
 * Converts the EN_CHANGE in onChange.
 */
bool Edit::onReflectedCommand(int id, int code, LRESULT &lResult)
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

//////////////////////////////////////////////////////////////////////
// PasswordEdit

PasswordEdit::PasswordEdit(const String &text, Widget *parent, Style style)
  : Edit(text, parent, style)
{
}

PasswordEdit::~PasswordEdit()
{
}

Character PasswordEdit::getPasswordCharacter()
{
  return static_cast<Character>(sendMessage(EM_GETPASSWORDCHAR, 0, 0));
}

void PasswordEdit::setPasswordCharacter(Character passwordChar)
{
  sendMessage(EM_SETPASSWORDCHAR, passwordChar, 0);
}

//////////////////////////////////////////////////////////////////////
// MultilineEdit

MultilineEdit::MultilineEdit(const String &text, Widget *parent, Style style)
  : Edit(text, parent, style)
{
}

MultilineEdit::~MultilineEdit()
{
}

bool MultilineEdit::getWantReturnMode()
{
  HWND hwnd = getHWND();
  assert(::IsWindow(hwnd));
  return (GetWindowLong(hwnd, GWL_STYLE) & ES_WANTRETURN) != 0 ? true: false;
}

void MultilineEdit::setWantReturnMode(bool wantReturn)
{
  HWND hwnd = getHWND();
  assert(::IsWindow(hwnd));
  int style = GetWindowLong(hwnd, GWL_STYLE) & ~ES_WANTRETURN;
  SetWindowLong(hwnd, GWL_STYLE, style | (wantReturn ? ES_WANTRETURN: 0));
}

int MultilineEdit::getLineCount()
{
  return static_cast<int>(sendMessage(EM_GETLINECOUNT, 0, 0));
}

String MultilineEdit::getLine(int lineNo)
{
  int length = getLineLength(lineNo);
  if (length > 0) {
    LPTSTR tstr = new _TCHAR[length];

    sendMessage(EM_GETLINE, lineNo, reinterpret_cast<LPARAM>(tstr));

    String string(length);

    LPTSTR p = tstr;
    for (int c=0; c<length; ++c, ++p)
      string.push_back(*p);

    delete tstr;
    return string;
  }
  else
    return String();
}

int MultilineEdit::getLineLength(int lineNo)
{
  return static_cast<int>(sendMessage(EM_LINELENGTH, lineNo, 0));
}

void MultilineEdit::scrollLines(int lines)
{
  sendMessage(EM_LINESCROLL, 0, lines);
}

//////////////////////////////////////////////////////////////////////
// RichEdit

// bool RichEdit::canRedo()
// {
//   return _sendMessage(EM_CANREDO, 0, 0) ? true: false;
// }

// void RichEdit::redo()
// {
//   _sendMessage(EM_REDO, 0, 0);
// }
