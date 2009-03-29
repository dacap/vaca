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

#include "Vaca/RichEdit.h"
#include "Vaca/Event.h"
#include "Vaca/System.h"
#include "Vaca/Register.h"
#include "Vaca/WidgetClass.h"

#include <richedit.h>
#include <memory>

using namespace Vaca;

HINSTANCE RichEditRegister::hmod = NULL;

/// Tries to load the Riched20 DLL.
/// 
/// @throw RegisterException If the DLL is not found.
/// 
RichEditRegister::RichEditRegister()
{
  if (hmod == NULL) {
    // try SciLexer.dll
    hmod = LoadLibrary(L"Riched20.dll");
    if (hmod == NULL)
      throw RegisterException(L"Cannot load Riched20.dll");
  }
}

RichEdit::RichEdit(const String& text, Widget* parent, Style style)
  : Widget(WidgetClassName(RICHEDIT_CLASS), parent, style)
{
  setText(text);
  // setBgColor(System::getColor(COLOR_WINDOW));
}

RichEdit::~RichEdit()
{
}

void RichEdit::setBgColor(const Color& color)
{
  Widget::setBgColor(color);

  sendMessage(EM_SETBKGNDCOLOR, 0, color.getColorRef());
}

size_t RichEdit::getTextLength() const
{
  GETTEXTLENGTHEX tl;
  tl.flags = GTL_DEFAULT; // TODO test the difference with GTL_NUMCHARS
  tl.codepage = 1200;
  return const_cast<RichEdit*>(this)->sendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&tl, 0);
}

size_t RichEdit::getTextLimit() const
{
  return const_cast<RichEdit*>(this)->sendMessage(EM_GETLIMITTEXT, 0, 0);
}

void RichEdit::setTextLimit(size_t maxChars)
{
  assert(maxChars > 0);

  sendMessage(EM_SETLIMITTEXT, maxChars, 0);
  //sendMessage(EM_EXLIMITTEXT, 0, maxChars);
}

/// Returns true if the current content in the clipboard can be pasted
/// in the RichEdit widget.
/// 
/// @win32
///   Sends the @msdn{EM_CANPASTE} message to the widget.
/// @endwin32
/// 
/// @todo It should support a Clipboard::Format parameter
/// 
bool RichEdit::canPaste() const
{
  return const_cast<RichEdit*>(this)->sendMessage(EM_CANPASTE, 0, 0) != 0 ? true: false;
}

/// Returns true if the user can undo the last action.
/// 
/// @win32
///   Sends the @msdn{EM_CANUNDO} message to the widget.
/// @endwin32
/// 
bool RichEdit::canUndo() const
{
  return const_cast<RichEdit*>(this)->sendMessage(EM_CANUNDO, 0, 0) != 0;
}

/// Returns true if the user can redo an action.
/// 
/// @win32
///   Sends the @msdn{EM_CANREDO} message to the widget.
/// @endwin32
/// 
bool RichEdit::canRedo() const
{
  return const_cast<RichEdit*>(this)->sendMessage(EM_CANREDO, 0, 0) != 0;
}

/// Undoes the last user action.
/// 
/// @win32
///   Sends the @msdn{EM_UNDO} message to the widget.
/// @endwin32
/// 
void RichEdit::undo()
{
  sendMessage(WM_UNDO, 0, 0);
}

/// Redoes the last undid action.
/// 
/// @win32
///   Sends the @msdn{EM_REDO} message to the widget.
/// @endwin32
/// 
void RichEdit::redo()
{
  sendMessage(EM_REDO, 0, 0);
}

/// @win32
///   Sends the @msdn{EM_SETUNDOLIMIT} message to the widget.
/// @endwin32
/// 
void RichEdit::setUndoLimit(int maximumActions)
{
  sendMessage(EM_SETUNDOLIMIT, 0, 0);
}

float RichEdit::getZoomFactor()
{
  int num, dem;
  sendMessage(EM_SETZOOM, (WPARAM)&num, (LPARAM)&dem);
  if (dem != 0)
    return (float)num / (float)dem;
  else
    return 1.0f;
}

void RichEdit::setZoomFactor(float f)
{
  sendMessage(EM_SETZOOM, (WPARAM)(f * 1000), 1000);
}

/// If you set this to true, all text added in the RichEdit that
/// contains URLs will be automatically detected and replaced with a
/// hyperlink.
/// 
/// @win32
///   Sends the @msdn{EM_AUTOURLDETECT} message to the widget.
/// @endwin32
/// 
void RichEdit::setDetectUrls(bool state)
{
  sendMessage(EM_AUTOURLDETECT, state ? TRUE: FALSE, 0);
}

/// Returns true if the autodetection of URLs is activated.
/// 
/// @win32
///   Sends the @msdn{EM_GETAUTOURLDETECT} message to the widget.
/// @endwin32
/// 
bool RichEdit::isDetectUrls()
{
  return sendMessage(EM_GETAUTOURLDETECT, 0, 0) != 0 ? true: false;
}

void RichEdit::cut()
{
  sendMessage(WM_CUT, 0, 0);
}

void RichEdit::copy()
{
  sendMessage(WM_COPY, 0, 0);
}

void RichEdit::paste()
{
  sendMessage(WM_PASTE, 0, 0);
}

/// Selects all the text in the Edit widget.
/// 
void RichEdit::selectAll()
{
  CHARRANGE cr;
  cr.cpMin = 0;
  cr.cpMax = -1;
  sendMessage(EM_EXSETSEL, 0, (LPARAM)&cr);
}

/// Selects a range of text.
/// 
void RichEdit::selectRange(int start, int end)
{
  CHARRANGE cr;
  cr.cpMin = start;
  cr.cpMax = end;
  sendMessage(EM_EXSETSEL, 0, (LPARAM)&cr);
}

/// Removes the text's selection.
/// 
void RichEdit::deselect()
{
  sendMessage(EM_SETSEL, static_cast<WPARAM>(-1), 0);
}

/// Returns the current selected range in the text-box.
/// 
/// @win32
///   Sends the @msdn{EM_EXGETSEL} message to the widget.
/// @endwin32
/// 
void RichEdit::getSelection(int& start, int& end)
{
  CHARRANGE cr;
  sendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
  start = cr.cpMin;
  end = cr.cpMax;
}

/// Returns the current selected text.
/// 
/// @win32
///   Sends the @msdn{EM_GETSELTEXT} message to the widget.
/// @endwin32
/// 
String RichEdit::getSelectedText() const
{
  size_t length = getTextLength();
  std::auto_ptr<Char> bufferWrap(new Char[length > 0 ? length: 1]);
  Char* buffer = bufferWrap.get();

  const_cast<RichEdit*>(this)->sendMessage(EM_GETSELTEXT, 0, (LPARAM)buffer);
  return String(buffer);
}
