// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

#ifndef VACA_SPINNER_H
#define VACA_SPINNER_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/Register.h"
#include "Vaca/Edit.h"
#include "Vaca/SpinButton.h"

namespace Vaca {

#define SpinnerStyle		(ChildStyle +		\
				 ContainerStyle)

/**
 * Win32 class used by the Spinner class.
 */
class SpinnerClass : public WidgetClass
{
public:
  static LPCTSTR getClassName() { return _T("Vaca.Spinner"); }
};

/**
 * A Spinner is a couple of Widgets: an Edit and a SpinButton at the
 * right side.
 *
 * The default range is from 0 to 100. The default position is 0.
 */
class VACA_DLL Spinner : public Register<SpinnerClass>, public Widget
{
  Edit m_edit;
  SpinButton m_spin;
  
public:

  Spinner(Widget *parent,
	  Style spinStyle = SpinButtonStyle,
	  Style style = SpinnerStyle);
  Spinner(int minValue, int maxValue, int value,
	  Widget *parent,
	  Style spinStyle = SpinButtonStyle,
	  Style style = SpinnerStyle);
  virtual ~Spinner();

  Edit &getEdit();
  SpinButton &getSpinButton();

  int getMinimum();
  int getMaximum();
  void getRange(int &minValue, int &maxValue);
  void setRange(int minValue, int maxValue);

  int getValue();
  void setValue(int value);

  int getBase();
  void setBase(int base);

  virtual void layout();

protected:

  // events
  virtual void onPreferredSize(Size &sz);
  
};

} // namespace Vaca

#endif
