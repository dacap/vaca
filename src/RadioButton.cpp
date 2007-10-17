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

#include "stdvaca.h"
#include "Vaca/RadioButton.h"
#include "Vaca/Debug.h"
#include "Vaca/Event.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// RadioButton

RadioButton::RadioButton(const String &text, const RadioGroup &group, Widget *parent, Style style)
  : ButtonBase(parent, style)
  , mRadioGroup(group)
{
  setText(text);
}

RadioButton::~RadioButton()
{
}

const RadioGroup &RadioButton::getRadioGroup() const
{
  return mRadioGroup;
}

bool RadioButton::onCommand(int id, int code, LRESULT &lResult)
{
  if (code == BN_CLICKED) {
    Widget::Container siblings = getParent()->getChildren();
    RadioGroup radioGroup(getRadioGroup());

    for (Widget::Container::iterator it=siblings.begin(); it!=siblings.end(); ++it) {
      RadioButton *sibling = dynamic_cast<RadioButton *>(*it);

      if (sibling != NULL && sibling != this) {
	if (sibling->getRadioGroup() == radioGroup &&
	    sibling->isSelected()) {
	  sibling->setSelected(false);
	}
      }
    }

    setSelected(true);
  }

  return ButtonBase::onCommand(id, code, lResult);
}

//////////////////////////////////////////////////////////////////////
// RadioGroup

int RadioGroup::radioGroupCounter = 0;

RadioGroup::RadioGroup()
{
  mGroupId = ++RadioGroup::radioGroupCounter;
}

RadioGroup::RadioGroup(const RadioGroup &group)
{
  mGroupId = group.mGroupId;
}

bool RadioGroup::operator==(const RadioGroup &other) const
{
  return mGroupId == other.mGroupId;
}
