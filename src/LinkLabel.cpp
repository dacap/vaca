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
#include "Vaca/LinkLabel.h"
#include "Vaca/Point.h"
#include "Vaca/MouseEvent.h"
#include "Vaca/Cursor.h"
#include "Vaca/Debug.h"

using namespace Vaca;

LinkLabel::LinkLabel(const String &url, Widget *parent, Style style)
  : CustomLabel(url, parent, style)
  , mUrl(url)
{
  setText(url);
  updateFont(getFont());
  mTriState = Outside;
}

LinkLabel::LinkLabel(const String &url, const String &text, Widget *parent, Style style)
  : CustomLabel(text, parent, style)
  , mUrl(url)
{
  setText(text);
  updateFont(getFont());
  mTriState = Outside;
}

LinkLabel::~LinkLabel()
{
}

void LinkLabel::setFont(Font &font)
{
  Widget::setFont(font);

  // update the underline font
  updateFont(font);
}

Color LinkLabel::getLinkColor()
{
  return Color(0, 0, 255);
}

Color LinkLabel::getHoverColor()
{
  return Color(0, 0, 255);
}

/**
 * Draws the background and the label. By default, the background
 * color is getBgColor() and the label color is getLinkColor(), if the
 * mouse is over the label, it's drawn using the mUnderlineFont font and
 * the getHoverColor() color.
 */
void LinkLabel::onPaint(Graphics &g)
{
  Rect rc = getClientBounds();

  g.setColor(getBgColor());
  g.fillRect(rc);

  Rect bounds = getLinkBounds(g);
  int flags = getFlagsForDrawString();

  switch (getTextAlign()) {
    case LeftAlign:   flags |= DT_LEFT;   break;
    case CenterAlign: flags |= DT_CENTER; break;
    case RightAlign:  flags |= DT_RIGHT;  break;
  }

  if (mTriState == Hover) {
    g.setFont(mUnderlineFont);
    g.setColor(getHoverColor());
  }
  else {
    g.setFont(getFont());
    g.setColor(getLinkColor());
  }

  if (isEnabled())
    g.drawString(getText(), bounds, flags);
  else
    g.drawDisabledString(getText(), bounds, flags);
}

void LinkLabel::onMouseEnter(MouseEvent &ev)
{
  mTriState = Inside;
}

void LinkLabel::onMouseMove(MouseEvent &ev)
{
  ScreenGraphics g;
  Rect rc = getLinkBounds(g);

  if (mTriState == Inside) {
    if (rc.contains(ev.getPoint())) {
      invalidate(true);
//       setCursor(Cursor::Hand);
      mTriState = Hover;
    }
  }
  else if (mTriState == Hover) {
    if (!rc.contains(ev.getPoint())) {
      invalidate(true);
//       setCursor(Cursor::Arrow);
      mTriState = Inside;
    }
  }
}

void LinkLabel::onMouseLeave()
{
  if (mTriState == Hover) {
    invalidate(true);
//     setCursor(Cursor::Arrow);
  }
  mTriState = Outside;
}

/**
 * Opens the URL (if it's not empty), and calls the onAction event.
 */
void LinkLabel::onMouseDown(MouseEvent &ev)
{
  if (mTriState == Hover) {
    if (!mUrl.empty())
      ShellExecute(NULL, _T("open"), mUrl.c_str(), NULL, NULL, SW_SHOW);

    Event subEv(this);
    onAction(subEv);
  }
}

/**
 * Uses the Cursor::Hand when the mouse is over the label.
 */
void LinkLabel::onSetCursor(int hitTest)
{
  switch (mTriState) {
    case Hover:
      setCursor(Cursor(HandCursor));
      break;
    default:
      setCursor(Cursor(ArrowCursor));
      break;
  }
}

/**
 * If the label is resized, we must to redraw it. This is necessary
 * mainly if the LinkLabel isn't TextAlign::Left.
 */
// void LinkLabel::onResize(const Size &sz)
// {
//   invalidate(true);
// }

/**
 * Called when the user press the mouse button down over the label.
 */
void LinkLabel::onAction(Event &ev)
{
  Action(ev);
}

void LinkLabel::updateFont(Font &font)
{
  mUnderlineFont = Font(font, Font::Style::Underline);
}

Rect LinkLabel::getLinkBounds(Graphics &g)
{
  g.setFont(getFont());

  Rect rc = getClientBounds();
  Size sz = g.measureString(getText(), rc.w, getFlagsForDrawString());
  Point pt = rc.getOrigin();

  switch (getTextAlign()) {
    case CenterAlign: pt.x += rc.w/2 - sz.w/2; break;
    case RightAlign: pt.x += rc.w - sz.w; break;
    default:
      // do nothing
      break;
  }

  return Rect(pt, sz);
}
