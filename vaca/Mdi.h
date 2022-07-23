// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_MDI_H
#define VACA_MDI_H

#include "vaca/base.h"
#include "vaca/Frame.h"

namespace vaca {

/**
   ID for the first MdiChild. These IDs are used internally by Win32
   and the MdiListMenu.
*/
#define VACA_FIRST_MDICHILD 0xff00

/**
   Represents the Win32 class used by MdiChild.
*/
class MdiChildClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName(L"Vaca.MdiChild"); }
};

/**
   Controls a child window for a MdiClient. Represent a Frame inside a
   MdiClient.
*/
class VACA_DLL MdiChild : public Register<MdiChildClass>, public Frame
{
  void initialize();

public:

  struct VACA_DLL Styles {
    static constexpr Style Default =
      Frame::Styles::Default |
      Style(WS_CHILD, WS_EX_MDICHILD);
  };

  MdiChild(const String& title, MdiClient* parent, Style style = Styles::Default);
  MdiChild(const String& title, MdiFrame* parent, Style style = Styles::Default);
  virtual ~MdiChild();

protected:
  // Reflected notifications
  // virtual bool onReflectedCommand(int id, int code, LRESULT& lResult);

  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
//   virtual LRESULT defWndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
//   virtual void destroyHWND(HWND hwnd);
};

/**
   Widget which its client area contains MdiChild windows.
*/
class VACA_DLL MdiClient : public Widget
{
public:

  struct VACA_DLL Styles {
    static constexpr Style Default =
      Widget::Styles::Visible |
      Widget::Styles::Scroll |
      Widget::Styles::ClientEdge |
      Widget::Styles::Container;
  };

  MdiClient(Widget* parent, Style style = Styles::Default);
  virtual ~MdiClient();

  void cascade();
  void tileHorizontal();
  void tileVertical();
  void arrangeIcons();

  MdiChild* getActive();
  void activate(MdiChild* mdiChild);
  void activateNext(MdiChild* mdiChild = NULL);
  void activatePrevious(MdiChild* mdiChild = NULL);

  MdiChild* getChildById(int wID);

  // void maximize(MdiChild* window);
  // void restore(MdiChild* window);

  virtual bool preTranslateMessage(Message& message);

private:
  virtual HWND createHandle(LPCTSTR className, Widget* parent, Style style);
};

/**
   Default implementation to use the MDI interface.

   You can use this kind of Frame when you want MDI applications.

   @win32
     It takes care of a MdiClient and uses the @msdn{DefFrameProc} of
     the Windows API as default message handler.
   @endwin32
*/
class VACA_DLL MdiFrame : public Frame
{
  friend class Frame;

private:

  MdiClient* m_mdiClient;

public:

  struct VACA_DLL Styles {
    static constexpr Style Default =
      Frame::Styles::Default;
  };

  MdiFrame(const String& title, Widget* parent = NULL, Style style = Styles::Default,
	   bool customMdiClient = false);
//   MdiFrame(Widget* parent, Style style = ChildStyle); // TODO ChildStyle???
  virtual ~MdiFrame();

  MdiClient* getMdiClient();
  MdiClient* setMdiClient(MdiClient* mdiClient);

  virtual MenuBar* setMenuBar(MenuBar* menubar);
  void refreshMenuBar();

protected:
  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
//   virtual LRESULT defWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};


} // namespace vaca

#endif // VACA_MDI_H
