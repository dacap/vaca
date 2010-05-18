// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
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

#include "Vaca/Slider.h"
#include "Vaca/Debug.h"
#include "Vaca/Event.h"
#include "Vaca/ScrollEvent.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/PreferredSizeEvent.h"
#include <limits>
#include <limits.h>

using namespace Vaca;

const int Slider::MinLimit = SHRT_MIN; // std::numeric_limits<short>::min();
const int Slider::MaxLimit = SHRT_MAX; // std::numeric_limits<short>::max();

Slider::Slider(Widget* parent, Style style)
  : Widget(WidgetClassName(TRACKBAR_CLASS), parent, style)
{
}

Slider::Slider(int minValue, int maxValue, int value,
	       Widget* parent, Style style)
  : Widget(WidgetClassName(TRACKBAR_CLASS), parent, style)
{
  setRange(minValue, maxValue);
  setValue(value);
}

Slider::~Slider()
{
}

Orientation Slider::getOrientation()
{
  // we can't use TBS_HORZ to ask for the orientation because TBS_HORZ=0
  if (getStyle().regular & TBS_VERT)
    return Orientation::Vertical;
  else
    return Orientation::Horizontal;
}

void Slider::setOrientation(Orientation orientation)
{
  Style style = getStyle();

  if (orientation == Orientation::Vertical)
    style.regular |= TBS_VERT;	// TBS_VERT = 2
  else
    style.regular &= ~TBS_VERT;	// TBS_HORZ = 0

  setStyle(style);
}

Sides Slider::getSides()
{
  Style style = getStyle();

  if (style.regular & TBS_BOTH)
    return Sides::None;
  else if (style.regular & TBS_VERT) {
    if (style.regular & TBS_LEFT)
      return Sides::Left;
    else
      return Sides::Right;
  }
  else {
    if (style.regular & TBS_TOP)
      return Sides::Top;
    else
      return Sides::Bottom;
  }
}

void Slider::setSides(Sides sides)
{
  Style style = getStyle();

  style.regular &= ~(TBS_BOTH | TBS_TOP);

  if (style.regular & TBS_VERT) {
    bool left = (sides & Sides::Left) != 0;
    bool right = (sides & Sides::Right) != 0;

    if (left && !right)
      style.regular |= TBS_LEFT;
    else if (!left && right)
      style.regular |= TBS_RIGHT;
    else
      style.regular |= TBS_BOTH;
  }
  else {
    bool top = (sides & Sides::Top) != 0;
    bool bottom = (sides & Sides::Bottom) != 0;

    if (top && !bottom)
      style.regular |= TBS_TOP;
    else if (!top && bottom)
      style.regular |= TBS_BOTTOM;
    else
      style.regular |= TBS_BOTH;
  }

  setStyle(style);
}

/**
   Sets the visibility state of the tick marks in the slider.

   @see #setTickFreq
*/
void Slider::setTickVisible(bool state)
{
  if (state)
    removeStyle(Style(TBS_NOTICKS, 0));
  else
    addStyle(Style(TBS_NOTICKS, 0));
}

/**
   Sets the frequency of the tick marks in the slider.

   @see #setTickVisible
*/
void Slider::setTickFreq(int freq)
{
  sendMessage(TBM_SETTICFREQ, freq, 0);
}

int Slider::getMinValue()
{
  return sendMessage(TBM_GETRANGEMIN, 0, 0);
}

int Slider::getMaxValue()
{
  return sendMessage(TBM_GETRANGEMAX, 0, 0);
}

/**
   Gets the current range of the slider which indicates the posible
   values that the user can select.

   @param minValue Minimum value in the range.
   @param maxValue Maximum value in the range.

   @see #getMinValue, #getMaxValue
*/
void Slider::getRange(int& minValue, int& maxValue)
{
  minValue = getMinValue();
  maxValue = getMaxValue();
}

/**
   Sets the range of values which the user can select from.

   @param minValue
     Must be great or equal to #MinLimit.
   @param maxValue
     Must be less or equal to #MaxLimit.
     And it must be also equal Must be greater or equal to minValue.

   @see #getRange, #MinLimit, #MaxLimit
*/
void Slider::setRange(int minValue, int maxValue)
{
  assert(minValue <= maxValue);
  assert(minValue >= Slider::MinLimit);
  assert(maxValue <= Slider::MaxLimit);

  sendMessage(TBM_SETRANGE, TRUE, MAKELONG(minValue, maxValue));
}

/**
   Returns the selected value in the range of the slider.
*/
int Slider::getValue()
{
  return sendMessage(TBM_GETPOS, 0, 0);
}

/**
   Sets the selected value in the slider.

   @param value
     Must be inside the slider's range. You can obtain the
     current minimum and maximum values using the #getRange
	 member function.
*/
void Slider::setValue(int value)
{
  assert(getMinValue() <= value);
  assert(value <= getMaxValue());

  sendMessage(TBM_SETPOS, TRUE, value);
}

/**
   Returns the current line size, it is how many units the value will
   move when the user press the arrow keys.

   @see #setLineSize
*/
int Slider::getLineSize()
{
  return sendMessage(TBM_GETLINESIZE, 0, 0);
}

/**
   Sets the current line size, it is how many units the value will
   move when the user press the arrow keys.

   @see #getLineSize
*/
void Slider::setLineSize(int lineSize)
{
  sendMessage(TBM_SETLINESIZE, 0, lineSize);
}

/**
   Returns the current page size, it is how many units the value will
   move when the user press the PageUp/Down keys.

   @see #setPageSize
*/
int Slider::getPageSize()
{
  return sendMessage(TBM_GETPAGESIZE, 0, 0);
}

/**
   Sets the current page size, it is how many units the value will
   move when the user press the PageUp/Down keys.

   @see #getPageSize
*/
void Slider::setPageSize(int pageSize)
{
  sendMessage(TBM_SETPAGESIZE, 0, pageSize);
}

void Slider::onPreferredSize(PreferredSizeEvent& ev)
{
  int style = getStyle().regular;
  Size sz;

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

  ev.setPreferredSize(sz);
}

void Slider::onScroll(ScrollEvent& ev)
{
  assert(getOrientation() == ev.getOrientation());

  Event subEvent(this);
  onChange(subEvent);
}

void Slider::onChange(Event& ev)
{
  Change(ev);
}
