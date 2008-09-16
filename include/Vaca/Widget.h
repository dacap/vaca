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

#ifndef VACA_WIDGET_H
#define VACA_WIDGET_H

#include "Vaca/base.h"
#include "Vaca/Color.h"
#include "Vaca/Component.h"
#include "Vaca/Exception.h"
#include "Vaca/Font.h"
#include "Vaca/Graphics.h"
#include "Vaca/Rect.h"
#include "Vaca/ScrollInfo.h"
#include "Vaca/Signal.h"
#include "Vaca/Size.h"
#include "Vaca/String.h"
#include "Vaca/Style.h"

#include <vector>

namespace Vaca {

class Command;  
class Constraint;
class Cursor;
class DropFilesEvent;
class Event;
class KeyEvent;
class Layout;
class MakeWidgetRef;
class MouseEvent;
class Widget;
class WidgetClassName;

//////////////////////////////////////////////////////////////////////
// Vaca Widget Styles

/**
 * Without a special style.
 */
#define NoStyle			(Style(0, 0))

/**
 * When widget can be viewed by the user (@msdn{WS_VISIBLE}).
 */
#define VisibleStyle            (Style(WS_VISIBLE, 0))

/**
 * Basic style for every child that will be inside a container
 * (@msdn{WS_CHILD} | @msdn{WS_VISIBLE}).
 */
#define ChildStyle              (Style(WS_CHILD | WS_VISIBLE, 0))

/**
 * Style to indicate that a Widget can receive the focus in Dialogs
 * (@msdn{WS_TABSTOP}).
 */
#define FocusableStyle		(Style(WS_TABSTOP, 0))

/**
 * When the widget needs both scroll bars: horizontal and vertical
 * (@msdn{WS_HSCROLL} | @msdn{WS_VSCROLL}).
 */
#define ScrollStyle		(Style(WS_HSCROLL | WS_VSCROLL, 0))

/**
 * When the widget needs the horizontal scroll bar (@msdn{WS_HSCROLL}).
 */
#define HorizontalScrollStyle	(Style(WS_HSCROLL, 0))

/**
 * When the widget needs the veritcal scroll bar (@msdn{WS_VSCROLL}).
 */
#define VerticalScrollStyle	(Style(WS_VSCROLL, 0))

/**
 * This style makes the widget to have an edge in its client area
 * (@msdn{WS_EX_CLIENTEDGE}). This style is used by text-fields (Edit) to
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
 * Use this style if you want to receive the Widget#onDropFiles event.
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
 * It indicates a place inside a Widget which the mouse cursor
 * could be.
 *
 * One of the following values:
 * @li WidgetHit::Error
 * @li WidgetHit::Transparent
 * @li WidgetHit::Nowhere
 * @li WidgetHit::Client
 * @li WidgetHit::Caption
 * @li WidgetHit::SystemMenu
 * @li WidgetHit::Size
 * @li WidgetHit::Menu
 * @li WidgetHit::HorizontalScroll
 * @li WidgetHit::VerticalScroll
 * @li WidgetHit::MinimizeButton
 * @li WidgetHit::MaximizeButton
 * @li WidgetHit::Left
 * @li WidgetHit::Right
 * @li WidgetHit::Top
 * @li WidgetHit::TopLeft
 * @li WidgetHit::TopRight
 * @li WidgetHit::Bottom
 * @li WidgetHit::BottomLeft
 * @li WidgetHit::BottomRight
 * @li WidgetHit::Border
 * @li WidgetHit::Object
 * @li WidgetHit::Close
 * @li WidgetHit::Help
 */
typedef Enum<WidgetHitTestEnum> WidgetHitTest;

//////////////////////////////////////////////////////////////////////

/**
 * This exception should be thrown when the operating system or
 * window manager can't create the Window.
 *
 * In Win32, it's thrown when the Widget::createHandle method fails
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
 * This is the core of Vaca. Calls @msdn{CreateWindowEx} and @msdn{DestroyWindow},
 * and its wndProc() converts the main messages (@c "WM_*") to events.
 */
class VACA_DLL Widget : public Component
{
  friend class MakeWidgetRef;
  friend void delete_widget(Widget* widget);
  
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
  HWND m_handle;

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
   * @see #setDoubleBuffered, #doPaint
   */
  bool m_doubleBuffered : 1;

  /**
   * Current font of the Widget (used mainly to draw the text of the widget).
   *
   * @see #setText, #setFont
   */
  Font m_font;

  /**
   * Manually set preferred size. If it's equal to NULL (by default it
   * is) the preferred size is automatically calculated, if not this size
   * is returned by #getPreferredSize.
   *
   * @see #setPreferredSize
   */
  Size* m_preferredSize;

  // TODO Remove this (it's only needed for onSetCursor)
  WPARAM m_wparam;
  LPARAM m_lparam;

  // TODO Remove this (it's only needed for onSetCursor)
  HBRUSH m_hbrush;

  // ============================================================
  // Special hooks...
  // ============================================================

  /**
   * Procedure of the original Win32's control (like @msdn{BUTTON} or @msdn{EDIT}).
   *
   * It's set in #subClass method.
   */
  WNDPROC m_baseWndProc;

  /**
   * The default Win32's window procedure to be called if a
   * @link Thread::Message message@endlink isn't used.
   * 
   * By default it is Win32's @msdn{DefWindowProc}, but you can change it
   * using #setDefWndProc to replace it with other procedure like
   * Win32's @msdn{DefFrameProc}.
   *
   * @see #setDefWndProc, #defWndProc
   */
  WNDPROC m_defWndProc;

  /**
   * Used by MdiChild to send a WM_MDIDESTROY instead of calling
   * Win32's DestroyWindow function.
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

  virtual Font getFont() const;
  virtual void setFont(Font font);

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
  void setPreferredSize(int fixedWidth, int fixedHeight);
  
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
  void updateIndicators();

  // ===============================================================
  // COMMON PROPERTIES
  // ===============================================================

  bool isVisible();
  virtual void setVisible(bool visible);

  bool isEnabled();
  void setEnabled(bool state);

  Color getFgColor();
  Color getBgColor();
  virtual void setFgColor(const Color& color);
  virtual void setBgColor(const Color& color);

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

  // ===============================================================
  // SCROLL
  // ===============================================================

  ScrollInfo getScrollInfo(Orientation orientation);
  void setScrollInfo(Orientation orientation, const ScrollInfo& scrollInfo);
  int getScrollPos(Orientation orientation);
  void setScrollPos(Orientation orientation, int pos);
  Point getScrollPoint();
  void setScrollPoint(const Point& pt);
  void hideScrollBar(Orientation orientation);

  // ===============================================================
  // COMMANDS
  // ===============================================================

  Command* findCommandById(CommandId id);

  // ===============================================================
  // WIN32 SPECIFIC
  // ===============================================================

  HWND getHandle();
  HWND getParentHandle();

  static Widget* fromHandle(HWND hwnd);
  static WNDPROC getGlobalWndProc();
  
  virtual bool preTranslateMessage(MSG& msg);
  LRESULT sendMessage(UINT message, WPARAM wParam, LPARAM lParam);

  // TODO public? I don't think so
  void addChild(Widget* child, bool setParent);
  void removeChild(Widget* child, bool setParent);

  // ===============================================================
  // SIGNALS (signals are public, see TN004)
  // ===============================================================

  Signal1<void, const Size&> Resize; ///< @see onResize
  Signal1<void, MouseEvent&> MouseEnter; ///< @see onMouseEnter
  Signal0<void> MouseLeave; ///< @see onMouseLeave
  Signal1<void, MouseEvent&> MouseDown; ///< @see onMouseDown
  Signal1<void, MouseEvent&> MouseUp; ///< @see onMouseUp
  Signal1<void, MouseEvent&> DoubleClick; ///< @see onDoubleClick
  Signal1<void, MouseEvent&> MouseMove; ///< @see onMouseMove
  Signal1<void, MouseEvent&> MouseWheel; ///< @see onMouseWheel
  Signal0<void> CancelMode; ///< @see CancelMode
  Signal1<void, KeyEvent&> KeyUp; ///< @see onKeyUp
  Signal1<void, KeyEvent&> KeyDown; ///< @see onKeyDown
  Signal1<void, Event&> GotFocus; ///< @see onGotFocus
  Signal1<void, Event&> LostFocus; ///< @see onLostFocus
  Signal1<void, DropFilesEvent&> DropFiles; ///< @see onDropFiles

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
  virtual void onKeyDown(KeyEvent& ev);
  virtual void onKeyUp(KeyEvent& ev);
  virtual void onGotFocus(Event& ev);
  virtual void onLostFocus(Event& ev);
  virtual bool onCommand(CommandId id);
  virtual void onUpdateIndicators();
  virtual void onBeforePosChange();
  virtual void onAfterPosChange();
  virtual void onScroll(Orientation orientation, int code);
  virtual void onDropFiles(DropFilesEvent& ev);
  virtual void onRemoveChild(Widget* child);

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

  virtual HWND createHandle(LPCTSTR className, Widget* parent, Style style);

  static LRESULT CALLBACK globalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

/**
 * Makes a reference to the specified widget. Reference lives in the
 * scope of the MakeWidgetRef's instance.
 */
class VACA_DLL MakeWidgetRef : private NonCopyable
{
  Widget::Container m_container;

public:
  MakeWidgetRef(Widget* widget);
  virtual ~MakeWidgetRef();

private:
  void safeDelete(Widget* widget);
};

void VACA_DLL delete_widget(Widget* widget);

} // namespace Vaca

#endif // VACA_WIDGET_H
