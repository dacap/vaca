// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_STATUSBAR_H
#define VACA_STATUSBAR_H

#include "vaca/base.h"
#include "vaca/Widget.h"

namespace vaca {

class VACA_DLL StatusBar : public Widget
{
public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  StatusBar(Widget* parent, Style style = Styles::Default);
  virtual ~StatusBar();

  virtual bool isLayoutFree() const;

protected:
  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);
  virtual void onLayout(LayoutEvent& ev);
};

} // namespace vaca

#endif // VACA_STATUSBAR_H
