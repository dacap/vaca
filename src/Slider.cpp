// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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
#include "Vaca/Slider.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/Event.hpp"
#include "Vaca/WidgetClass.hpp"

using namespace Vaca;

Slider::Slider(Widget* parent, Style style)
  : Widget(WidgetClassName(TRACKBAR_CLASS), parent, style)
{
}

Slider::Slider(int minValue, int maxValue, int value,
	       Widget* parent, Style style)
  : Widget(WidgetClassName(TRACKBAR_CLASS), parent, style)
{
  assert(minValue <= value);
  assert(value <= maxValue);

  setRange(minValue, maxValue);
  setValue(value);
}

Slider::~Slider()
{
}

Orientation Slider::getOrientation()
{
  if ((getStyle().regular & TBS_VERT) == TBS_VERT)
    return Orientation::Vertical;
  else
    return Orientation::Horizontal;
}

void Slider::setOrientation(Orientation orientation)
{
  removeStyle(Style(TBS_HORZ | TBS_VERT, 0));
  addStyle(Style(orientation == Orientation::Vertical ? TBS_VERT:
							TBS_HORZ, 0));
}

// void Slider::hideTickMarks()
// {
//   addStyle(Style(TBS_NOTICKS, 0));
// }

// void Slider::setTickMarksSide(TypeSide side)
// {
//   removeStyle(Style(TBS_NOTICKS | TBS_BOTH, 0));
  
//   // horizontal
//   if (getOrientation() == Orientation::Horizontal) {
//     assert(side == TopSide || side == BottomSide);
    
//     if (side == TopSide)
//       addStyle(Style(TBS_TOP, 0));
//     else if (side == BottomSide)
//       addStyle(Style(TBS_BOTTOM, 0));
//   }
//   // vertical
//   else {
//     assert(side == LeftSide || side == RightSide);

//     if (side == LeftSide)
//       addStyle(Style(TBS_LEFT, 0));
//     else if (side == RightSide)
//       addStyle(Style(TBS_RIGHT, 0));
//   }
// }

// void Slider::setTickMarksOnBothSides()
// {
//   removeStyle(Style(TBS_NOTICKS | TBS_LEFT | TBS_TOP | TBS_RIGHT | TBS_BOTTOM, 0));
//   addStyle(Style(TBS_BOTH, 0));
// }

void Slider::pointNowhere()
{
  removeStyle(Style(TBS_LEFT | TBS_TOP | TBS_RIGHT | TBS_BOTTOM, 0));
  addStyle(Style(TBS_BOTH, 0));
}

void Slider::pointToSide(Side side)
{
  removeStyle(Style(TBS_LEFT | TBS_TOP | TBS_RIGHT | TBS_BOTTOM, 0));
  
  switch (side) {
    case Side::Left:
      addStyle(Style(TBS_LEFT, 0));
      break;
    case Side::Top:
      addStyle(Style(TBS_TOP, 0));
      break;
    case Side::Right:
      addStyle(Style(TBS_RIGHT, 0));
      break;
    case Side::Bottom:
      addStyle(Style(TBS_BOTTOM, 0));
      break;
  }
}

void Slider::setVisibleTickMarks(bool state)
{
  if (state)
    removeStyle(Style(TBS_NOTICKS, 0));
  else
    addStyle(Style(TBS_NOTICKS, 0));
}

void Slider::addTickMark(int markPosition)
{
  sendMessage(TBM_SETTIC, 0, markPosition);
}

int Slider::getTickMarksCount()
{
  return sendMessage(TBM_GETNUMTICS, 0, 0);
}

void Slider::clearTickMarks()
{
  sendMessage(TBM_CLEARTICS, TRUE, 0);
}

int Slider::getMinimum()
{
  return sendMessage(TBM_GETRANGEMIN, 0, 0);
}

int Slider::getMaximum()
{
  return sendMessage(TBM_GETRANGEMAX, 0, 0);
}

void Slider::getRange(int& minValue, int& maxValue)
{
  minValue = getMinimum();
  maxValue = getMaximum();
}

void Slider::setRange(int minValue, int maxValue)
{
  sendMessage(TBM_SETRANGE, TRUE, MAKELONG(minValue, maxValue));
}

int Slider::getValue()
{
  return sendMessage(TBM_GETPOS, 0, 0);
}

void Slider::setValue(int value)
{
  sendMessage(TBM_SETPOS, TRUE, value);
}

void Slider::onPreferredSize(Size& sz)
{
  int style = getStyle().regular;
  
  // without ticks
  if (style & TBS_NOTICKS)
    sz = Size(25, 25);
  // with ticks in the top or the left side
  else if ((style & (TBS_LEFT | TBS_TOP)) != 0)
    sz = Size(33, 33);
  // with ticks in both sides
  else if ((style & TBS_BOTH) != 0)
    sz = Size(40, 40);
  // with ticks in the bottom or the right side
  else
    sz = Size(30, 30);
}

void Slider::onScroll(Orientation orientation, int code)
{
  assert(getOrientation() == orientation);
  
  switch (code) {

    case TB_BOTTOM:
    case TB_ENDTRACK:
    case TB_LINEDOWN:
    case TB_LINEUP:
    case TB_PAGEDOWN:
    case TB_PAGEUP:
    case TB_THUMBPOSITION:
    case TB_THUMBTRACK:
    case TB_TOP: {
      Event ev(this);
      onChange(ev);
      return;
    }

  }

  // impossible
  assert(false);
}

void Slider::onChange(Event& ev)
{
  Change(ev);
}

// #define TBM_GETTIC	(WM_USER+3)
// #define TBM_SETTIC	(WM_USER+4)
// #define TBM_CLEARTICS	(WM_USER+9)
// #define TBM_SETSEL	(WM_USER+10)
// #define TBM_SETSELSTART	(WM_USER+11)
// #define TBM_SETSELEND	(WM_USER+12)
// #define TBM_GETPTICS	(WM_USER+14)
// #define TBM_GETTICPOS	(WM_USER+15)
// #define TBM_GETNUMTICS	(WM_USER+16)
// #define TBM_GETSELSTART	(WM_USER+17)
// #define TBM_GETSELEND	(WM_USER+18)
// #define TBM_CLEARSEL	(WM_USER+19)
// #define TBM_SETTICFREQ	(WM_USER+20)
// #define TBM_SETPAGESIZE	(WM_USER+21)
// #define TBM_GETPAGESIZE	(WM_USER+22)
// #define TBM_SETLINESIZE	(WM_USER+23)
// #define TBM_GETLINESIZE	(WM_USER+24)
// #define TBM_GETTHUMBRECT	(WM_USER+25)
// #define TBM_GETCHANNELRECT	(WM_USER+26)
// #define TBM_SETTHUMBLENGTH	(WM_USER+27)
// #define TBM_GETTHUMBLENGTH	(WM_USER+28)
// #define TBM_SETTOOLTIPS	(WM_USER+29)
// #define TBM_GETTOOLTIPS	(WM_USER+30)
// #define TBM_SETTIPSIDE	(WM_USER+31)
// #define TBM_SETBUDDY	(WM_USER+32)
// #define TBM_GETBUDDY	(WM_USER+33)
// #define TBM_GETUNICODEFORMAT	CCM_GETUNICODEFORMAT 
// #define TBM_SETUNICODEFORMAT	CCM_SETUNICODEFORMAT
