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

#include "Vaca/ComboBox.h"
#include "Vaca/Event.h"
#include "Vaca/Debug.h"
#include "Vaca/System.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/PreferredSizeEvent.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// ComboBox

ComboBox::ComboBox(Widget* parent, Style style)
  : Widget(WidgetClassName(WC_COMBOBOX), parent, style)
{
  setBgColor(System::getColor(COLOR_WINDOW));
}

ComboBox::~ComboBox()
{
}

/// @win32
///   It uses @msdn{CB_ADDSTRING} message.
/// @endwin32
/// 
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

/// @win32
///   It uses @msdn{CB_INSERTSTRING} message.
/// @endwin32
/// 
void ComboBox::insertItem(int itemIndex, const String& text)
{
  sendMessage(CB_INSERTSTRING, itemIndex, reinterpret_cast<LPARAM>(text.c_str()));
  updateMaxItemSize(text);
}

/// @win32
///   It uses @msdn{CB_DELETESTRING} message.
/// @endwin32
/// 
void ComboBox::removeItem(int itemIndex)
{
  sendMessage(CB_DELETESTRING, itemIndex, 0);
}

/// @win32
///   It uses @msdn{CB_RESETCONTENT)} message.
/// @endwin32
/// 
void ComboBox::removeAllItems()
{
  sendMessage(CB_RESETCONTENT, 0, 0);

  m_maxItemSize = Size(0, 0);
}

/// @win32
///   This event is generated when @msdn{CB_GETCOUNT} message is received.
/// @endwin32
/// 
int ComboBox::getItemCount()
{
  return sendMessage(CB_GETCOUNT, 0, 0);
}

/// @win32
///   It uses @msdn{CB_GETLBTEXT} and @msdn{CB_GETLBTEXTLEN} messages.
/// @endwin32
/// 
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

/// Returns the current selected item index. Returns -1
/// if there aren't selection at all.
///
/// @win32
///   It uses the @msdn{CB_GETCURSEL} message.
/// @endwin32
/// 
int ComboBox::getSelectedItem()
{
  int index = sendMessage(CB_GETCURSEL, 0, 0);
  if (index != CB_ERR && index >= 0)
    return index;
  else
    return index;
}

/// Changes the current selected item to the @a itemIndex only.
///
/// @win32
///   It uses the @msdn{CB_SETCURSEL} message.
/// @endwin32
/// 
void ComboBox::setSelectedItem(int itemIndex)
{
  sendMessage(CB_SETCURSEL, itemIndex, 0);
}

/// Selects the item which its text begins with @a firstText.
/// 
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
  return Rect(&rc);
}

void ComboBox::layout()
{
  Widget::layout();
  
  if ((getStyle().regular & 3) <= 1) { // CBS_SIMPLE = 1
    // do nothing
  }
  else {
    Rect rc = getBounds();
    Rect arc = getAbsoluteBounds();
    Rect wa = System::getWorkAreaBounds();
    int maxHeightTop = arc.y-wa.y;
    int maxHeightBottom = wa.h-(arc.y-wa.y+arc.h);
  
    setBounds(rc.x, rc.y, rc.w,
	      min_value(rc.h + getHeightForAllItems(),
			max_value(maxHeightTop,
				  maxHeightBottom)));
  }
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

/// When the user changes the current selected item.
///
/// @win32
/// It is called when CBN_SELCHANGE notification is received.
/// @endwin32
///
void ComboBox::onSelChange(Event& ev)
{
  SelChange(ev);
}

/// When the user changes the text of the ComboBox.
///
/// Only for combo-boxes with ComboBox::Styles::Editable.
///
/// @win32
/// It is called when CBN_EDITCHANGE notification is received.
/// @endwin32
/// 
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
