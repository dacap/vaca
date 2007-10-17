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
#include "Vaca/Clipboard.h"
#include "Vaca/Widget.h"

using namespace Vaca;

Clipboard::Clipboard(Widget *owner)
{
  mOwner = owner;
}

Clipboard::~Clipboard()
{
}

String Clipboard::getString() const
{
  HWND hwndOwner = mOwner ? mOwner->getHwnd(): NULL;
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

void Clipboard::setString(const String &str)
{
  HWND hwndOwner = mOwner ? mOwner->getHwnd(): NULL;

  if (!OpenClipboard(hwndOwner))
    return;

  EmptyClipboard();

  int len = str.size();
  HGLOBAL hglobal = GlobalAlloc(GMEM_MOVEABLE, (len + 1));
  LPTSTR lptstr = static_cast<LPTSTR>(GlobalLock(hglobal));
#ifdef UNICODE
  wcscpy(lptstr, str.c_str());
#else
  strcpy(lptstr, str.c_str());
#endif
  GlobalUnlock(hglobal);
 
  SetClipboardData(CF_TEXT, hglobal);

  CloseClipboard();
}
