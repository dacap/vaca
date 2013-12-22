// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/Font.h"
#include "Vaca/Debug.h"
#include "Vaca/Graphics.h"
#include "Vaca/String.h"

using namespace Vaca;

/**
   Constructs the default font.
*/
Font::Font()
  : SharedPtr<GdiObject<HFONT> >(new GdiObject<HFONT>(reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT))))
{
}

/**
   Makes a reference to the specified font.
*/
Font::Font(const Font& font)
  : SharedPtr<GdiObject<HFONT> >(font)
{
}

/**
   Makes a copy of the font changing it's style.
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

  copy_string_to(familyName, lf.lfFaceName, LF_FACESIZE);

  assign(&lf);
}

/**
   Wrapper constructor for HFONT.
*/
Font::Font(HFONT hfont)
  : SharedPtr<GdiObject<HFONT> >(new GdiObject<HFONT>(hfont))
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
   Returns the size of the font in points.

   @return -1 if there is an error getting the point size.
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
   Makes a copy of @a font.

   You can safely delete the @a font after calling this member function.
*/
Font& Font::operator=(const Font& font)
{
  SharedPtr<GdiObject<HFONT> >::operator=(font);
  return *this;
}

void Font::assign(LPLOGFONT lplf)
{
  SharedPtr<GdiObject<HFONT> >::operator=(Font(CreateFontIndirect(lplf)));
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
