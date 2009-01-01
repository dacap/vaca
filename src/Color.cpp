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

#include "Vaca/Color.h"

using namespace Vaca;

/**
 * White color, RGB(255,255,255) or \#ffffff.
 */
const Color Color::White(0xff, 0xff, 0xff);

/**
 * Light gray color, RGB(192,192,192) or \#c0c0c0.
 */
const Color Color::LightGray(0xc0, 0xc0, 0xc0);

/**
 * Gray color, RGB(128,128,128) or \#808080
 */
const Color Color::Gray(0x80, 0x80, 0x80);

/**
 * Dark gray color, RGB(64,64,64) or \#404040
 */
const Color Color::DarkGray(0x40, 0x40, 0x40);

/**
 * Black color, RGB(0,0,0) or \#000000.
 */
const Color Color::Black(0x00, 0x00, 0x00);

/**
 * Red color, RGB(255,0,0) or \#ff0000.
 */
const Color Color::Red(0xff, 0x00, 0x00);

/**
 * Pink color, RGB(255,175,175) or \#ffafaf.
 */
const Color Color::Pink(0xff, 0xaf, 0xaf);

/**
 * Orange color, RGB(255,200,0) or \#ffc800.
 */
const Color Color::Orange(0xff, 0xc8, 0x00);

/**
 * Yellow color, RGB(255,255,0) or \#ffff00.
 */
const Color Color::Yellow(0xff, 0xff, 0x00);

/**
 * Green color, RGB(0,255,0) or \#00ff00.
 */
const Color Color::Green(0x00, 0xff, 0x00);

/**
 * Magenta color, RGB(255,0,255) or \#ff00ff.
 */
const Color Color::Magenta(0xff, 0x00, 0xff);

/**
 * Cyan color, RGB(0,255,255) or \#00ffff.
 */
const Color Color::Cyan(0x00, 0xff, 0xff);

/**
 * Blue color, RGB(0,0,255) or \#0000ff.
 */
const Color Color::Blue(0x00, 0x00, 0xff);

/**
 * Creates a null color (all 0, it is black really).
 */
Color::Color()
{
  m_colorRef = 0;
}

Color::Color(const Color& color)
{
  m_colorRef = color.m_colorRef;
}

/**
 * Creates a RGB color.
 *
 * @param r Red component (0-255).
 * @param g Green component (0-255).
 * @param b Blue component (0-255).
 */
Color::Color(int r, int g, int b)
{
  m_colorRef = RGB(r, g, b);
}

Color::~Color()
{
}

/**
 * Returns the red component of this color.
 */
int Color::getR() const
{
  return GetRValue(m_colorRef);
}

/**
 * Returns the green component of this color.
 */
int Color::getG() const
{
  return GetGValue(m_colorRef);
}

/**
 * Returns the blue component of this color.
 */
int Color::getB() const
{
  return GetBValue(m_colorRef);
}

Color Color::negative()
{
  return Color(255-getR(), 255-getG(), 255-getB());
}

Color Color::toBlackAndWhite()
{
  return (getR()*30+getG()*59+getB()*11)/100 < 128 ? Color::Black:
						     Color::White;
}

Color& Color::operator=(const Color& color)
{
  m_colorRef = color.m_colorRef;
  return *this;
}

bool Color::operator==(const Color& color) const
{
  return m_colorRef == color.m_colorRef;
}

bool Color::operator!=(const Color& color) const
{
  return m_colorRef != color.m_colorRef;
}

/**
 * Adds to each RGB components of this color the RGB components of @c color.
 */
Color Color::operator+(const Color& color) const
{
  int r = getR()+color.getR();
  int g = getG()+color.getG();
  int b = getB()+color.getB();
  
  return Color(r < 0 ? 0: (r > 255 ? 255: r),
	       g < 0 ? 0: (g > 255 ? 255: g),
	       b < 0 ? 0: (b > 255 ? 255: b));
}

/**
 * Substract to each RGB components of this color the RGB components of @c color.
 */
Color Color::operator-(const Color& color) const
{
  int r = getR()-color.getR();
  int g = getG()-color.getG();
  int b = getB()-color.getB();
  
  return Color(r < 0 ? 0: (r > 255 ? 255: r),
	       g < 0 ? 0: (g > 255 ? 255: g),
	       b < 0 ? 0: (b > 255 ? 255: b));
}

/**
 * Multiplies each RGB components by the @c value.
 */
Color Color::operator*(int value) const
{
  int r = getR()*value;
  int g = getG()*value;
  int b = getB()*value;
  
  return Color(r < 0 ? 0: (r > 255 ? 255: r),
	       g < 0 ? 0: (g > 255 ? 255: g),
	       b < 0 ? 0: (b > 255 ? 255: b));
}

/**
 * Divides each RGB components by the @c value.
 */
Color Color::operator/(int value) const
{
  int r = getR()/value;
  int g = getG()/value;
  int b = getB()/value;
  
  return Color(r < 0 ? 0: (r > 255 ? 255: r),
	       g < 0 ? 0: (g > 255 ? 255: g),
	       b < 0 ? 0: (b > 255 ? 255: b));
}

/**
 * Multiplies each RGB components by the @c value.
 */
Color Color::operator*(double value) const
{
  int r = static_cast<int>(getR()*value);
  int g = static_cast<int>(getG()*value);
  int b = static_cast<int>(getB()*value);
  
  return Color(r < 0 ? 0: (r > 255 ? 255: r),
	       g < 0 ? 0: (g > 255 ? 255: g),
	       b < 0 ? 0: (b > 255 ? 255: b));
}

/**
 * Divides each RGB components by the @c value.
 */
Color Color::operator/(double value) const
{
  int r = static_cast<int>(getR()/value);
  int g = static_cast<int>(getG()/value);
  int b = static_cast<int>(getB()/value);
  
  return Color(r < 0 ? 0: (r > 255 ? 255: r),
	       g < 0 ? 0: (g > 255 ? 255: g),
	       b < 0 ? 0: (b > 255 ? 255: b));
}

Color::Color(COLORREF rgb)
{
  m_colorRef = rgb;
}

COLORREF Color::getColorRef() const
{
  return m_colorRef;
}
