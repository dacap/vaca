// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

#ifndef VACA_RADIOBUTTON_H
#define VACA_RADIOBUTTON_H

#include "Vaca/ButtonBase.h"

namespace Vaca {

/// Groups a set of @link Vaca::RadioButton RadioButtons@endlink. It's used to known
/// which RadioButton should be desactived when the user select a
/// RadioButton of its same group.
/// 
class VACA_DLL RadioGroup : public NonCopyable
{
  friend class RadioButton;

  typedef std::vector<RadioButton*> Container;

  /// Unique ID of the group.
  /// 
  /// @internal You never use (get or set) this value.
  /// 
  unsigned int m_groupId;

  /// Members in the group.
  /// 
  Container m_members;

public:
  RadioGroup();
  virtual ~RadioGroup();

  int getSelectedIndex() const;
  void setSelectedIndex(int index);

  bool operator==(const RadioGroup& other) const;

  // signals
  Signal1<void, Event&> Change; ///< @see onChange

protected:
  // new events
  virtual void onChange(Event& ev);

private:
  void addMember(RadioButton* newMember);
  void removeMember(RadioButton* currentMember);
};

/// A radio button.
/// 
/// @image html RadioButton.png
/// 
/// @warning
///   Win32: It is not a BS_AUTORADIOBUTTON, it's only an
///   BS_RADIOBUTTON (so you can't get @msdn{Q261192} bug).
/// 
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
  // events
  virtual void onAction(Event& ev);

  // reflection
  virtual bool onReflectedCommand(int id, int code, LRESULT& lResult);
};

} // namespace Vaca

#endif // VACA_RADIOBUTTON_H
