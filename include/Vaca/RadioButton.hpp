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

#ifndef VACA_RADIOBUTTON_HPP
#define VACA_RADIOBUTTON_HPP

#include "Vaca/ButtonBase.hpp"

namespace Vaca {

#define RadioButtonStyle	(ChildStyle +			\
				 FocusableStyle +		\
				 Style(BS_RADIOBUTTON, 0))
  
/**
 * Groups a set of @ref RadioButton "RadioButtons". It's used to known
 * which RadioButton should be desactived when the user select a
 * RadioButton of its same group.
 */
class VACA_DLL RadioGroup
{
  static int radioGroupCounter;

  int m_groupId;

public:
  RadioGroup();
  RadioGroup(const RadioGroup &group);

  bool operator==(const RadioGroup &other) const;
};

/**
 * Handles a radio button.
 *
 * @warning It is not a BS_AUTORADIOBUTTON, it's only an
 *          BS_RADIOBUTTON (so you can't have the Q261192 bug).
 */
class VACA_DLL RadioButton : public ButtonBase
{
  RadioGroup mRadioGroup;

public:

  RadioButton(const String &text,
	      const RadioGroup &group,
	      Widget *parent,
	      Style style = RadioButtonStyle);
  virtual ~RadioButton();

  const RadioGroup &getRadioGroup() const;

protected:
  // reflection
  virtual bool onReflectedCommand(int id, int code, LRESULT &lResult);
};

} // namespace Vaca

#endif
