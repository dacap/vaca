// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/ButtonBase.h"
#include "Vaca/Debug.h"
#include "Vaca/Event.h"
#include "Vaca/System.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/PreferredSizeEvent.h"

using namespace Vaca;

ButtonBase::ButtonBase(Widget* parent, Style style)
  : Widget(WidgetClassName(WC_BUTTON), parent, style)
{
}

ButtonBase::ButtonBase(HWND handle)
  : Widget(handle)
{
}

ButtonBase::~ButtonBase()
{
}

/**
   Returns true if the button is selected or checked. It's useful to
   see if a CheckBox, RadioButton or ToggleButton is checked.

   @see setSelected
*/
bool ButtonBase::isSelected()
{
//   return (sendMessage(BM_GETSTATE, 0, 0) & (BST_CHECKED | BST_PUSHED)) != 0;
  return sendMessage(BM_GETCHECK, 0, 0) == BST_CHECKED;
}

/**
   Selects or deselects (checks or unchecks) the button depending of
   @a state parameter. The button is automatically redrawn, you don't
   need to call #invalidate().

   @param state
       true if you want to check the button, or false if you want
       to uncheck.

   @see isSelected
*/
void ButtonBase::setSelected(bool state)
{
  sendMessage(BM_SETCHECK, state ? BST_CHECKED: BST_UNCHECKED, 0);
}

#ifndef BCM_FIRST
#  define BCM_FIRST 0x1600
#  define BCM_GETIDEALSIZE BCM_FIRST+1
#endif

/**
   Returns the preferred size for the button.
*/
void ButtonBase::onPreferredSize(PreferredSizeEvent& ev)
{
  assert(::IsWindow(getHandle()));

  int style = getStyle().regular;
  Size sz;

  bool pushLike =
    ((style & 15) == BS_PUSHBUTTON) ||
    ((style & 15) == BS_DEFPUSHBUTTON) ||
    ((style & 15) == BS_USERBUTTON) ||
    ((style & 15) == BS_OWNERDRAW) ||
    ((style & BS_PUSHLIKE) != 0);

#if 0				// TODO
  SIZE size;

  // Button_GetIdealSize only in WinXP
  if (pushLike &&
      (System::isWinXP()) &&
      (sendMessage(BCM_GETIDEALSIZE, 0,
		   reinterpret_cast<LPARAM>(&size)) != FALSE)) {
    sz = Size(&size);
  }
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

      sz = Size(66, 0).createUnion(textSize+border);
    }
    // check-box
    else if ((style & 15) == BS_CHECKBOX ||
	     (style & 15) == BS_AUTOCHECKBOX ||
	     (style & 15) == BS_3STATE ||
	     (style & 15) == BS_AUTO3STATE) {
      sz = Size(textSize.h+6+textSize.w, textSize.h);
    }
    // radio
    else if ((style & 15) == BS_RADIOBUTTON ||
	     (style & 15) == BS_AUTORADIOBUTTON) {
      sz = Size(textSize.h+6+textSize.w, textSize.h);
    }
    // maybe a BS_GROUPBOX...
    else {
      // just returns the size of the text
      sz = textSize;
    }
#if 0
  }
#endif

  ev.setPreferredSize(sz);
}

/**
   Event fired when the user activates the button.

   The user can press and release a mouse button or press space-bar
   key to activate a button widget. For Button it is fired when the
   user push the button, for CheckBox and ToggleButton when the
   button-state is changed, and for RadioButton when a different
   option is selected.
*/
void ButtonBase::onClick(Event& ev)
{
  Click(ev);
}

/**
   Receives BN_CLICKED and fires onClick event.
*/
bool ButtonBase::onReflectedCommand(int id, int code, LRESULT& lResult)
{
  if (Widget::onReflectedCommand(id, code, lResult))
    return true;

  switch (code) {

    case BN_CLICKED: {
      Event ev(this);
      onClick(ev);
      break;
    }

  }
  return false;
}
