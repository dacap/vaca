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

#include <cmath>
#include "SectionContent.h"

SectionContent::SectionContent(Widget *parent)
  : Panel(parent)
  , mTimer(1000/60)
{
  setDoubleBuffered(true);
  setLayout(new BoxLayout(Vertical, false, 4));
  setBgColor(Color::White);
    
  mTimer.Action.connect(Bind(&SectionContent::animateSize, this));
  mCurrentSize = Size(0, 0);

  setVisible(false);
}

void SectionContent::animateVisibility(bool visible)
{
  mTimer.start();

  if (visible) {
    mDestSize = Size(0, 0);
    Panel::onPreferredSize(mDestSize);
  }
  else
    mDestSize = Size(0, 0);

  mStartHeight = mCurrentSize.h;
  mTickCounter = 0;
}

void SectionContent::onPreferredSize(Size &sz)
{
  sz = mCurrentSize;
}

void SectionContent::onResize(const Size &sz)
{
  Panel::onResize(sz);
  invalidate(true);
}

void SectionContent::onPaint(Graphics &g)
{
  Rect rc = getClientBounds();
  Pen pen(Color(200, 200, 200));
    
  g.drawRoundRect(pen, Rect(rc.x+1, rc.y-8, rc.w-1, rc.h+8), Size(8, 8));
}
  
void SectionContent::animateSize()
{
  double height = (mDestSize.h - mStartHeight);
  height += height < 0 ? -1: +1;

  // exponential movement
  mCurrentSize.h = static_cast<int>
    (mStartHeight +
     height * (1 - std::exp(-mTickCounter * (25.0/60.0))));

  if (mStartHeight < mDestSize.h) {
    // show the widget
    if (mCurrentSize.h == 0) {
      // the animation start, make the widget visible
      setVisible(true);
    }

    // stop the animation
    if (mCurrentSize.h >= mDestSize.h) {
      mCurrentSize.h = mDestSize.h;
      mTimer.stop();
    }
  }
  else if (mStartHeight > mDestSize.h) {
    // stop the animation
    if (mCurrentSize.h <= mDestSize.h) {
      mCurrentSize.h = mDestSize.h;
      mTimer.stop();

      // hide the widget
      if (mDestSize.h == 0)
	setVisible(false);
    }
  }
  else
    mTimer.stop();

  mTickCounter++;

  // relayout the parent
  getParent()->layout();
}
