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

#ifndef VACA_BUTTONBASE_H
#define VACA_BUTTONBASE_H

#include <boost/signal.hpp>

#include "Vaca/Widget.h"

namespace Vaca {

//////////////////////////////////////////////////////////////////////
// Win32 Button Styles

#ifndef BS_3STATE
#define BS_3STATE		5
#define BS_AUTO3STATE		6
#define BS_AUTOCHECKBOX		3
#define BS_AUTORADIOBUTTON	9
#define BS_BITMAP		128
#define BS_BOTTOM		0x800
#define BS_CENTER		0x300
#define BS_CHECKBOX		2
#define BS_DEFPUSHBUTTON	1
#define BS_GROUPBOX		7
#define BS_ICON			64
#define BS_LEFT			256
#define BS_LEFTTEXT		32
#define BS_MULTILINE		0x2000
#define BS_NOTIFY		0x4000
#define BS_OWNERDRAW		0xb
#define BS_PUSHBUTTON		0
#define BS_PUSHLIKE		4096
#define BS_RADIOBUTTON		4
#define BS_RIGHT		512
#define BS_RIGHTBUTTON		32
#define BS_TEXT			0
#define BS_TOP			0x400
#define BS_USERBUTTON		8
#define BS_VCENTER		0xc00
#define BS_FLAT			0x8000
#endif

/**
 * Base for every button (it's a wrapper for the "BUTTON" class of
 * Win32).
 */
class VACA_DLL ButtonBase : public Widget
{
public:

  ButtonBase(Widget *parent, Style style);
  virtual ~ButtonBase();

  bool isSelected();
  void setSelected(bool state);

  // signals
  boost::signal<void (Event &)> Action; ///< @see onAction

protected:
  // events
  virtual void onPreferredSize(Size &sz);

  // new events
  virtual void onAction(Event &ev);

  // reflection
  virtual bool onReflectedCommand(int id, int code, LRESULT &lResult);
};

} // namespace Vaca

#endif
