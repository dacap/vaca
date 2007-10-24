// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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
#include "Vaca/Font.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/Graphics.hpp"

using namespace Vaca;

Font *Font::defaultFont = NULL;

/**
 * Constructs an invalid font.
 */
Font::Font()
  : m_HFONT(NULL)
{
  m_autoDelete = false;
}

Font::Font(const Font &font)
{
  assign(font);
}

Font::Font(const Font &font, FontStyle style)
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
  : m_HFONT(NULL)
{
  ScreenGraphics g;
  LOGFONT lf;

  lf.lfHeight = -MulDiv(size, GetDeviceCaps(g.getHDC(), LOGPIXELSY), 72);
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
  : m_HFONT(NULL)
{
  assign(hfont);
}

Font::Font(LPLOGFONT lplf)
  : m_HFONT(NULL)
{
  assign(lplf);
}

Font::~Font()
{
  if (m_HFONT != NULL && m_autoDelete)
    DeleteObject(m_HFONT);

  if (this == Font::defaultFont)
    Font::defaultFont = NULL;
}

bool Font::isValid()
{
  return m_HFONT != NULL;
}

/**
 * Returns the size of the font in points.
 */
int Font::getPointSize()
{
  LOGFONT lf;
  if (getLogFont(&lf))
    return -lf.lfHeight * 72 / GetDeviceCaps(ScreenGraphics().getHDC(), LOGPIXELSY);
  else
    return -1;			// TODO error, document it
}

FontStyle Font::getStyle()
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

//   lf.lfHeight = -MulDiv(size, GetDeviceCaps(g.getHDC(), LOGPIXELSY), 72);
//   lf.lfWidth = 0;
//   lf.lfEscapement = 0;
//   lf.lfOrientation = 0;
//   lf.lfWeight = style & Font::Style::Bold ? FW_BOLD: FW_REGULAR;
//   lf.lfItalic = style & Font::Style::Italic;
//   lf.lfUnderline = style & Font::Style::Underline;
//   lf.lfStrikeOut = style & Font::Style::Strikeout;
//   lf.lfCharSet = DEFAULT_CHARSET;
//   lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
//   lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
//   lf.lfQuality = DEFAULT_QUALITY; //ANTIALIASED_QUALITY
//   lf.lfPitchAndFamily = DEFAULT_PITCH;

}

/**
 * Makes a copy of @a font. You can safely delete the @a font after
 * this method.
 */
Font &Font::operator=(const Font &font)
{
  assign(font);
  return *this;
}

void Font::assign(const Font &font)
{
  LOGFONT lf;
  if (font.getLogFont(&lf))
    assign(&lf);
}

void Font::assign(HFONT hfont)
{
  if (m_HFONT != NULL && m_autoDelete) {
    DeleteObject(m_HFONT);
    m_HFONT = NULL;
  }

  m_HFONT = hfont;
  m_autoDelete = false;
}

void Font::assign(LPLOGFONT lplf)
{
  if (m_HFONT != NULL && m_autoDelete) {
    DeleteObject(m_HFONT);
    m_HFONT = NULL;
  }

  m_HFONT = CreateFontIndirect(lplf);
  m_autoDelete = true;
}

// FontMetrics Font::getMetrics()
// {
//   return FontMetrics(this);
// }

Font *Font::getDefault()
{
  if (Font::defaultFont == NULL)
    Font::defaultFont = new Font(reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT)));

  assert(Font::defaultFont != NULL);

  return Font::defaultFont;
}

HFONT Font::getHFONT()
{
  return m_HFONT;
}

bool Font::getLogFont(LPLOGFONT lplf) const
{
  assert(m_HFONT != NULL);

  return GetObject(m_HFONT,
		   sizeof(LOGFONT),
		   reinterpret_cast<LPVOID>(lplf)) != 0;
}

void Font::deleteHandles()
{
  if (Font::defaultFont != NULL) {
    delete Font::defaultFont;
    Font::defaultFont = NULL;
  }
}

