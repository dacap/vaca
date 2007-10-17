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
#include "Vaca/ButtonBase.h"
#include "Vaca/Debug.h"
#include "Vaca/Event.h"
#include "Vaca/System.h"

using namespace Vaca;

ButtonBase::ButtonBase(Widget *parent, Style style)
  : Widget(WC_BUTTON, parent, style)
{
}

ButtonBase::~ButtonBase()
{
}

bool ButtonBase::isSelected()
{
//   return (sendMessage(BM_GETSTATE, 0, 0) & (BST_CHECKED | BST_PUSHED)) != 0;
  return sendMessage(BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void ButtonBase::setSelected(bool state)
{
  sendMessage(BM_SETCHECK, state ? BST_CHECKED: BST_UNCHECKED, 0);
}

#ifndef BCM_FIRST
#  define BCM_FIRST 0x1600
#  define BCM_GETIDEALSIZE BCM_FIRST+1
#endif

/**
 * Returns the preferred size for the button.
 */
Size ButtonBase::preferredSize()
{
  assert(getHWND() != NULL);

  int style = getStyle().regular;

  bool pushLike =
    ((style & 15) == BS_PUSHBUTTON) ||
    ((style & 15) == BS_DEFPUSHBUTTON) ||
    ((style & 15) == BS_USERBUTTON) ||
    ((style & 15) == BS_OWNERDRAW) ||
    ((style & BS_PUSHLIKE) != 0);

  SIZE size;

#if 0				// TODO
  // Button_GetIdealSize only in WinXP
  if (pushLike &&
      (System::isWinXP()) &&
      (sendMessage(BCM_GETIDEALSIZE, 0,
		   reinterpret_cast<LPARAM>(&size)) != FALSE))
    return Size(&size);
  // ...well, we must to do this by hand...
  else {
#endif
    // first of all, obtain the text's size
    Size textSize;
    {
      ScreenGraphics g;
      g.setFont(getFont());
      textSize = g.measureString(getText());
    }

    // push-like
    if (pushLike) {
      Size border(6, 6);

      // has 3d borders?
      if ((style & BS_FLAT) == 0)
	border += Size(GetSystemMetrics(SM_CXEDGE),
		       GetSystemMetrics(SM_CYEDGE))*2;

      return Size(66, 0).createUnion(textSize+border);
    }
    // check-box
    else if ((style & 15) == BS_CHECKBOX ||
	     (style & 15) == BS_AUTOCHECKBOX ||
	     (style & 15) == BS_3STATE ||
	     (style & 15) == BS_AUTO3STATE) {
      return Size(textSize.h+6+textSize.w, textSize.h);
    }
    // radio
    else if ((style & 15) == BS_RADIOBUTTON ||
	     (style & 15) == BS_AUTORADIOBUTTON) {
      return Size(textSize.h+6+textSize.w, textSize.h);
    }

    // maybe a BS_GROUPBOX...
    // just returns the size of the text
    return textSize;
#if 0
  }
#endif
}

/**
 * Event fired when you should take some responsability. For Button
 * it's fired when the user push the button, for CheckBox and
 * ToggleButton when the button-state is changed, and for RadioButton
 * when one option is selected.
 */
void ButtonBase::onAction(Event &ev)
{
  Action(ev);
}

/**
 * Catchs BN_CLICKED to fire onAction event.
 */
bool ButtonBase::onCommand(int id, int code, LRESULT &lResult)
{
  if (Widget::onCommand(id, code, lResult))
    return true;

  switch (code) {

    case BN_CLICKED: {
      Event ev(this);
      onAction(ev);
      break;
    }

  }
  return false;
}