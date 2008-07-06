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

#ifndef VACA_COMMAND_HPP
#define VACA_COMMAND_HPP

#if 0

#include "Vaca/base.hpp"
#include "Vaca/String.hpp"

namespace Vaca {

class Event;

class VACA_DLL CommandState
{
  String* m_text;
  bool* m_enabled;
  bool* m_checked;
  bool* m_radio;

public:

  CommandState();
  virtual ~CommandState();

  void setText(const String& text);
  void setEnabled(bool state);
  void setChecked(bool state);
  void setRadio(bool state);

  const String* getText() const;
  const bool* isEnabled() const;
  const bool* isChecked() const;
  const bool* isRadio() const;
  
};

/**
 * A command for a MenuItem or for a ToolBar's button.
 */
class VACA_DLL Command
{
  int mId;             // Command ID to be used with Win32 resources and WM_COMMAND message.
//   String mText;        // Label for the ToolBar's button.
//   String mToolTip;     // Text for the ToolBar tip.
//   String mDescription; // Short description for the StatusBar or the ToolBar tip).

public:

  Command(int id);
  virtual ~Command();

  int getId() const;

//   virtual const String& getText() const { return mText; }
//   virtual const String& getToolTip() const { return mToolTip; }
//   virtual const String& getDescription() const { return mDescription; }

  Signal0<void> Action;
  Signal1<void, CommandState&> Update;

  // events
  virtual void onAction();
  virtual void onUpdate(CommandState& cmdState);

};

} // namespace Vaca

#endif

#endif
