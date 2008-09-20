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

#ifndef VACA_MSGBOX_H
#define VACA_MSGBOX_H

#include <vector>

#include "Vaca/base.h"
#include "Vaca/Color.h"
#include "Vaca/Component.h"
#include "Vaca/Exception.h"
#include "Vaca/Graphics.h"
#include "Vaca/Rect.h"
#include "Vaca/ScrollInfo.h"
#include "Vaca/Signal.h"
#include "Vaca/Size.h"
#include "Vaca/String.h"
#include "Vaca/Style.h"

namespace Vaca {

/**
 * Class to show a message box.
 */
class VACA_DLL MsgBox
{
public:

  struct TypeEnum
  {
    enum enumeration {
      Ok,
      OkCancel,
      YesNo,
      YesNoCancel,
      RetryCancel,
      CancelRetryContinue,
    };
    static const enumeration default_value = Ok;
  };

  struct IconEnum
  {
    enum enumeration {
      None,
      Error,
      Warning,
      Question,
      Information,
    };
    static const enumeration default_value = None;
  };

  struct ResultEnum
  {
    enum enumeration {
      Ok, Cancel,
      Yes, No,
      Retry, Continue,
    };
    static const enumeration default_value = Ok;
  };

  typedef Enum<TypeEnum> Type;
  typedef Enum<IconEnum> Icon;
  typedef Enum<ResultEnum> Result;
  
public:

  static Result show(Widget* parent,
		     const String& title,
		     const String& text,
		     Type type = Type::Ok,
		     Icon icon = Icon::None,
		     int default_button = -1);

};

} // namespace Vaca

#endif // VACA_MSGBOX_H
