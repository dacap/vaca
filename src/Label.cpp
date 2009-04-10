// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

#include "Vaca/Label.h"
#include "Vaca/Font.h"
#include "Vaca/Debug.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/PreferredSizeEvent.h"

using namespace Vaca;

/// Creates a new Label widget.
/// 
/// @warning
///   Remember that a character with a preceding ampersand (&)
///   will be underlined. So if you want to show a string with the '&',
///   you have to use "&&".
/// 
Label::Label(const String& text, Widget* parent, Style style)
  : Widget(WidgetClassName(WC_STATIC), parent, style)
{
  setText(text);
}

Label::Label(HWND handle)
  : Widget(handle)
{
}

Label::~Label()
{
}

/// Returns the current text alignment.
/// 
/// @return TextAlign#Center, TextAlign#RightAlign, TextAlign#LeftAlign
/// 
TextAlign Label::getTextAlign() const
{
  int style = getStyle().regular;

  if ((style & 31) == SS_CENTER)
    return TextAlign::Center;
  else if ((style & 31) == SS_RIGHT)
    return TextAlign::Right;
  else
    return TextAlign::Left;
}

/// Sets the text alignment.
/// 
void Label::setTextAlign(TextAlign align)
{
  int type = (getStyle().regular & 31);

  if (type == SS_LEFT ||
      type == SS_CENTER ||
      type == SS_RIGHT) {
    int style = SS_LEFT;

    switch (align) {
      case TextAlign::Left:   style = SS_LEFT;   break;
      case TextAlign::Center: style = SS_CENTER; break;
      case TextAlign::Right:  style = SS_RIGHT;  break;
    }

    removeStyle(Style(SS_LEFT | SS_CENTER | SS_RIGHT, 0));
    addStyle(Style(style, 0));

    invalidate(true);
  }
}

bool Label::useWordWrap()
{
  // TODO SS_RIGHTJUST

  int style = getStyle().regular & 0xffff;

  return (((style & 31) != SS_SIMPLE) &&
	  ((style & 31) != SS_LEFTNOWORDWRAP) &&
	  ((style & SS_ELLIPSISMASK) == 0));
}

/// @todo the return value is too Win32 specific.
/// 
int Label::getFlagsForDrawString()
{
  // TODO complete this
  if (useWordWrap())
    return DT_WORDBREAK;
  else
    return DT_SINGLELINE;
}

/// Returns the preferred size of the label using Graphics#measureString.
/// 
void Label::onPreferredSize(PreferredSizeEvent& ev)
{
  // TODO HTHEME stuff

  ScreenGraphics g;
  g.setFont(getFont());

  if (ev.fitInWidth() && useWordWrap())
    ev.setPreferredSize(g.measureString(getText(), ev.fitInWidth()));
  else
    ev.setPreferredSize(g.measureString(getText()));
}

/// If the label is resized, we must to redraw it. This is necessary
/// mainly if the Label isn't TextAlign::Left.
/// 
void Label::onResize(ResizeEvent& ev)
{
  invalidate(true);
  Widget::onResize(ev);
}
