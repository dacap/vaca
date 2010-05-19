// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
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

#include "Vaca/ListColumn.h"
#include "Vaca/ListView.h"
#include "Vaca/Debug.h"
#include "Vaca/win32.h"

using namespace Vaca;

ListColumn::ListColumn(const String& text, TextAlign textAlign)
  : m_text(text)
  , m_textAlign(textAlign)
  , m_width(0)
{
  m_index = -1;
  m_owner = NULL;
}

ListColumn::~ListColumn()
{
  if (m_index >= 0)
    removeFromListView();
}

/**
   Returns the width (in pixels) of the column.

   @note Useful for ListView of report type (ListViewType#Report).
*/
int ListColumn::getWidth() const
{
  if (m_owner != NULL) {
    assert(::IsWindow(m_owner->getHandle()));
    assert(m_index >= 0);

    return ListView_GetColumnWidth(m_owner->getHandle(), m_index);
  }
  else
    return m_width;
}

/**
   Sets the @a width (in pixels) of the column.

   @note Useful for ListView of report type (ListViewType#Report).
*/
void ListColumn::setWidth(int width)
{
  if (m_owner != NULL) {
    assert(::IsWindow(m_owner->getHandle()));
    assert(m_index >= 0);

    ListView_SetColumnWidth(m_owner->getHandle(), m_index, width);
  }
  m_width = width;
}

/**
   Sets the width of the column to its preferred size.

   @param useHeader
      True means that in the preferred size must be included the
      header, not just the items' text.

   @note Useful for ListView of report type (ListViewType#Report).
*/
void ListColumn::setPreferredWidth(bool useHeader)
{
  if (m_owner != NULL) {
    assert(::IsWindow(m_owner->getHandle()));
    assert(m_index >= 0);

    ListView_SetColumnWidth(m_owner->getHandle(), m_index,
 			    useHeader ? LVSCW_AUTOSIZE_USEHEADER:
					LVSCW_AUTOSIZE);
  }
  else {
    m_width = 0;
  }
}

Rect ListColumn::getBounds() const
{
  if (m_owner != NULL) {
    assert(::IsWindow(m_owner->getHandle()));

    HWND hHeader = ListView_GetHeader(m_owner->getHandle());
    if (hHeader != NULL) {
      RECT rc;
      if (Header_GetItemRect(hHeader, m_index, &rc))
	return convert_to<Rect>(rc);
    }
  }

  // empty rectangle
  return Rect();
}

void ListColumn::addToListView(ListView* listView)
{
  assert(m_owner == NULL);

  m_owner = listView;
  if (listView != NULL) {
    LVCOLUMN lvc;

    lvc.mask = LVCF_FMT | LVCF_TEXT;
    lvc.fmt = 0
      | (m_textAlign == TextAlign::Left   ? LVCFMT_LEFT   : 0)
      | (m_textAlign == TextAlign::Center ? LVCFMT_CENTER : 0)
      | (m_textAlign == TextAlign::Right  ? LVCFMT_RIGHT  : 0)
      ;
    lvc.pszText = const_cast<LPTSTR>(m_text.c_str());

    HWND hHeader = ListView_GetHeader(listView->getHandle());
    m_index = Header_GetItemCount(hHeader);
    m_index = ListView_InsertColumn(listView->getHandle(), m_index, &lvc);

    if (m_width > 0)
      setWidth(m_width);
    else
      setPreferredWidth(true);
  }
  else
    m_index = -1;
}

void ListColumn::removeFromListView()
{
  assert(m_owner != NULL);
  assert(::IsWindow(m_owner->getHandle()));
  assert(m_index >= 0);

  ListView_DeleteColumn(m_owner->getHandle(), m_index);

  m_owner = NULL;
  m_index = -1;
}
