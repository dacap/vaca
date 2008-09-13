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

#ifndef VACA_REGISTER_H
#define VACA_REGISTER_H

#include "Vaca/base.h"
#include "Vaca/Application.h"
#include "Vaca/Widget.h"
#include "Vaca/Exception.h"

namespace Vaca {

/**
 * Thrown when Register can't registers the WidgetClass.
 */
class RegisterException : public Exception
{
public:

  RegisterException() { }
  RegisterException(const String& message) : Exception(message) { }
  virtual ~RegisterException() throw() { }

};

/**
 * Registers a window class to be used in Win32 routines (like
 * CreateWindow). T must be of WidgetClass type, or a derived class of
 * WidgetClass. @ref Register() "The constructor of Register" calls
 * the RegisterClassEx function of Win32, to register a class with
 * the specified parameters by T (see WidgetClass).
 *
 * @see @ref TN001.
 */
template<class T>
class Register
{
public:

  /**
   * Calls Win32 RegisterClassEx filling a WNDCLASSEX structure
   * with the values returned by the T class (T must be derived from
   * WidgetClass).
   */
  Register()
  {
    WidgetClassName widget_class_name = T::getClassName();
    LPCTSTR class_name = widget_class_name.toLPCTSTR();
    WNDCLASSEX wcex;

    if (!GetClassInfoEx(Application::getHandle(), class_name, &wcex)) {
      wcex.cbSize        = sizeof(WNDCLASSEX); 
      wcex.style         = T::getStyle();
      wcex.lpfnWndProc   = Widget::getGlobalWndProc();
      wcex.cbClsExtra    = 0;
      wcex.cbWndExtra    = T::getWndExtra();
      wcex.hInstance     = Application::getHandle();
      wcex.hIcon         = (HICON)NULL;//LoadIcon(hInstance, IDI_GFC);
      wcex.hCursor       = (HCURSOR)NULL;//LoadCursor(NULL, IDC_ARROW);
      wcex.hbrBackground = reinterpret_cast<HBRUSH>(T::getColor()+1);
      wcex.lpszMenuName  = (LPCTSTR)NULL;
      wcex.lpszClassName = class_name;
      wcex.hIconSm       = NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

      if (RegisterClassEx(&wcex) == 0)
	throw RegisterException(String("Error registering class \"") +
				String(class_name) + "\"");
    }
  }

};

} // namespace Vaca

#endif // VACA_REGISTER_H
