// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

#include "Vaca/ListItem.h"
#include "Vaca/ListView.h"
#include "Vaca/Debug.h"

using namespace Vaca;

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
    return Rect(prc);

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
