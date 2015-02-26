// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Style.h"

using namespace vaca;

// This constructor must be here to export const styles
// (e.g. Widget::Styles::Default, etc.)
Style::Style(int regular, int extended)
  : regular(regular),
    extended(extended)
{
}

Style::Style(const Style& style)
  : regular(style.regular),
    extended(style.extended)
{
}

Style Style::operator+(const Style& style) const
{
  return Style(regular | style.regular,
	       extended | style.extended);
}

Style Style::operator|(const Style& style) const
{
  return Style(regular | style.regular,
	       extended | style.extended);
}

Style Style::operator-(const Style& style) const
{
  return Style(regular & ~style.regular,
	       extended & ~style.extended);
}

Style Style::operator&(const Style& style) const
{
  return Style(regular & style.regular,
	       extended & style.extended);
}

Style Style::operator~() const
{
  return Style(~regular, ~extended);
}

bool Style::operator==(const Style& style) const
{
  return
    regular == style.regular &&
    extended == style.extended;
}

bool Style::operator!=(const Style& style) const
{
  return
    regular != style.regular ||
    extended != style.extended;
}
