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

#include "Vaca/Clipboard.h"
#include "Vaca/Widget.h"
#include "Vaca/System.h"

using namespace Vaca;

Clipboard::Clipboard(Widget* owner)
{
  m_owner = owner;
}

Clipboard::~Clipboard()
{
}

bool Clipboard::isText() const
{
  return IsClipboardFormatAvailable(CF_TEXT) ? true: false;
}

String Clipboard::getString() const
{
  HWND hwndOwner = m_owner ? m_owner->getHandle(): NULL;
  String str;

  if (!IsClipboardFormatAvailable(CF_TEXT))
    return str;

  if (!OpenClipboard(hwndOwner)) 
    return str;

  HGLOBAL hglobal = GetClipboardData(CF_TEXT); 
  if (hglobal != NULL) {
    LPTSTR lptstr = static_cast<LPTSTR>(GlobalLock(hglobal));
    if (lptstr != NULL) {
      str.assign(lptstr);
      GlobalUnlock(hglobal);
    }
  }

  CloseClipboard();

  return str;
}

void Clipboard::setString(const String& str)
{
  HWND hwndOwner = m_owner ? m_owner->getHandle(): NULL;

  if (!OpenClipboard(hwndOwner))
    return;

  EmptyClipboard();

  // NT, 2K and XP
  if (System::isWinNT_2K_XP()) {
    int len = str.size();
    HGLOBAL hglobal = GlobalAlloc(GMEM_MOVEABLE, sizeof(TCHAR)*(len+1));
    LPTSTR lptstr = static_cast<LPTSTR>(GlobalLock(hglobal));
    str.copyTo(lptstr, len);
    GlobalUnlock(hglobal);
 
#ifdef _UNICODE
    SetClipboardData(CF_UNICODETEXT, hglobal);
#else
    SetClipboardData(CF_TEXT, hglobal);
#endif
  }
  // 98, Me
  else {
#ifdef _UNICODE
    // TODO convert the str to ANSI and copy the content to Clipboard
#else
    int len = str.size();
    HGLOBAL hglobal = GlobalAlloc(GMEM_MOVEABLE, sizeof(TCHAR)*(len+1));
    LPTSTR lptstr = static_cast<LPTSTR>(GlobalLock(hglobal));
    str.copyTo(lptstr, len);
    GlobalUnlock(hglobal);
 
    SetClipboardData(CF_TEXT, hglobal);
#endif
  }

  CloseClipboard();
}
