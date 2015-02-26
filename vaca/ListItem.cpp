// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/ListItem.h"
#include "vaca/ListView.h"
#include "vaca/Debug.h"
#include "vaca/win32.h"

using namespace vaca;

ListItem::ListItem(const String& text, int imageIndex)
{
  m_text.push_back(text);
  m_image = imageIndex;
  m_index = -1;
  m_owner = NULL;
}

ListItem::~ListItem()
{
  if (m_owner != NULL) {
    assert(m_index >= 0);

    // ListView_DeleteItem(m_owner->getHandle(), m_index);
  }
}

ListView* ListItem::getListView()
{
  return m_owner;
}

int ListItem::getIndex()
{
  return m_index;
}

String ListItem::getText(size_t columnIndex)
{
  assert(columnIndex >= 0);

  if (columnIndex < m_text.size())
    return m_text[columnIndex];
  else
    return L"";
}

int ListItem::getImage()
{
  return m_image;
}

/**
   @todo is it absolute or relative?
*/
Rect ListItem::getBounds() const
{
  assert(::IsWindow(m_owner->getHandle()));

  int code = LVIR_BOUNDS;
  RECT rc;
  LPRECT prc = &rc; // to avoid a warning with MinGW with the ListView_GetItemRect macro
  if (ListView_GetItemRect(m_owner->getHandle(), m_index, prc, code))
    return convert_to<Rect>(*prc);

  // empty rectangle
  return Rect();
}

void ListItem::setText(const String& text, size_t columnIndex)
{
  assert(columnIndex >= 0);

  if (columnIndex >= m_text.size())
    m_text.resize(columnIndex+1);

  m_text[columnIndex] = text;
}

void ListItem::setImage(int image)
{
  m_image = image;
  update();
}

bool ListItem::isSelected() const
{
  assert(m_owner != NULL);
  assert(::IsWindow(m_owner->getHandle()));

  return ListView_GetItemState(m_owner->getHandle(), m_index, LVIS_SELECTED) ? true: false;
}

void ListItem::setSelected(bool state)
{
  assert(m_owner != NULL);
  assert(::IsWindow(m_owner->getHandle()));

  ListView_SetItemState(m_owner->getHandle(), m_index,
			(state ? LVIS_SELECTED: 0), LVIS_SELECTED);
}

void ListItem::update()
{
  assert(m_owner != NULL);
  assert(::IsWindow(m_owner->getHandle()));

  ListView_Update(m_owner->getHandle(), m_index);
}

void ListItem::ensureVisible()
{
  assert(m_owner != NULL);
  assert(::IsWindow(m_owner->getHandle()));

  ListView_EnsureVisible(m_owner->getHandle(), m_index, false);
}

// ListItem* ListItem::fromIndex(HWND hwnd, int index)
// {
//   LVITEM lvi;
//   lvi.mask = LVIF_PARAM;
//   lvi.iItem = index;
//   lvi.iSubItem = 0;
//   lvi.lParam = NULL;

//   if (ListView_GetItem(hwnd, &lvi))
//     return reinterpret_cast<ListItem*>(lvi.lParam);
//   else
//     return NULL;
// }

/**
   Connects the item to the Win32 ListView.
*/
void ListItem::addToListView(ListView* listView)
{
  assert(m_owner == NULL);

  m_owner = listView;
//   if (listView != NULL) {
//     LVITEM lvi;

//     lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//     lvi.iItem = ListView_GetItemCount(listView->getHandle());
//     lvi.iSubItem = 0;
//     lvi.pszText = LPSTR_TEXTCALLBACK;
//     lvi.iImage = I_IMAGECALLBACK;
//     lvi.lParam = reinterpret_cast<LPARAM>(this);

//     m_index = ListView_InsertItem(listView->getHandle(), &lvi);
//   }
}

/**
   Disconnects the item from the Win32 ListView.
*/
void ListItem::removeFromListView()
{
  if (m_owner != NULL) {
    // assert(m_index >= 0);

    // ListView_DeleteItem(m_owner->getHandle(), m_index);

    // m_index = -1;
    m_owner = NULL;
  }

  // assert(m_index < 0);
}
