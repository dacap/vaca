// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/CustomLabel.h"
#include "vaca/Debug.h"

using namespace vaca;

CustomLabel::CustomLabel(const String& text, Widget* parent, Style style)
  : Label(text, parent, style)
  , m_textAlign(TextAlign::Left)
{
}

CustomLabel::~CustomLabel()
{
}

TextAlign CustomLabel::getTextAlign() const
{
  return m_textAlign;
}

void CustomLabel::setTextAlign(TextAlign align)
{
  m_textAlign = align;
  invalidate(true);
}

bool CustomLabel::onReflectedDrawItem(Graphics& g, LPDRAWITEMSTRUCT lpDrawItem)
{
  assert(lpDrawItem->CtlType == ODT_STATIC);

  return doPaint(g);
}
