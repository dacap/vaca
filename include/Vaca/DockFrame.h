// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_DOCKFRAME_H
#define VACA_DOCKFRAME_H

#include "Vaca/base.h"
#include "Vaca/Frame.h"
#include "Vaca/Event.h"

namespace Vaca {

/**
   Represents the Win32 class used by DockFrame.
*/
class DockFrameClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName(L"Vaca.DockFrame"); }
};

/**
   A Frame for a DockBar, commondly called "Palette Window".

   A DockFrame is the container for a floating DockBar, it's a
   window with a small title-bar.

   @warning This class is intended to be only
	    used internally by DockBar.

   @win32
     This kind of windows has the @msdn{WS_EX_TOOLWINDOW} style.
   @endwin32

   @internal
*/
class VACA_DLL DockFrame : public Register<DockFrameClass>, public Frame
{
  /**
     The DockBar that is inside the client area.
  */
  DockBar* m_dockBar;

public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  DockFrame(DockBar* dockBar, Widget* parent = NULL, Style style = Styles::Default);
  virtual ~DockFrame();

protected:

  virtual bool keepSynchronized();

  // Events
  virtual void onResizing(CardinalDirection dir, Rect& rc);

  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
};

} // namespace Vaca

#endif // VACA_DOCKFRAME_H
