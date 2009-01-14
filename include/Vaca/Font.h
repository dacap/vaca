// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, 2009, David Capello
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

#ifndef VACA_FONT_H
#define VACA_FONT_H

#include "Vaca/base.h"
#include "Vaca/SharedPtr.h"
#include "Vaca/GdiObject.h"

namespace Vaca {

//////////////////////////////////////////////////////////////////////

/**
 * @see FontStyle
 */
struct FontStyleEnumSet
{
  enum enumeration {
    Regular   = 0,
    Bold      = 1,
    Italic    = 2,
    Underline = 4,
    Strikeout = 8,
  };
};

/**
 * A FontStyle can be one of the following values:
 * @li FontStyle::Regular
 * @li FontStyle::Bold
 * @li FontStyle::Italic
 * @li FontStyle::Underline
 * @li FontStyle::Strikeout
 */
typedef EnumSet<FontStyleEnumSet> FontStyle;

//////////////////////////////////////////////////////////////////////

/**
 * Dimensions of a Font.
 *
 * @see Graphics#getFontMetrics
 */
class VACA_DLL FontMetrics
{
  friend class Graphics;

  TEXTMETRIC m_textMetric;

public:
  FontMetrics() { }
  FontMetrics(const FontMetrics& fm) : m_textMetric(fm.m_textMetric) { }
  int getHeight() const { return m_textMetric.tmHeight; }
  int getAscent() const { return m_textMetric.tmAscent; }
  int getDescent() const { return m_textMetric.tmDescent; }
  int getAverageCharWidth() const { return m_textMetric.tmAveCharWidth; }
  int getMaximumCharWidth() const { return m_textMetric.tmMaxCharWidth; }
  int getLeading() const { return m_textMetric.tmInternalLeading; }
};

/**
 * A shared pointer to a font that can be used in Graphics or Widget to
 * draw text.
 * 
 * This is a SharedPtr, so if you copy instances of fonts they will be
 * referencing to the same place. You can't clone fonts because you
 * can't modify them.
 *
 * @win32
 *   This is a @msdn{HFONT} wrapper.
 * @endwin32
 *
 * @see Graphics#setFont, Graphics#drawString
 */
class VACA_DLL Font : private SharedPtr<GdiObject<HFONT> >
{
  friend class Application;

public:

  Font();
  Font(const Font& font);
  Font(const Font& font, FontStyle style);
  Font(String familyName, int size, FontStyle style = FontStyle::Regular);
  explicit Font(HFONT hfont);
  Font(LPLOGFONT lplf);
  virtual ~Font();

  int getPointSize() const;
  FontStyle getStyle() const;

  Font& operator=(const Font& font);

  HFONT getHandle() const;
  bool getLogFont(LPLOGFONT lplf) const;

private:
  void assign(LPLOGFONT lplf);
};

} // namespace Vaca

#endif // VACA_FONT_H
