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
#include "Vaca/Cursor.h"
#include "Vaca/Application.h"
#include "Vaca/Debug.h"
#include "Vaca/ResourceException.h"

using namespace Vaca;

Cursor::Cursor(int cursorId)
{
  m_HCURSOR = LoadCursor(Application::getHINSTANCE(),
			 MAKEINTRESOURCE(cursorId));
  m_autoDelete = true;

  if (m_HCURSOR == NULL)
    throw ResourceException();
}

Cursor::Cursor(SysCursor cursor)
{
  LPCTSTR sysCursor = IDC_ARROW;

  switch (cursor) {

    // special cursor that has HCURSOR = NULL
    case NoCursor:
      m_HCURSOR = NULL;
      m_autoDelete = false;
      return;

    case ArrowCursor:     sysCursor = IDC_ARROW; break;
    case CrosshairCursor: sysCursor = IDC_CROSS; break;
// #ifdef IDC_HAND
    case HandCursor:      sysCursor = IDC_HAND; break;
// #endif
    case HelpCursor:      sysCursor = IDC_HELP; break;
    case TextCursor:      sysCursor = IDC_IBEAM; break;
    case ForbiddenCursor: sysCursor = IDC_NO; break;
    case MoveCursor:      sysCursor = IDC_SIZEALL; break;
    case SizeECursor:     sysCursor = IDC_SIZEWE; break;
    case SizeNCursor:     sysCursor = IDC_SIZENS; break;
    case SizeNECursor:    sysCursor = IDC_SIZENESW; break;
    case SizeNWCursor:    sysCursor = IDC_SIZENWSE; break;
    case SizeSCursor:     sysCursor = IDC_SIZENS; break;
    case SizeSECursor:    sysCursor = IDC_SIZENWSE; break;
    case SizeSWCursor:    sysCursor = IDC_SIZENESW; break;
    case SizeWCursor:     sysCursor = IDC_SIZEWE; break;
    case UpArrowCursor:   sysCursor = IDC_UPARROW; break;
    case WaitCursor:      sysCursor = IDC_WAIT; break;
// #ifdef IDC_APPSTARTING
    case WaitBgCursor:    sysCursor = IDC_APPSTARTING; break;
// #endif
  }
  
  m_HCURSOR = LoadCursor(NULL, sysCursor);
  m_autoDelete = false;

  if (m_HCURSOR == NULL)
    throw ResourceException();
}

Cursor::Cursor(const String &fileName)
{
  m_HCURSOR = reinterpret_cast<HCURSOR>
    (LoadImage(Application::getHINSTANCE(),
	       fileName.c_str(),
	       IMAGE_CURSOR,
	       0, 0, LR_LOADFROMFILE));

  m_autoDelete = true;

  if (m_HCURSOR == NULL)
    throw ResourceException();
}

Cursor::Cursor(const Cursor &cursor)
{
  if (cursor.m_autoDelete) {
    m_HCURSOR = CopyCursor(cursor.m_HCURSOR);
    m_autoDelete = true;
  }
  else {
    m_HCURSOR = cursor.m_HCURSOR;
    m_autoDelete = false;
  }

  if (m_HCURSOR == NULL)
    throw ResourceException();
}

Cursor::~Cursor()
{
  if (m_HCURSOR != NULL && m_autoDelete)
    DestroyCursor(m_HCURSOR);
}

HCURSOR Cursor::getHCURSOR()
{
  return m_HCURSOR;
}
