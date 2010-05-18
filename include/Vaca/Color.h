// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
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

#ifndef VACA_COLOR_H
#define VACA_COLOR_H

#include "Vaca/base.h"

namespace Vaca {

/**
   A color that can be used to create pens or brushes.
*/
class VACA_DLL Color
{
public:

  static const Color White;
  static const Color LightGray;
  static const Color Gray;
  static const Color DarkGray;
  static const Color Black;
  static const Color Red;
  static const Color Pink;
  static const Color Orange;
  static const Color Yellow;
  static const Color Green;
  static const Color Magenta;
  static const Color Cyan;
  static const Color Blue;

  Color();
  Color(const Color& color);
  Color(int r, int g, int b);
  virtual ~Color();

  int getR() const;
  int getG() const;
  int getB() const;

  Color negative();
  Color toBlackAndWhite();

  Color& operator=(const Color& color);

  bool operator==(const Color& color) const;
  bool operator!=(const Color& color) const;

  Color operator+(const Color& color) const;
  Color operator-(const Color& color) const;
  Color operator*(int value) const;
  Color operator/(int value) const;
  Color operator*(double value) const;
  Color operator/(double value) const;

private:
  unsigned int m_raw;
};

} // namespace Vaca

#endif // VACA_COLOR_H
