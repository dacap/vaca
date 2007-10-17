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
#include "Vaca/Label.h"
#include "Vaca/Font.h"
#include "Vaca/Debug.h"

using namespace Vaca;

Label::Label(const String &text, Widget *parent, Style style)
  : Widget(_T("STATIC"), parent, style)
{
  setText(text);
}

TextAlign Label::getTextAlign()
{
  int style = getStyle().regular;

  if ((style & 31) == SS_CENTER)
    return CenterAlign;
  else if ((style & 31) == SS_RIGHT)
    return RightAlign;
  else
    return LeftAlign;
}

/**
 * @warning You can't change the text-alignment of a label with the
 *          SS_SIMPLE style.
 */
void Label::setTextAlign(TextAlign align)
{
  int type = (getStyle().regular & 31);

  if (type == SS_LEFT ||
      type == SS_CENTER ||
      type == SS_RIGHT) {
    int style = SS_LEFT;

    switch (align) {
      case LeftAlign:   style = SS_LEFT;   break;
      case CenterAlign: style = SS_CENTER; break;
      case RightAlign:  style = SS_RIGHT;  break;
    }

    removeStyle(Style(SS_LEFT | SS_CENTER | SS_RIGHT, 0));
    addStyle(Style(style, 0));

    invalidate(true);
  }
}

Size Label::preferredSize()
{
  ScreenGraphics g;
  g.setFont(getFont());
  return g.measureString(getText());
}

/**
 * Uses Win32 GetTextExtentExPoint to returns the preferred size for
 * the "fitIn" size.
 */
Size Label::preferredSize(const Size &fitIn)
{
  // TODO HTHEME stuff
  // TODO SS_RIGHTJUST

  int style = getStyle().regular & 0xffff;

  if ((fitIn.w > 0) &&
      (((style & 31) != SS_SIMPLE) &&
       ((style & 31) != SS_LEFTNOWORDWRAP) &&
       ((style & SS_ELLIPSISMASK) == 0))) {
    ScreenGraphics g;
    g.setFont(getFont());
    return g.measureString(getText(), fitIn.w);
  }

  return preferredSize();
}

/**
 * If the label is resized, we must to redraw it. This is necessary
 * mainly if the Label isn't TextAlign::Left.
 */
void Label::onResize(const Size &sz)
{
  invalidate(true);
}
