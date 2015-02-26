// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/ProgressBar.h"
#include "vaca/Debug.h"
#include "vaca/WidgetClass.h"
#include "vaca/PreferredSizeEvent.h"
#include "vaca/win32.h"

#ifndef PBM_SETMARQUEE
#define PBM_SETMARQUEE 1034
#endif

using namespace vaca;

ProgressBar::ProgressBar(Widget* parent, Style style)
  : Widget(WidgetClassName(PROGRESS_CLASS), parent, style)
{
  // default values
  setRange(0, 100);
  setValue(0);
}

ProgressBar::ProgressBar(int minValue, int maxValue,
			 Widget* parent, Style style)
  : Widget(WidgetClassName(PROGRESS_CLASS), parent, style)
{
  assert(minValue <= maxValue);

  setRange(minValue, maxValue);
  setValue(minValue);	// default value
}

ProgressBar::~ProgressBar()
{
}

/**
   PBM_SETBKCOLOR
*/
void ProgressBar::setBgColor(const Color& color)
{
  assert(::IsWindow(getHandle()));

  Widget::setBgColor(color);
  sendMessage(PBM_SETBKCOLOR, 0, convert_to<COLORREF>(color));
}

// void ProgressBar::setBarColor(const Color& color)
// {
// }

int ProgressBar::getMinimum()
{
  return sendMessage(PBM_GETRANGE, TRUE, 0);
}

int ProgressBar::getMaximum()
{
  return sendMessage(PBM_GETRANGE, FALSE, 0);
}

/**
   PBM_GETRANGE
*/
void ProgressBar::getRange(int& minValue, int& maxValue)
{
  PBRANGE pbr;
  sendMessage(PBM_GETRANGE, 0, reinterpret_cast<LPARAM>(&pbr));
  minValue = pbr.iLow;
  maxValue = pbr.iHigh;
}

/**
   PBM_SETRANGE32
*/
void ProgressBar::setRange(int minValue, int maxValue)
{
  sendMessage(PBM_SETRANGE32, minValue, maxValue);
}

/**
   PBM_GETPOS
*/
int ProgressBar::getValue()
{
  return sendMessage(PBM_GETPOS, 0, 0);
}

/**
   PBM_SETPOS
*/
void ProgressBar::setValue(int value)
{
  sendMessage(PBM_SETPOS, value, 0);
}

/**
   Increments the current progress bar value (position), in @a delta
   units (PBM_DELTAPOS).
*/
void ProgressBar::addValue(int delta)
{
  sendMessage(PBM_DELTAPOS, delta, 0);
}

void ProgressBar::setMarquee(int msecsInterval)
{
  sendMessage(PBM_SETMARQUEE, msecsInterval == 0 ? FALSE: TRUE, msecsInterval);
}

void ProgressBar::onPreferredSize(PreferredSizeEvent& ev)
{
  // WinXP with themes needs at least 10 pixels of height
  ev.setPreferredSize(16, 16);
}
