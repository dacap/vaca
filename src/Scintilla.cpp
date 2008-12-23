// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
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

#include "Vaca/Scintilla.h"
#include "Vaca/Font.h"
#include "Vaca/WidgetClass.h"
#include "Scintilla.h"
#include "SciLexer.h"

using namespace Vaca;

HINSTANCE SciRegister::hmod = NULL;

SciRegister::SciRegister()
{
  if (hmod == NULL) {
    // try SciLexer.dll
    hmod = LoadLibrary(_T("SciLexer.dll"));
    if (hmod == NULL) {
      // try Scintilla.dll
      hmod = LoadLibrary(_T("Scintilla.dll"));
      if (hmod == NULL) {
	// fail
	MessageBox(NULL,
		   _T("The Scintilla DLL could not be loaded."),
		   _T("Error loading Scintilla"),
		   MB_OK | MB_ICONERROR);
      }
    }
  }
}

SciEdit::SciEdit(Widget* parent, Style style)
  : Widget(WidgetClassName(_T("Scintilla")), parent, style)
{
//   sendMessage(SCI_SETLEXER, SCLEX_HTML, 0);
//   sendMessage(SCI_SETSTYLEBITS, 7, 0);

  // We are on Windows, set CR/LF mode by default
  setEolMode(SC_EOL_CRLF);
}

SciEdit::~SciEdit()
{
}

/**
 * Sets the font used to paint text inside the editor. WARNING: this
 * sets the default style of the Scintilla editor (see for
 * STYLE_DEFAULT in the Scintilla documentation).
 */
void SciEdit::setFont(Font font)
{
  Widget::setFont(font);

  LOGFONT lf;
  if (font.getLogFont(&lf)) {
    FontStyle style = font.getStyle();

    // set font for the default style
    sendMessage(SCI_STYLESETFONT, STYLE_DEFAULT, reinterpret_cast<LPARAM>(lf.lfFaceName));
    sendMessage(SCI_STYLESETSIZE, STYLE_DEFAULT, font.getPointSize());
    sendMessage(SCI_STYLESETBOLD, STYLE_DEFAULT, style & FontStyle::Bold ? TRUE: FALSE);
    sendMessage(SCI_STYLESETITALIC, STYLE_DEFAULT, style & FontStyle::Italic ? TRUE: FALSE);
    sendMessage(SCI_STYLESETUNDERLINE, STYLE_DEFAULT, style & FontStyle::Underline ? TRUE: FALSE);
  }
}

//////////////////////////////////////////////////////////////////////
// Text retrieval and modification

String SciEdit::getText() const
{
  int length = const_cast<SciEdit*>(this)->sendMessage(SCI_GETLENGTH, 0, 0);
  if (length > 0) {
    char* text = new char[length+1];
    const_cast<SciEdit*>(this)->sendMessage(SCI_GETTEXT, length+1, reinterpret_cast<LPARAM>(text));
    String str(text);
    delete text;
    return str;
  }
  else
    return String("");
}

void SciEdit::setText(const String& str)
{
  sendMessage(SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(str.c_str()));
}

void SciEdit::setSavePoint()
{
  sendMessage(SCI_SETSAVEPOINT, 0, 0);
}

/**
 * Returns a lien of text. Line 0 is the first line. The text returned
 * includes the new line character.
 */
String SciEdit::getLine(int line) const
{
  int length = getLineLength(line);
  if (length > 0) {
    char* text = new char[length+1];
    ZeroMemory(text, length+1);
    const_cast<SciEdit*>(this)->sendMessage(SCI_GETLINE, line, reinterpret_cast<LPARAM>(text));
    String str(text, length);
    delete text;
    return str;
  }
  else
    return String();
}

void SciEdit::replaceSel(const String& str)
{
  sendMessage(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(str.c_str()));
}

void SciEdit::setReadOnly(bool readOnly)
{
  sendMessage(SCI_SETREADONLY, readOnly, 0);
}

bool SciEdit::getReadOnly() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_GETREADONLY, 0, 0) != 0;
}

/**
 * Adds the first @c length characters of @c str string at the current
 * position of the document.
 */
void SciEdit::addText(const char* str, int length)
{
  sendMessage(SCI_ADDTEXT, length, reinterpret_cast<LPARAM>(str));
}

/**
 * Adds the string @c str at the current position of the document.
 */
void SciEdit::addText(const String& str)
{
  sendMessage(SCI_ADDTEXT, str.size(), reinterpret_cast<LPARAM>(str.c_str()));
}

/**
 * Adds the first @c length characters of @c str string to the end of
 * the document.
 */
void SciEdit::appendText(const char* str, int length)
{
  sendMessage(SCI_APPENDTEXT, length, reinterpret_cast<LPARAM>(str));
}

/**
 * Adds the string @c str to the end of the document.
 */
void SciEdit::appendText(const String& str)
{
  sendMessage(SCI_APPENDTEXT, str.size(), reinterpret_cast<LPARAM>(str.c_str()));
}

void SciEdit::insertText(int pos, const String& str)
{
  sendMessage(SCI_INSERTTEXT, pos, reinterpret_cast<LPARAM>(str.c_str()));
}

void SciEdit::clearAll()
{
  sendMessage(SCI_CLEARALL, 0, 0);
}

Character SciEdit::getCharAt(int pos) const
{
  return (Character)
    const_cast<SciEdit*>(this)->sendMessage(SCI_GETCHARAT, pos, 0);
}

//////////////////////////////////////////////////////////////////////
// Searching


void SciEdit::searchAnchor()
{
  sendMessage(SCI_SEARCHANCHOR, 0, 0);
}

bool SciEdit::searchNext(int flags, String& str)
{
  return sendMessage(SCI_SEARCHNEXT,
		     flags,
		     reinterpret_cast<LPARAM>(str.c_str())) >= 0;
}

bool SciEdit::searchPrev(int flags, String& str)
{
  return sendMessage(SCI_SEARCHPREV, flags,
		     reinterpret_cast<LPARAM>(str.c_str())) >= 0;
}

//////////////////////////////////////////////////////////////////////
// Searching and replace using target

// bool SciEdit::findText(const String& str, bool matchCase, bool wholeWord)
// {
//   int pos = sendMessage(SCI_SEARCHNEXT,
// 			(matchCase ? SCFIND_MATCHCASE: 0) |
// 			(wholeWord ? SCFIND_WHOLEWORD: 0),
// 			reinterpret_cast<LPARAM>(&ttf));

//   int pos = sendMessage(SCI_SEARCHNEXT,
// 			(matchCase ? SCFIND_MATCHCASE: 0) |
// 			(wholeWord ? SCFIND_WHOLEWORD: 0),
// 			reinterpret_cast<LPARAM>(&ttf));


// //   TextToFind ttf;

// //   ttf.chrg.cpMin = 0;
// //   ttf.chrg.cpMax = -1;
// //   ttf.lpstrText = const_cast<Char*>(str.c_str());

// //   int pos = sendMessage(SCI_FINDTEXT,
// // 			(matchCase ? SCFIND_MATCHCASE: 0) |
// // 			(wholeWord ? SCFIND_WHOLEWORD: 0),
// // 			reinterpret_cast<LPARAM>(&ttf));
// //   if (pos < 0)
// //     return false;

// //   sendMessage(SCI_SETSEL, ttf.chrgText.cpMin, ttf.chrgText.cpMax);
// //   return true;
// }

//////////////////////////////////////////////////////////////////////
// Overtype (overwrite-mode)

void SciEdit::setOverwriteMode(bool state)
{
  sendMessage(SCI_SETOVERTYPE, state, 0);
}

bool SciEdit::getOverwriteMode() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_GETOVERTYPE, 0, 0) != 0;
}

//////////////////////////////////////////////////////////////////////
// Cut, copy and paste


void SciEdit::cutTextToClipboard()
{
  sendMessage(SCI_CUT, 0, 0);
}

void SciEdit::copyTextToClipboard()
{
  sendMessage(SCI_COPY, 0, 0);
}

void SciEdit::pasteTextFromClipboard()
{
  sendMessage(SCI_PASTE, 0, 0);
}

void SciEdit::clearText()
{
  sendMessage(SCI_CLEAR, 0, 0);
}

//////////////////////////////////////////////////////////////////////
// Error handling


//////////////////////////////////////////////////////////////////////
// Undo and redo


void SciEdit::undo()
{
  sendMessage(SCI_UNDO, 0, 0);
}

bool SciEdit::canUndo() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_CANUNDO, 0, 0) != 0;
}

void SciEdit::redo()
{
  sendMessage(SCI_REDO, 0, 0);
}

bool SciEdit::canRedo() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_CANREDO, 0, 0) != 0;
}

void SciEdit::emptyUndoBuffer()
{
  sendMessage(SCI_EMPTYUNDOBUFFER, 0, 0);
}

void SciEdit::beginUndoAction()
{
  sendMessage(SCI_BEGINUNDOACTION, 0, 0);
}

void SciEdit::endUndoAction()
{
  sendMessage(SCI_ENDUNDOACTION, 0, 0);
}

//////////////////////////////////////////////////////////////////////
// Selection and information


int SciEdit::getTextLength() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_GETTEXTLENGTH, 0, 0);
}

/**
 * Returns the number of lines that has the text.
 *
 * @warning The last line doesn't have a end of line (\\n) character.
 */
int SciEdit::getLineCount() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_GETLINECOUNT, 0, 0);
}

int SciEdit::getFirstVisibleLine() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_GETFIRSTVISIBLELINE, 0, 0);
}

// int SciEdit::getVisibleLineCount()
int SciEdit::getLinesOnScreen() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_LINESONSCREEN, 0, 0);
}

bool SciEdit::isModified() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_GETMODIFY, 0, 0) != 0;
}

void SciEdit::goToPos(int pos)
{
  sendMessage(SCI_GOTOPOS, pos, 0);
}

void SciEdit::goToLine(int line)
{
  sendMessage(SCI_GOTOLINE, line, 0);
}

void SciEdit::setCurrentPos(int pos)
{
  sendMessage(SCI_SETCURRENTPOS, pos, 0);
}

int SciEdit::getCurrentPos() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_GETCURRENTPOS, 0, 0);
}

void SciEdit::setAnchor(int pos)
{
  sendMessage(SCI_SETANCHOR, pos, 0);
}

int SciEdit::getAnchor() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_GETANCHOR, 0, 0);
}

int SciEdit::getSelectionStart() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_GETSELECTIONSTART, 0, 0);
}

int SciEdit::getSelectionEnd() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_GETSELECTIONEND, 0, 0);
}

/**
 * Returns the length of the line (including the end of line). The
 * first line is 0.
 */
int SciEdit::getLineLength(int line) const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_LINELENGTH, line, 0);
}

String SciEdit::getSelText() const
{
  int length = getSelectionEnd() - getSelectionStart() + 1;
  char* text = new char[length+1];
  const_cast<SciEdit*>(this)->sendMessage(SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(text));
  String str(text);
  delete text;
  return str;
}

//////////////////////////////////////////////////////////////////////
// Scrolling and automatic scrolling


//////////////////////////////////////////////////////////////////////
// White space


//////////////////////////////////////////////////////////////////////
// Cursor


//////////////////////////////////////////////////////////////////////
// Mouse capture


//////////////////////////////////////////////////////////////////////
// Line endings


/**
 * @param eolMode
 * @li SC_EOL_CRLF
 * @li SC_EOL_CR
 * @li SC_EOL_LF
 */
void SciEdit::setEolMode(int eolMode)
{
  sendMessage(SCI_SETEOLMODE, eolMode, 0);
}

int SciEdit::getEolMode() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_GETEOLMODE, 0, 0);
}

void SciEdit::convertEols(int eolMode)
{
  sendMessage(SCI_CONVERTEOLS, eolMode, 0);
}

void SciEdit::setViewEol(bool visible)
{
  sendMessage(SCI_SETVIEWEOL, visible, 0);
}

bool SciEdit::getViewEol() const
{
  return const_cast<SciEdit*>(this)->sendMessage(SCI_GETVIEWEOL, 0, 0) != 0;
}

//////////////////////////////////////////////////////////////////////
// Styling


//////////////////////////////////////////////////////////////////////
// Style definition


//////////////////////////////////////////////////////////////////////
// Caret, selection, and hotspot styles


//////////////////////////////////////////////////////////////////////
// Margins


//////////////////////////////////////////////////////////////////////
// Other settings


//////////////////////////////////////////////////////////////////////
// Brace highlighting


//////////////////////////////////////////////////////////////////////
// Tabs and Indentation Guides


//////////////////////////////////////////////////////////////////////
// Markers


//////////////////////////////////////////////////////////////////////
// Indicators


//////////////////////////////////////////////////////////////////////
// Autocompletion


//////////////////////////////////////////////////////////////////////
// User lists


//////////////////////////////////////////////////////////////////////
// Call tips


//////////////////////////////////////////////////////////////////////
// Keyboard commands


//////////////////////////////////////////////////////////////////////
// Key bindings


//////////////////////////////////////////////////////////////////////
// Popup edit menu


//////////////////////////////////////////////////////////////////////
// Macro recording


//////////////////////////////////////////////////////////////////////
// Printing


//////////////////////////////////////////////////////////////////////
// Direct access


//////////////////////////////////////////////////////////////////////
// Multiple views

void* SciEdit::getDocPointer()
{
  return reinterpret_cast<void*>(sendMessage(SCI_GETDOCPOINTER, 0, 0));
}

void SciEdit::setDocPointer(void* doc)
{
  sendMessage(SCI_SETDOCPOINTER, 0, reinterpret_cast<LPARAM>(doc));
}

//////////////////////////////////////////////////////////////////////
// Folding


//////////////////////////////////////////////////////////////////////
// Line wrapping


//////////////////////////////////////////////////////////////////////
// Zooming


void SciEdit::zoomIn()
{
  sendMessage(SCI_ZOOMIN, 0, 0);
}

void SciEdit::zoomOut()
{
  sendMessage(SCI_ZOOMOUT, 0, 0);
}

void SciEdit::setZoom(int zoomInPoints)
{
  sendMessage(SCI_SETZOOM, zoomInPoints, 0);
}

int SciEdit::getZoom()
{
  return sendMessage(SCI_GETZOOM, 0, 0);
}

//////////////////////////////////////////////////////////////////////
// Long lines


//////////////////////////////////////////////////////////////////////
// Lexer


//////////////////////////////////////////////////////////////////////
// Notifications

bool SciEdit::onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult)
{
  if (Widget::onReflectedNotify(lpnmhdr, lResult))
    return true;

  switch (lpnmhdr->code) {

//     case SCN_MODIFIED:
//       SCNotification* scn = static_cast<SCNotification *>(lpnmhdr);
//       break;

    case SCN_UPDATEUI:
      onUpdateUI();
      break;

  }

  return false;
}

/**
 * Called when the text or the selection range change
 * (SCN_UPDATEUI). In response to this event you should update your UI
 * elements.  The default implementation fires the UpdateUI signal.
 */
void SciEdit::onUpdateUI()
{
  UpdateUI();
}
