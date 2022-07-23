// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_GROUPBOX_H
#define VACA_GROUPBOX_H

#include "vaca/Widget.h"

namespace vaca {

/**
   An edge (with a label optionally) that can be used to group
   sub-widgets.
*/
class VACA_DLL GroupBox : public Widget
{
public:

  struct VACA_DLL Styles {
    /**
       Default style for GroupBox widget.
    */
    static constexpr Style Default =
      Widget::Styles::Visible |
      Widget::Styles::Container |
      Style(BS_GROUPBOX, 0);
  };

  GroupBox(const String& text, Widget* parent, Style style = Styles::Default);
  virtual ~GroupBox();

  Size getNonClientSize();

protected:

  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);
  virtual void onLayout(LayoutEvent& ev);

  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

};

} // namespace vaca

#endif // VACA_GROUPBOX_H
