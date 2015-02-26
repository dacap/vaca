// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/ListBox.h"
#include "vaca/Event.h"
#include "vaca/Debug.h"
#include "vaca/System.h"
#include "vaca/WidgetClass.h"
#include "vaca/PreferredSizeEvent.h"
#include "vaca/win32.h"

using namespace vaca;

// ======================================================================
// ListBox

/**
   Creates a ListBox.

   The default background color for a ListBox is System::getColor(COLOR_WINDOW).

   @param parent The parent Widget.
   @param style Add the LBS_MULTIPLESEL if you want a multiselection ListBox.
*/
ListBox::ListBox(Widget* parent, Style style)
  : Widget(WidgetClassName(WC_LISTBOX), parent, style)
{
  setBgColor(System::getColor(COLOR_WINDOW));
}

ListBox::~ListBox()
{
}

/**
   (LB_ADDSTRING)
*/
int ListBox::addItem(const String& text)
{
  int index = sendMessage(LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(text.c_str()));
  if (index == LB_ERR)
    return -1;
  else
    return index;
}

/**
   (LB_INSERTSTRING)

*/
void ListBox::insertItem(int itemIndex, const String& text)
{
  sendMessage(LB_INSERTSTRING, itemIndex, reinterpret_cast<LPARAM>(text.c_str()));
}

void ListBox::removeItem(int itemIndex)
{
  sendMessage(LB_DELETESTRING, itemIndex, 0);
}

/**
   (LB_GETCOUNT)
*/
int ListBox::getItemCount()
{
  return sendMessage(LB_GETCOUNT, 0, 0);
}

/**
   Returns the rectangle that the item @a itemIndex uses (LB_GETITEMRECT).

   @a itemIndex is zero-based.
*/
Rect ListBox::getItemBounds(int itemIndex)
{
  RECT rc;
  sendMessage(LB_GETITEMRECT, itemIndex, reinterpret_cast<LPARAM>(&rc));
  return convert_to<Rect>(rc);
}

/**
   (LB_GETTEXT and LB_GETTEXTLEN)
*/
String ListBox::getItemText(int itemIndex)
{
  int len = sendMessage(LB_GETTEXTLEN, itemIndex, 0);
  if (!len)
    return L"";
  else {
    Char* buf = new Char[len+1];
    sendMessage(LB_GETTEXT, itemIndex, reinterpret_cast<LPARAM>(buf));
    String str(buf);
    delete buf;
    return str;
  }
}

void ListBox::setItemText(int itemIndex, const String& text)
{
  bool reselect = getSelectedItem() == itemIndex;

  removeItem(itemIndex);
  insertItem(itemIndex, text);

  if (reselect)
    setSelectedItem(itemIndex);
}

/**
   Returns the current selected item index (LB_GETCURSEL). Returns -1
   if there aren't selection at all.
*/
int ListBox::getSelectedItem()
{
  int index = sendMessage(LB_GETCURSEL, 0, 0);
  if (index != LB_ERR && index >= 0)
    return index;
  else
    return index;
}

/**
   Changes the current selected item to the @a itemIndex only (LB_SETCURSEL).
*/
void ListBox::setSelectedItem(int itemIndex)
{
  sendMessage(LB_SETCURSEL, itemIndex, 0);
}

/**
   Returns the set of selected items (it's useful for ListBoxes with
   the LBS_MULTIPLESEL style).
*/
std::vector<int> ListBox::getSelectedItems()
{
  size_t count = sendMessage(LB_GETSELCOUNT, 0, 0);
  if (count > 0) {
    std::vector<int> items(count);
    sendMessage(LB_GETSELITEMS, count, (LPARAM)(*items.begin()));
    return items;
  }
  else
    return std::vector<int>();
}

void ListBox::onPreferredSize(PreferredSizeEvent& ev)
{
  // TODO HTHEME stuff
  Size sz(2*::GetSystemMetrics(SM_CXEDGE),
	  2*::GetSystemMetrics(SM_CYEDGE));

  int i, n = getItemCount();
  Rect rc;

  for (i=0; i<n; ++i) {
    rc = getItemBounds(i);
    sz = Size(max_value(sz.w, rc.w), sz.h+rc.h);
  }

  ev.setPreferredSize(sz);
}

/**
   When the user press double-click in some item (LBN_DBLCLK).
*/
void ListBox::onItemDoubleClick(Event& ev)
{
  ItemDoubleClick(ev);
}

/**
   When the user changes the current selected item (LBN_SELCHANGE).
*/
void ListBox::onSelChange(Event& ev)
{
  SelChange(ev);
}

bool ListBox::onReflectedCommand(int id, int code, LRESULT& lResult)
{
  if (Widget::onReflectedCommand(id, code, lResult))
    return true;

  switch (code) {

    case LBN_DBLCLK: {
      Event ev(this);
      onItemDoubleClick(ev);
      return true;
    }

    case LBN_SELCHANGE: {
      Event ev(this);
      onSelChange(ev);
      return true;
    }

  }

  return false;
}

// ======================================================================
// DragListBox

DragListBox::DragListBox(Widget* parent, Style style)
  : ListBox(parent, style)
{
  BOOL res;

  res = MakeDragList(getHandle());

  assert(res != FALSE);	// TODO throw exception
}
