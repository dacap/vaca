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
#include "Vaca/Font.h"
#include "Vaca/Debug.h"
#include "Vaca/Graphics.h"

using namespace Vaca;

Font *Font::defaultFont;

/**
 * Constructs an invalid font.
 */
Font::Font()
  : mHFONT(NULL)
{
  mAutoDelete = false;
}

Font::Font(const Font &font)
{
  assign(font);
}

Font::Font(const Font &font, Font::Style::Type style)
{
  LOGFONT lf;
  if (font.getLogFont(&lf)) {
    lf.lfWeight = style & Font::Style::Bold ? FW_BOLD: FW_REGULAR;
    lf.lfItalic = style & Font::Style::Italic;
    lf.lfUnderline = style & Font::Style::Underline;
    lf.lfStrikeOut = style & Font::Style::Strikeout;

    assign(&lf);
  }
}

Font::Font(String familyName, int size, Font::Style::Type style)
  : mHFONT(NULL)
{
  ScreenGraphics g;
  LOGFONT lf;

  lf.lfHeight = -MulDiv(size, GetDeviceCaps(g.getHDC(), LOGPIXELSY), 72);
  lf.lfWidth = 0;
  lf.lfEscapement = 0;
  lf.lfOrientation = 0;
  lf.lfWeight = style & Font::Style::Bold ? FW_BOLD: FW_REGULAR;
  lf.lfItalic = style & Font::Style::Italic;
  lf.lfUnderline = style & Font::Style::Underline;
  lf.lfStrikeOut = style & Font::Style::Strikeout;
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
  : mHFONT(NULL)
{
  assign(hfont);
}

Font::Font(LPLOGFONT lplf)
  : mHFONT(NULL)
{
  assign(lplf);
}

Font::~Font()
{
  if (mHFONT != NULL && mAutoDelete)
    DeleteObject(mHFONT);

  if (this == Font::defaultFont)
    Font::defaultFont = NULL;
}

bool Font::isValid()
{
  return mHFONT != NULL;
}

/**
 * Returns the size of the font in points.
 */
int Font::getSize()
{
  LOGFONT lf;
  if (getLogFont(&lf))
    return -lf.lfHeight * 72 / GetDeviceCaps(ScreenGraphics().getHDC(), LOGPIXELSY);
  else
    return -1;			// TODO error, document it
}

Font::Style::Type Font::getStyle()
{
  LOGFONT lf;
  if (getLogFont(&lf)) {
    Font::Style::Type style = 0
      | (lf.lfWeight == FW_BOLD ? Font::Style::Bold: 0)
      | (lf.lfItalic ? Font::Style::Italic: 0)
      | (lf.lfUnderline ? Font::Style::Underline: 0)
      ; 
    return style;
  }
  else
    return 0;			// TODO error, document it

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
  if (mHFONT != NULL && mAutoDelete) {
    DeleteObject(mHFONT);
    mHFONT = NULL;
  }

  mHFONT = hfont;
  mAutoDelete = false;
}

void Font::assign(LPLOGFONT lplf)
{
  if (mHFONT != NULL && mAutoDelete) {
    DeleteObject(mHFONT);
    mHFONT = NULL;
  }

  mHFONT = CreateFontIndirect(lplf);
  mAutoDelete = true;
}

// FontMetrics Font::getMetrics()
// {
//   return FontMetrics(this);
// }

Font &Font::getDefault()
{
  if (!Font::defaultFont)
    Font::defaultFont = new Font(reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT)));

  assert(Font::defaultFont != NULL);

  return *Font::defaultFont;
}

HFONT Font::getHFONT()
{
  return mHFONT;
}

bool Font::getLogFont(LPLOGFONT lplf) const
{
  assert(mHFONT != NULL);

  return GetObject(mHFONT,
		   sizeof(LOGFONT),
		   reinterpret_cast<LPVOID>(lplf)) != 0;
}
