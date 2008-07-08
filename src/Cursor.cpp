// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#include "Vaca/Cursor.hpp"
#include "Vaca/Application.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/ResourceException.hpp"

using namespace Vaca;

Cursor::Cursor(ResourceId cursorId)
{
  m_HCURSOR = LoadCursor(Application::getHINSTANCE(), cursorId.toLPTSTR());
  m_autoDelete = true;

  if (m_HCURSOR == NULL)
    throw ResourceException("Can't load the cursor resource " + cursorId.toString());
}

Cursor::Cursor(SysCursor cursor)
{
  LPCTSTR winCursor = IDC_ARROW;

  switch (cursor) {

    // special cursor that has HCURSOR = NULL
    case SysCursor::None:
      m_HCURSOR = NULL;
      m_autoDelete = false;
      return;

    case SysCursor::Arrow:     winCursor = IDC_ARROW; break;
    case SysCursor::Crosshair: winCursor = IDC_CROSS; break;
// #ifdef IDC_HAND
    case SysCursor::Hand:      winCursor = IDC_HAND; break;
// #endif
    case SysCursor::Help:      winCursor = IDC_HELP; break;
    case SysCursor::Text:      winCursor = IDC_IBEAM; break;
    case SysCursor::Forbidden: winCursor = IDC_NO; break;
    case SysCursor::Move:      winCursor = IDC_SIZEALL; break;
    case SysCursor::SizeE:     winCursor = IDC_SIZEWE; break;
    case SysCursor::SizeN:     winCursor = IDC_SIZENS; break;
    case SysCursor::SizeNE:    winCursor = IDC_SIZENESW; break;
    case SysCursor::SizeNW:    winCursor = IDC_SIZENWSE; break;
    case SysCursor::SizeS:     winCursor = IDC_SIZENS; break;
    case SysCursor::SizeSE:    winCursor = IDC_SIZENWSE; break;
    case SysCursor::SizeSW:    winCursor = IDC_SIZENESW; break;
    case SysCursor::SizeW:     winCursor = IDC_SIZEWE; break;
    case SysCursor::UpArrow:   winCursor = IDC_UPARROW; break;
    case SysCursor::Wait:      winCursor = IDC_WAIT; break;
// #ifdef IDC_APPSTARTING
    case SysCursor::WaitBg:    winCursor = IDC_APPSTARTING; break;
// #endif
  }
  
  m_HCURSOR = LoadCursor(NULL, winCursor);
  m_autoDelete = false;

  if (m_HCURSOR == NULL)
    throw ResourceException("Can't load the SysCursor " + String::fromInt(cursor));
}

Cursor::Cursor(const String& fileName)
{
  m_HCURSOR = reinterpret_cast<HCURSOR>
    (LoadImage(Application::getHINSTANCE(),
	       fileName.c_str(),
	       IMAGE_CURSOR,
	       0, 0, LR_LOADFROMFILE));

  m_autoDelete = true;

  if (m_HCURSOR == NULL)
    throw ResourceException("Can't load cursor from file " + fileName);
}

Cursor::Cursor(const Cursor& cursor)
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
    throw ResourceException("Can't create a copy from the specified cursor");
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
