// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/SpinButton.h"
#include "vaca/Application.h"
#include "vaca/Debug.h"
#include "vaca/WidgetClass.h"
#include "vaca/PreferredSizeEvent.h"

using namespace vaca;

SpinButton::SpinButton(Widget* parent, Style style)
  : Widget(WidgetClassName(UPDOWN_CLASS), parent, style)
{
  // default values
  setRange(0, 100);
  setValue(0);
}

SpinButton::SpinButton(int minValue, int maxValue, int value,
		       Widget* parent, Style style)
  : Widget(WidgetClassName(UPDOWN_CLASS), parent, style)
{
  assert(minValue <= value);
  assert(value <= maxValue);

  setRange(minValue, maxValue);
  setValue(value);
}

SpinButton::~SpinButton()
{
}

bool SpinButton::isHorizontal()
{
  return ((getStyle().regular & UDS_HORZ) == UDS_HORZ);
}

bool SpinButton::isVertical()
{
  return ((getStyle().regular & UDS_HORZ) == 0);
}

int SpinButton::getMinimum()
{
  INT minValue;

  sendMessage(UDM_GETRANGE32, reinterpret_cast<WPARAM>(&minValue), 0);

  return minValue;
}

int SpinButton::getMaximum()
{
  INT maxValue;

  sendMessage(UDM_GETRANGE32, 0, reinterpret_cast<LPARAM>(&maxValue));

  return maxValue;
}

void SpinButton::getRange(int& minValue, int& maxValue)
{
  LPINT lpMinValue = &minValue;
  LPINT lpMaxValue = &minValue;

  sendMessage(UDM_GETRANGE32,
	      reinterpret_cast<WPARAM>(lpMinValue),
	      reinterpret_cast<LPARAM>(lpMaxValue));
}

void SpinButton::setRange(int minValue, int maxValue)
{
  sendMessage(UDM_SETRANGE32, minValue, maxValue);
}

int SpinButton::getValue()
{
  BOOL error;
  int res;

  res = sendMessage(UDM_GETPOS32, 0, reinterpret_cast<LPARAM>(&error));

  return !error ? res: 0;
}

void SpinButton::setValue(int posValue)
{
  sendMessage(UDM_SETPOS32, 0, posValue);
}

/**
   Returns the radix base used in the buddy widget (an TextEdit generally).
*/
int SpinButton::getBase()
{
  return sendMessage(UDM_GETBASE, 0, 0);
}

/**
   Sets the radix base to be used in the buddy widget (an TextEdit
   generally). This can be 10 for decimal or 16 for hexadecimal.
*/
void SpinButton::setBase(int base)
{
  sendMessage(UDM_SETBASE, base, 0);
}

Widget* SpinButton::getBuddy()
{
  HWND hwndBuddy = reinterpret_cast<HWND>(sendMessage(UDM_GETBUDDY, 0, 0));
  if (hwndBuddy != NULL)
    return Widget::fromHandle(hwndBuddy);
  else
    return NULL;
}

void SpinButton::setBuddy(Widget* buddy)
{
  sendMessage(UDM_SETBUDDY, reinterpret_cast<WPARAM>(buddy->getHandle()), 0);
}

void SpinButton::onPreferredSize(PreferredSizeEvent& ev)
{
  Widget* buddy = getBuddy();
  Size sz;

  if (buddy != NULL)
    sz = Size(17, buddy->getPreferredSize().h);
  else
    sz = Size(17, 17);

  ev.setPreferredSize(sz);
}

void SpinButton::onChange(SpinButtonEvent& ev)
{
  Change(ev);
}

// void SpinButton::onBeforeChange(SpinButtonEvent& ev)
// {
//   BeforeChange(ev);
// }

// void SpinButton::onAfterChange(SpinButtonEvent& ev)
// {
//   AfterChange(ev);
// }

// void SpinButton::onScroll(ScrollEvent& ev)
// {
//   if (ev.getRequest() == ScrollRequest::BoxPosition) {
//     SpinButtonEvent ev(this, pos, 0, TopSide);
//     onAfterChange(ev);
//   }
// }

bool SpinButton::onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult)
{
  if (Widget::onReflectedNotify(lpnmhdr, lResult))
    return true;

  if (lpnmhdr->code == UDN_DELTAPOS) {
    LPNMUPDOWN lpnmud = reinterpret_cast<LPNMUPDOWN>(lpnmhdr);
    Side side;

    // horizontal
    if (isHorizontal()) {
      // strange? not, it's inverted
      if (lpnmud->iDelta < 0)
	side = Side::Right;
      else
	side = Side::Left;
    }
    // vertical
    else {
      if (lpnmud->iDelta < 0)
	side = Side::Top;
      else
	side = Side::Bottom;
    }

    SpinButtonEvent ev(this,
		       lpnmud->iPos,
		       lpnmud->iDelta,
		       side);
//     onBeforeChange(ev);
    onChange(ev);

    if (ev.isCanceled())
      lResult = TRUE;		// prevent the change
    else
      lResult = FALSE;		// allow the change

    return true;
  }
  else
    return false;
}
