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
#include "Vaca/FindTextDialog.h"
#include "Vaca/Debug.h"
#include "Vaca/Application.h"

using namespace Vaca;

#define FINDREPLACE_BUFSIZE 1024

#define WIN32_ID_OK            0x001
#define WIN32_ID_REPLACE       0x400
#define WIN32_ID_REPLACEALL    0x401
#define WIN32_ID_CANCEL        0x002
#define WIN32_ID_FINDWHAT      0x480
#define WIN32_ID_REPLACEWITH   0x481
#define WIN32_ID_WHOLEWORD     0x410
#define WIN32_ID_MATCHCASE     0x411
#define WIN32_ID_DIRBACKWARD   0x420
#define WIN32_ID_DIRFORWARD    0x421

//////////////////////////////////////////////////////////////////////
// FindTextDialog

FindTextDialog::FindTextDialog(bool replace, Widget *parent)
  : Dialog(NULL, "", NULL, NoStyle)
{
  m_replace = replace;
  m_findMsgId = RegisterWindowMessage(FINDMSGSTRING);

  // now Widget::create calls FindTextDialog::createHWND because FindTextDialog is constructed
  create(NULL, parent, NoStyle);
}

FindTextDialog::~FindTextDialog()
{
  if (m_replace)
    delete m_findReplace.lpstrReplaceWith;

  delete m_findReplace.lpstrFindWhat;
}

HWND FindTextDialog::createHWND(LPCTSTR className, Widget *parent, Style style)
{
  ZeroMemory(&m_findReplace, sizeof(m_findReplace));

  m_findReplace.lStructSize = sizeof(m_findReplace);
  m_findReplace.hwndOwner = parent->getHWND();
  // m_findReplace.hInstance = NULL;//Application::getHINSTANCE();
  m_findReplace.Flags = 0
    | FR_DOWN
    | FR_ENABLEHOOK
    ;

  m_findReplace.lpstrFindWhat = new Character[FINDREPLACE_BUFSIZE];
  m_findReplace.wFindWhatLen = FINDREPLACE_BUFSIZE;

  ZeroMemory(m_findReplace.lpstrFindWhat, m_findReplace.wFindWhatLen);

  // m_findReplace.lCustData = 0;
  m_findReplace.lpfnHook = &FindTextDialog::hookProc;
  // m_findReplace.lpfnHook = NULL;
  // m_findReplace.lpTemplateName = NULL;

  if (m_replace) {
    m_findReplace.lpstrReplaceWith = m_replace ? new Character[FINDREPLACE_BUFSIZE]: NULL;
    m_findReplace.wReplaceWithLen = m_replace ? FINDREPLACE_BUFSIZE: 0;

    ZeroMemory(m_findReplace.lpstrReplaceWith, m_findReplace.wReplaceWithLen);
  }

  return !m_replace ? ::FindText(&m_findReplace):
		     ::ReplaceText(&m_findReplace);
}

bool FindTextDialog::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
  // don't call Widget::wndProc

  return false;//Frame::wndProc(message, wParam, lParam, lResult);
}

String FindTextDialog::getFindWhat()
{
  assert(::IsWindow(getHWND()));

  HWND hwnd = GetDlgItem(getHWND(), WIN32_ID_FINDWHAT);
  if (::IsWindow(hwnd)) {
    int len = GetWindowTextLength(hwnd);
    if (len > 0) {
      LPTSTR buf = (LPTSTR)new _TCHAR[len+1];
      ::GetWindowText(hwnd, buf, len+1);
      String str = String(buf);
      delete buf;
      return str;
    }
  }

  return String("");
}

String FindTextDialog::getReplaceWith()
{
  assert(::IsWindow(getHWND()));

  HWND hwnd = GetDlgItem(getHWND(), WIN32_ID_REPLACEWITH);
  if (::IsWindow(hwnd)) {
    int len = GetWindowTextLength(hwnd);
    if (len > 0) {
      LPTSTR buf = (LPTSTR)new _TCHAR[len+1];
      ::GetWindowText(hwnd, buf, len+1);
      String str = String(buf);
      delete buf;
      return str;
    }
  }

  return String("");
}

bool FindTextDialog::isWholeWord()
{
  return IsDlgButtonChecked(getHWND(), WIN32_ID_WHOLEWORD) == BST_CHECKED;
}

bool FindTextDialog::isMatchCase()
{
  return IsDlgButtonChecked(getHWND(), WIN32_ID_MATCHCASE) == BST_CHECKED;
}

bool FindTextDialog::isBackward()
{
  if (m_replace)
    return false;
  else
    return IsDlgButtonChecked(getHWND(), WIN32_ID_DIRBACKWARD) == BST_CHECKED;
}

bool FindTextDialog::isForward()
{
  if (m_replace)
    return true;
  else
    return IsDlgButtonChecked(getHWND(), WIN32_ID_DIRFORWARD) == BST_CHECKED;
}

void FindTextDialog::onFindNext()
{
  FindNext();
}

void FindTextDialog::onReplace()
{
  Replace();
}

void FindTextDialog::onReplaceAll()
{
  ReplaceAll();
}

void FindTextDialog::onCancel()
{
  Cancel();
}

UINT_PTR CALLBACK FindTextDialog::hookProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  FindTextDialog *dlg = dynamic_cast<FindTextDialog *>(Widget::fromHWND(hdlg));

  switch (message) {

    case WM_INITDIALOG:
      // avoid to show the dialog by default
      return FALSE;
//       return TRUE;

    case WM_COMMAND:
      switch (HIWORD(wParam)) {

// 	case 1:
	case BN_CLICKED:
	  switch (LOWORD(wParam)) {

	    case WIN32_ID_OK:
	      dlg->onFindNext();
	      break;

	    case WIN32_ID_REPLACE:
	      dlg->onReplace();
	      break;

	    case WIN32_ID_REPLACEALL:
	      dlg->onReplaceAll();
	      break;

	    case WIN32_ID_CANCEL:
	      dlg->onCancel();
	      break;

	  }

// 	  VACA_TRACE("BN_CLICKED %d\n", LOWORD(wParam));
	  break;

      }
      break;

  }

//   if (dlg != NULL) {
//     if (dlg->m_findMsgId == message) {
//       Beep(200, 10);
//     }
//   }

//   LRESULT lResult;
//   if (Dialog::wndProc(uiMsg, wParam, lParam, lResult))
//     return lResult;
//   else
//     return FALSE;

//   LPFINDREPLACE fr = reinterpret_cast<LPFINDREPLACE>(lParam);

//   if ((fr->Flags & FR_DIALOGTERM) != 0) {
//   }
//   else if ((fr->Flags & FR_FINDNEXT) != 0) {
//   }
//   else if ((fr->Flags & FR_REPLACE) != 0) {
//   }
//   else if ((fr->Flags & FR_REPLACEALL) != 0) {
//   }

  return FALSE;
}
