// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_REGISTER_H
#define VACA_REGISTER_H

#include "Vaca/base.h"
#include "Vaca/Application.h"
#include "Vaca/Widget.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/Exception.h"
#include "Vaca/String.h"

namespace Vaca {

/**
   Thrown when Register can't registers the WidgetClass.
*/
class RegisterException : public Exception
{
public:

  RegisterException() { }
  RegisterException(const String& message) : Exception(message) { }
  virtual ~RegisterException() throw() { }

};

/**
   Registers a window class to be used in Win32 routines (like
   @msdn{CreateWindow}).

   @ref Register() "The constructor of Register" calls
   the Win32's @msdn{RegisterClassEx} function, to register a
   class with the specified parameters by T (see WidgetClass).

   @tparam T
     Must be of WidgetClass type, or a derived class of it.

   @see @ref page_tn_001
*/
template<class T>
class Register
{
public:

  /**
     Calls @msdn{RegisterClassEx} filling a @msdn{WNDCLASSEX} structure
     with the values returned by the T class (T must be derived from
     WidgetClass).
  */
  Register()
  {
    WidgetClassName class_name = T::getClassName();
    WNDCLASSEX wcex;

    if (!GetClassInfoEx(Application::getHandle(), class_name.c_str(), &wcex)) {
      wcex.cbSize        = sizeof(WNDCLASSEX);
      wcex.style         = T::getStyle();
      wcex.lpfnWndProc   = T::getWndProc();
      wcex.cbClsExtra    = 0;
      wcex.cbWndExtra    = T::getWndExtra();
      wcex.hInstance     = Application::getHandle();
      wcex.hIcon         = (HICON)NULL;//LoadIcon(hInstance, IDI_GFC);
      wcex.hCursor       = (HCURSOR)NULL;//LoadCursor(NULL, IDC_ARROW);
      wcex.hbrBackground = reinterpret_cast<HBRUSH>(T::getColor()+1);
      wcex.lpszMenuName  = (LPCTSTR)NULL;
      wcex.lpszClassName = class_name.c_str();
      wcex.hIconSm       = NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

      if (RegisterClassEx(&wcex) == 0)
	throw RegisterException(format_string(L"Error registering class \"%s\"",
					      class_name.c_str()));
    }
  }

};

} // namespace Vaca

#endif // VACA_REGISTER_H
