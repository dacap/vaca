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

#ifndef VACA_PANEL_H
#define VACA_PANEL_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/Register.h"

namespace Vaca {

#define PanelStyle		(ChildStyle +			\
				 ClipChildrenStyle +		\
				 Style(0, WS_EX_CONTROLPARENT))

class PanelClass : public WidgetClass
{
public:
  static LPCTSTR getClassName() { return _T("Vaca.Panel"); }
};

/**
 * A basic widget. You can derive from this widget to make your own
 * widgets. Also you don't need to register a WidgetClass because it
 * uses the PanelClass.
 */
class VACA_DLL Panel : public Register<PanelClass>, public Widget
{
public:

  Panel(Widget *parent, Style style = PanelStyle);
  Panel(LPCTSTR className, Widget *parent, Style style = PanelStyle);
  virtual ~Panel();

};

} // namespace Vaca

#endif
