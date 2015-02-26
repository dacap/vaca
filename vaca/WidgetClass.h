// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_WIDGETCLASS_H
#define VACA_WIDGETCLASS_H

#include "vaca/base.h"

namespace vaca {

/**
   Represents the name of a WidgetClass.

   @win32
     This is used internally to wrap the name of a Win32 class
     in the registration process (Register).
   @endwin32
*/
class VACA_DLL WidgetClassName
{
public:
  /**
     Represents the no-name class, which means that you don't want
     to register a WidgetClass at the moment.
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
   Base class to retrieve information about a new Win32 class to be
   registered using the @c Register class.

   Vaca needs this class to register a Windows class (through the
   @msdn{RegisterClassEx} function) before to construct the widget
   because Widget::Widget calls the @msdn{CreateWindowEx} function. The
   only way to register the class before and automatically, is using
   multiple-inheritance.

   @win32
   The next example shows how you can register your own widget
   class before to create it:

   @code
   class MyWidgetClass : public WidgetClass
   {
   public:
     static WidgetClassName getClassName()
     { return WidgetClassName(L"Vaca.MyWidget"); }
   };
   @endcode

   Then you should create your how @c MyWidget using:

   @code
   class MyWidget : public Register<MyWidgetClass>, public Widget
   {
   public:
     MyWidget::MyWidget(..., Widget* parent, Style style = ...)
       : Widget(MyWidgetClass::getClassName(), parent, style)
     {
       // ...Your code here...
     }
     ...
   };
   @endcode
   @endwin32

   @see @ref page_tn_001
*/
class VACA_DLL WidgetClass
{
public:

  static WidgetClassName getClassName();
  static int getStyle();
  static int getColor();
  static int getWndExtra();
  static WNDPROC getWndProc();

};

} // namespace vaca

#endif // VACA_WIDGETCLASS_H
