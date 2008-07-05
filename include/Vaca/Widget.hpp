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

#ifndef VACA_WIDGET_HPP
#define VACA_WIDGET_HPP

#include <vector>
#include <boost/signal.hpp>
#include <boost/noncopyable.hpp>

#include "Vaca/base.hpp"
#include "Vaca/Color.hpp"
#include "Vaca/Component.hpp"
#include "Vaca/Exception.hpp"
#include "Vaca/Graphics.hpp"
#include "Vaca/Rect.hpp"
#include "Vaca/ScrollInfo.hpp"
#include "Vaca/Size.hpp"
#include "Vaca/String.hpp"
#include "Vaca/Style.hpp"

namespace Vaca {

class Constraint;
class Cursor;
class DropFilesEvent;
class Event;
class KeyEvent;
class Layout;
class MouseEvent;
class Widget;
class WidgetClassName;

void VACA_DLL delete_widget(Widget* widget);
void __internal_checked_delete_widget(Widget* widget);

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
 * Without a special style.
 */
#define NoStyle			(Style(0, 0))

/**
 * When widget can be viewed by the user (WS_VISIBLE).
 */
#define VisibleStyle            (Style(WS_VISIBLE, 0))

/**
 * Basic style for every child that will be inside a container
 * (WS_CHILD | WS_VISIBLE).
 */
#define ChildStyle              (Style(WS_CHILD | WS_VISIBLE, 0))

/**
 * Style to indicate that a Widget can receive the focus in Dialogs
 * (WS_TABSTOP).
 */
#define FocusableStyle		(Style(WS_TABSTOP, 0))

/**
 * When the widget needs both scroll bars: horizontal and vertical
 * (WS_HSCROLL | WS_VSCROLL).
 */
#define ScrollStyle		(Style(WS_HSCROLL | WS_VSCROLL, 0))

/**
 * When the widget needs the horizontal scroll bar (WS_HSCROLL).
 */
#define HorizontalScrollStyle	(Style(WS_HSCROLL, 0))

/**
 * When the widget needs the veritcal scroll bar (WS_VSCROLL).
 */
#define VerticalScrollStyle	(Style(WS_VSCROLL, 0))

/**
 * This style makes the widget to have an edge in its client area
 * (WS_EX_CLIENTEDGE). This style is used by text-fields (Edit) to
 * enclose the typed text for example.
 */
#define ClientEdgeStyle         (Style(0, WS_EX_CLIENTEDGE))

/**
 * Style used for container widgets, that is, widget that contains
 * children (WS_CLIPCHILDREN | WS_EX_CONTROLPARENT).
 */
#define ContainerStyle		(Style(WS_CLIPCHILDREN,		\
				       WS_EX_CONTROLPARENT))
// #define ClipChildrenStyle	(Style(WS_CLIPCHILDREN, 0))

/**
 * Use this style if you want to receive the onDropFiles() event.
 */
#define AcceptFilesStyle	(Style(0, WS_EX_ACCEPTFILES))

//////////////////////////////////////////////////////////////////////

/**
 * @see WidgetHitTest
 */
struct WidgetHitTestEnum
{
  enum enumeration {
    Error,
    Transparent,

    /**
     * When the mouse is in the background of a Widget.
     */
    Nowhere,

    /**
     * When the mouse is in the client area of a Widget.
     */
    Client,

    /**
     * When the mouse is in the title bar of a Frame.
     */
    Caption,

    /**
     * TODO
     */
    SystemMenu,

    /**
     * TODO
     */
    Size,

    /**
     * When the mouse is in a menu of a Frame.
     */
    Menu,

    /**
     * When the mouse is in a horizontal scroll bar.
     */
    HorizontalScroll,

    /**
     * When the mouse is in a vertical scroll bar.
     */
    VerticalScroll,

    /**
     * When the mouse is in the minimize button.
     */
    MinimizeButton,

    /**
     * When the mouse is in the maximize button.
     */
    MaximizeButton,

    /**
     * When the mouse is in the left side to resize a Frame.
     */
    Left,

    /**
     * When the mouse is in the right side to resize a Frame.
     */
    Right,

    /**
     * When the mouse is in the top side to resize a Frame.
     */
    Top,

    /**
     * When the mouse is in the top-left corner to resize a Frame.
     */
    TopLeft,

    /**
     * When the mouse is in the top-right corner to resize a Frame.
     */
    TopRight,

    /**
     * When the mouse is in the bottom side to resize a Frame.
     */
    Bottom,

    /**
     * When the mouse is in the bottom-left corner to resize a Frame.
     */
    BottomLeft,

    /**
     * When the mouse is in the bottom-right corner to resize a Frame.
     */
    BottomRight,

    /**
     * The cursor is in a border of the widget that doesn't support.
     */
    Border,
    
    Object,
    Close,
    Help
  };
  static const enumeration default_value = Nowhere;
};

/**
 * It indicates a place where the mouse cursor could be above a
 * Widget.
 */
typedef Enum<WidgetHitTestEnum> WidgetHitTest;

//////////////////////////////////////////////////////////////////////

/**
 * This exception should be thrown when the operating system or
 * window manager can't create the Window.
 *
 * In Win32, it's thrown when the Widget::createHWND method fails
 * (returns NULL).
 */
class CreateWidgetException : public Exception
{
public:

  CreateWidgetException() : Exception() { }
  CreateWidgetException(const String& message) : Exception(message) { }
  virtual ~CreateWidgetException() throw() { }

};

/**
 * Base class for every control in an window.
 *
 * This is the core of Vaca. Calls CreateWindowEx and DestroyWindow,
 * and its wndProc() converts the main messages (@c "WM_*") to events.
 */
class VACA_DLL Widget : private boost::noncopyable
		      , public Component
{
  friend void delete_widget(Widget* widget);
  friend void Vaca::__internal_checked_delete_widget(Widget* widget);
  
public:

  /**
   * Collection of widgets. Used to handle the list of children of
   * each widget.
   */
  typedef std::vector<Widget*> Container;

  // ============================================================
  // Members
  // ============================================================

private:

  /**
   * The window handler to use with the Windows API.
   */
  HWND m_HWND;

  /**
   * Sorted collection of children.
   */
  Container m_children;

  /**
   * The parent widget. This could be NULL if the Widget is a Frame or
   * something like that.
   */
  Widget* m_parent;

  /**
   * Foreground color, generally borders and text color.
   */
  Color m_fgColor;

  /**
   * Background color of the widget.
   */
  Color m_bgColor;

  /**
   * Constraint used by the layout manager that own the parent widget.
   */
  Constraint* m_constraint;

  /**
   * Layout manager of this widget used to arrange its children.
   */
  Layout* m_layout;

  /**
   * Flag to indicate if this widget has the mouse.
   */
  bool m_hasMouse : 1;

  /**
   * Flag to indicate if we must delete the widget after the current
   * event/message is processed.
   */
  bool m_deleteAfterEvent : 1;

  /**
   * Use an automatic double-buffering technique to call the #onPaint event.
   *
   * @see #setDoubleBuffered
   */
  bool m_doubleBuffered : 1;

  /**
   * Current font of the Widget (used mainly to draw the text of the widget).
   *
   * @see #setText, #setFont
   */
  Font* m_font;

  /**
   * Manually set preferred size. If it's equal to NULL (by default it
   * is) the preferred size is automatically calculated, if not this size
   * is returned by #getPreferredSize.
   *
   * @see #setPreferredSize
   */
  Size* m_preferredSize;

  // ============================================================
  // TODO Remove this (it's only needed for onSetCursor)
  // ============================================================

  WPARAM m_wparam;
  LPARAM m_lparam;

  // ============================================================
  // Special hooks...
  // ============================================================

  /**
   * Procedure of the original Win32 control (like BUTTON or EDIT).
   */
  WNDPROC m_baseWndProc;

  /**
   * Used to replace the DefWindowProc by other procedure (like DefFrameProc).
   */
  WNDPROC m_defWndProc;

  /**
   * Used by MdiChild to use WM_MDIDESTROY instead of DestroyWindow.
   */
  void (*m_destroyHWNDProc)(HWND hwnd);

public:

  // ============================================================
  // CTOR & DTOR
  // ============================================================

  Widget(const WidgetClassName& className, Widget* parent, Style style = NoStyle);
  virtual ~Widget();

  // ============================================================
  // PARENT & CHILDREN RELATIONSHIP
  // ============================================================

  Widget* getParent();
  Container getChildren();

  // ===============================================================
  // LAYOUT & CONSTRAINT
  // ===============================================================

  Layout* getLayout();
  Layout* setLayout(Layout* layout);

  Constraint* getConstraint();
  Constraint* setConstraint(Constraint* constraint);

  virtual void layout();
  virtual bool isLayoutFree();

  // ===============================================================
  // TEXT & FONT
  // ===============================================================

  virtual String getText();
  virtual void setText(const String& str);

  virtual Font* getFont();
  virtual void setFont(Font* font);

  // ===============================================================
  // WIDGET STYLE
  // ===============================================================

  Style getStyle();
  void setStyle(Style style);
  void addStyle(Style style);
  void removeStyle(Style style);

  // ===============================================================
  // SIZE & POSITION
  // ===============================================================

  Rect getBounds();
  Rect getAbsoluteBounds();
  Rect getClientBounds();
  Rect getAbsoluteClientBounds();

  // TODO public virtual
  virtual Rect getLayoutBounds();

  void setBounds(const Rect& rc);
  void setBounds(int x, int y, int w, int h);

  void center();
  void setOrigin(const Point& pt);
  void setOrigin(int x, int y);
  void setSize(const Size& sz);
  void setSize(int w, int h);

  Size getPreferredSize();
  Size getPreferredSize(const Size& fitIn);
  void setPreferredSize(const Size& fixedSize);
  
  // ===============================================================
  // REFRESH ISSUES
  // ===============================================================

  bool isDoubleBuffered();
  void setDoubleBuffered(bool doubleBuffered);

  void validate();
  void validate(const Rect& rc);
  void invalidate(bool eraseBg);
  void invalidate(const Rect& rc, bool eraseBg);
  void update();

  // ===============================================================
  // COMMON PROPERTIES
  // ===============================================================

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

  // ===============================================================
  // FOCUS & MOUSE
  // ===============================================================

  void requestFocus();
  void releaseFocus();
  void captureMouse();
  void releaseMouse();
  bool hasFocus();
  bool hasMouse();
  bool hasMouseAbove();
  bool hasCapture();

  void setCursor(const Cursor& cursor);

  // ===============================================================
  // WIDGET LAYER
  // ===============================================================

  void bringToTop();
  void sendToBack();
  void moveAfter(Widget* brother);
  void moveBefore(Widget* brother);

  ScrollInfo getScrollInfo(Orientation orientation);
  void setScrollInfo(Orientation orientation, const ScrollInfo& scrollInfo);
  int getScrollPos(Orientation orientation);
  void setScrollPos(Orientation orientation, int pos);
  Point getScrollPoint();
  void setScrollPoint(const Point& pt);
  void hideScrollBar(Orientation orientation);

  int msgBox(const String& text, const String& title, int flags);

  HWND getHWND();
  HWND getParentHWND();

  static Widget* fromHWND(HWND hwnd);
  static WNDPROC getGlobalWndProc();
  
  virtual bool preTranslateMessage(MSG& msg);
  LRESULT sendMessage(UINT message, WPARAM wParam, LPARAM lParam);

  // TODO public? I don't think so
  void addChild(Widget* child, bool setParent);
  void removeChild(Widget* child, bool setParent);

  // ===============================================================
  // SIGNALS (signals are public, see TN004)
  // ===============================================================

  boost::signal<void (const Size&)> Resize;		///< @see onResize
  boost::signal<void (MouseEvent&)> MouseEnter;		///< @see onMouseEnter
  boost::signal<void ()> MouseLeave;			///< @see onMouseLeave
  boost::signal<void (MouseEvent&)> MouseDown;		///< @see onMouseDown
  boost::signal<void (MouseEvent&)> MouseUp;		///< @see onMouseUp
  boost::signal<void (MouseEvent&)> DoubleClick;	///< @see onDoubleClick
  boost::signal<void (MouseEvent&)> MouseMove;		///< @see onMouseMove
  boost::signal<void (MouseEvent&)> MouseWheel;		///< @see onMouseWheel
  boost::signal<void ()> CancelMode;			///< @see CancelMode
  boost::signal<void (KeyEvent&)> KeyUp;		///< @see onKeyUp
  boost::signal<void (KeyEvent&)> KeyDown;		///< @see onKeyDown
  boost::signal<void (KeyEvent&)> KeyTyped;		///< @see onKeyTyped
  boost::signal<void (Event&)> GotFocus;		///< @see onGotFocus
  boost::signal<void (Event&)> LostFocus;		///< @see onLostFocus
  boost::signal<void (DropFilesEvent&)> DropFiles;	///< @see onDropFiles

protected:

  // ===============================================================
  // EVENTS
  // ===============================================================

  virtual void onPreferredSize(Size& sz);
  virtual void onPaint(Graphics& g);
  virtual void onResize(const Size& sz);
  virtual void onMouseEnter(MouseEvent& ev);
  virtual void onMouseLeave();
  virtual void onMouseDown(MouseEvent& ev);
  virtual void onMouseUp(MouseEvent& ev);
  virtual void onDoubleClick(MouseEvent& ev);
  virtual void onMouseMove(MouseEvent& ev);
  virtual void onMouseWheel(MouseEvent& ev);
  virtual void onCancelMode();
  virtual void onSetCursor(WidgetHitTest hitTest);
  virtual void onKeyUp(KeyEvent& ev);
  virtual void onKeyDown(KeyEvent& ev);
  virtual void onKeyTyped(KeyEvent& ev);
  virtual void onGotFocus(Event& ev);
  virtual void onLostFocus(Event& ev);
  virtual bool onActionById(int actionId);
  virtual void onBeforePosChange();
  virtual void onAfterPosChange();
  virtual void onScroll(Orientation orientation, int code);
  virtual void onDropFiles(DropFilesEvent& ev);

  // ===============================================================
  // REFLECTION
  // ===============================================================

  virtual bool onReflectedCommand(int id, int code, LRESULT& lResult);
  virtual bool onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult);
  virtual bool onReflectedDrawItem(Graphics& g, LPDRAWITEMSTRUCT lpDrawItem);

  // ===============================================================
  // CREATION & MESSAGE PROCESSING
  // ===============================================================

  void create(const WidgetClassName& className, Widget* parent, Style style);
  void subClass();
  LRESULT defWndProc(UINT message, WPARAM wParam, LPARAM lParam);
  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

  bool doPaint(Graphics& g);

  void setDefWndProc(WNDPROC proc);
  void setDestroyHWNDProc(void (*proc)(HWND));

private:

  virtual HWND createHWND(LPCTSTR className, Widget* parent, Style style);

  static LRESULT CALLBACK globalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

} // namespace Vaca

#endif
