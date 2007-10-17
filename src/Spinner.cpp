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
// #include "Vaca/Application.h"
#include "Vaca/Spinner.h"
// #include "Vaca/Debug.h"
// #include "Vaca/Event.h"

using namespace Vaca;

Spinner::Spinner(Widget *parent, Style spinStyle, Style style)
  : Widget(SpinnerClass::getClassName(), parent, style)
  , mEdit("", this)
  , mSpin(this, spinStyle)
{
  mSpin.setBuddy(&mEdit);
}

Spinner::Spinner(int minValue, int maxValue, int posValue,
		 Widget *parent, Style spinStyle, Style style)
  : Widget(SpinnerClass::getClassName(), parent, style)
  , mEdit("", this)
  , mSpin(minValue, maxValue, posValue, this, spinStyle)
{
  mSpin.setBuddy(&mEdit);
}

Spinner::~Spinner()
{
}

Size Spinner::preferredSize()
{
  Size edit(mEdit.preferredSize());
  Size spin(mSpin.preferredSize());
  return Size(edit.w - 2 + spin.w,
	      VACA_MAX(edit.h, spin.h));
}

Edit &Spinner::getEdit()
{
  return mEdit;
}

SpinButton &Spinner::getSpinButton()
{
  return mSpin;
}

int Spinner::getMinimum()
{
  return mSpin.getMinimum();
}

int Spinner::getMaximum()
{
  return mSpin.getMaximum();
}

void Spinner::getRange(int &minValue, int &maxValue)
{
  mSpin.getRange(minValue, maxValue);
}

void Spinner::setRange(int minValue, int maxValue)
{
  mSpin.setRange(minValue, maxValue);
}

int Spinner::getValue()
{
  return mSpin.getValue();
}

void Spinner::setValue(int posValue)
{
  mSpin.setValue(posValue);
}

/**
 * Returns the radix base used in Edit widget.
 */
int Spinner::getBase()
{
  return mSpin.getBase();
}

/**
 * Sets the radix base to be used in the Edit widget. This can be 10
 * for decimal or 16 for hexadecimal.
 */
void Spinner::setBase(int base)
{
  mSpin.setBase(base);
}

void Spinner::layout()
{
  Rect bounds(getLayoutBounds());
  Size spin(mSpin.preferredSize());

  mEdit.setBounds(Rect(bounds.x,
		       bounds.y,
		       bounds.w-spin.w+2,
		       bounds.h));

  mSpin.setBounds(Rect(bounds.x+bounds.w-spin.w,
		       bounds.y,
		       spin.w,
		       bounds.h));
}
