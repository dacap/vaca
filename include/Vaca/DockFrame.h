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

#ifndef VACA_DOCKFRAME_H
#define VACA_DOCKFRAME_H

#include "Vaca/base.h"
#include "Vaca/Frame.h"
#include "Vaca/Event.h"

namespace Vaca {

/**
 * Default style for DockFrame.
 *
 * @win32
 *   Includes WS_POPUP and WS_EX_TOOLWINDOW.
 * @endwin32
 */
#define DockFrameStyle		(WithCaptionFrameStyle +		\
				 WithSystemMenuFrameStyle +		\
				 ResizableFrameStyle +			\
				 Style(WS_POPUP, WS_EX_TOOLWINDOW))

class DockBar;

/**
 * Represents the Win32 class used by DockFrame.
 */
class DockFrameClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName("Vaca.DockFrame"); }
};

/**
 * A Frame for a DockBar, commondly called "Palette Window".
 *
 * A DockFrame is the container for a floating DockBar, it's a
 * window with a small title-bar.
 *
 * @warning This class is intended to be only
 *          used internally by DockBar.
 *
 * @win32 
 *   This kind of windows has the @msdn{WS_EX_TOOLWINDOW} style.
 * @endwin32
 *
 * @internal 
 */
class VACA_DLL DockFrame : public Register<DockFrameClass>, public Frame
{
  /**
   * The DockBar that is inside the client area.
   */
  DockBar* m_dockBar;

public:
  DockFrame(DockBar* dockBar, Widget* parent = NULL, Style style = DockFrameStyle);
  virtual ~DockFrame();

protected:

  virtual bool keepSynchronized();

  // events
  virtual void onResizing(CardinalDirection dir, Rect& rc);

  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
};

} // namespace Vaca

#endif // VACA_DOCKFRAME_H
