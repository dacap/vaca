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

#ifndef VACA_FONT_HPP
#define VACA_FONT_HPP

#include "Vaca/base.hpp"
#include "Vaca/String.hpp"

namespace Vaca {

class Application;

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
 * - Regular
 * - Bold
 * - Italic
 * - Underline
 * - Strikeout
 */
typedef EnumSet<FontStyleEnumSet> FontStyle;

//////////////////////////////////////////////////////////////////////

/**
 * A font to be used in Graphics context.
 */
class VACA_DLL Font
{
  friend class Application;

private:

  static Font *defaultFont;

  HFONT m_HFONT;
  bool m_autoDelete;

public:

  Font();
  Font(const Font &font);
  Font(const Font &font, FontStyle style);
  Font(String familyName, int size, FontStyle style = FontStyle::Regular);
  explicit Font(HFONT hfont);
  Font(LPLOGFONT lplf);
  virtual ~Font();

  bool isValid();

  int getPointSize();
  FontStyle getStyle();

  Font &operator=(const Font &font);
  void assign(const Font &font);
  void assign(HFONT hfont);
  void assign(LPLOGFONT lplf);

  static Font *getDefault();

//   FontMetrics getMetrics();

  HFONT getHFONT();
  bool getLogFont(LPLOGFONT lplf) const;

private:

  static void deleteHandles();

};

} // namespace Vaca

#endif
