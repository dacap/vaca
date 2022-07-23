// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SEPARATOR_H
#define VACA_SEPARATOR_H

#include "vaca/base.h"
#include "vaca/Widget.h"

namespace vaca {

class VACA_DLL Separator : public Widget
{
public:

  struct VACA_DLL Styles {
    static constexpr Style Default =
      Widget::Styles::Visible |
      Style(SS_NOTIFY | SS_SUNKEN, 0);
  };

  Separator(Widget* parent, Style style = Styles::Default);
  virtual ~Separator();

protected:

  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);

};

} // namespace vaca

#endif // VACA_SEPARATOR_H
