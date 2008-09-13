// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#include "Vaca/LinkLabel.h"
#include "Vaca/Point.h"
#include "Vaca/MouseEvent.h"
#include "Vaca/Cursor.h"
#include "Vaca/Debug.h"
#include "Vaca/Font.h"
#include "Vaca/Brush.h"
#include "Vaca/Pen.h"
#include "Vaca/KeyEvent.h"
#include "Vaca/Image.h"

using namespace Vaca;

/**
 * It creates a LinkLabel.
 *
 * @param urlOrText
 *     If it contains "www", "://", or "@@", the
 *     LinkLabel'll open the browser when it's clicked.
 *     If not, it's just like test, and you should hook
 *     the LinkLabel::Action signal or
 *     LinkLabel::onAction event.
 *
 * @param parent
 *     It's a Widget (generally a Frame) that will act as the container
 *     parent of the new LinkLabel.
 * 
 * @param style
 *     Style to put to this widget.
 */
LinkLabel::LinkLabel(const String& urlOrText, Widget* parent, Style style)
  : CustomLabel("", parent, style)
{
  // is a URL?
  if (urlOrText.find_first_of(_T("www")) != String::npos ||
      urlOrText.find_first_of(_T("://")) != String::npos ||
      urlOrText.find_first_of(_T("@")) != String::npos)
    m_url = urlOrText;

  init(urlOrText);
}

LinkLabel::LinkLabel(const String& url, const String& text, Widget* parent, Style style)
  : CustomLabel("", parent, style)
  , m_url(url)
{
  init(text);
}

LinkLabel::LinkLabel(const String& url, Image& image, Widget* parent, Style style)
  : CustomLabel("", parent, style)
  , m_url(url)
{
  init("", &image);
}

LinkLabel::~LinkLabel()
{
}

void LinkLabel::setFont(Font font)
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

void LinkLabel::onPreferredSize(Size& sz)
{
  // TODO add support for both: text and image
  if (m_image != NULL)
    sz = m_image->getSize();
  else
    CustomLabel::onPreferredSize(sz);
}

/**
 * Draws the background and the label. By default, the background
 * color is getBgColor() and the label color is getLinkColor(), if the
 * mouse is over the label, it's drawn using the m_underlineFont font and
 * the getHoverColor() color.
 */
void LinkLabel::onPaint(Graphics& g)
{
  Rect rc = getClientBounds();
  Brush brush(getBgColor());

  g.fillRect(brush, rc);

  Rect bounds = getLinkBounds(g);

  // draw image
  if (m_image != NULL) {
    g.drawImage(*m_image, bounds.x, bounds.y);
  }

  // draw text
  if (!getText().empty()) {
    int flags = getFlagsForDrawString();
  
    switch (getTextAlign()) {
      case TextAlign::Left:   flags |= DT_LEFT;   break;
      case TextAlign::Center: flags |= DT_CENTER; break;
      case TextAlign::Right:  flags |= DT_RIGHT;  break;
    }

    if (m_state == Hover) {
      g.setFont(m_underlineFont);
      g.setColor(getHoverColor());
    }
    else {
      g.setFont(getFont());
      g.setColor(getLinkColor());
    }

    // draw text
    if (isEnabled())
      g.drawString(getText(), bounds, flags);
    else
      g.drawDisabledString(getText(), bounds, flags);
  }

  // draw focus
  if (hasFocus())
    g.drawFocus(bounds);
}

void LinkLabel::onMouseEnter(MouseEvent& ev)
{
  m_state = Inside;
}

void LinkLabel::onMouseMove(MouseEvent& ev)
{
  ScreenGraphics g;
  Rect rc = getLinkBounds(g);

  if (m_state == Inside) {
    if (rc.contains(ev.getPoint())) {
      invalidate(true);
      m_state = Hover;
    }
  }
  else if (m_state == Hover) {
    if (!rc.contains(ev.getPoint())) {
      invalidate(true);
      m_state = Inside;
    }
  }
}

void LinkLabel::onMouseLeave()
{
  if (m_state == Hover)
    invalidate(true);

  m_state = Outside;
}

/**
 * Opens the URL (if it's not empty), and calls the onAction event.
 */
void LinkLabel::onMouseDown(MouseEvent& ev)
{
  if (m_state == Hover) {
    requestFocus();
    
    action();
  }
}

/**
 * Uses the Cursor::Hand when the mouse is over the label.
 */
void LinkLabel::onSetCursor(WidgetHitTest hitTest)
{
  switch (m_state) {
    case Hover:
      setCursor(Cursor(SysCursor::Hand));
      break;
    default:
      setCursor(Cursor(SysCursor::Arrow));
      break;
  }
}

void LinkLabel::onGotFocus(Event& ev)
{
  CustomLabel::onGotFocus(ev);
  invalidate(true);
}

void LinkLabel::onLostFocus(Event& ev)
{
  CustomLabel::onGotFocus(ev);
  invalidate(true);
}

void LinkLabel::onKeyDown(KeyEvent& ev)
{
  CustomLabel::onKeyDown(ev);

  if (hasFocus() &&
      (ev.getKeyCode() == Keys::Space ||
       ev.getKeyCode() == Keys::Enter)) {
    action();
  }
}

/**
 * If the label is resized, we must to redraw it. This is necessary
 * mainly if the LinkLabel isn't TextAlign::Left.
 */
// void LinkLabel::onResize(const Size& sz)
// {
//   invalidate(true);
// }

/**
 * Called when the user press the mouse button down over the label.
 */
void LinkLabel::onAction(Event& ev)
{
  Action(ev);
}

void LinkLabel::init(String text, Image* image)
{
  m_underlineFont = NULL;
  m_image = image;

  updateFont(getFont());
  m_state = Outside;

  setText(text);
}

void LinkLabel::action()
{
  if (!m_url.empty())
    ShellExecute(NULL, _T("open"), m_url.c_str(), NULL, NULL, SW_SHOW);

  Event ev(this);
  onAction(ev);
}

void LinkLabel::updateFont(const Font& font)
{
  m_underlineFont = Font(font, font.getStyle() | FontStyle::Underline);
}

Rect LinkLabel::getLinkBounds(Graphics& g)
{
  g.setFont(getFont());

  Rect rc = getClientBounds();
  Point pt = rc.getOrigin();
  Size sz;

  // TODO add support for both: text and image

  if (m_image != NULL) {
    sz = m_image->getSize();
    pt.x += rc.w/2 - sz.w/2;
    pt.y += rc.h/2 - sz.h/2;
  }
  else {
    sz = g.measureString(getText(), rc.w, getFlagsForDrawString());

    switch (getTextAlign()) {
      case TextAlign::Center: pt.x += rc.w/2 - sz.w/2; break;
      case TextAlign::Right: pt.x += rc.w - sz.w; break;
      default:
	// do nothing
	break;
    }
  }

  return Rect(pt, sz);
}
