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

#ifndef VACA_WIDGETCLASS_H
#define VACA_WIDGETCLASS_H

#include "Vaca/base.h"

namespace Vaca {

/**
 * Represents the name of a WidgetClass.
 *
 * @win32 
 *   This is used internally to wrap the name of a Win32 class
 *   in the registration process (Register).
 * @endwin32
 */
class VACA_DLL WidgetClassName
{
public:
  /**
   * Represents the no-name class, which means that you don't want
   * to register a WidgetClass at the moment.
   */
  static const WidgetClassName None;

private:
  String m_className;

  WidgetClassName();		// None constructor

public:
  explicit WidgetClassName(const String& className);
  WidgetClassName(const WidgetClassName& className);
  virtual ~WidgetClassName();

  WidgetClassName& operator=(const WidgetClassName& className);
  bool operator==(const WidgetClassName& className) const;
  bool operator!=(const WidgetClassName& className) const;

  inline const Char* c_str() const { return m_className.c_str(); }
};

/**
 * Base class to retrieve information about a new Win32 class to be
 * registered using the @c Register class.
 *
 * Vaca needs this class to register a Windows class (through the
 * @msdn{RegisterClassEx} function) before to construct the widget
 * because Widget::Widget calls the @msdn{CreateWindowEx} function. The
 * only way to register the class before and automatically, is using
 * multiple-inheritance.
 *
 * @win32
 * The next example shows how you can register your own widget
 * class before to create it:
 *
 * @code
 * class MyWidgetClass : public WidgetClass
 * {
 * public:
 *   static WidgetClassName getClassName()
 *   { return WidgetClassName(L"Vaca.MyWidget"); }
 * };
 * @endcode
 *
 * Then you should create your how @c MyWidget using:
 *
 * @code
 * class MyWidget : public Register<MyWidgetClass>, public Widget
 * {
 * public:
 *   MyWidget::MyWidget(..., Widget* parent, Style style = ...)
 *     : Widget(MyWidgetClass::getClassName(), parent, style)
 *   {
 *     // ...Your code here...
 *   }
 *   ...
 * };
 * @endcode
 * @endwin32
 *
 * @see @ref page_tn_001
 */
class WidgetClass
{
public:

  /**
   * Returns the class name.
   *
   * @win32
   *   The class name is used in the lpszClassName field of
   *   the @msdn{WNDCLASSEX} structure inside the @link Register#Register Register constructor@endlink.
   * @endwin32
   */
  static WidgetClassName getClassName()
  { return WidgetClassName(L"Vaca.Widget"); }

  /**
   * Returns the set of CS_ Win32 flags.
   * 
   * All Widgets by default received double-clicks, so it returns
   * CS_DBLCLKS.
   * 
   */
  static int getStyle() { return CS_DBLCLKS; }

  static int getColor() { return COLOR_3DFACE; }

  static int getWndExtra() { return 0; }

};

} // namespace Vaca

#endif // VACA_WIDGETCLASS_H
