// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#include "Vaca/RadioButton.h"
#include "Vaca/Debug.h"
#include "Vaca/Event.h"
#include "Vaca/Mutex.h"
#include "Vaca/ScopedLock.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// RadioButton

RadioButton::RadioButton(const String& text, RadioGroup& group, Widget* parent, Style style)
  : ButtonBase(parent, style)
  , m_radioGroup(group)
{
  m_radioGroup.addMember(this);
  setText(text);
}

RadioButton::~RadioButton()
{
  m_radioGroup.removeMember(this);
}

const RadioGroup& RadioButton::getRadioGroup() const
{
  return m_radioGroup;
}

void RadioButton::onAction(Event& ev)
{
  ButtonBase::onAction(ev);
  m_radioGroup.onChange(ev);
}

bool RadioButton::onReflectedCommand(int id, int code, LRESULT& lResult)
{
  if (code == BN_CLICKED) {
    // deselect all siblings RadioButtons
    RadioGroup::Container& siblings = m_radioGroup.m_members;

    for (RadioGroup::Container::iterator
	   it = siblings.begin();
	 it != siblings.end(); ++it) {
      RadioButton* sibling = *it;
      if (sibling->getRadioGroup() == m_radioGroup &&
	  sibling->isSelected()) {
	sibling->setSelected(false);
      }
    }

    // select this RadioButton
    setSelected(true);
  }

  return ButtonBase::onReflectedCommand(id, code, lResult);
}

//////////////////////////////////////////////////////////////////////
// RadioGroup

static Mutex mutex;
static unsigned int radioGroupCounter = 0;

RadioGroup::RadioGroup()
{
  ScopedLock hold(mutex);
  m_groupId = ++radioGroupCounter;
}

RadioGroup::~RadioGroup()
{
}

int RadioGroup::getSelectedIndex() const
{
  for (unsigned i=0; i<m_members.size(); ++i)
    if (m_members[i]->isSelected())
      return i;

  return -1;
}

void RadioGroup::setSelectedIndex(int index)
{
  assert(index >= 0 && index < static_cast<int>(m_members.size()));

  for (int i=0; i<static_cast<int>(m_members.size()); ++i)
    m_members[i]->setSelected(i == index);
}

bool RadioGroup::operator==(const RadioGroup& other) const
{
  return m_groupId == other.m_groupId;
}

void RadioGroup::onChange(Event& ev)
{
  Change(ev);
}

void RadioGroup::addMember(RadioButton* newMember)
{
  // the newMember can't be in the group
  assert(find(m_members.begin(), m_members.end(), newMember) == m_members.end());

  // now it is
  m_members.push_back(newMember);
}

void RadioGroup::removeMember(RadioButton* currentMember)
{
  remove_from_container(m_members, currentMember);
}
