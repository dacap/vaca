// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_CUSTOMLABEL_H
#define VACA_CUSTOMLABEL_H

#include "vaca/base.h"
#include "vaca/Label.h"

namespace vaca {

/**
   Customized static label control.
*/
class VACA_DLL CustomLabel : public Label
{
  // SS_OWNERDRAW has the bit 1, so we can't use the
  // SS_CENTER/SS_RIGHT to known the text-alignment. We must to hold
  // on in some place.
  TextAlign m_textAlign;

public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  CustomLabel(const String& text, Widget* parent, Style style = Styles::Default);
  virtual ~CustomLabel();

  virtual TextAlign getTextAlign() const;
  virtual void setTextAlign(TextAlign align);

protected:

  // Reflected notifications
  virtual bool onReflectedDrawItem(Graphics& g, LPDRAWITEMSTRUCT lpDrawItem);

};

} // namespace vaca

#endif // VACA_CUSTOMLABEL_H
