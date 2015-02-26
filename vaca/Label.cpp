// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Label.h"
#include "vaca/Font.h"
#include "vaca/Debug.h"
#include "vaca/WidgetClass.h"
#include "vaca/PreferredSizeEvent.h"

using namespace vaca;

/**
   Creates a new Label widget.

   @warning
     Remember that a character with a preceding ampersand (&)
     will be underlined. So if you want to show a string with the '&',
     you have to use "&&".
*/
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

/**
   Returns the current text alignment.

   @return TextAlign#Center, TextAlign#RightAlign, TextAlign#LeftAlign
*/
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

/**
   Sets the text alignment.
*/
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

/**
   @todo the return value is too Win32 specific.
*/
int Label::getFlagsForDrawString()
{
  // TODO complete this
  if (useWordWrap())
    return DT_WORDBREAK;
  else
    return DT_SINGLELINE;
}

/**
   Returns the preferred size of the label using Graphics#measureString.
*/
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

/**
   If the label is resized, we must to redraw it. This is necessary
   mainly if the Label isn't TextAlign::Left.
*/
void Label::onResize(ResizeEvent& ev)
{
  invalidate(true);
  Widget::onResize(ev);
}
