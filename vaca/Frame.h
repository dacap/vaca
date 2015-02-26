// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_FRAME_H
#define VACA_FRAME_H

#include "vaca/base.h"
#include "vaca/Widget.h"
#include "vaca/WidgetClass.h"
#include "vaca/Register.h"
#include "vaca/Icon.h"

#include <vector>

namespace vaca {

/**
   Represents the Win32 class used by Frame.
*/
class FrameClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName(L"Vaca.Frame"); }
};

/**
   A window with title-bar that can be minimized, maximized, etc.
*/
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

} // namespace vaca

#endif // VACA_FRAME_H
