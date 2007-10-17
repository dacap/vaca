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

#ifndef VACA_CUSTOMLABEL_H
#define VACA_CUSTOMLABEL_H

#include "Vaca/base.h"
#include "Vaca/Label.h"

namespace Vaca {

/**
 * Default style for CustomLabel: a LabelStyle with SS_OWNERDRAW.
 */
#define CustomLabelStyle	(LabelStyle + Style(SS_OWNERDRAW, 0))

/**
 * Customized static label control.
 */
class VACA_DLL CustomLabel : public Label
{
  /**
   * SS_OWNERDRAW has the bit 1, so we can't use the
   * SS_CENTER/SS_RIGHT to known the text-alignment. We must to hold
   * on in some place.
   */
  TextAlign mTextAlign;

public:

  CustomLabel(const String &text, Widget *parent, Style style = CustomLabelStyle);
  virtual ~CustomLabel();

  virtual TextAlign getTextAlign();
  virtual void setTextAlign(TextAlign align);

protected:

  virtual bool onDrawItem(Graphics &g, LPDRAWITEMSTRUCT lpDrawItem);

};

} // namespace Vaca

#endif