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

#ifndef VACA_FONT_H
#define VACA_FONT_H

#include "Vaca/base.h"
#include "Vaca/String.h"

namespace Vaca {

/**
 * A text font.
 */
class VACA_DLL Font
{
public:

  struct Style {
    typedef int Type;
    enum {
      Regular   = 0,
      Bold      = 1,
      Italic    = 2,
      Underline = 4,
      Strikeout = 8,
    };
  };

private:

  static Font *defaultFont;

  HFONT mHfont;
  bool mDeletePtr;

public:

  Font();
  Font(const Font &font);
  Font(const Font &font, Font::Style::Type style);
  Font(String familyName, int size, Font::Style::Type style = Font::Style::Regular);
  explicit Font(HFONT hfont);
  Font(LPLOGFONT lplf);
  virtual ~Font();

  bool isValid();

  int getSize();
  Font::Style::Type getStyle();

  Font &operator=(const Font &font);
  void assign(const Font &font);
  void assign(HFONT hfont);
  void assign(LPLOGFONT lplf);

  static Font &getDefault();

//   FontMetrics getMetrics();

  HFONT getHfont();
  bool getLogFont(LPLOGFONT lplf) const;

};

} // namespace Vaca

#endif
