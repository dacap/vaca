// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Spinner.h"
#include "vaca/PreferredSizeEvent.h"
#include "vaca/LayoutEvent.h"

using namespace vaca;

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
   Returns the radix base used in TextEdit widget.
*/
int Spinner::getBase()
{
  return m_spin.getBase();
}

/**
   Sets the radix base to be used in the TextEdit widget. This can be
   10 for decimal or 16 for hexadecimal.
*/
void Spinner::setBase(int base)
{
  m_spin.setBase(base);
}

/**
   Fills @a sz with the size of both controls for the width and the
   maximun height of both controls.
*/
void Spinner::onPreferredSize(PreferredSizeEvent& ev)
{
  Size edit(m_edit.getPreferredSize());
  Size spin(m_spin.getPreferredSize());

  ev.setPreferredSize(edit.w - 2 + spin.w,
		      max_value(edit.h, spin.h));
}

/**
   Positions the children: the spin button at the right of the edit
   control to connect theirs edges and to see it like a whole control.
*/
void Spinner::onLayout(LayoutEvent& ev)
{
  Rect bounds = ev.getBounds();
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
