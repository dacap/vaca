// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/ListView.h"
#include "vaca/System.h"
#include "vaca/Debug.h"
#include "vaca/ImageList.h"
#include "vaca/WidgetClass.h"
#include "vaca/ListItem.h"
#include "vaca/ListColumn.h"
#include "vaca/win32.h"

using namespace vaca;

#ifndef LVS_EX_DOUBLEBUFFER
#define LVS_EX_DOUBLEBUFFER 0x00010000
#endif

/**
   Creates a new ListView.

   @win32
     It uses the @msdn{LVS_SHAREIMAGELISTS} to avoid destroying the image-list
     that you specified using #setImageList.
   @endwin32
*/
ListView::ListView(Widget* parent, Style style)
  : Widget(WidgetClassName(WC_LISTVIEW), parent,
	   style | Style(LVS_SHAREIMAGELISTS | LVS_OWNERDATA, 0))
{
  setBgColor(System::getColor(COLOR_WINDOW));

  ListView_SetExtendedListViewStyleEx(getHandle(),
	LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
}

ListView::~ListView()
{
  removeAllItems();
  removeAllColumns();
}

void ListView::setBgColor(const Color& color)
{
  assert(::IsWindow(getHandle()));

  Widget::setBgColor(color);
  ListView_SetBkColor(getHandle(), convert_to<COLORREF>(color));
}

Color ListView::getTextColor() const
{
  assert(::IsWindow(getHandle()));

  return convert_to<Color>(ListView_GetTextColor(getHandle()));
}

void ListView::setTextColor(const Color& color)
{
  assert(::IsWindow(getHandle()));

  ListView_SetTextColor(getHandle(), convert_to<COLORREF>(color));
}

Color ListView::getTextBgColor() const
{
  assert(::IsWindow(getHandle()));

  return convert_to<Color>(ListView_GetTextBkColor(getHandle()));
}

void ListView::setTextBgColor(const Color& color)
{
  assert(::IsWindow(getHandle()));

  ListView_SetTextBkColor(getHandle(), convert_to<COLORREF>(color));
}

ListViewType ListView::getType() const
{
  int style = getStyle().regular & LVS_TYPEMASK;

  switch (style) {
    case LVS_ICON:      return ListViewType::Icon;
    case LVS_REPORT:    return ListViewType::Report;
    case LVS_SMALLICON: return ListViewType::SmallIcon;
    case LVS_LIST:      return ListViewType::List;
  }

  // impossible
  assert(false);
  return ListViewType::List;
}

void ListView::setType(ListViewType type)
{
  int style = 0;

  switch (type) {
    case ListViewType::Icon:      style = LVS_ICON;      break;
    case ListViewType::Report:    style = LVS_REPORT;    break;
    case ListViewType::SmallIcon: style = LVS_SMALLICON; break;
    case ListViewType::List:      style = LVS_LIST;      break;
  }

  setStyle(getStyle()
	   - Style(LVS_TYPEMASK, 0)
	   + Style(style, 0));

  // Reconfigure columns
  int count = getColumnCount();
  for (int i=0; i<count; ++i) {
    ListColumn* column = getColumn(i);
    column->setWidth(column->m_width);
  }
}

void ListView::setImageList(const ImageList& imageList)
{
  assert(::IsWindow(getHandle()));

  m_imageList = imageList;
  ListView_SetImageList(getHandle(), m_imageList.getHandle(), LVSIL_NORMAL);
}

void ListView::setSmallImageList(const ImageList& imageList)
{
  assert(::IsWindow(getHandle()));

  m_smallImageList = imageList;
  ListView_SetImageList(getHandle(), m_smallImageList.getHandle(), LVSIL_SMALL);
}

void ListView::setStateImageList(const ImageList& imageList)
{
  assert(::IsWindow(getHandle()));

  m_stateImageList = imageList;
  ListView_SetImageList(getHandle(), m_stateImageList.getHandle(), LVSIL_STATE);
}

ImageList ListView::getImageList() const
{
  return m_imageList;
}

ImageList ListView::getSmallImageList() const
{
  return m_smallImageList;
}

ImageList ListView::getStateImageList() const
{
  return m_stateImageList;
}

int ListView::addColumn(ListColumn* column)
{
  assert(::IsWindow(getHandle()));
  assert(column != NULL);

  column->addToListView(this);

  m_columns.push_back(column);

  return column->m_index;
}

#if 0
int ListView::insertColumn(int columnIndex, const String& header, TextAlign textAlign)
{
  assert(::IsWindow(getHandle()));

  bool withDummyColumn = false;

  if (columnIndex == 0 && textAlign != TextAlign::Left) {
    // the first column can't have a textAlign != LeftAlign (Win32
    // limitation), so we can use a dummy-column (MSDN solution)
    insertDummyColumn();
    ++columnIndex;

    withDummyColumn = true;
  }

  LVCOLUMN lvc;

  lvc.mask = LVCF_FMT | LVCF_TEXT;
  lvc.fmt = 0
    | (textAlign == TextAlign::Left   ? LVCFMT_LEFT   : 0)
    | (textAlign == TextAlign::Center ? LVCFMT_CENTER : 0)
    | (textAlign == TextAlign::Right  ? LVCFMT_RIGHT  : 0)
    ;
  lvc.pszText = const_cast<LPTSTR>(header.c_str());

  ListView_InsertColumn(getHandle(), columnIndex, &lvc);

  if (withDummyColumn) {
    removeDummyColumn();
    --columnIndex;
  }

  setPreferredColumnWidth(columnIndex, true);

  return columnIndex;
}
#endif

/**
   Removes the specified column.
*/
void ListView::removeColumn(ListColumn* column)
{
  assert(column != NULL);
  assert(column->m_owner == this);

  column->removeFromListView();
}

void ListView::removeAllColumns()
{
  assert(::IsWindow(getHandle()));

  // Break relation-ship between ListColumns and ListView
  int count = getColumnCount();
  for (int i=count-1; i>=0; --i) {
    ListColumn* column = getColumn(i);
    delete column;
  }
  m_columns.clear();
}

/**
   Returns the number of columns. Only for Report views.
*/
int ListView::getColumnCount() const
{
  return m_columns.size();
}

ListColumn* ListView::getColumn(size_t columnIndex) const
{
  assert(columnIndex >= 0 && columnIndex < m_columns.size());

  return m_columns[columnIndex];
}

/**
   Inserts a new item in the last position.

   @param item
     The item to be append in the list, this item will be
     deleted automatically in ListView destruction. You can
     avoid the deletion of the item if you call #removeItem
     before ListView destructor.

   @see #removeItem
*/
int ListView::addItem(ListItem* item)
{
  assert(::IsWindow(getHandle()));
  assert(item != NULL);

  item->addToListView(this);
  item->m_index = m_items.size();

  m_items.push_back(item);

  ListView_SetItemCountEx(getHandle(), m_items.size(), LVSICF_NOINVALIDATEALL);

  return item->m_index;
}

#if 0
/**
   Inserts a new item in the @a itemIndex position.

   @return The index (generally @a itemIndex).
*/
int ListView::insertItem(int itemIndex, const String& text, int imageIndex)
{
  assert(::IsWindow(getHandle()));

  LVITEM lvi;

  lvi.mask = LVIF_TEXT | (imageIndex >= 0 ? LVIF_IMAGE: 0);
  lvi.iItem = itemIndex;
  lvi.iSubItem = 0;
  lvi.pszText = const_cast<LPTSTR>(text.c_str());
  lvi.iImage = imageIndex;

  return ListView_InsertItem(getHandle(), &lvi);
}
#endif

/**
   Removes one item from the list.

   @param item
     The item to be removed from the list. This routine does not
     delete the item, so you should delete it.
*/
void ListView::removeItem(ListItem* item)
{
  assert(::IsWindow(getHandle()));
  assert(item != NULL);

  item->removeFromListView();

  remove_from_container(m_items, item);

  ListView_SetItemCountEx(getHandle(), m_items.size(), LVSICF_NOINVALIDATEALL);
}

/**
   Clears the list.

   Removes and @b deletes all items in the list.
*/
void ListView::removeAllItems()
{
  assert(::IsWindow(getHandle()));

  // Break relation-ship between ListItems and ListView
  int count = getItemCount();
  for (int i=0; i<count; ++i) {
    ListItem* item = getItem(i);
    delete item;
  }

  m_items.clear();
  ListView_SetItemCountEx(getHandle(), 0, 0);
}

/**
   Returns the number of items.
*/
int ListView::getItemCount() const
{
  return m_items.size();
}

/**
   Returns the ListItem in the given position.

   @param index
     Zero-based index of the item
*/
ListItem* ListView::getItem(size_t index) const
{
  assert(index >= 0 && index < m_items.size());

  return m_items[index];
}

#if 0
/**
   Creates an TextEdit control to edit the specified item.
*/
void ListView::editItemText(int itemIndex)
{
  assert(::IsWindow(getHandle()));

  // TODO it returns an HWND for the TextEdit control
  ListView_EditLabel(getHandle(), itemIndex);
}
#endif

int ListView::getSelectedItemCount() const
{
  assert(::IsWindow(getHandle()));

  return (int)ListView_GetSelectedCount(getHandle());
}

// Returns the index of the item that has the focus
// (LVM_GETSELECTIONMARK).
//
// int ListView::getCurrentItem()
// {
//   assert(::IsWindow(getHandle()));

//   return ::ListView_GetSelectionMark(getHandle());
// }

// static int CALLBACK CompareListViewItems(LPARAM lParam1,
// 					 LPARAM lParam2,
// 					 LPARAM lParamSort)
// {
//   std::less<ListItem> *functor =
//     reinterpret_cast<std::less<ListItem> *>(lParamSort);
//   bool res;
//   res = (*functor)(*reinterpret_cast<ListItem*>(lParam1),
// 		   *reinterpret_cast<ListItem*>(lParam2));
//   return res ? -1: 1;
// }

// void ListView::sortItems(std::less<ListItem> functor)
// {
//   assert(::IsWindow(getHandle()));

//   ListView_SortItems(getHandle(),
// 		     CompareListViewItems,
// 		     reinterpret_cast<LPARAM>(&functor));
// }

void ListView::onResize(ResizeEvent& ev)
{
  invalidate(false);
  Widget::onResize(ev);
}

void ListView::onBeforeSelect(ListViewEvent& ev)
{
  BeforeSelect(ev);
}

void ListView::onAfterSelect(ListViewEvent& ev)
{
  AfterSelect(ev);
}

void ListView::onColumnClick(ListViewEvent& ev)
{
  ColumnClick(ev);
}

bool ListView::onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult)
{
  if (Widget::onReflectedNotify(lpnmhdr, lResult))
    return true;

  switch (lpnmhdr->code) {

    case LVN_GETDISPINFO: {
      NMLVDISPINFO* lplvdi = reinterpret_cast<NMLVDISPINFO*>(lpnmhdr);
      size_t i = lplvdi->item.iItem;

      assert(i >= 0 && i < m_items.size());

      ListItem* item = m_items[i];
      assert(item != NULL);

      if (lplvdi->item.mask & LVIF_TEXT) {
      	m_tmpBuffer = item->getText(lplvdi->item.iSubItem);
      	lplvdi->item.pszText = const_cast<LPTSTR>(m_tmpBuffer.c_str());
      }

      if (lplvdi->item.mask & LVIF_IMAGE)
      	lplvdi->item.iImage = item->getImage();

      // if (lplvdi->item.mask & TVIF_STATE)
      // 	lptvdi->item.iImage = node->getImage();

      return true;
    }

    case LVN_ITEMCHANGING: {
      LPNMLISTVIEW lpnmlv = reinterpret_cast<LPNMLISTVIEW>(lpnmhdr);
      size_t i = lpnmlv->iItem;
      ListItem* item;

      if (i >= 0 && i < m_items.size())
	item = m_items[i];
      else
	item = NULL;

      ListViewEvent ev(this, item, NULL);
      onBeforeSelect(ev);
      lResult = ev.isCanceled() ? TRUE: FALSE;
      return true;
    }

    case LVN_ITEMCHANGED: {
      LPNMLISTVIEW lpnmlv = reinterpret_cast<LPNMLISTVIEW>(lpnmhdr);
      size_t i = lpnmlv->iItem;
      ListItem* item;

      if (i >= 0 && i < m_items.size())
	item = m_items[i];
      else
	item = NULL;

      ListViewEvent ev(this, item, NULL);
      onAfterSelect(ev);
      break;
    }

    case LVN_COLUMNCLICK: {
      LPNMLISTVIEW lpnmlv = reinterpret_cast<LPNMLISTVIEW>(lpnmhdr);
      size_t i = lpnmlv->iSubItem;
      ListColumn* column;

      if (i >= 0 && i < m_columns.size())
	column = m_columns[i];
      else
	column = NULL;

      ListViewEvent ev(this, NULL, column);
      onColumnClick(ev);
      break;
    }

  }

  return false;
}

void ListView::insertDummyColumn()
{
  LVCOLUMN lvc;
  lvc.mask = LVCF_WIDTH;
  lvc.cx = 0;
  ListView_InsertColumn(getHandle(), 0, &lvc);
}

void ListView::removeDummyColumn()
{
  ListView_DeleteColumn(getHandle(), 0);
}
