// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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

#include <boost/signal.hpp>

#include "Vaca/base.h"
#include "Vaca/Frame.h"
#include "Vaca/Event.h"

namespace Vaca {

/**
 * Default style for DockFrame: CaptionStyle, SizeBoxStyle,
 * SysMenuStyle, WS_POPUP, and WS_EX_TOOLWINDOW.
 */
#define DockFrameStyle		(WithCaptionFrameStyle +		\
				 WithSystemMenuFrameStyle +		\
				 ResizableFrameStyle +			\
				 Style(WS_POPUP, WS_EX_TOOLWINDOW))

class DockBar;

/**
 * Win32 class used by the DockFrame class.
 */
class DockFrameClass : public WidgetClass
{
public:
  static LPCTSTR getClassName() { return _T("Vaca.DockFrame"); }
};

/**
 * A Frame for a DockBar, commondly called "Pallete Window". A
 * DockFrame is the container for a floating DockBar, it's the window
 * with the WS_EX_TOOLWINDOW style.
 *
 * @internal You shouldn't use this class, it's used internally by DockBar.
 */
class VACA_DLL DockFrame : public Register<DockFrameClass>, public Frame
{
private:

  DockBar *mDockBar;		///< The DockBar that is inside the client area.

public:

  DockFrame(DockBar *dockBar, Widget *parent = NULL, Style style = DockFrameStyle);
  ~DockFrame();

  boost::signal<void ()> Destroy;

protected:

  virtual bool keepEnabledSynchronised();

  // events
  virtual void onDestroy();
  virtual void onResizing(int edge, Rect &rc);

  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult);
};

} // namespace Vaca

#endif
