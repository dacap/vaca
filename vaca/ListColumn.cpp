// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/ListColumn.h"
#include "vaca/ListView.h"
#include "vaca/Debug.h"
#include "vaca/win32.h"

using namespace vaca;

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
