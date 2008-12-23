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

#include "Vaca/ListView.h"
#include "Vaca/System.h"
#include "Vaca/Debug.h"
#include "Vaca/ImageList.h"
#include "Vaca/WidgetClass.h"

using namespace Vaca;

/**
 * Creates a new ListView.
 *
 * It uses the LVS_SHAREIMAGELISTS to avoid destroying the image-list
 * that you specified using setImageList().
 */
ListView::ListView(Widget* parent, Style style)
  : Widget(WidgetClassName(WC_LISTVIEW), parent, style + Style(LVS_SHAREIMAGELISTS, 0))
{
  setBgColor(System::getColor(COLOR_WINDOW));
}

ListView::~ListView()
{
}

void ListView::setBgColor(const Color& color)
{
  assert(::IsWindow(getHandle()));

  Widget::setBgColor(color);
  ListView_SetBkColor(getHandle(), color.getColorRef());
}

Color ListView::getTextColor() const
{
  assert(::IsWindow(getHandle()));

  return Color(ListView_GetTextColor(getHandle()));
}

void ListView::setTextColor(const Color& color)
{
  assert(::IsWindow(getHandle()));

  ListView_SetTextColor(getHandle(), color.getColorRef());
}

Color ListView::getTextBgColor() const
{
  assert(::IsWindow(getHandle()));

  return Color(ListView_GetTextBkColor(getHandle()));
}

void ListView::setTextBgColor(const Color& color)
{
  assert(::IsWindow(getHandle()));

  ListView_SetTextBkColor(getHandle(), color.getColorRef());
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
}

void ListView::setNormalImageList(const ImageList& imageList)
{
  assert(::IsWindow(getHandle()));

  m_normalImageList = imageList;
  ListView_SetImageList(getHandle(), m_normalImageList.getHandle(), LVSIL_NORMAL);
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

int ListView::addColumn(const String& header, TextAlign textAlign)
{
  return insertColumn(getColumnCount(), header, textAlign);
}

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

/**
 * Removes the specified column @a columnIndex. 
 * 
 */
void ListView::removeColumn(int columnIndex)
{
  assert(::IsWindow(getHandle()));

  // MSDN says that column with columnIndex == 0 can't be deleted, but
  // I tested it on Win2K and all work just fine

  ListView_DeleteColumn(getHandle(), columnIndex);
}

/**
 * Returns the number of columns. Only for Report views.
 * 
 */
int ListView::getColumnCount()
{
  assert(::IsWindow(getHandle()));

  HWND hHeader = ListView_GetHeader(getHandle());
  if (hHeader != NULL)
    return Header_GetItemCount(hHeader);
  else
    return 0;
}

Rect ListView::getColumnBounds(int columnIndex)
{
  assert(::IsWindow(getHandle()));

  HWND hHeader = ListView_GetHeader(getHandle());
  if (hHeader != NULL) {
    RECT rc;
    if (Header_GetItemRect(hHeader, columnIndex, &rc))
      return Rect(&rc);
  }

  // empty rectangle
  return Rect();
}

/**
 * Returns the width (in pixels) of the specified @a columnIndex.
 * Only for Report views.
 * 
 */
int ListView::getColumnWidth(int columnIndex)
{
  assert(::IsWindow(getHandle()));

  return ListView_GetColumnWidth(getHandle(), columnIndex);
}

/**
 * Sets the @a width (in pixels) of the specified column @a columnIndex.
 * Only for Report views.
 * 
 */
void ListView::setColumnWidth(int columnIndex, int width)
{
  assert(::IsWindow(getHandle()));

  ListView_SetColumnWidth(getHandle(), columnIndex, width);
}

/**
 * Sets the width of the @a columnIndex to its preferred size. If @a useHeader
 * is true means that in the preferred size must be included the header, not just
 * the items' text. Only for Report views.
 * 
 */
void ListView::setPreferredColumnWidth(int columnIndex, bool useHeader)
{
  assert(::IsWindow(getHandle()));

  ListView_SetColumnWidth(getHandle(),
			  columnIndex,
			  useHeader ? LVSCW_AUTOSIZE_USEHEADER:
				      LVSCW_AUTOSIZE);
}

/**
 * Inserts a new item in the last position.
 * 
 */
int ListView::addItem(const String& text, int imageIndex)
{
  return insertItem(getItemCount(), text, imageIndex);
}

/**
 * Inserts a new item in the @a itemIndex position.
 *
 * @return The index (generally @a itemIndex).
 * 
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

/**
 * Removes one item from the list.
 * 
 */
void ListView::removeItem(int itemIndex)
{
  assert(::IsWindow(getHandle()));

  ListView_DeleteItem(getHandle(), itemIndex);
}

/**
 * Clears the list.
 * 
 */
void ListView::removeAllItems()
{
  assert(::IsWindow(getHandle()));

  ListView_DeleteAllItems(getHandle());
}

/**
 * Returns the number of items.
 * 
 */
int ListView::getItemCount()
{
  assert(::IsWindow(getHandle()));
  
  return ListView_GetItemCount(getHandle());
}

/**
 * TODO it's absolute or relative?
 * 
 */
Rect ListView::getItemBounds(int itemIndex, int code)
{
  assert(::IsWindow(getHandle()));

  RECT rc;
  if (ListView_GetItemRect(getHandle(), itemIndex, &rc, code))
    return Rect(&rc);

  // empty rectangle
  return Rect();
}

/**
 * Returns the text of the specified item.
 * 
 */
String ListView::getItemText(int itemIndex, int columnIndex)
{
  assert(::IsWindow(getHandle()));

  int size = 4096;		// TODO how to get the text length of
				// the item?
  LPTSTR lpstr = new TCHAR[size];

  ListView_GetItemText(getHandle(), itemIndex, columnIndex, lpstr, size);

  String res(lpstr);
  delete lpstr;
  return res;
}

/**
 * Changes the text of the speficied item. The @a columnIndex can be
 * used to change the text of the different columns in a Report view.
 * 
 */
void ListView::setItemText(int itemIndex, const String& text, int columnIndex)
{
  assert(::IsWindow(getHandle()));

  ListView_SetItemText(getHandle(),
		       itemIndex,
		       columnIndex,
		       const_cast<LPTSTR>(text.c_str()));
}

/**
 * Creates an TextEdit control to edit the specified item.
 */
void ListView::editItemText(int itemIndex)
{
  assert(::IsWindow(getHandle()));

  // TODO it returns an HWND for the TextEdit control
  ListView_EditLabel(getHandle(), itemIndex);
}

bool ListView::isItemSelected(int itemIndex)
{
  assert(::IsWindow(getHandle()));

  return ListView_GetItemState(getHandle(), itemIndex, LVIS_SELECTED) ? true: false;
}

/**
 * Returns the index of the item that has the focus
 * (LVM_GETSELECTIONMARK).
 */
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

    case LVN_ITEMCHANGING: {
      LPNMLISTVIEW lpnmlv = reinterpret_cast<LPNMLISTVIEW>(lpnmhdr);
      ListViewEvent ev(this, lpnmlv->iItem, lpnmlv->iSubItem);
      onBeforeSelect(ev);
      lResult = ev.isCanceled() ? TRUE: FALSE;
      return true;
    }

    case LVN_ITEMCHANGED: {
      LPNMLISTVIEW lpnmlv = reinterpret_cast<LPNMLISTVIEW>(lpnmhdr);
      ListViewEvent ev(this, lpnmlv->iItem, lpnmlv->iSubItem);
      onAfterSelect(ev);
      break;
    }

    case LVN_COLUMNCLICK: {
      LPNMLISTVIEW lpnmlv = reinterpret_cast<LPNMLISTVIEW>(lpnmhdr);
      ListViewEvent ev(this, lpnmlv->iItem, lpnmlv->iSubItem);
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
