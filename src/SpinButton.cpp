// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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

#include "stdvaca.h"
#include "Vaca/SpinButton.h"
#include "Vaca/Application.h"
#include "Vaca/Debug.h"

using namespace Vaca;

SpinButton::SpinButton(Widget *parent, Style style)
  : Widget(UPDOWN_CLASS, parent, style)
{
}

Size SpinButton::preferredSize()
{
  Widget *buddy = getBuddy();

  if (buddy != NULL)
    return Size(17, buddy->preferredSize().h);
  else
    return Size(17, 17);
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

void SpinButton::getRange(int &minValue, int &maxValue)
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

int SpinButton::getPosition()
{
  BOOL error;
  int res;

  res = sendMessage(UDM_GETPOS32, 0, reinterpret_cast<LPARAM>(&error));

  return !error ? res: 0;
}

void SpinButton::setPosition(int posValue)
{
  sendMessage(UDM_SETPOS32, 0, posValue);
}

Widget *SpinButton::getBuddy()
{
  HWND hwndBuddy = reinterpret_cast<HWND>(sendMessage(UDM_GETBUDDY, 0, 0));
  if (hwndBuddy != NULL)
    return Widget::fromHwnd(hwndBuddy);
  else
    return NULL;
}

void SpinButton::setBuddy(Widget *buddy)
{
  sendMessage(UDM_SETBUDDY, reinterpret_cast<WPARAM>(buddy->getHwnd()), 0);
}

void SpinButton::onChange(SpinButtonEvent &ev)
{
  Change(ev);
}

// void SpinButton::onBeforeChange(SpinButtonEvent &ev)
// {
//   BeforeChange(ev);
// }

// void SpinButton::onAfterChange(SpinButtonEvent &ev)
// {
//   AfterChange(ev);
// }

// void SpinButton::onVScroll(int code, int pos/*, ScrollBar *scrollbar*/)
// {
//   if (code == TB_THUMBPOSITION) {
//     SpinButtonEvent ev(this, pos, 0, TopSide);
//     onAfterChange(ev);
//   }
// }

// void SpinButton::onHScroll(int code, int pos/*, ScrollBar *scrollbar*/)
// {
//   if (code == TB_THUMBPOSITION) {
//     SpinButtonEvent ev(this, pos, 0, LeftSide);
//     onAfterChange(ev);
//   }
// }

bool SpinButton::onNotify(LPNMHDR lpnmhdr, LRESULT &lResult)
{
  if (lpnmhdr->code == UDN_DELTAPOS) {
    LPNMUPDOWN lpnmud = reinterpret_cast<LPNMUPDOWN>(lpnmhdr);
    Side side;

    // horizontal
    if (isHorizontal()) {
      // strange? not, it's inverted
      if (lpnmud->iDelta < 0)
	side = RightSide;
      else
	side = LeftSide;
    }
    // vertical
    else {
      if (lpnmud->iDelta < 0)
	side = TopSide;
      else
	side = BottomSide;
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
