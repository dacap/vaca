// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifdef VACA_WINDOWS
  #include <windows.h>
#endif

#include "vaca/FindTextDialog.h"
#include "vaca/Debug.h"
#include "vaca/Application.h"
#include "vaca/WidgetClass.h"
#include "vaca/CloseEvent.h"

using namespace vaca;

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

UINT FindTextDialog::m_findMsgId = 0;	// TODO add multithreading support

FindTextDialog::FindTextDialog(bool replace, Widget* parent)
  : Dialog(WidgetClassName::None, L"", NULL, Widget::Styles::None)
  , m_replace(replace)
{
  if (!m_findMsgId)
    m_findMsgId = RegisterWindowMessage(FINDMSGSTRING);

  // now Widget::create calls FTDialog::createHandle
  create(WidgetClassName::None, parent, Widget::Styles::None);
}

FindTextDialog::~FindTextDialog()
{
  if (m_replace)
    delete m_findReplace.lpstrReplaceWith;
  delete m_findReplace.lpstrFindWhat;
}

String FindTextDialog::getFindWhat()
{
  String res;
  getTextFromDlgItem(WIN32_ID_FINDWHAT, res);
  return res;
}

String FindTextDialog::getReplaceWith()
{
  String res;
  getTextFromDlgItem(WIN32_ID_REPLACEWITH, res);
  return res;
}

void FindTextDialog::setFindWhat(const String& s)
{
  setTextFromDlgItem(WIN32_ID_FINDWHAT, s);
}

void FindTextDialog::setReplaceWith(const String& s)
{
  setTextFromDlgItem(WIN32_ID_REPLACEWITH, s);
}

bool FindTextDialog::isWholeWord()
{
  return ::IsDlgButtonChecked(getHandle(), WIN32_ID_WHOLEWORD) == BST_CHECKED;
}

bool FindTextDialog::isMatchCase()
{
  return ::IsDlgButtonChecked(getHandle(), WIN32_ID_MATCHCASE) == BST_CHECKED;
}

bool FindTextDialog::isBackward()
{
  if (m_replace)
    return false;
  else
    return ::IsDlgButtonChecked(getHandle(), WIN32_ID_DIRBACKWARD) == BST_CHECKED;
}

bool FindTextDialog::isForward()
{
  if (m_replace)
    return true;
  else
    return ::IsDlgButtonChecked(getHandle(), WIN32_ID_DIRFORWARD) == BST_CHECKED;
}

void FindTextDialog::onFindNext(Event& ev)
{
  FindNext(ev);
}

void FindTextDialog::onReplace(Event& ev)
{
  Replace(ev);
}

void FindTextDialog::onReplaceAll(Event& ev)
{
  ReplaceAll(ev);
}

void FindTextDialog::getTextFromDlgItem(int id, String& s)
{
  assert(::IsWindow(getHandle()));

  HWND hwnd = GetDlgItem(getHandle(), id);
  if (::IsWindow(hwnd)) {
    int len = GetWindowTextLength(hwnd);
    if (len > 0) {
      Char* buf = new Char[len+1];
      ::GetWindowText(hwnd, buf, len+1);
      s = buf;
      delete buf;
    }
  }
}

void FindTextDialog::setTextFromDlgItem(int id, const String& s)
{
  assert(::IsWindow(getHandle()));

  HWND hwnd = GetDlgItem(getHandle(), id);
  if (::IsWindow(hwnd))
    ::SetWindowText(hwnd, s.c_str());
}

HWND FindTextDialog::createHandle(LPCTSTR className, Widget* parent, Style style)
{
  assert(parent != NULL);

  ZeroMemory(&m_findReplace, sizeof(m_findReplace));

  m_findReplace.lStructSize = sizeof(m_findReplace);
  m_findReplace.hwndOwner = parent->getHandle();
  m_findReplace.hInstance = Application::getHandle();
  m_findReplace.Flags = 0
    | FR_DOWN
    | FR_ENABLEHOOK
    ;

  m_findReplace.lpstrFindWhat = new Char[FINDREPLACE_BUFSIZE];
  m_findReplace.wFindWhatLen = FINDREPLACE_BUFSIZE;

  ZeroMemory(m_findReplace.lpstrFindWhat, m_findReplace.wFindWhatLen);

  m_findReplace.lpfnHook = &FindTextDialog::hookProc;

  if (m_replace) {
    m_findReplace.lpstrReplaceWith = m_replace ? new Char[FINDREPLACE_BUFSIZE]: NULL;
    m_findReplace.wReplaceWithLen = m_replace ? FINDREPLACE_BUFSIZE: 0;

    ZeroMemory(m_findReplace.lpstrReplaceWith, m_findReplace.wReplaceWithLen);
  }

  return !m_replace ? ::FindText(&m_findReplace):
		      ::ReplaceText(&m_findReplace);
}

UINT_PTR CALLBACK FindTextDialog::hookProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  FindTextDialog* dlg = dynamic_cast<FindTextDialog*>(Widget::fromHandle(hdlg));

  switch (message) {

    case WM_INITDIALOG:
      // avoid to show the dialog by default
      return FALSE;

    case WM_COMMAND:
      switch (HIWORD(wParam)) {

	case BN_CLICKED:
	  switch (LOWORD(wParam)) {

	    case WIN32_ID_OK: {
	      Event ev(dlg);
	      dlg->onFindNext(ev);
	      break;
	    }

	    case WIN32_ID_REPLACE: {
	      Event ev(dlg);
	      dlg->onReplace(ev);
	      break;
	    }

	    case WIN32_ID_REPLACEALL: {
	      Event ev(dlg);
	      dlg->onReplaceAll(ev);
	      break;
	    }

	    case WIN32_ID_CANCEL:
	      // don't destroy the HWND
	      return TRUE;

	  }
	  break;

      }
      break;

  }

  return FALSE;
}
