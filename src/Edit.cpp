// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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
#include "Vaca/Edit.h"
#include "Vaca/WidgetEvent.h"
#include "Vaca/Debug.h"
#include "Vaca/System.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// Edit

Edit::Edit(const String &text, Widget *parent, Style style)
  : Widget(_T("EDIT"), parent, style)
{
  setText(text);
  // setCursor(Cursor::Text);
  setBgColor(System::getColor(COLOR_WINDOW));
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
  VACA_ASSERT(textLimit > 0);

  sendMessage(EM_SETLIMITTEXT, textLimit, 0);
}

// TextAlign::Type Edit::getTextAlign()
// {
//   int style = getStyle();

//   if ((style & ES_CENTER) != 0)
//     return TextAlign::Center;
//   else if ((style & ES_RIGHT) != 0)
//     return TextAlign::Right;
//   else
//     return TextAlign::Left;
// }

// void Edit::setTextAlign(TextAlign::Type align)
// {
//   int style = ES_LEFT;

//   switch (align) {
//     case TextAlign::Left:   style = ES_LEFT;   break;
//     case TextAlign::Center: style = ES_CENTER; break;
//     case TextAlign::Right:  style = ES_RIGHT;  break;
//   }

//   removeStyle(ES_LEFT | ES_CENTER | ES_RIGHT);
//   addStyle(style);

//   invalidate(true);
// }

bool Edit::isReadOnly()
{
  return (getStyle().regular & ES_READONLY) != 0 ? true: false;
}

/**
 * Wrapper for the EM_SETREADONLY message.
 */
void Edit::setReadOnly(bool readOnly)
{
  sendMessage(EM_SETREADONLY, readOnly, 0);
}

bool Edit::canUndo()
{
  return sendMessage(EM_CANUNDO, 0, 0) ? true: false;
}

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
  //VACA_ASSERT(startIndex >= 0 && startIndex <= endIndex && endIndex < getTextLength());

  sendMessage(EM_SETSEL, startIndex, endIndex);
}

/**
 * Removes the text's selection.
 */
void Edit::deselect()
{
  sendMessage(EM_SETSEL, static_cast<WPARAM>(-1), 0);
}

Size Edit::preferredSize()
{
  return preferredSize(Size(0, 0));
}

Size Edit::preferredSize(const Size &fitIn)
{
  Size textSize;
  Style style = getStyle();
    
  {
    ScreenGraphics g;
    g.setFont(getFont());
    textSize = g.measureString(getText(), fitIn.w == 0 ? 32767: fitIn.w,
			       ((style.regular & ES_MULTILINE) != 0) ? DT_WORDBREAK: 0);
  }

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

  return textSize+border;
}

void Edit::onChange(WidgetEvent &ev)
{
  Change(ev);
}

bool Edit::onCommand(int commandCode, LRESULT &lResult)
{
  switch (commandCode) {
	    
    case EN_CHANGE: {
      WidgetEvent ev(this);
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

bool MultilineEdit::getWantReturnMode()
{
  HWND hwnd = getHwnd();
  VACA_ASSERT(hwnd != NULL);
  return (GetWindowLong(hwnd, GWL_STYLE) & ES_WANTRETURN) != 0 ? true: false;
}

void MultilineEdit::setWantReturnMode(bool wantReturn)
{
  HWND hwnd = getHwnd();
  VACA_ASSERT(hwnd != NULL);
  int style = GetWindowLong(hwnd, GWL_STYLE) & ~ES_WANTRETURN;
  SetWindowLong(hwnd, GWL_STYLE, style | (wantReturn ? ES_WANTRETURN: 0));
}

int MultilineEdit::getLineCount()
{
  return sendMessage(EM_GETLINECOUNT, 0, 0);
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
  return sendMessage(EM_LINELENGTH, lineNo, 0);
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
