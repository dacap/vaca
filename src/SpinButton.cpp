// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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
// * Neither the name of the Vaca nor the names of its contributors
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

#include "Vaca/SpinButton.h"
#include "Vaca/Application.h"
#include "Vaca/Debug.h"
#include "Vaca/WidgetClass.h"

using namespace Vaca;

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
 * Returns the radix base used in the buddy widget (an Edit generally).
 */
int SpinButton::getBase()
{
  return sendMessage(UDM_GETBASE, 0, 0);
}

/**
 * Sets the radix base to be used in the buddy widget (an Edit
 * generally). This can be 10 for decimal or 16 for hexadecimal.
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

void SpinButton::onPreferredSize(Size& sz)
{
  Widget* buddy = getBuddy();

  if (buddy != NULL)
    sz = Size(17, buddy->getPreferredSize().h);
  else
    sz = Size(17, 17);
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

// void SpinButton::onScroll(Orientation orientation, int code)
// {
//   if (code == TB_THUMBPOSITION) {
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
