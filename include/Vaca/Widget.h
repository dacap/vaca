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

#ifndef VACA_WIDGET_H
#define VACA_WIDGET_H

#include "Vaca/base.h"
#include "Vaca/Color.h"
#include "Vaca/Event.h"
#include "Vaca/Graphics.h"
#include "Vaca/Rect.h"
#include "Vaca/Size.h"
#include "Vaca/String.h"
#include "Vaca/Exception.h"
#include "Vaca/Style.h"

#include <vector>
#include <boost/signal.hpp>
#include <boost/noncopyable.hpp>

namespace Vaca {

class Layout;
class Constraint;
class WidgetEvent;
class KeyEvent;
class MouseEvent;
class Cursor;

//////////////////////////////////////////////////////////////////////
// Win32 Regular Window Styles

#ifndef WS_BORDER
#define WS_BORDER		0x800000
#define WS_CAPTION		0xc00000
#define WS_CHILD		0x40000000
#define WS_CHILDWINDOW		0x40000000
#define WS_CLIPCHILDREN		0x2000000
#define WS_CLIPSIBLINGS		0x4000000
#define WS_DISABLED		0x8000000
#define WS_DLGFRAME		0x400000
#define WS_GROUP		0x20000
#define WS_HSCROLL		0x100000
#define WS_ICONIC		0x20000000
#define WS_MAXIMIZE		0x1000000
#define WS_MAXIMIZEBOX		0x10000
#define WS_MINIMIZE		0x20000000
#define WS_MINIMIZEBOX		0x20000
#define WS_OVERLAPPED		0
#define WS_OVERLAPPEDWINDOW	0xcf0000
#define WS_POPUP		0x80000000
#define WS_POPUPWINDOW		0x80880000
#define WS_SIZEBOX		0x40000
#define WS_SYSMENU		0x80000
#define WS_TABSTOP		0x10000
#define WS_THICKFRAME		0x40000
#define WS_TILED		0
#define WS_TILEDWINDOW		0xcf0000
#define WS_VISIBLE		0x10000000
#define WS_VSCROLL		0x200000
#endif

//////////////////////////////////////////////////////////////////////
// Win32 Extended Window Styles

#ifndef WS_EX_ACCEPTFILES
#define WS_EX_ACCEPTFILES	16
#define WS_EX_APPWINDOW		0x40000
#define WS_EX_CLIENTEDGE	512
#define WS_EX_CONTEXTHELP	0x400
#define WS_EX_CONTROLPARENT	0x10000
#define WS_EX_DLGMODALFRAME	1
#define WS_EX_LEFT		0
#define WS_EX_LEFTSCROLLBAR	0x4000
#define WS_EX_LTRREADING	0
#define WS_EX_MDICHILD		64
#define WS_EX_NOPARENTNOTIFY	4
#define WS_EX_OVERLAPPEDWINDOW	0x300
#define WS_EX_PALETTEWINDOW	0x188
#define WS_EX_RIGHT		0x1000
#define WS_EX_RIGHTSCROLLBAR	0
#define WS_EX_RTLREADING	0x2000
#define WS_EX_STATICEDGE	0x20000
#define WS_EX_TOOLWINDOW	128
#define WS_EX_TOPMOST		8
#define WS_EX_TRANSPARENT	32
#define WS_EX_WINDOWEDGE	256
#endif

#ifndef WS_EX_LAYOUTRTL
#define WS_EX_LAYOUTRTL		0x400000
#endif
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED		0x80000
#endif
#ifndef WS_EX_NOACTIVATE
#define WS_EX_NOACTIVATE	0x8000000
#endif
#ifndef WS_EX_NOINHERITLAYOUT
#define WS_EX_NOINHERITLAYOUT	0x100000
#endif

// #ifndef WS_EX_COMPOSITED
// #define WS_EX_COMPOSITED	0x2000000
// #endif

//////////////////////////////////////////////////////////////////////
// Vaca Widget Styles

/**
 * Without styles.
 */
#define NoStyle			(Style(0, 0))

/**
 * Basic style for every child (WS_CHILD, WS_VISIBLE).
 */
#define ChildStyle              (Style(WS_CHILD | WS_VISIBLE, 0))

/**
 * Style to receive the focus in Dialogs (WS_TABSTOP).
 */
#define TabStopStyle            (Style(WS_TABSTOP, 0))

/**
 * The widget has both scroll bars (WS_HSCROLL | WS_VSCROLL).
 */
#define ScrollStyle		(Style(WS_HSCROLL | WS_VSCROLL, 0))

#define HorizontalScrollStyle	(Style(WS_HSCROLL, 0))

#define VerticalScrollStyle	(Style(WS_VSCROLL, 0))

/**
 * Edge in the client area (WS_EX_CLIENTEDGE).
 */
#define ClientEdgeStyle         (Style(0, WS_EX_CLIENTEDGE))

#define ClipChildrenStyle	(Style(WS_CLIPCHILDREN, 0))

/**
 * Use double-buffering to draw the widget (WS_EX_COMPOSITED)
 *
 * This works only on WinXP, in other platforms it's NoStyle because
 * CreateWindowEx fails if you specified WS_EX_COMPOSITED.
 *
 * @deprecated This has too much restrictions (in the same WinXP).
 */
// #define DoubleBufferingStyle	(System::isWinXP() ? Style(0, WS_EX_COMPOSITED): NoStyle)

/**
 * Thrown when the Widget::createHwnd method returns NULL.
 */
class CreateHwndException : public Exception
{
public:

  CreateHwndException() throw() : Exception() { }
  virtual ~CreateHwndException() throw() { }

};

/**
 * Base class for every control in an window.
 *
 * This is the core of Vaca, calls CreateWindowEx and DestroyWindow,
 * and its wndProc() converts the main messages to events.
 */
class VACA_DLL Widget : private boost::noncopyable
{
public:

  typedef std::vector<Widget *> Container;

private:

  HWND        mHwnd;     // The window handler to use with the Windows API.
  Container   mChildren; // Collection of children of this widget.
  Widget     *mParent;   // The parent widget.
  Color       mBgColor;
  Constraint *mConstraint;
  Layout     *mLayout;
  WNDPROC     mSuperWndProc;
  bool        mDisposed : 1;	      // This widget was disposed before.
  bool        mHasMouse : 1;	      // Flag to indicate that this widget has the mouse.
  bool        mDeleteAfterEvent : 1;  // Flag to indicate that we must delete the widget after the event.
//   bool        mDisposeAfterEvent : 1; // Flag to indicate that we must dispose the widget after the event.
  int         mCriticalInner;         // Counts how many times we inner the globalWndProc using this widget.
  Font       *mFont;

  // TODO something to remove this (it's only needed for WM_CTLCOLOR)
  HBRUSH mHbrush;

  // TODO something to remove this (it's only needed for onSetCursor)
  WPARAM mWparam;
  LPARAM mLparam;

public:

  Widget(LPCTSTR className, Widget *parent, Style style = NoStyle);
  virtual ~Widget();

  Widget *getParent();
  Container getChildren();

  Layout *getLayout();
  Layout *setLayout(Layout *layout);

  Constraint *getConstraint();
  Constraint *setConstraint(Constraint *constraint);

  // TODO public virtual, I don't like it
  virtual String getText();
  virtual void setText(const String &str);

  virtual Font &getFont();
  virtual void setFont(Font &font);

  Style getStyle();
  void setStyle(Style style);
  void addStyle(Style style);
  void removeStyle(Style style);

  Rect getBounds();
  Rect getAbsoluteBounds();
  virtual Rect getClientBounds();
  Rect getAbsoluteClientBounds();

  // TODO public virtual
  virtual Rect getLayoutBounds();

  void setBounds(const Rect &rc);

  void center();
  void setOrigin(const Point &pt);
  void setOrigin(int x, int y);
  void setSize(const Size &sz);
  void setSize(int w, int h);

  void validate();
  void validate(const Rect &rc);
  void invalidate(bool eraseBg);
  void invalidate(const Rect &rc, bool eraseBg);
  void update();

  bool isVisible();
  virtual void setVisible(bool visible);

  bool isEnabled();
  void setEnabled(bool state);

  Color getBgColor();
  virtual void setBgColor(Color color);

  int getOpacity();
  void setOpacity(int opacity);

//   Cursor &getCursor();
  void setCursor(const Cursor &cursor);

  void focus();
  void acquireCapture();
  void releaseCapture();
  bool hasCapture();
  bool hasMouse();
  void bringToTop();

  int msgBox(String text, String title, int flags);

//   Point getCursorPos();
//   Point getAbsoluteCursorPos();

  void deleteAfterEvent();

  int getThreadOwnerId();

  HWND getHwnd();
  HWND getParentHwnd();

  static Widget *fromHwnd(HWND hwnd);
  static WNDPROC getGlobalWndProc();

//   virtual Size minimumSize();
  virtual Size preferredSize();
  virtual Size preferredSize(const Size &fitIn);
//   virtual Size maximumSize();

  virtual void layout();
  virtual bool isLayoutFree();
  virtual bool wantArrowCursor();
  virtual bool keepEnabledSynchronised();

  // Signals are public, see TN004
  boost::signal<void (KeyEvent &)> KeyUp;   ///< @see onKeyUp
  boost::signal<void (KeyEvent &)> KeyDown; ///< @see onKeyDown
  boost::signal<void (WidgetEvent &ev)> GotFocus; ///< @see onGotFocus
  boost::signal<void (WidgetEvent &ev)> LostFocus; ///< @see onLostFocus

  /**
   * Fired after the wndProc and/or defWndProc processed the current
   * message. When it's fired, all slots are disconnected, so if you
   * bind something to this, the binded method/function will be called
   * just one time, and then disconnected.
   */
//   boost::signal<void ()> AfterEvent;

protected:
  // new events
  virtual void onDestroy();
  virtual void onPaint(Graphics &g);
  virtual void onResize(const Size &sz);
  virtual bool onPowerQuerySuspend(bool interactive);
  virtual void onPowerQuerySuspendFailed();
  virtual void onPowerSuspend();
  virtual void onPowerResumeSuspend();
  virtual void onMouseEnter(MouseEvent &ev);
  virtual void onMouseLeave();
  virtual void onMouseDown(MouseEvent &ev);
  virtual void onMouseUp(MouseEvent &ev);
  virtual void onDoubleClick(MouseEvent &ev);
  virtual void onMouseMove(MouseEvent &ev);
//   virtual void onMouseHover(MouseEvent &ev);
  virtual void onMouseWheel(MouseEvent &ev);
  virtual void onCancelMode();
  virtual void onSetCursor(int hitTest);
  virtual void onKeyUp(KeyEvent &ev);
  virtual void onKeyDown(KeyEvent &ev);
//   virtual void onKeyTyped(KeyEvent &ev);
  virtual void onGotFocus(WidgetEvent &ev);
  virtual void onLostFocus(WidgetEvent &ev);
  virtual bool onIdAction(int id);
  virtual void onBeforePosChange();
  virtual void onAfterPosChange();
  virtual void onVScroll(int code, int pos/*, ScrollBar *scrollbar*/);
  virtual void onHScroll(int code, int pos/*, ScrollBar *scrollbar*/);

  // reflection
  virtual bool onCommand(int commandCode, LRESULT &lResult);
  virtual bool onNotify(LPNMHDR lpnmhdr, LRESULT &lResult);
  virtual bool onDrawItem(Graphics &g, LPDRAWITEMSTRUCT lpDrawItem);

protected:
  // creation & destruction
  void create(LPCTSTR className, Widget *parent, Style style);
  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult);
  virtual LRESULT defWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
  virtual HWND createHwnd(LPCTSTR className, Widget *parent, Style style);
  virtual void destroyHwnd(HWND hwnd);
  bool isDisposedAscendent();
public:
  void dispose();
  virtual bool preTranslateMessage(MSG &msg);
  LRESULT sendMessage(UINT message, WPARAM wParam, LPARAM lParam);

  // TODO public? I don't think so
  void addChild(Widget *child, bool setParent);
  void removeChild(Widget *child, bool setParent);

private:
  Container getEnabledSynchronisedGroup();
//   Container getLayoutFreeGroup();

  static LRESULT CALLBACK globalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

} // namespace Vaca

#endif
