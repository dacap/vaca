// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_CUSTOMBUTTON_H
#define VACA_CUSTOMBUTTON_H

#include "vaca/Button.h"

namespace vaca {

/**
   Customizable button to draw it with your own Widget#onPaint member function.

   Example:
   @code
   class MyFancyButton : public CustomButton
   {
   public:
     MyFancyButton(const String& text, Widget* parent)
       : CustomButton(text, parent)
     {
       // ...
     }

     // paint event...
     virtual void onPaint(PaintEvent& ev)
     {
       // don't call parent implementation, use your own code to
       // paint your customized button
     }
   }
   @endcode

   @win32
     It is a button with the @msdn{BS_OWNERDRAW} style, the paint event
     is intercepted by the parent of the CustomButton and then reflected
     to it (see @ref page_tn_009).
   @endwin32
*/
class VACA_DLL CustomButton : public Button
{
  UINT m_itemAction;
  UINT m_itemState;

public:

  struct Styles {
    /**
       Default style for CustomButton.

       @win32
       WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_OWNERDRAW
       @endwin32
    */
    static constexpr Style Default =
      Button::Styles::Default |
      Style(BS_OWNERDRAW, 0);
  };

  CustomButton(const String& text, Widget* parent, Style style = Styles::Default);
  virtual ~CustomButton();

protected:

  // Reflected notifications
  virtual bool onReflectedDrawItem(Graphics& g, LPDRAWITEMSTRUCT lpDrawItem);

public:

  bool isDrawEntire();
  bool isFocusChanged();
  bool isSelectionChanged();

  // Visual Aspect

  // bool hasCheckedVisualAspect();
  bool hasDefaultOptionVisualAspect();
  bool hasDisabledVisualAspect();
  bool hasFocusVisualAspect();
  // bool hasGrayedVisualAspect();
  // bool hasHotLightVisualAspect();
  // bool hasInactiveVisualAspect();
  bool hasNoAccelVisualAspect();
  bool hasNoFocusRectVisualAspect();
  bool hasSelectedVisualAspect();

};

} // namespace vaca

#endif // VACA_CUSTOMBUTTON_H
