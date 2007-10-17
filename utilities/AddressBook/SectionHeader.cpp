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

#include "SectionHeader.h"

SectionHeader::SectionHeader(const String &text, Color color, Widget *parent)
  : LightingPanel(parent, PanelStyle
			  + FocusableStyle
			  - ContainerStyle) // to make the focus work, it shouldn't has the ContainerStyle
{
  setText(text);
  setBgColor(Color::White);
  setFgColor(color);

  mSelected = false;
}

void SectionHeader::back()
{
  mSelected = false;
  invalidate(true);
}

void SectionHeader::onPreferredSize(Size &sz)
{
  ScreenGraphics g;
  g.setFont(getFont());
  sz = g.measureString("")+Size(4, 4);
}

void SectionHeader::onResize(const Size &sz)
{
  LightingPanel::onResize(sz);
  invalidate(true);
}
  
void SectionHeader::onMouseDown(MouseEvent &ev)
{
  LightingPanel::onMouseDown(ev);

  // TODO
//   mTimer.stop();

  // start selecting
  acquireFocus();
  acquireCapture();

  mStartSelected = mSelected;
  mSelected = mStartSelected ? false: true;
  invalidate(true);
}

void SectionHeader::onMouseMove(MouseEvent &ev)
{
  LightingPanel::onMouseMove(ev);

  // selecting?
  if (hasCapture()) {
    bool selected = hasMouseAbove();
    selected = mStartSelected ? !selected: selected;

    if (mSelected != selected) {
      mSelected = selected;
      invalidate(true);
    }
  }
}

void SectionHeader::onMouseUp(MouseEvent &ev)
{
  LightingPanel::onMouseUp(ev);

  // selecting?
  if (hasCapture()) {
    if (hasMouseAbove())
      Action(mSelected ? this: NULL); // fire action signal

    releaseCapture();
  }
}

void SectionHeader::onSetCursor(int hitTest)
{
  if (hitTest == HTCLIENT)
    setCursor(Cursor(HandCursor));
  else
    LightingPanel::onSetCursor(hitTest);
}
  
void SectionHeader::onPaint(Graphics &g)
{
  Rect rc = getClientBounds();
  Color color;
  double light = getLightLevel();

  if (mSelected)
    color = getFgColor()*0.75;
  else
    color =
      Color(200, 200, 200)*(1-light) +
      getFgColor()*light;

  Pen pen(color);		// TODO remove this pen, use GraphicsPath

  g.beginPath();
  g.moveTo(rc.x, rc.y+rc.h);
  g.curveTo(rc.x, rc.y+1,
	    rc.x+1, rc.y,
	    rc.x+4, rc.y);
  g.lineTo(pen, rc.x+rc.w-1, rc.y);
  g.lineTo(pen, rc.x+rc.w-1, rc.y+rc.h-1);
  g.closeFigure();
  g.endPath();

  // TODO what about a GradientBrush, like GDI+?

  Region region;
  g.getRegionFromPath(region);
  g.intersectClipRegion(region);

  Brush brush(color);
  g.fillRect(brush, Rect(rc.x, rc.y, rc.w/2, rc.h));

  g.fillGradientRect(Rect(rc.x+rc.w/2, rc.y, rc.w-rc.w/2, rc.h),
		     color,
		     Color::White,
		     Horizontal);

  // draw the text
  g.setColor(Color::White);
  g.drawString(getText(), rc.x+8, rc.y+2);

  // draw focus
  if (hasFocus()) {
    g.drawRect(pen, rc.inflate(0, -1));
    g.drawRect(pen, rc.inflate(-1, -1));
  }
}

void SectionHeader::onKeyDown(KeyEvent &ev)
{
  if (hasFocus() &&
      (ev.getKeyCode() == Keys::Space ||
       ev.getKeyCode() == Keys::Enter)) {
    mSelected = mSelected ? false: true;
    invalidate(true);

    Action(mSelected ? this: NULL); // fire action signal
  }
}

