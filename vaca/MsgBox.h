// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_MSGBOX_H
#define VACA_MSGBOX_H

#include "vaca/base.h"
#include "vaca/Color.h"
#include "vaca/Component.h"
#include "vaca/Exception.h"
#include "vaca/Graphics.h"
#include "vaca/Rect.h"
#include "vaca/ScrollInfo.h"
#include "vaca/Signal.h"
#include "vaca/Size.h"
#include "vaca/Style.h"

#include <vector>

namespace vaca {

/**
   Class to show a message box.
*/
class VACA_DLL MsgBox
{
public:

  enum class Type {
    Ok,
    OkCancel,
    YesNo,
    YesNoCancel,
    RetryCancel,
    CancelRetryContinue,
  };

  enum class Icon {
    None,
    Error,
    Warning,
    Question,
    Information,
  };

  enum class Result {
    Ok, Cancel,
    Yes, No,
    Retry, Continue,
  };

public:

  static Result show(Widget* parent,
		     const String& title,
		     const String& text,
		     Type type = Type::Ok,
		     Icon icon = Icon::None,
		     int default_button = -1);

  static void showException(Widget* parent,
			    const Exception& exception);

};

} // namespace vaca

#endif // VACA_MSGBOX_H
