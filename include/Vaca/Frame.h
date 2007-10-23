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

#ifndef VACA_FRAME_H
#define VACA_FRAME_H

#include <vector>
#include <boost/signal.hpp>

#include "Vaca/base.h"
#include "Vaca/Widget.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/Register.h"
#include "Vaca/DockArea.h"

namespace Vaca {

class CloseEvent;
class Icon;
class Menu;
class MenuBar;
class MenuItem;

#define WithCaptionFrameStyle		(Style(WS_CAPTION, 0))

#define WithSystemMenuFrameStyle	(Style(WS_SYSMENU, 0))

#define MinimizableFrameStyle		(Style(WS_MINIMIZEBOX, 0))

#define MaximizableFrameStyle		(Style(WS_MAXIMIZEBOX, 0))

#define ResizableFrameStyle		(Style(WS_SIZEBOX, 0))

#define InitiallyMinimizedFrameStyle	(Style(WS_MINIMIZE, 0))

#define InitiallyMaximizedFrameStyle	(Style(WS_MAXIMIZE, 0))

#define FrameStyle			(ContainerStyle +		\
					 WithCaptionFrameStyle +	\
					 WithSystemMenuFrameStyle +	\
					 MinimizableFrameStyle +	\
					 MaximizableFrameStyle +	\
					 ResizableFrameStyle)

/**
 * Win32 class used by the Frame class.
 */
class FrameClass : public WidgetClass
{
public:
  static LPCTSTR getClassName() { return _T("Vaca.Frame"); }
};

/**
 * Controls a frame window that can be minimized, maximized, etc.
 * Every application should contain (at least) one frame window (or a
 * window derived from Frame like Dialog).
 */
class VACA_DLL Frame : public Register<FrameClass>, public Widget
{
  MenuBar *m_menuBar;		       // The menu bar.
//   std::vector<Command *> m_commands;   // collection of Commands that can be executed in this Frame
  std::vector<DockArea *> m_dockAreas; // Areas where you can dock a tool-bar.
  bool m_counted;		       // true if this Frame is counted in the frames' list

public:

  Frame(const String &title, Widget *parent = NULL, Style style = FrameStyle);
  Frame(LPCTSTR className, const String &title, Widget *parent = NULL, Style style = FrameStyle);
  virtual ~Frame();

  // bool isVisible();
  virtual void setVisible(bool visible);

  MenuBar *getMenuBar();
  virtual MenuBar *setMenuBar(MenuBar *menuBar);

  void setIcon(Icon *icon, bool bigIcon);
  void setIcon(int iconId);

  Size getNonClientSize();
  virtual Rect getLayoutBounds();
  
  void addDockArea(DockArea *dockArea);
  void removeDockArea(DockArea *dockArea);
  void defaultDockAreas();
  void deleteDockAreas();
  std::vector<DockArea *> getDockAreas();
  DockArea *getDockArea(Side side);
  virtual DockArea *getDefaultDockArea();

  virtual void layout();
  virtual bool isLayoutFree();
  virtual bool keepSynchronized();

  boost::signal<void (Event &)> Activate;     ///< @see onActivate
  boost::signal<void (Event &)> Deactivate;   ///< @see onDeactivate
  boost::signal<void (CloseEvent &)> Close;   ///< @see onClose
  boost::signal<void (int, Rect &)> Resizing; ///< @see onResizing

  // static int getFramesCount();
//   static int getVisibleFramesByThread(int threadId);

  virtual bool preTranslateMessage(MSG &msg);

protected:
  // events
  virtual void onPreferredSize(Size &sz);
//   virtual void onDestroy();
  virtual void onResize(const Size &sz);
//   virtual void onKeyDown(KeyEvent &ev);
  virtual bool onActionById(int actionId);

  // new events
  virtual void onActivate(Event &ev);
  virtual void onDeactivate(Event &ev);
  virtual void onClose(CloseEvent &ev);
  virtual void onResizing(int edge, Rect &rc);

  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult);

private:

  void initialize(const String &title);
//   Menu *getMenuByHMENU(HMENU hmenu);
  void updateMenuItem(MenuItem *menuItem);
  Container getSynchronizedGroup();
  
};

} // namespace Vaca

#endif
