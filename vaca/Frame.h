// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
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

  struct Styles {

    /**
       This Frame is a window that as a title bar.

       @win32
       It is WS_CAPTION
       @endwin32
    */
    static constexpr Style WithCaption = Style(WS_CAPTION, 0);

    /**
       @win32
       It is WS_SYSMENU
       @endwin32
    */
    static constexpr Style WithSystemMenu = Style(WS_SYSMENU, 0);

    /**
       @win32
       It is WS_MINIMIZEBOX
       @endwin32
    */
    static constexpr Style Minimizable = Style(WS_MINIMIZEBOX, 0);

    /**
       @win32
       It is WS_MAXIMIZEBOX
       @endwin32
    */
    static constexpr Style Maximizable = Style(WS_MAXIMIZEBOX, 0);

    /**
       The user will be able to change the window's size.

       @win32
       It is WS_SIZEBOX
       @endwin32
    */
    static constexpr Style Resizable = Style(WS_SIZEBOX, 0);

    /**
       The window can be minimized.

       @win32
       It is WS_MINIMIZE
       @endwin32
    */
    static constexpr Style InitiallyMinimized = Style(WS_MINIMIZE, 0);

    /**
       The window can be maximized.

       @win32
       It is WS_MAXIMIZE
       @endwin32
    */
    static constexpr Style InitiallyMaximized = Style(WS_MAXIMIZE, 0);

    /**
       Default style for Frame widget.

       It contains the following styles:
       @li Widget::Styles::Container
       @li Frame::Styles::WithCaption
       @li Frame::Styles::WithSystemMenu
       @li Frame::Styles::Minimizable
       @li Frame::Styles::Maximizable
       @li Frame::Styles::Resizable

       @win32
       It is WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU |
       WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX |
       WS_EX_CONTROLPARENT
       @endwin32
    */
    static constexpr Style Default =
      Widget::Styles::Container |
      Frame::Styles::WithCaption |
      Frame::Styles::WithSystemMenu |
      Frame::Styles::Minimizable |
      Frame::Styles::Maximizable |
      Frame::Styles::Resizable;
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

  Signal<void(Event&)> Activate;     ///< @see onActivate
  Signal<void(Event&)> Deactivate;   ///< @see onDeactivate
  Signal<void(CloseEvent&)> Close;   ///< @see onClose
  Signal<void(CardinalDirection, Rect&)> Resizing; ///< @see onResizing

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
