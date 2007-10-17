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
#include "Vaca/ProgressBar.h"
#include "Vaca/Debug.h"

using namespace Vaca;

ProgressBar::ProgressBar(Widget *parent, Style style)
  : Widget(PROGRESS_CLASS, parent, style)
{
}

void ProgressBar::setBgColor(Color color)
{
  VACA_ASSERT(getHwnd() != NULL);

  Widget::setBgColor(color);
  sendMessage(PBM_SETBKCOLOR, 0, color.getColorRef());
}

// void ProgressBar::setBarColor(Color color)
// {
// }

Size ProgressBar::preferredSize()
{
  // WinXP with themes needs at least 10 pixels of height
  return Size(16, 16);
}

int ProgressBar::getMinimum()
{
  return sendMessage(PBM_GETRANGE, TRUE, 0);
}

int ProgressBar::getMaximum()
{
  return sendMessage(PBM_GETRANGE, FALSE, 0);
}

void ProgressBar::getRange(int &minValue, int &maxValue)
{
  PBRANGE pbr;
  sendMessage(PBM_GETRANGE, 0, reinterpret_cast<LPARAM>(&pbr));
  minValue = pbr.iLow;
  maxValue = pbr.iHigh;
}

void ProgressBar::setRange(int minValue, int maxValue)
{
  sendMessage(PBM_SETRANGE32, minValue, maxValue);
}

int ProgressBar::getPosition()
{
  return sendMessage(PBM_GETPOS, 0, 0);
}

void ProgressBar::setPosition(int posValue)
{
  sendMessage(PBM_SETPOS, posValue, 0);
}

void ProgressBar::advancePosition(int increment)
{
  sendMessage(PBM_DELTAPOS, increment, 0);
}
