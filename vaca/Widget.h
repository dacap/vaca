// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_WIDGET_H
#define VACA_WIDGET_H

#include "vaca/base.h"
#include "vaca/Color.h"
#include "vaca/Component.h"
#include "vaca/Exception.h"
#include "vaca/Font.h"
#include "vaca/Graphics.h"
#include "vaca/Rect.h"
#include "vaca/Register.h"
#include "vaca/Signal.h"
#include "vaca/Size.h"
#include "vaca/Style.h"
#include "vaca/WidgetClass.h"
#include "vaca/WidgetHit.h"
#include "vaca/WidgetList.h"

#include <vector>

namespace vaca {

/**
   This exception is thrown when the operating system can't create the Widget.

   @win32
     It is thrown when the Widget#createHandle member function fails,
     generally because of @msdn{CreateWindowEx} that returns NULL.
   @endwin32
*/
class CreateWidgetException : public Exception
{
public:
  CreateWidgetException() : Exception() { }
  CreateWidgetException(const String& message) : Exception(message) { }
  virtual ~CreateWidgetException() throw() { }
};

/**
   Base class for widgets.

   What is a widget? Widgets are @link Frame windows@endlink,
   @link Button buttons@endlink, @link Edit text-fields@endlink,
   @link ComboBox combo-boxes@endlink, etc. Each element in a
   window is a widget.

   @win32
     This is a wrapper for @msdn{HWND}. Calls @msdn{CreateWindowEx}
     and @msdn{DestroyWindow}, and its #wndProc member function
     converts the messages (@c "WM_*") to events.
   @endwin32
*/
class VACA_DLL Widget : public Register<WidgetClass>, public Component
{
  friend class MakeWidgetRef;
  friend VACA_DLL void delete_widget(Widget* widget);

public:

  // ============================================================
  // STYLES
  // ============================================================

  struct VACA_DLL Styles {
    /**
       Without style.
    */
    static constexpr Style None = Style(0, 0);

    /**
       A widget that should be shown to the user.

       @win32
       It is WS_VISIBLE.
       @endwin32
    */
    static constexpr Style Visible = Style(WS_VISIBLE, 0);

    /**
       Indicates that a Widget can receive the focus in Dialogs.

       @win32
       It is WS_TABSTOP.
       @endwin32
    */
    static constexpr Style Focusable = Style(WS_TABSTOP, 0);

    /**
       When the widget needs both scroll bars (horizontal and vertical).

       @win32
       It is WS_HSCROLL | WS_VSCROLL.
       @endwin32
    */
    static constexpr Style Scroll = Style(WS_HSCROLL | WS_VSCROLL, 0);

    /**
       When the widget needs the horizontal scroll bar.

       @win32
       It is WS_HSCROLL.
       @endwin32
    */
    static constexpr Style HorizontalScroll = Style(WS_HSCROLL, 0);

    /**
       When the widget needs the veritcal scroll bar.

       @win32
       It is WS_VSCROLL.
       @endwin32
    */
    static constexpr Style VerticalScroll = Style(WS_VSCROLL, 0);

    /**
       This style makes the widget to have an edge in its client area.
       E.g.: This style is used by text-fields (TextEdit) to enclose the
       typed text in a box for example.

       @win32
       It is WS_EX_CLIENTEDGE.
       @endwin32
    */
    static constexpr Style ClientEdge = Style(0, WS_EX_CLIENTEDGE);

    /**
       Style used for container widgets, that is, widget that contains
       children.

       @win32
       It is WS_CLIPCHILDREN | WS_EX_CONTROLPARENT.
       @endwin32
    */
    static constexpr Style Container = Style(WS_CLIPCHILDREN,
                                             WS_EX_CONTROLPARENT);

    /**
       Use this style if the user should be able to drop files on your widget.

       @warning Using this style is the only way to receive the Widget#onDropFiles event.

       @win32
       It is WS_EX_ACCEPTFILES.
       @endwin32
    */
    static constexpr Style AcceptFiles = Style(0, WS_EX_ACCEPTFILES);

    /**
       Default widget style.
    */
    static constexpr Style Default = (Widget::Styles::Visible |
                                      Widget::Styles::Container);
  };

private:

  // ============================================================
  // PRIVATE MEMBERS
  // ============================================================

  /**
     The window handler to use with the Windows API.
  */
  HWND m_handle;

  /**
     Sorted collection of children.
  */
  WidgetList m_children;

  /**
     The parent widget. This could be NULL if the Widget is a Frame or
     something like that.
  */
  Widget* m_parent;

  /**
     Foreground color, generally borders and text color.
  */
  Color m_fgColor;

  /**
     Background color of the widget.
  */
  Color m_bgColor;

  /**
     Constraint used by the layout manager that own the parent widget.
  */
  ConstraintPtr m_constraint;

  /**
     Layout manager of this widget used to arrange its children.
  */
  LayoutPtr m_layout;

  /**
     Flag to indicate if this widget has the mouse.
  */
  bool m_hasMouse : 1;

  /**
     Flag to indicate if we must delete the widget after the current
     event/message is processed.
  */
  bool m_deleteAfterEvent : 1;

  /**
     Use an automatic double-buffering technique to call the #onPaint event.

     @see #setDoubleBuffered, #doPaint
  */
  bool m_doubleBuffered : 1;

  /**
     Current font of the Widget (used mainly to draw the text of the widget).

     @see #setText, #setFont
  */
  Font m_font;

  /**
     Manually set preferred size. If it's equal to NULL (by default it
     is) the preferred size is automatically calculated, if not this size
     is returned by #getPreferredSize.

     @see #setPreferredSize
  */
  Size* m_preferredSize;

  /**
     @todo Try to remove this field (it's only needed for WM_CTLCOLOR* events)
  */
  HBRUSH m_hbrush;

  // ============================================================
  // Special hooks...
  // ============================================================

  /**
     Procedure of the original Win32's control (like @msdn{BUTTON} or @msdn{EDIT}).

     It's set in #subClass member function.
  */
  WNDPROC m_baseWndProc;

  /**
     The default Win32's window procedure to be called if a
     @link Message message@endlink isn't used.

     By default it is Win32's @msdn{DefWindowProc}, but you can change it
     using #setDefWndProc to replace it with other procedure like
     Win32's @msdn{DefFrameProc}.

     @see #setDefWndProc, #defWndProc
  */
  WNDPROC m_defWndProc;

  /**
     Used by MdiChild to send a WM_MDIDESTROY instead of calling
     Win32's DestroyWindow function.
  */
  void (*m_destroyHandleProc)(HWND hwnd);

public:

  // ============================================================
  // CTOR & DTOR
  // ============================================================

  Widget(const WidgetClassName& className, Widget* parent, Style style);
  explicit Widget(Widget* parent, Style style = Styles::Default);
  explicit Widget(HWND handle);
  virtual ~Widget();

  // ============================================================
  // PARENT & CHILDREN RELATIONSHIP
  // ============================================================

  Widget* getRoot();
  Widget* getParent() const;
  Widget* getFirstChild() const;
  Widget* getLastChild() const;
  Widget* getPreviousSibling() const;
  Widget* getNextSibling() const;
  WidgetList getChildren() const;

  void addChild(Widget* child);
  void removeChild(Widget* child);
  bool hasChild(const Widget* child) const;
  bool hasDescendant(const Widget* descendant) const;

  void moveBeforeWidget(Widget* sibling);
  void moveAfterWidget(Widget* sibling);

  // ===============================================================
  // LAYOUT & CONSTRAINT
  // ===============================================================

  LayoutPtr getLayout();
  void setLayout(LayoutPtr layout);

  ConstraintPtr getConstraint();
  void setConstraint(ConstraintPtr constraint);

  virtual bool isLayoutFree() const;

  void layout();

  // ===============================================================
  // TEXT & FONT
  // ===============================================================

  virtual String getText() const;
  virtual void setText(const String& str);

  virtual Font getFont() const;
  virtual void setFont(Font font);

  // ===============================================================
  // COMMAND
  // ===============================================================

  CommandId getId() const;
  void setId(CommandId id);

  Command* getCommandById(CommandId id);
  Command* findCommandById(CommandId id);

  // ===============================================================
  // WIDGET STYLE
  // ===============================================================

  Style getStyle() const;
  bool hasStyle(Style style) const;

  void setStyle(Style style);
  void addStyle(Style style);
  void removeStyle(Style style);

  // ===============================================================
  // SIZE & POSITION
  // ===============================================================

  Rect getBounds() const;
  Rect getAbsoluteBounds() const;
  Rect getClientBounds() const;
  Rect getAbsoluteClientBounds() const;

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

  // ===============================================================
  // SCROLL
  // ===============================================================

  ScrollInfo getScrollInfo(Orientation orientation) const;
  void setScrollInfo(Orientation orientation, const ScrollInfo& scrollInfo);

  int getScrollPos(Orientation orientation) const;
  void setScrollPos(Orientation orientation, int pos);

  Point getScrollPoint() const;
  void setScrollPoint(const Point& pt);

  void hideScrollBar(Orientation orientation);

  void scrollRect(const Rect& rect, const Point& delta);

  // ===============================================================
  // MESSAGES
  // ===============================================================

  void enqueueMessage(const Message& message);
  virtual bool preTranslateMessage(Message& message);

  // ===============================================================
  // WIN32 SPECIFIC
  // ===============================================================

  HWND getHandle() const;
  HWND getParentHandle() const;

  static Widget* fromHandle(HWND hwnd);
  static WNDPROC getGlobalWndProc();

  LRESULT sendMessage(UINT message, WPARAM wParam, LPARAM lParam);

  // ===============================================================
  // SIGNALS (signals are public, see TN004)
  // ===============================================================

  Signal<void(ResizeEvent&)> Resize; ///< @see onResize
  Signal<void(MouseEvent&)> MouseEnter; ///< @see onMouseEnter
  Signal<void(MouseEvent&)> MouseLeave; ///< @see onMouseLeave
  Signal<void(MouseEvent&)> MouseDown; ///< @see onMouseDown
  Signal<void(MouseEvent&)> MouseUp; ///< @see onMouseUp
  Signal<void(MouseEvent&)> MouseMove; ///< @see onMouseMove
  Signal<void(MouseEvent&)> MouseWheel; ///< @see onMouseWheel
  Signal<void(MouseEvent&)> DoubleClick; ///< @see onDoubleClick
  Signal<void(KeyEvent&)> KeyUp; ///< @see onKeyUp
  Signal<void(KeyEvent&)> KeyDown; ///< @see onKeyDown
  Signal<void(FocusEvent&)> FocusEnter; ///< @see onFocusEnter
  Signal<void(FocusEvent&)> FocusLeave; ///< @see onFocusLeave
  Signal<void(DropFilesEvent&)> DropFiles; ///< @see onDropFiles

protected:

  // ===============================================================
  // EVENTS
  // ===============================================================

  virtual void onPreferredSize(PreferredSizeEvent& ev);
  virtual void onLayout(LayoutEvent& ev);
  virtual void onPaint(PaintEvent& ev);
  virtual void onResize(ResizeEvent& ev);
  virtual void onMouseEnter(MouseEvent& ev);
  virtual void onMouseLeave(MouseEvent& ev);
  virtual void onMouseDown(MouseEvent& ev);
  virtual void onMouseUp(MouseEvent& ev);
  virtual void onMouseMove(MouseEvent& ev);
  virtual void onMouseWheel(MouseEvent& ev);
  virtual void onDoubleClick(MouseEvent& ev);
  virtual void onSetCursor(SetCursorEvent& ev);
  virtual void onKeyDown(KeyEvent& ev);
  virtual void onKeyUp(KeyEvent& ev);
  virtual void onFocusEnter(FocusEvent& ev);
  virtual void onFocusLeave(FocusEvent& ev);
  virtual void onCommand(CommandEvent& ev);
  virtual void onUpdateIndicators();
  virtual void onScroll(ScrollEvent& ev);
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
  void setDestroyHandleProc(void (*proc)(HWND));

private:

  void initialize();
  void addChildWin32(Widget* child, bool setParent);
  void removeChildWin32(Widget* child, bool setParent);

  virtual HWND createHandle(LPCTSTR className, Widget* parent, Style style);

  static LRESULT CALLBACK globalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

/**
   Makes a reference to the specified widget. Reference lives in the
   scope of the MakeWidgetRef's instance.
*/
class VACA_DLL MakeWidgetRef : private NonCopyable
{
  WidgetList m_container;

public:
  MakeWidgetRef(Widget* widget);
  virtual ~MakeWidgetRef();

private:
  void safeDelete(Widget* widget);
};

VACA_DLL void delete_widget(Widget* widget);

} // namespace vaca

#endif // VACA_WIDGET_H
