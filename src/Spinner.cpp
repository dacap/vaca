// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, 2009, David Capello
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
// * Neither the name of the author nor the names of its contributors
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

#include "Vaca/Spinner.h"

using namespace Vaca;

Spinner::Spinner(Widget* parent, Style spinStyle, Style style)
  : Widget(SpinnerClass::getClassName(), parent, style)
  , m_edit(L"", this)
  , m_spin(this, spinStyle)
{
  m_spin.setBuddy(&m_edit);
}

Spinner::Spinner(int minValue, int maxValue, int posValue,
		 Widget* parent, Style spinStyle, Style style)
  : Widget(SpinnerClass::getClassName(), parent, style)
  , m_edit(L"", this)
  , m_spin(minValue, maxValue, posValue, this, spinStyle)
{
  m_spin.setBuddy(&m_edit);
}

Spinner::~Spinner()
{
}

TextEdit& Spinner::getTextEdit()
{
  return m_edit;
}

SpinButton& Spinner::getSpinButton()
{
  return m_spin;
}

int Spinner::getMinimum()
{
  return m_spin.getMinimum();
}

int Spinner::getMaximum()
{
  return m_spin.getMaximum();
}

void Spinner::getRange(int& minValue, int& maxValue)
{
  m_spin.getRange(minValue, maxValue);
}

void Spinner::setRange(int minValue, int maxValue)
{
  m_spin.setRange(minValue, maxValue);
}

int Spinner::getValue()
{
  return m_spin.getValue();
}

void Spinner::setValue(int posValue)
{
  m_spin.setValue(posValue);
}

/**
 * Returns the radix base used in TextEdit widget.
 */
int Spinner::getBase()
{
  return m_spin.getBase();
}

/**
 * Sets the radix base to be used in the TextEdit widget. This can be
 * 10 for decimal or 16 for hexadecimal.
 */
void Spinner::setBase(int base)
{
  m_spin.setBase(base);
}

/**
 * Positions the children: the spin button at the right of the edit
 * control to connect theirs edges and to see it like a whole control.
 */
void Spinner::layout()
{
  Rect bounds = getLayoutBounds();
  Size spin = m_spin.getPreferredSize();

  m_edit.setBounds(Rect(bounds.x,
		       bounds.y,
		       bounds.w-spin.w+2,
		       bounds.h));

  m_spin.setBounds(Rect(bounds.x+bounds.w-spin.w,
		       bounds.y,
		       spin.w,
		       bounds.h));
}

/**
 * Fills @a sz with the size of both controls for the width and the
 * maximun height of both controls.
 */
void Spinner::onPreferredSize(Size& sz)
{
  Size edit(m_edit.getPreferredSize());
  Size spin(m_spin.getPreferredSize());

  sz = Size(edit.w - 2 + spin.w,
	    max_value(edit.h, spin.h));
}
