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
#include "Vaca/Component.h"
#include "Vaca/Color.h"
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

class Constraint;
class Cursor;
class DropFilesEvent;
class Event;
class KeyEvent;
class Layout;
class MouseEvent;
class Widget;

void VACA_DLL delete_widget(Widget *widget);
void __internal_checked_delete_widget(Widget *widget);

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
 * A visible widget (WS_VISIBLE).
 */
#define VisibleStyle            (Style(WS_VISIBLE, 0))

/**
 * Basic style for every child (WS_CHILD | WS_VISIBLE).
 */
#define ChildStyle              (Style(WS_CHILD | WS_VISIBLE, 0))

/**
 * Style to indicate that a Widget can receive the focus in Dialogs
 * (WS_TABSTOP).
 */
#define FocusableStyle		(Style(WS_TABSTOP, 0))

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

/**
 * Style for container widgets (WS_CLIPCHILDREN | WS_EX_CONTROLPARENT).
 */
// #define ClipChildrenStyle	(Style(WS_CLIPCHILDREN, 0))
#define ContainerStyle		(Style(WS_CLIPCHILDREN,		\
				       WS_EX_CONTROLPARENT))

/**
 * Use this style if you want to receive the onDropFiles() event.
 */
#define AcceptFilesStyle	(Style(0, WS_EX_ACCEPTFILES))

/**
 * Thrown when the Widget::createHWND method returns NULL.
 */
class CreateHWNDException : public Exception
{
public:

  CreateHWNDException() throw() : Exception() { }
  virtual ~CreateHWNDException() throw() { }

};

/**
 * Auxiliary structure to get scroll information.
 */
struct ScrollInfo
{
  int minPos, maxPos;
  int pageSize;
  int pos;
  int trackPos;
};

/**
 * Base class for every control in an window.
 *
 * This is the core of Vaca, calls CreateWindowEx and DestroyWindow,
 * and its wndProc() converts the main messages to events.
 */
class VACA_DLL Widget : private boost::noncopyable
		      , public Component
{
  friend void delete_widget(Widget *widget);
  friend void Vaca::__internal_checked_delete_widget(Widget *widget);
  
public:

  typedef std::vector<Widget *> Container;

private:

  HWND        m_HWND;     // The window handler to use with the Windows API
  Container   m_children; // Unsorted collection of children
  Widget     *m_parent;   // The parent widget
  Color       m_fgColor;
  Color       m_bgColor;
  Constraint *m_constraint;
  Layout     *m_layout;
  bool        m_hasMouse : 1;	      // Flag to indicate that this widget has the mouse.
  bool        m_deleteAfterEvent : 1; // Flag to indicate that we must delete the widget after the event.
  bool        m_doubleBuffered : 1;   // Use an automatic double-buffering technique
  Font       *m_font;
  Size       *m_preferredSize;

  // TODO something to remove this (it's only needed for onSetCursor)
  WPARAM m_wparam;
  LPARAM m_lparam;

  ////////////////////////////////////////
  // Special hooks...
  WNDPROC m_baseWndProc;	        // procedure of the original Win32 control (like BUTTON or EDIT)
  WNDPROC m_defWndProc;		        // used to replace the DefWindowProc by other procedure (like DefFrameProc)
  void (*m_destroyHWNDProc)(HWND hwnd); // used by MdiChild to use WM_MDIDESTROY instead of DestroyWindow
  ////////////////////////////////////////

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

  virtual Font *getFont();
  virtual void setFont(Font *font);

  Style getStyle();
  void setStyle(Style style);
  void addStyle(Style style);
  void removeStyle(Style style);

  bool isDoubleBuffered();
  void setDoubleBuffered(bool doubleBuffered);

  Rect getBounds();
  Rect getAbsoluteBounds();
  Rect getClientBounds();
  Rect getAbsoluteClientBounds();

  // TODO public virtual
  virtual Rect getLayoutBounds();

  void setBounds(const Rect &rc);
  void setBounds(int x, int y, int w, int h);

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

  Color getFgColor();
  Color getBgColor();
  virtual void setFgColor(Color color);
  virtual void setBgColor(Color color);

  int getOpacity();
  void setOpacity(int opacity);

  void setCursor(const Cursor &cursor);

  void acquireFocus();
  void releaseFocus();
  void acquireCapture();
  void releaseCapture();
  bool hasFocus();
  bool hasMouse();
  bool hasMouseAbove();
  bool hasCapture();

  void bringToTop();
  void sendToBack();
  void moveAfter(Widget *brother);
  void moveBefore(Widget *brother);

  ScrollInfo getScrollInfo(Orientation orientation);
  void setScrollInfo(Orientation orientation, const ScrollInfo &scrollInfo);
  int getScrollPos(Orientation orientation);
  void setScrollPos(Orientation orientation, int pos);
  Point getScrollPoint();
  void setScrollPoint(const Point &pt);
  void hideScrollBar(Orientation orientation);

//   void setTimer(int timerId, int millisecondsForTimeOut);
//   void killTimer(int timerId);

  int msgBox(const String &text, const String &title, int flags);

  int getThreadOwnerId();

  HWND getHWND();
  HWND getParentHWND();

  static Widget *fromHWND(HWND hwnd);
  static WNDPROC getGlobalWndProc();

  Size getPreferredSize();
  Size getPreferredSize(const Size &fitIn);
  void setPreferredSize(const Size &fixedSize);
  
//   virtual Size minimumSize();
//   virtual Size preferredSize();
//   virtual Size preferredSize(const Size &fitIn);
//   virtual Size maximumSize();

  virtual void layout();
  virtual bool isLayoutFree();
  
  virtual bool preTranslateMessage(MSG &msg);
  LRESULT sendMessage(UINT message, WPARAM wParam, LPARAM lParam);

  // TODO public? I don't think so
  void addChild(Widget *child, bool setParent);
  void removeChild(Widget *child, bool setParent);

  // Signals are public, see TN004
  boost::signal<void (KeyEvent &)> KeyUp;   ///< @see onKeyUp
  boost::signal<void (KeyEvent &)> KeyDown; ///< @see onKeyDown
  boost::signal<void (Event &ev)> GotFocus; ///< @see onGotFocus
  boost::signal<void (Event &ev)> LostFocus; ///< @see onLostFocus
  boost::signal<void (DropFilesEvent &ev)> DropFiles; ///< @see onDropFiles

protected:

  // new events
  virtual void onPreferredSize(Size &sz);
  virtual void onPaint(Graphics &g);
  virtual void onResize(const Size &sz);
//   virtual void onTimer(int timerId);
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
  virtual void onGotFocus(Event &ev);
  virtual void onLostFocus(Event &ev);
  virtual bool onIdAction(int id);
  virtual void onBeforePosChange();
  virtual void onAfterPosChange();
  virtual void onScroll(Orientation orientation, int code);
  virtual void onDropFiles(DropFilesEvent &ev);

  // reflection
  virtual bool onCommand(int id, int code, LRESULT &lResult);
  virtual bool onNotify(LPNMHDR lpnmhdr, LRESULT &lResult);
  virtual bool onDrawItem(Graphics &g, LPDRAWITEMSTRUCT lpDrawItem);

  // creation & message processing
  void create(LPCTSTR className, Widget *parent, Style style);
  void subClass();
  LRESULT defWndProc(UINT message, WPARAM wParam, LPARAM lParam);
  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult);

  bool doPaint(Graphics &g);

  void setDefWndProc(WNDPROC proc);
  void setDestroyHWNDProc(void (*proc)(HWND));

private:

  virtual HWND createHWND(LPCTSTR className, Widget *parent, Style style);

  static LRESULT CALLBACK globalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

} // namespace Vaca

#endif
