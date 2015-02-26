// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/ComboBox.h"
#include "vaca/Event.h"
#include "vaca/Debug.h"
#include "vaca/System.h"
#include "vaca/WidgetClass.h"
#include "vaca/PreferredSizeEvent.h"
#include "vaca/win32.h"

using namespace vaca;

// ======================================================================
// ComboBox

ComboBox::ComboBox(Widget* parent, Style style)
  : Widget(WidgetClassName(WC_COMBOBOX), parent, style)
{
  setBgColor(System::getColor(COLOR_WINDOW));
}

ComboBox::~ComboBox()
{
}

/**
   @win32
     It uses @msdn{CB_ADDSTRING} message.
   @endwin32
*/
int ComboBox::addItem(const String& text)
{
  int index = sendMessage(CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(text.c_str()));
  if (index == LB_ERR)
    return -1;
  else {
    updateMaxItemSize(text);
    return index;
  }
}

/**
   @win32
     It uses @msdn{CB_INSERTSTRING} message.
   @endwin32
*/
void ComboBox::insertItem(int itemIndex, const String& text)
{
  sendMessage(CB_INSERTSTRING, itemIndex, reinterpret_cast<LPARAM>(text.c_str()));
  updateMaxItemSize(text);
}

/**
   @win32
     It uses @msdn{CB_DELETESTRING} message.
   @endwin32
*/
void ComboBox::removeItem(int itemIndex)
{
  sendMessage(CB_DELETESTRING, itemIndex, 0);
}

/**
   @win32
     It uses @msdn{CB_RESETCONTENT)} message.
   @endwin32
*/
void ComboBox::removeAllItems()
{
  sendMessage(CB_RESETCONTENT, 0, 0);

  m_maxItemSize = Size(0, 0);
}

/**
   @win32
     This event is generated when @msdn{CB_GETCOUNT} message is received.
   @endwin32
*/
int ComboBox::getItemCount()
{
  return sendMessage(CB_GETCOUNT, 0, 0);
}

/**
   @win32
     It uses @msdn{CB_GETLBTEXT} and @msdn{CB_GETLBTEXTLEN} messages.
   @endwin32
*/
String ComboBox::getItemText(int itemIndex)
{
  int len = sendMessage(CB_GETLBTEXTLEN, itemIndex, 0);
  if (!len)
    return L"";
  else {
    Char* buf = new Char[len+1];
    sendMessage(CB_GETLBTEXT, itemIndex, reinterpret_cast<LPARAM>(buf));
    String str(buf);
    delete buf;
    return str;
  }
}

void ComboBox::setItemText(int itemIndex, const String& text)
{
  bool reselect = getSelectedItem() == itemIndex;

  removeItem(itemIndex);
  insertItem(itemIndex, text);

  if (reselect)
    setSelectedItem(itemIndex);
}

/**
   Returns the current selected item index. Returns -1
   if there aren't selection at all.

   @win32
     It uses the @msdn{CB_GETCURSEL} message.
   @endwin32
*/
int ComboBox::getSelectedItem()
{
  int index = sendMessage(CB_GETCURSEL, 0, 0);
  if (index != CB_ERR && index >= 0)
    return index;
  else
    return index;
}

/**
   Changes the current selected item to the @a itemIndex only.

   @win32
     It uses the @msdn{CB_SETCURSEL} message.
   @endwin32
*/
void ComboBox::setSelectedItem(int itemIndex)
{
  sendMessage(CB_SETCURSEL, itemIndex, 0);
}

/**
   Selects the item which its text begins with @a firstText.
*/
void ComboBox::setSelectedItem(const String& firstText)
{
  sendMessage(CB_SELECTSTRING,
	      static_cast<WPARAM>(-1),
	      reinterpret_cast<LPARAM>(firstText.c_str()));
}

void ComboBox::setDropDownVisibile(bool visible)
{
  sendMessage(CB_SHOWDROPDOWN, visible, 0);
}

bool ComboBox::isDropDownVisible()
{
  return sendMessage(CB_GETDROPPEDSTATE, 0, 0) ? true: false;
}

Rect ComboBox::getDropDownBounds()
{
  RECT rc;
  sendMessage(CB_GETDROPPEDCONTROLRECT, 0, reinterpret_cast<LPARAM>(&rc));
  return convert_to<Rect>(rc);
}

int ComboBox::getHeightForAllItems()
{
  // TODO CBS_OWNERDRAWVARIABLE, see the MSDN doc of CB_GETITEMHEIGHT
  int height = sendMessage(CB_GETITEMHEIGHT, 0, 0);
  return height*getItemCount()+2;
}

void ComboBox::updateMaxItemSize(const String& text)
{
  ScreenGraphics g;
  g.setFont(getFont());
  m_maxItemSize = m_maxItemSize.createUnion(g.measureString(text));
}

void ComboBox::onPreferredSize(PreferredSizeEvent& ev)
{
  Size sz(m_maxItemSize.w + (::GetSystemMetrics(SM_CXEDGE)*4 +
			     ::GetSystemMetrics(SM_CXVSCROLL)),
	  m_maxItemSize.h + (::GetSystemMetrics(SM_CYEDGE)*4));

  ev.setPreferredSize(sz);
}

void ComboBox::onLayout(LayoutEvent& ev)
{
  Widget::onLayout(ev);

  if ((getStyle().regular & 3) <= 1) { // CBS_SIMPLE = 1
    // do nothing
  }
  else {
    Rect rc = getBounds();
    Rect arc = getAbsoluteBounds();
    Rect wa = System::getWorkAreaBounds();
    int maxHeightTop = arc.y-wa.y;
    int maxHeightBottom = wa.h-(arc.y-wa.y+arc.h);

    // TODO setBounds or ev.setBounds
    setBounds(rc.x, rc.y, rc.w,
	      min_value(rc.h + getHeightForAllItems(),
			max_value(maxHeightTop,
				  maxHeightBottom)));
  }
}

/**
   When the user changes the current selected item.

   @win32
   It is called when CBN_SELCHANGE notification is received.
   @endwin32
*/
void ComboBox::onSelChange(Event& ev)
{
  SelChange(ev);
}

/**
   When the user changes the text of the ComboBox.

   Only for combo-boxes with ComboBox::Styles::Editable.

   @win32
   It is called when CBN_EDITCHANGE notification is received.
   @endwin32
*/
void ComboBox::onEditChange(Event& ev)
{
  EditChange(ev);
}

bool ComboBox::onReflectedCommand(int id, int code, LRESULT& lResult)
{
  if (Widget::onReflectedCommand(id, code, lResult))
    return true;

  switch (code) {

    case CBN_SELCHANGE: {
      Event ev(this);
      onSelChange(ev);
      return true;
    }

    case CBN_EDITCHANGE: {
      Event ev(this);
      onEditChange(ev);
      return true;
    }

  }

  return false;
}
