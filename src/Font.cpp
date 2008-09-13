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

#include "Vaca/Font.h"
#include "Vaca/Debug.h"
#include "Vaca/Graphics.h"

using namespace Vaca;

/**
 * Constructs the default font.
 */
Font::Font()
  : SmartPtr<GdiObject<HFONT> >(new GdiObject<HFONT>(reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT))))
{
}

/**
 * Makes a reference to the specified font.
 */
Font::Font(const Font& font)
  : SmartPtr<GdiObject<HFONT> >(font)
{
}

/**
 * Makes a copy of the font changing it's style.
 */
Font::Font(const Font& font, FontStyle style)
{
  LOGFONT lf;
  if (font.getLogFont(&lf)) {
    lf.lfWeight = style & FontStyle::Bold ? FW_BOLD: FW_REGULAR;
    lf.lfItalic = style & FontStyle::Italic ? TRUE: FALSE;
    lf.lfUnderline = style & FontStyle::Underline ? TRUE: FALSE;
    lf.lfStrikeOut = style & FontStyle::Strikeout ? TRUE: FALSE;

    assign(&lf);
  }
}

Font::Font(String familyName, int size, FontStyle style)
{
  ScreenGraphics g;
  LOGFONT lf;

  lf.lfHeight = -MulDiv(size, GetDeviceCaps(g.getHandle(), LOGPIXELSY), 72);
  lf.lfWidth = 0;
  lf.lfEscapement = 0;
  lf.lfOrientation = 0;
  lf.lfWeight = style & FontStyle::Bold ? FW_BOLD: FW_REGULAR;
  lf.lfItalic = style & FontStyle::Italic ? TRUE: FALSE;
  lf.lfUnderline = style & FontStyle::Underline ? TRUE: FALSE;
  lf.lfStrikeOut = style & FontStyle::Strikeout ? TRUE: FALSE;
  lf.lfCharSet = DEFAULT_CHARSET;
  lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
  lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  lf.lfQuality = DEFAULT_QUALITY; //ANTIALIASED_QUALITY
  lf.lfPitchAndFamily = DEFAULT_PITCH;

  familyName.copyTo(lf.lfFaceName, LF_FACESIZE);

  assign(&lf);
}

/**
 * Wrapper constructor for HFONT.
 */
Font::Font(HFONT hfont)
  : SmartPtr<GdiObject<HFONT> >(new GdiObject<HFONT>(hfont))
{
}

Font::Font(LPLOGFONT lplf)
{
  assign(lplf);
}

Font::~Font()
{
}

/**
 * Returns the size of the font in points.
 *
 * @return -1 if there is an error getting the point size.
 */
int Font::getPointSize() const
{
  LOGFONT lf;
  if (getLogFont(&lf))
    return -lf.lfHeight * 72 / GetDeviceCaps(ScreenGraphics().getHandle(), LOGPIXELSY);
  else
    return -1;
}

FontStyle Font::getStyle() const
{
  LOGFONT lf;
  if (getLogFont(&lf)) {
    FontStyle style;
    if (lf.lfWeight == FW_BOLD)  style |= FontStyle::Bold;
    if (lf.lfItalic != FALSE)    style |= FontStyle::Italic;
    if (lf.lfUnderline != FALSE) style |= FontStyle::Underline;
    return style;
  }
  else
    return FontStyle::Regular;			// TODO error, document it
}

/**
 * Makes a copy of @a font. You can safely delete the @a font after
 * this method.
 */
Font& Font::operator=(const Font& font)
{
  SmartPtr<GdiObject<HFONT> >::operator=(font);
  return *this;
}

void Font::assign(LPLOGFONT lplf)
{
  SmartPtr<GdiObject<HFONT> >::operator=(Font(CreateFontIndirect(lplf)));
}

HFONT Font::getHandle() const
{
  return get()->getHandle();
}

bool Font::getLogFont(LPLOGFONT lplf) const
{
  assert(get()->isValid());
  return GetObject(getHandle(),
		   sizeof(LOGFONT),
		   reinterpret_cast<LPVOID>(lplf)) != 0;
}
