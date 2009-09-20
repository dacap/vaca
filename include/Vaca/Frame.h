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

#ifndef VACA_FRAME_H
#define VACA_FRAME_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/Register.h"
#include "Vaca/Icon.h"

#include <vector>

namespace Vaca {

/// Represents the Win32 class used by Frame.
/// 
class FrameClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName(L"Vaca.Frame"); }
};

/// A window with title-bar that can be minimized, maximized, etc.
/// 
class VACA_DLL Frame : public Register<FrameClass>, public Widget
{
  MenuBar* m_menuBar;		       // The menu bar.
  bool m_counted;		       // true if this Frame is counted in the frames' list
  Icon m_smallIcon;
  Icon m_bigIcon;

public:

  struct VACA_DLL Styles {
    static const Style Default;
    static const Style WithCaption;
    static const Style WithSystemMenu;
    static const Style Minimizable;
    static const Style Maximizable;
    static const Style Resizable;
    static const Style InitiallyMinimized;
    static const Style InitiallyMaximized;
  };

  Frame(const String& title, Widget* parent = NULL, Style style = Styles::Default);
  Frame(const WidgetClassName& className, const String& title, Widget* parent = NULL, Style style = Styles::Default);
  explicit Frame(HWND handle);
  virtual ~Frame();

  virtual void setVisible(bool visible);

  MenuBar* getMenuBar();
  virtual MenuBar* setMenuBar(MenuBar* menuBar);

  void setSmallIcon(const Icon& icon);
  void setBigIcon(const Icon& icon);
  void setIcon(ResourceId iconId);

  virtual Size getNonClientSize();

  virtual bool isLayoutFree() const;
  virtual bool keepSynchronized();

  Signal1<void, Event&> Activate;     ///< @see onActivate
  Signal1<void, Event&> Deactivate;   ///< @see onDeactivate
  Signal1<void, CloseEvent&> Close;   ///< @see onClose
  Signal2<void, CardinalDirection, Rect&> Resizing; ///< @see onResizing

  virtual bool preTranslateMessage(Message& message);

protected:
  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);
  virtual void onResize(ResizeEvent& ev);
  virtual void onCommand(CommandEvent& ev);
  virtual void onUpdateIndicators();

  // New events
  virtual void onActivate(Event& ev);
  virtual void onDeactivate(Event& ev);
  virtual void onClose(CloseEvent& ev);
  virtual void onResizing(CardinalDirection dir, Rect& rc);

  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

private:

  void initialize(const String& title);
  void updateMenuItem(MenuItem* menuItem);
  WidgetList getSynchronizedGroup();
  
};

} // namespace Vaca

#endif // VACA_FRAME_H
