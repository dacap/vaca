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

#include "LightingPanel.h"

LightingPanel::LightingPanel(Widget *parent, Style style)
  : Panel(parent, style)
  , mTimer(1000/60)
  , mSpeed(0.3)
{
  setDoubleBuffered(true);

  mLight = 0.0;

  mTimer.Action.connect(Bind(&LightingPanel::animateLight, this));
}

double LightingPanel::getLightLevel()
{
  return mLight;
}

void LightingPanel::setLightSpeed(double secs)
{
  mSpeed = secs;
}

void LightingPanel::onMouseEnter(MouseEvent &ev)
{
  Panel::onMouseEnter(ev);

  if (!hasFocus()) {
    mIncLight = true;		// increment light
    mTimer.start();
  }
}

void LightingPanel::onMouseLeave()
{
  Panel::onMouseLeave();

  if (!hasFocus()) {
    mIncLight = false;	// decrement light
    mTimer.start();
  }
}
  
void LightingPanel::onGotFocus(Event &ev)
{
  Panel::onGotFocus(ev);
  invalidate(true);

  if (!hasMouse()) {
    mIncLight = true;		// increment light
    mTimer.start();
  }
}

void LightingPanel::onLostFocus(Event &ev)
{
  Panel::onLostFocus(ev);
  invalidate(true);

  if (!hasMouse()) {
    mIncLight = false;		// decrement light
    mTimer.start();
  }
}

// this method is called in each tick of mTimer
void LightingPanel::animateLight()
{
  if (mIncLight) {
    mLight += 1.0 / (mSpeed*60.0);
    if (mLight > 1.0) {
      mLight = 1.0;
      mTimer.stop();
    }
  }
  else {
    mLight -= 1.0 / (mSpeed*60.0);
    if (mLight < 0.0) {
      mLight = 0.0;
      mTimer.stop();
    }
  }

  invalidate(true);
}

