// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_COLOR_H
#define VACA_COLOR_H

#include "vaca/base.h"

namespace vaca {

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

} // namespace vaca

#endif // VACA_COLOR_H
