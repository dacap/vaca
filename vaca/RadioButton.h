// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_RADIOBUTTON_H
#define VACA_RADIOBUTTON_H

#include "vaca/ButtonBase.h"

namespace vaca {

typedef std::vector<RadioButton*> RadioButtonList;

/**
   Groups a set of @link vaca::RadioButton RadioButtons@endlink. It's used to known
   which RadioButton should be desactived when the user select a
   RadioButton of its same group.
*/
class VACA_DLL RadioGroup : public NonCopyable
{
  friend class RadioButton;

  /**
     Unique ID of the group.

     @internal You never use (get or set) this value.
  */
  unsigned int m_groupId;

  /**
     Members in the group.
  */
  RadioButtonList m_members;

public:
  RadioGroup();
  virtual ~RadioGroup();

  int getSelectedIndex() const;
  void setSelectedIndex(int index);

  bool operator==(const RadioGroup& other) const;

  // Signals
  Signal1<void, Event&> Change; ///< @see onChange

protected:
  // New events
  virtual void onChange(Event& ev);

private:
  void addMember(RadioButton* newMember);
  void removeMember(RadioButton* currentMember);
};

/**
   A radio button.

   @image html RadioButton.png

   @warning
     Win32: It is not a BS_AUTORADIOBUTTON, it's only an
     BS_RADIOBUTTON (so you can't get @msdn{Q261192} bug).
*/
class VACA_DLL RadioButton : public ButtonBase
{
  RadioGroup& m_radioGroup;

public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  RadioButton(const String& text, RadioGroup& group, Widget* parent,
	      Style style = Styles::Default);
  virtual ~RadioButton();

  const RadioGroup& getRadioGroup() const;

protected:
  // Events
  virtual void onClick(Event& ev);

  // Reflected notifications
  virtual bool onReflectedCommand(int id, int code, LRESULT& lResult);
};

} // namespace vaca

#endif // VACA_RADIOBUTTON_H
