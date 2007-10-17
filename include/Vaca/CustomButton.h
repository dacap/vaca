// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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
// * Neither the name of the Vaca nor the names of its contributors
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

#ifndef VACA_CUSTOMBUTTON_H
#define VACA_CUSTOMBUTTON_H

#include "Vaca/Button.h"

namespace Vaca {

/**
 * Default style for CustomButton: a ButtonStyle with BS_OWNERDRAW.
 */
#define CustomButtonStyle	(ButtonStyle + Style(BS_OWNERDRAW, 0))

/**
 * Customizable button to draw it with your own onPaint method. It's
 * like the BS_OWNERDRAW, but here the paint event is received by the
 * button itself, and not by the parent window (see @ref TN009).
 */
class VACA_DLL CustomButton : public Button
{
  UINT mItemAction;
  UINT mItemState;

public:

  CustomButton(const String &text, Widget *parent, Style style = CustomButtonStyle);
  virtual ~CustomButton();

protected:

  virtual bool onDrawItem(Graphics &g, LPDRAWITEMSTRUCT lpDrawItem);

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

} // namespace Vaca

#endif