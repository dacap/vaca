// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/RadioButton.h"
#include "vaca/Debug.h"
#include "vaca/Event.h"
#include "vaca/Mutex.h"
#include "vaca/ScopedLock.h"

using namespace vaca;

// ======================================================================
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

void RadioButton::onClick(Event& ev)
{
  ButtonBase::onClick(ev);
  m_radioGroup.onChange(ev);
}

bool RadioButton::onReflectedCommand(int id, int code, LRESULT& lResult)
{
  if (code == BN_CLICKED) {
    // deselect all siblings RadioButtons
    RadioButtonList& siblings = m_radioGroup.m_members;

    for (RadioButtonList::iterator
	   it = siblings.begin(); it != siblings.end(); ++it) {
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

// ======================================================================
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
