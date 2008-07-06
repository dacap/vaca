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

#include "Vaca/Widget.hpp"
#include "Vaca/WidgetClass.hpp"
#include "Vaca/Brush.hpp"
#include "Vaca/Constraint.hpp"
#include "Vaca/Cursor.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/Dialog.hpp"
#include "Vaca/DropFilesEvent.hpp"
#include "Vaca/Font.hpp"
#include "Vaca/Frame.hpp"
#include "Vaca/Image.hpp"
#include "Vaca/KeyEvent.hpp"
#include "Vaca/Layout.hpp"
#include "Vaca/MouseEvent.hpp"
#include "Vaca/Point.hpp"
#include "Vaca/Region.hpp"
#include "Vaca/System.hpp"
#include "Vaca/Mutex.hpp"
#include "Vaca/ScopedLock.hpp"

// comment this to use the old behaviour (using GWL_USERDATA to store
// the "Widget" pointer)
#define USE_PROP

// uncomment this if you want message reporting in the "vaca.log"
// #define REPORT_MESSAGES

using namespace Vaca;

#ifdef USE_PROP
#define VACAATOM (reinterpret_cast<LPCTSTR>(MAKELPARAM(vacaAtom, 0)))

static Mutex vacaAtomMutex; // used to access vacaAtom
static volatile ATOM vacaAtom = 0;
#endif

// default callback to destroy a HWND
static void Widget_DestroyHWNDProc(HWND hwnd)
{
  ::DestroyWindow(hwnd);
}

//////////////////////////////////////////////////////////////////////
// References stuff

/**
 * Safe way to delete from memory the widget. It deletes the specified
 * widget if it isn't referenced, or defer its deletion for a secure
 * point of deletion (for example, when it's completelly
 * unreferenced).
 */
void Vaca::delete_widget(Widget* widget)
{
  // is unreferenced?
  if (widget->getRefCount() == 0)
    delete widget;
  // is referenced, we must defer the "delete" for "__internal_checked_delete_widget"
  else {
    assert(widget->getRefCount() > 0);

    // hide the window
    ShowWindow(widget->m_HWND, SW_HIDE);

    // delete after event flag
    widget->m_deleteAfterEvent = true;
  }
}

/**
 * @internal
 *
 * Checks if @a widget should be deleted right now, and if it is, deletes it.
 * Don't use this function, it's an internal routine that uses Vaca.
 */
void Vaca::__internal_checked_delete_widget(Widget* widget)
{
  // is "delete-after-event" flag activated and is the widget unreferenced?
  if (widget->m_deleteAfterEvent && widget->getRefCount() == 0) {
    // ok, so we can delete it...
    widget->m_deleteAfterEvent = false;
    delete widget;
  }
}

// increment the ref counter of "widget" and all the parents, filling
// the "container" with all the widgets visited (referenced)
static void ref_widget(Widget* widget, Widget::Container& container)
{
  while (widget != NULL) {
    container.push_back(widget);
    widget->ref();
    widget = widget->getParent();
  }
}

// decrement the ref counter of all the widget in the "container"
static void unref_widget(Widget::Container& container)
{
  for (std::vector<Widget*>::iterator it = container.begin();
       it != container.end();
       ++it) {
    (*it)->unref();
    __internal_checked_delete_widget(*it);
  }
}

// ============================================================
// CTOR & DTOR
// ============================================================

/**
 * Creates a new widget with the specified class name. You can specify a
 * NULL @a className if you want to call create() method by your self
 * (see @ref TN002).
 *
 * You can't create widgets from a HWND, so you can't treat an
 * existent HWND like a Widget.
 *
 * @param className
 *     Must be a registered WNDCLASS for Windows.
 *	
 * @param parent
 *     It's a Widget (generally a Frame) that will act as the container
 *     parent of the new widget.
 * 
 * @param style
 *     You can specify the style to be used to create the
 *     widget. Generally Vaca handles the styles in this way: If the
 *     class is called @c Button, the default style is @c ButtonStyle.
 *     There exist another global styles that can be used for
 *     <em>every widget</em> like @c FocusableStyle or @c VisibleStyle,
 *     but the specific styles for a particular class,
 *     e.g. Edit, end with @c ...EditStyle, like @c ReadOnlyEditStyle.
 */
Widget::Widget(const WidgetClassName& className, Widget* parent, Style style)
{
  // creates the "VacaAtom" (the property name to put the "Widget*"
  // pointer in the HWNDs)
  {
    ScopedLock hold(vacaAtomMutex);
    if (vacaAtom == 0) {
      vacaAtom = GlobalAddAtom(_T("VacaAtom"));
      assert(vacaAtom != 0);
    }
  }

  // initialize members
  m_HWND             = NULL;
  m_parent           = NULL;
  m_fgColor          = System::getColor(COLOR_WINDOWTEXT);
  m_bgColor          = System::getColor(COLOR_3DFACE);
  m_constraint       = NULL;
  m_layout           = NULL;
  m_baseWndProc      = NULL;
  m_hasMouse         = false;
  m_deleteAfterEvent = false;
  m_doubleBuffered   = false;
  m_font             = NULL;
  m_preferredSize    = NULL;
  m_defWndProc       = ::DefWindowProc;
  m_destroyHWNDProc  = Widget_DestroyHWNDProc;

  // create with the specified "className"?
  if (className != WidgetClassName::None)
    create(className, parent, style);
}

/**
 * Destroys the widget, literally calling Win32 DestroyWindow function.
 * 
 * At this point the widget shouldn't have any children, also there is
 * an assert to valid this. So you should care about to remove all
 * dynamic-allocated and added children to this Widget explicity
 * before to reach the Widget destructor (a good place is your own
 * customized widget destructor that is called before Widget
 * destructor). E.g.:
 *
 * @code
 * class MyWidget : public Panel
 * {
 *   Widget* child;
 * public:
 *   MyWidget(Widget* parent)
 *     : Panel(parent)
 *   {
 *     child = new Button("Test", this);
 *   }
 *   virtual ~MyWidget()
 *   {
 *     // delete/remove the widget from the parent before to
 *     // reach the Widget dtor...
 *     delete child;
 *   }
 * }
 * @endcode
 *
 * @see @ref TN002
 */
Widget::~Widget()
{
  assert(::IsWindow(m_HWND));

  // Lost the focus. WARNING: if we don't make this, Dialogs will die
  // suddenly in an infinite loop when TAB key is pressed. It looks
  // like Win32 can't handle dialog boxes, the keyboard focus, and
  // destroyed HWND at the same time
  if (hasFocus())
    releaseFocus();

  // hide the window
  ::ShowWindow(m_HWND, SW_HIDE);

  if (getParent() != NULL) {
    // this is very important!!! we can't set the parent of the HWND:
    // MdiChild needs the parent HWND to send WM_MDIDESTROY
    getParent()->removeChild(this, false);
  }

#if 0
  // delete all children (this is the case for children added using
  // "new" operator and wasn't deleted)
  Container children = getChildren();
  for (Container::iterator it = children.begin();
       it != children.end(); ++it)
    delete (*it);
#endif
  // maybe you forget to remove some children, so we must to check if
  // m_children is empty...
  assert(m_children.empty());
  
  // delete the constraint
  if (m_constraint != NULL) {
    delete m_constraint;
    m_constraint = NULL;
  }

  // delete the layout manager
  if (m_layout != NULL) {
    delete m_layout;
    m_layout = NULL;
  }

  // preferred size
  if (m_preferredSize != NULL) {
    delete m_preferredSize;
    m_preferredSize = NULL;
  }

  // restore the old window-procedure
  if (m_baseWndProc != NULL)
    SetWindowLongPtr(m_HWND, GWLP_WNDPROC,
		     reinterpret_cast<LONG_PTR>(m_baseWndProc));

  // Please!!! don't remove the pointer from the widget, why?
  // because some messages could be lost (like WM_DESTROY)
  // 
  // ...
  //   #ifdef USE_PROP
  //     RemoveProp(m_HWND, VACAATOM);
  //   #else
  //     SetWindowLongPtr(m_HWND, GWL_USERDATA, static_cast<LONG_PTR>(NULL));
  //   #endif
  // ...
  
  // call the procedure to destroy the HWND handler
  m_destroyHWNDProc(m_HWND);
  m_HWND = NULL;
}

// ============================================================
// PARENT & CHILDREN RELATIONSHIP
// ============================================================

/**
 * Returns the parent of the widget. This method doesn't use the
 * GetParent routine of Win32.
 */
Widget* Widget::getParent()
{
  return m_parent;
}

/**
 * Returns the collection of children. The returned list is a
 * copy of the original, so you can do with it what you want,
 * in other words, doesn't matter if you add or remove elements
 * from them, the original list of children will not be modified.
 */
Widget::Container Widget::getChildren()
{
#if 0				// don't use this alternative (it
				// doesn't work to synchronize Frame)
  assert(::IsWindow(m_HWND));

  Container container;

  HWND hwndChild = ::GetWindow(m_HWND, GW_CHILD);
  if (hwndChild != NULL) {
    do {
      Widget* widget = Widget::fromHWND(hwndChild);
      if (widget != NULL)
	container.push_back(widget);
    } while ((hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT)) != NULL);
  }
  
  return container;
#else
  return m_children;
#endif
}

// ===============================================================
// LAYOUT & CONSTRAINT
// ===============================================================

/**
 * Returns the current Layout that arranges the widget's children. For
 * most widgets, generally it'll NULL, but for a Frame you should use
 * setLayout to change the Layout manager.
 *
 * @warning You can't delete the returned pointer (use "delete setLayout(NULL)" instead).
 *
 * @see setLayout, getConstraint, @ref TN010
 */
Layout* Widget::getLayout()
{
  return m_layout;
}

/**
 * Changes the current layout manager to arrange widget's children.
 *
 * The @a layout pointer'll be deleted automatically in the
 * #Vaca::Widget::~Widget destructor. If you change the layout manager, you
 * must to delete the old pointer returned.
 *
 * @code
 * {
 *   Layout betterLayout = new MyBetterLayout()
 *   ...
 *   {
 *     Frame frame(...);
 *     frame.setLayout(new MyLayout(...));
 *     delete frame.setLayout(betterLayout);
 *   }
 *   ...
 *   // here betterLayout doesn't exist (was automatically deleted in the Frame destructor)
 * }
 * @endcode
 *
 * Another example:
 *
 * @code
 * {
 *   Layout myLayout = new MyLayout(...);
 *   Frame frame(...);
 *   frame.setLayout(&myLayout);
 *   frame.setLayout(NULL); // <-- very important!!! to avoid double deletion
 * }
 * @endcode
 *
 * @return
 *     The old Layout manager (you should delete it).
 *
 * @see getLayout, setConstraint, @ref TN010
 */
Layout* Widget::setLayout(Layout* layout)
{
  Layout* oldLayout = m_layout;
  m_layout = layout;
  return oldLayout;
}

/**
 * Returns the widget's constraint. This constraint is used by the
 * parent widget's Layout to known how to lay out the widget. For
 * example, the AnchorLayout uses a Anchor constraint to known what
 * side of a widget is anchored.
 *
 * @warning You can't delete the returned pointer.
 *
 * @see setConstraint, getLayout, @ref TN010
 */
Constraint* Widget::getConstraint()
{
  return m_constraint;
}

/**
 * Modifies the widget's constraint.
 *
 * The @a constraint pointer'll be deleted automatically by
 * the #~Widget() destructor. If you change the constraint,
 * you must to delete the old pointer returned (it's the same behavior
 * that #setLayout()).
 *
 * @return
 *     The old constraint (you should delete it).
 * 
 * @see getConstraint, setLayout, @ref TN010
 */
Constraint* Widget::setConstraint(Constraint* constraint)
{
  Constraint* oldConstraint = m_constraint;
  m_constraint = constraint;
  return oldConstraint;
}

/**
 * Must arranges the children bounds. The default implementation calls
 * the Layout::layout method of the Widget's layout manager
 * (Widget::m_layout), but you can override this to obtain your own
 * behavior (you can avoid to use Layout manager if you want).
 * <p>
 * This method is called from Widget::onResize by default, so when the
 * Widget is shown for first time or it's resized, this method is
 * called.
 *
 * @see getLayout, setLayout, getLayoutBounds. The @ref
 * exampleCalendar "Calendar example" to known how to override this
 * method.
 */
void Widget::layout()
{
  Container children = getChildren();

  if (m_layout != NULL && !children.empty())
    m_layout->layout(this, children, getLayoutBounds());
}

/**
 * Returns true if the widget is layout-free, that means the Layout
 * shouldn't arrange this widget. The default implementation looks for
 * the VisibleStyle (if it's hidden, it's free of layout).
 *
 * @see getLayout, setLayout
 */
bool Widget::isLayoutFree()
{
  // a widget is free of layout if it's hidden
  return ((getStyle().regular & WS_VISIBLE) == WS_VISIBLE) ? false: true;
}

// ===============================================================
// TEXT & FONT
// ===============================================================

/**
 * Returns the widget's text, label, or frame's title. It uses
 * GetWindowTextLength and GetWindowText functions of Win32.
 *
 * @see setText
 */
String Widget::getText()
{
  assert(::IsWindow(m_HWND));

  int len = ::GetWindowTextLength(m_HWND);
  if (!len)
    return String("");
  else {
    LPTSTR buf = (LPTSTR)new TCHAR[len+1];
    ::GetWindowText(m_HWND, buf, len+1);
    String str = String(buf);
    delete[] buf;
    return str;
  }
}

/**
 * Changes the widget's text, label, or frame's title. It uses the
 * SetWindowText of Win32.
 */
void Widget::setText(const String& str)
{
  assert(::IsWindow(m_HWND));
  ::SetWindowText(m_HWND, str.c_str());
}

/**
 * Returns the current font used to paint the Widget. If you don't use
 * Widget::setFont, the default font is used (Font::getDefault).
 */
Font* Widget::getFont()
{
  assert(m_font != NULL);
  return m_font;
}

/**
 * Sets the font of the widget. The font should have the same life cycle
 * of the Widget. The font'll not be automatically deleted. If you use
 * Font::assign or Font::operator= methods for @a font, remember to
 * recall this routine with the same @a font.
 *
 * @see Font::assign, Font::operator=
 */
void Widget::setFont(Font* font)
{
  assert(font != NULL);
  
  m_font = font;
  sendMessage(WM_SETFONT, reinterpret_cast<WPARAM>(m_font->getHFONT()), TRUE);
}

// ===============================================================
// WIDGET STYLE
// ===============================================================

/**
 * Returns the current Widget style.
 */
Style Widget::getStyle()
{
  assert(::IsWindow(m_HWND));

  return Style(::GetWindowLong(m_HWND, GWL_STYLE),
	       ::GetWindowLong(m_HWND, GWL_EXSTYLE));
}

/**
 * Replaces all the styles of the Widget with the new ones specified
 * in @a style parameter.
 */
void Widget::setStyle(Style style)
{
  assert(::IsWindow(m_HWND));

  ::SetWindowLong(m_HWND, GWL_STYLE, style.regular);
  ::SetWindowLong(m_HWND, GWL_EXSTYLE, style.extended);

  // TODO MSDN says to do this after SetWindowLong
//   SetWindowPos(mWND, NULL, 0, 0, 0, 0,
// 	       SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

/**
 * Adds styles to the widget.
 */
void Widget::addStyle(Style style)
{
  assert(::IsWindow(m_HWND));

  setStyle(getStyle() + style);
}

/**
 * Removes styles from the widget.
 */
void Widget::removeStyle(Style style)
{
  assert(::IsWindow(m_HWND));

  setStyle(getStyle() - style);
}

// ===============================================================
// SIZE & POSITION
// ===============================================================

/**
 * Gets the dimensions of the entire bounding rectangle that enclose
 * the Widget. The bounds are relative to the upper-left corner of the
 * widget's bounds, or absolute to the screen if this Widget
 * hasn't parent.
 *
 * @see getClientBounds, getAbsoluteBounds
 */
Rect Widget::getBounds()
{
  assert(::IsWindow(m_HWND));

  RECT rc;
  ::GetWindowRect(m_HWND, &rc);

  if (m_parent != NULL &&
      (::GetWindowLong(m_HWND, GWL_STYLE) & WS_CAPTION) == 0) {
    POINT pt1 = Point(rc.left, rc.top);
    POINT pt2 = Point(rc.right, rc.bottom);

    ::ScreenToClient(m_parent->m_HWND, &pt1);
    ::ScreenToClient(m_parent->m_HWND, &pt2);

    return Rect(pt1.x, pt1.y, pt2.x-pt1.x, pt2.y-pt1.y);
  }
  else {
    return Rect(&rc);
  }
}

/**
 * Gets the dimensions of the bounding rectangle relative to the
 * upper-left corner of the screen (absolute position in the screen).
 *
 * @see getAbsoluteClientBounds, getBounds
 */
Rect Widget::getAbsoluteBounds()
{
  assert(::IsWindow(m_HWND));

  RECT rc;
  ::GetWindowRect(m_HWND, &rc);

  return Rect(&rc);
}

/**
 * Gets the client bounds. It's like Win32 GetClientRect.
 * Remember that it's the area which you should use to draw
 * the widget.
 *
 * @see getBounds, getAbsoluteClientBounds
 */
Rect Widget::getClientBounds()
{
  RECT rc;
  assert(::IsWindow(m_HWND));
  ::GetClientRect(m_HWND, &rc);
  return Rect(&rc);
}

/**
 * Gets the client bounds relative to the upper-left corner of the
 * screen.
 *
 * @see getAbsoluteBounds, getClientBounds
 */
Rect Widget::getAbsoluteClientBounds()
{
  RECT rc = getClientBounds();
  ::MapWindowPoints(m_HWND, NULL, (POINT*)&rc, 2);
  return Rect(&rc);
}

/**
 * Returns the area where the Layout must arrange the collection of
 * children. It's generally the client bounds, but other widgets (like
 * Tab) could use a small area inside the widget.
 *
 * @see getClientBounds
 */
Rect Widget::getLayoutBounds()
{
  return getClientBounds();
}

/**
 * Sets the boundary rectangle for the widget. The rectangle must be
 * in absolute coordinates for widgets without parent, and relative
 * otherwise.
 *
 * @see getBounds
 */
void Widget::setBounds(const Rect& rc)
{
  assert(::IsWindow(m_HWND));

  /* TODO remove this
  ::SetWindowPos(m_HWND, NULL,
		 rc.left, rc.top, rc.getWidth(), rc.getHeight(),
		 SWP_NOZORDER | SWP_NOACTIVATE);
  */

  ::MoveWindow(m_HWND, rc.x, rc.y, rc.w, rc.h, TRUE);
}

/**
 * TODO docme
 */
void Widget::setBounds(int x, int y, int w, int h)
{
  setBounds(Rect(x, y, w, h));
}

/**
 * Sets the bounds to the screen center.
 *
 * @see setOrigin, setBounds
 */
void Widget::center()
{
  Size sz = getBounds().getSize();
  Rect workArea = System::getWorkAreaBounds();
  Rect refBounds, newBounds;
  
  if (getParent() != NULL)
    refBounds = getParent()->getAbsoluteBounds();
  else
    refBounds = workArea;

  newBounds = Rect(Point(refBounds.x+refBounds.w/2-sz.w/2,
			 refBounds.y+refBounds.h/2-sz.h/2), sz);

  if (newBounds.x < workArea.x) newBounds.x = workArea.x;
  if (newBounds.y < workArea.y) newBounds.y = workArea.y;

  setBounds(newBounds);
}

/**
 * Sets the origin position of the widget. It doesn't affect the size.
 *
 * @see setBounds, center
 */
void Widget::setOrigin(const Point& pt)
{
  Size sz = getBounds().getSize();

  setBounds(Rect(pt, sz));
}

/**
 * TODO docme
 */
void Widget::setOrigin(int x, int y)
{
  setOrigin(Point(x, y));
}

/**
 * Sets the size of the widget. It doesn't affect the origin position.
 *
 * @see setSize(int,int)
 */
void Widget::setSize(const Size& sz)
{
  Point pt = getBounds().getOrigin();
  
  setBounds(Rect(pt, sz));
}

/**
 * TODO docme
 */
void Widget::setSize(int w, int h)
{
  setSize(Size(w, h));
}

/**
 * Returns the preferred size of the Widget. It checks if the
 * preferred size is static (it means when it was set through
 * #setPreferredSize before) or if it's dynamic (this is the
 * default and is when the #onPreferredSize is used to determined
 * the preferred size).
 *
 * In another words, if you don't use #setPreferredSize to set a
 * <em>static preferred size</em> for the widget then #onPreferredSize
 * will be used to calculate it.
 *
 * @see setPreferredSize, onPreferredSize, #getPreferredSize(const Size &)
 */
Size Widget::getPreferredSize()
{
  if (m_preferredSize != NULL)
    return *m_preferredSize;
  else {
    Size sz(0, 0);
    onPreferredSize(sz);
    return sz;
  }
}

/**
 * Returns the preferred size trying to fit in the specified size.
 * Remember that if you use #setPreferredSize this routine will
 * return the static size which you specified manually.
 *
 * @param fitIn
 *     This can have both attributes (width and height) in
 *     zero, which means that it'll behave same as #getPreferredSize().
 *     If the width is great than zero the #onPreferredSize will try to
 *     fit in that width (this is useful to fit #Label or #Edit controls
 *     in a specified width and calculate the height it could occupy).
 *
 * @see #getPreferredSize()
 */
Size Widget::getPreferredSize(const Size& fitIn)
{
  if (m_preferredSize != NULL)
    return *m_preferredSize;
  else {
    Size sz(fitIn);
    onPreferredSize(sz);
    return sz;
  }
}

/**
 * Sets a fixed preferred size specified by the user.
 * Widget::getPreferredSize() will return this value if it's setted.
 */
void Widget::setPreferredSize(const Size& fixedSize)
{
  if (m_preferredSize != NULL)
    delete m_preferredSize;

  m_preferredSize = new Size(fixedSize);
}

// ===============================================================
// REFRESH ISSUES
// ===============================================================

/**
 * Returns true if the double-buffering technique is activated
 * in this widget.
 *
 * @see setDoubleBuffered
 */
bool Widget::isDoubleBuffered()
{
  return m_doubleBuffered;
}

/**
 * Sets if you want or not to use a double-buffering technique to
 * draw the entire widget content. With double-buffering technique
 * you can avoid flickering.
 * 
 * @see isDoubleBuffered
 */
void Widget::setDoubleBuffered(bool doubleBuffered)
{
  m_doubleBuffered = doubleBuffered;
}

/**
 * Validates the entire widget. It removes all paint messages
 * from the message queue, because a validated widget is like
 * a widget that doesn't need to be repainted.
 *
 * @see invalidate()
 */
void Widget::validate()
{
  assert(::IsWindow(m_HWND));
  ::ValidateRect(m_HWND, NULL);
}

/**
 * Validates a part of the widget. This means that the specified rectangle
 * doesn't need to be repainted.
 *
 * @see invalidate(bool)
 */
void Widget::validate(const Rect& rc)
{
  assert(::IsWindow(m_HWND));

  RECT rc2 = rc;
  ::ValidateRect(m_HWND, &rc2);
}

/**
 * Invalidates the entire client area. All the widget will be repainted
 * through an onPaint() event.
 *
 * @param eraseBg
 *     true means that the background should be erased
 *     (with a WM_ERASEBKGND message).
 *
 * @see validate(), invalidate(const Rect &, bool), update()
 */
void Widget::invalidate(bool eraseBg)
{
  assert(::IsWindow(m_HWND));
  ::InvalidateRect(m_HWND, NULL, eraseBg);
}

/**
 * Invalidates the specified rectangle.
 *
 * When the next round of <em>paint messages</em> is processed by the
 * operating system the specified area will be redrawn. This means
 * that the area will not be repainted immediately (you should call
 * #update() method to do this).
 *
 * @param rc
 *     Area to repaint.
 *
 * @param eraseBg
 *     true means that the background should be erased with
 *     the background color specified by #getBgColor (with a
 *     WM_ERASEBKGND message for example).
 *
 * @see invalidate(bool), update()
 */
void Widget::invalidate(const Rect& rc, bool eraseBg)
{
  RECT rc2 = rc;

  assert(::IsWindow(m_HWND));
  ::InvalidateRect(m_HWND, &rc2, eraseBg);
}

/**
 * Flushes the widget invalidated area to redraw it now. This routine
 * should be used if you want to show the changes of the widget immediately
 * without to wait for the message queue to be processed.
 * <p>
 * It's like Win32's UpdateWindow.
 */
void Widget::update()
{
  assert(::IsWindow(m_HWND));
  ::UpdateWindow(m_HWND);
}

// ===============================================================
// COMMON PROPERTIES
// ===============================================================

/**
 * Returns true if this widget is visible. If this widget or some of
 * its parents has the visibility state, this routine returns
 * true.
 *
 * @see Frame::setVisible
 */
bool Widget::isVisible()
{
  assert(::IsWindow(m_HWND));

  return ::IsWindowVisible(m_HWND) != FALSE;
}

/**
 * Changes the visibility of this widget. The children are hidden
 * too.
 */
void Widget::setVisible(bool visible)
{
  assert(::IsWindow(m_HWND));

  if (hasFocus())
    releaseFocus();

  if (visible)
    ::ShowWindow(m_HWND, SW_SHOW);
  else
    ::ShowWindow(m_HWND, SW_HIDE);
}

/**
 * Returns true if the widget is enabled.
 * <p>
 * It's like to ask if the widget hasn't the WS_DISABLED style.
 * Really, this method calls the Win32 IsWindowEnabled function.
 */
bool Widget::isEnabled()
{
  assert(::IsWindow(m_HWND));

  return ::IsWindowEnabled(m_HWND) != FALSE;
}

/**
 * Changes the enable-state of the widget. If @a state is true removes
 * the WS_DISABLED style, if @a state is false adds the WS_DISABLED
 * style. It's like to call the Win32 EnableWindow.
 */
void Widget::setEnabled(bool state)
{
  assert(::IsWindow(m_HWND));

//   if (::GetFocus() == m_HWND)
//     ::SetFocus(NULL);
  if (hasFocus())
    releaseFocus();

  ::EnableWindow(m_HWND, state ? TRUE: FALSE);
}

/**
 * Gets the foreground color. The foreground color generally is the
 * font color to be used.
 *
 * @return
 *     The current foreground color for the widget.
 *
 * @see setFgColor
 */
Color Widget::getFgColor()
{
  return m_fgColor;
}

/**
 * Gets the background color.
 *
 * @return
 *     The current background color for the widget.
 *
 * @see setBgColor
 */
Color Widget::getBgColor()
{
  return m_bgColor;
}

/**
 * Sets the foreground color. This color is used when WM_CTLCOLOR*
 * messages are received. The default value is the System::getColor(COLOR_WINDOWTEXT).
 *
 * @see getFgColor
 */
void Widget::setFgColor(Color color)
{
  m_fgColor = color;
}

/**
 * Sets the background color. This color is used on WM_ERASEBKGND and
 * WM_COLORCTL* messages. The default value is the
 * System::getColor(COLOR_3DFACE).
 *
 * @see getBgColor
 */
void Widget::setBgColor(Color color)
{
  m_bgColor = color;
}

//////////////////////////////////////////////////////////////////////
// Based on code of J Brown

// Prototypes for Win2000/XP API
typedef BOOL (WINAPI * GLWAProc)(HWND hwnd, COLORREF *pcrKey, BYTE *pbAlpha, DWORD *pdwFlags);
typedef BOOL (WINAPI * SLWAProc)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

#ifndef LWA_COLORKEY
#define LWA_COLORKEY            0x00000001
#define LWA_ALPHA               0x00000002
#endif

#ifndef ULW_COLORKEY
#define ULW_COLORKEY            0x00000001
#define ULW_ALPHA               0x00000002
#define ULW_OPAQUE              0x00000004
#endif

/**
 * Returns the widget opacity. If the current OS doesn't support
 * translucent windows, it will always be 255.
 *
 * @return A value from 0 (completelly transparent) to 255 (completelly opaque)
 */
int Widget::getOpacity()
{
  assert(::IsWindow(m_HWND));

  HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
  GLWAProc pGLWA;
  if (hUser32 != NULL)
    pGLWA = (GLWAProc)GetProcAddress(hUser32, "GetLayeredWindowAttributes");
  else
    pGLWA = NULL;

  if (pGLWA == NULL) {
    return 255;
  }
  else if (GetWindowLong(m_HWND, GWL_EXSTYLE) & WS_EX_LAYERED) {
    BYTE opacity;
    pGLWA(m_HWND, NULL, &opacity, NULL);
    return opacity;
  }
  else {
    return 255;
  }
}

/**
 * Sets the alpha property of the widget. It uses the
 * SetLayeredWindowAttributes, if it's available, if not, does
 * nothing.
 *
 * @param opacity
 *     A value from 0 (completelly transparent) to 255 (completelly
 *     opaque)
 */
void Widget::setOpacity(int opacity)
{
  assert(::IsWindow(m_HWND));
  assert(opacity >= 0 && opacity < 256);

  HMODULE  hUser32 = GetModuleHandle(_T("USER32.DLL"));
  SLWAProc pSLWA = (SLWAProc)GetProcAddress(hUser32, "SetLayeredWindowAttributes");

  // The version of Windows running does not support translucent windows!
  if (pSLWA == 0) {
    // do nothing
    return;
  }
  else if (opacity == 255) {
    SetWindowLong(m_HWND, GWL_EXSTYLE,
		  GetWindowLong(m_HWND, GWL_EXSTYLE) & (~WS_EX_LAYERED));
  }
  else {
    SetWindowLong(m_HWND, GWL_EXSTYLE,
  		  GetWindowLong(m_HWND, GWL_EXSTYLE) | WS_EX_LAYERED);

    pSLWA(m_HWND, 0, opacity, LWA_ALPHA);
  }
}

// ===============================================================
// FOCUS & MOUSE
// ===============================================================


/**
 * Sets the keyboard focus to this Widget.
 */
void Widget::requestFocus()
{
  assert(::IsWindow(m_HWND));
#if 0
  bool inDialog = false;
  Widget* parent = this;
  while (parent != NULL) {
    if (dynamic_cast<Dialog*>(parent) != NULL) {
      inDialog = true;
      break;
    }
    parent = parent->getParent();
  }

  if (inDialog)
    parent->sendMessage(WM_NEXTDLGCTL,
			reinterpret_cast<WPARAM>(m_HWND),
			MAKELPARAM(TRUE, 0));
  else
    ::SetFocus(m_HWND);
#else
  ::SetFocus(m_HWND);
#endif
}

/**
 * Frees the keyboard focus.
 */
void Widget::releaseFocus()
{
  assert(::IsWindow(m_HWND));
  assert(m_HWND == ::GetFocus()); // you must to have the focus to use this method
#if 0
  bool inDialog = false;
  Widget* parent = this;
  while (parent != NULL) {
    if (dynamic_cast<Dialog*>(parent) != NULL) {
      inDialog = true;
      break;
    }
    parent = parent->getParent();
  }

  if (inDialog) {
    // WARNING: It's the only way that I found to fix a big problem
    // with focus, dialog boxes, and destroyed widgets in dialog-loop.
    // Without doing this, the dialog could finish in an infinite loop
    // (because focus handling I think)
    ::SetFocus(NULL);
    parent->sendMessage(WM_NEXTDLGCTL,
			static_cast<WPARAM>(NULL),
			MAKELPARAM(TRUE, 0));
  }
  else
    ::SetFocus(NULL);
#else
  ::SetFocus(NULL);
#endif
}

/**
 * Captures the mouse to receive its events even when it's outside the
 * widget (you must call Widget::releaseCapture when you are done with
 * the mouse).
 */
void Widget::captureMouse()
{
  assert(::IsWindow(m_HWND));

  ::SetCapture(m_HWND);
}

/**
 * Releases the previously captured mouse (Widget::acquireCapture).
 */
void Widget::releaseMouse()
{
  assert(::IsWindow(m_HWND));
  assert(m_HWND == ::GetCapture()); // you must to have the capture to use this method

  ::ReleaseCapture();
}

/**
 * Returns true if the widget has the focus.
 */
bool Widget::hasFocus()
{
  assert(::IsWindow(m_HWND));
  return m_HWND == ::GetFocus();
}

/**
 * Returns true if the widget has the mouse. When you capture the
 * mouse, you continue having the .
 *
 * @see hasCapture, hasMouseAbove
 */
bool Widget::hasMouse()
{
  return m_hasMouse;
}

/**
 * Returns true if the widget has the mouse above (it doesn't matter
 * if the widget has the capture status).
 *
 * @see hasMouse
 */
bool Widget::hasMouseAbove()
{
  return ::WindowFromPoint(System::getCursorPos()) == m_HWND;
}

/**
 * Returns true if the widget has captured the mouse.
 *
 * @see hasMouse
 */
bool Widget::hasCapture()
{
  assert(::IsWindow(m_HWND));

  return m_HWND == ::GetCapture();
}

/**
 * You should use this method only inside onSetCursor() event.
 */
void Widget::setCursor(const Cursor& cursor)
{
  // getHCURSOR can be NULL, like the Cursor(NoCursor)
  ::SetCursor(const_cast<Cursor*>(&cursor)->getHCURSOR());
}

// ===============================================================
// WIDGET LAYER
// ===============================================================

/**
 * Sends this window to the top.
 */
void Widget::bringToTop()
{
  assert(::IsWindow(m_HWND));
//   BringWindowToTop(m_HWND);
  ::SetWindowPos(m_HWND, HWND_TOP, 0, 0, 0, 0,
		 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
  
}

/**
 * Sends this window to the bottom.
 */
void Widget::sendToBack()
{
  assert(::IsWindow(m_HWND));
  SetWindowPos(m_HWND, HWND_BOTTOM, 0, 0, 0, 0,
	       SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

/**
 * TODO docme
 */
void Widget::moveAfter(Widget* brother)
{
  assert(m_HWND != NULL && brother != NULL && brother->m_HWND != NULL);
  assert(m_parent == brother->m_parent);

  SetWindowPos(m_HWND, brother->getHWND(), 0, 0, 0, 0,
	       SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

/**
 * TODO docme
 */
void Widget::moveBefore(Widget* brother)
{
  assert(m_HWND != NULL && brother != NULL && brother->m_HWND != NULL);
  assert(m_parent == brother->m_parent);

  HWND hPrev = GetWindow(brother->m_HWND, GW_HWNDPREV);
  if (hPrev != NULL)
    brother = Widget::fromHWND(hPrev);
  else
    brother = NULL;
  
  SetWindowPos(m_HWND, (brother != NULL ? brother->getHWND(): HWND_TOP),
	       0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

/**
 * TODO docme
 */
ScrollInfo Widget::getScrollInfo(Orientation orientation)
{
  assert(::IsWindow(m_HWND));
    
  int fnBar = (orientation == Orientation::Horizontal) ? SB_HORZ: SB_VERT;

  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask = SIF_ALL;

  GetScrollInfo(m_HWND, fnBar, &si);

  ScrollInfo scrollInfo;
  scrollInfo.minPos = si.nMin;
  scrollInfo.maxPos = si.nMax;
  scrollInfo.pageSize = si.nPage;
  scrollInfo.pos = si.nPos;
  scrollInfo.trackPos = si.nTrackPos;
  return scrollInfo;
}

/** 
 * Changes the scroll information about this Widget.
 * 
 * @param orientation
 *     What scroll bar use (Horizontal or Vertical)
 *	
 * @param scrollInfo
 *     New scroll information to be changed.
 */
void Widget::setScrollInfo(Orientation orientation, const ScrollInfo& scrollInfo)
{
  assert(::IsWindow(m_HWND));
    
  int fnBar = (orientation == Orientation::Horizontal) ? SB_HORZ: SB_VERT;

  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask  = SIF_ALL;
  si.nMin = scrollInfo.minPos;
  si.nMax = scrollInfo.maxPos;
  si.nPage = scrollInfo.pageSize;
  si.nPos = scrollInfo.pos;
  si.nTrackPos = scrollInfo.trackPos;

  ::SetScrollInfo(m_HWND, fnBar, &si, TRUE);
}
  
/**
 * TODO docme
 */
int Widget::getScrollPos(Orientation orientation)
{
  assert(::IsWindow(m_HWND));
    
  int fnBar = (orientation == Orientation::Horizontal) ? SB_HORZ: SB_VERT;

  SCROLLINFO si;
  si.cbSize = sizeof (si);
  si.fMask  = SIF_POS;
  ::GetScrollInfo(m_HWND, fnBar, &si);
  return si.nPos;
}

/**
 * TODO docme
 */
void Widget::setScrollPos(Orientation orientation, int pos)
{
  assert(::IsWindow(m_HWND));

  int fnBar = (orientation == Orientation::Horizontal) ? SB_HORZ: SB_VERT;
    
  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask  = SIF_ALL;
  ::GetScrollInfo(m_HWND, fnBar, &si);

  si.fMask = SIF_POS;
  si.nPos = VACA_MID(si.nMin,
		     pos,
		     si.nMax - VACA_MAX(static_cast<int>(si.nPage) - 1, 0));
  ::SetScrollInfo(m_HWND, fnBar, &si, TRUE);
}
  
/**
 * TODO docme
 */
Point Widget::getScrollPoint()
{
  return Point(getScrollPos(Orientation::Horizontal),
	       getScrollPos(Orientation::Vertical));
}

/**
 * TODO docme
 */
void Widget::setScrollPoint(const Point& pt)
{
  setScrollPos(Orientation::Horizontal, pt.x);
  setScrollPos(Orientation::Vertical, pt.y);
}

/**
 * TODO docme
 */
void Widget::hideScrollBar(Orientation orientation)
{
  assert(::IsWindow(m_HWND));

  int fnBar = (orientation == Orientation::Horizontal) ? SB_HORZ: SB_VERT;

  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask  = SIF_RANGE;
  si.nMin = si.nMax = 0;
  ::SetScrollInfo(getHWND(), fnBar, &si, TRUE);
}

/**
 * Shows a message box, locking the widget. It's like call Win32's
 * MessageBox using the Widget's m_HWND (Widget::getHWND).
 * <p>
 * The next code is a tipical example where is displayed a message box
 * with the "Yes" and "No" buttons (@c MB_YESNO), an icon to indicate
 * a warning (@c MB_ICONWARNING), and finally display by default the
 * "No" button with the focus (@c MB_DEFBUTTON2):
 *
 * @code
 *   ... inside a Widget's method ...
 *
 *   if (msgBox("Are you sure?", "Warning", MB_YESNO |
 *                                          MB_ICONWARNING |
 *                                          MB_DEFBUTTON2) == IDYES) {
 *	// the user press Yes, do it!
 *   }
 *
 *   ...
 * @endcode
 *
 * @param text
 *     Text shown in the center of the dialog box. It can contains "\n" to break lines.
 *	
 * @param title
 *     Title for the dialog box.
 *	
 * @param flags
 *     What kind of message box to show.
 *     <p>
 *     @a flags should be one of these values:
 *     - @c MB_OK
 *     - @c MB_YESNO
 *     - @c MB_YESNOCANCEL
 *     - @c MB_RETRYCANCEL
 *     - @c MB_ABORTRETRYIGNORE
 *     - @c MB_CANCELTRYCONTINUE
 *     <p>
 *     Optionally combined with one of these ones:
 *     - @c MB_ICONWARNING
 *     - @c MB_ICONERROR
 *     - @c MB_ICONQUESTION
 *     - @c MB_ICONINFORMATION
 *     <p>
 *     More optionally combined with one of these other ones:
 *     - @c MB_DEFBUTTON1
 *     - @c MB_DEFBUTTON2
 *     - @c MB_DEFBUTTON3
 *
 * @return
 *     It can returns:
 *     - @c IDOK for @c MB_OK
 *     - @c IDYES or @c IDNO for @c MB_YESNO
 *     - @c IDYES, @c IDNO or @c IDCANCEL for @c MB_YESNOCANCEL
 *     - @c IDRETRY or @c IDCANCEL for @c MB_RETRYCANCEL
 *     - @c IDABORT, @c IDRETRY or @c IDIGNORE for @c MB_ABORTRETRYIGNORE
 *     - @c IDCANCEL, @c IDTRYAGAIN or @c IDCONTINUE for @c MB_CANCELTRYCONTINUE
 */
int Widget::msgBox(const String& text, const String& title, int flags)
{
  return ::MessageBox(m_HWND, text.c_str(), title.c_str(), flags);
}

/**
 * Returns the HWND of this Widget. This can't be NULL.
 *
 * @see fromHWND, getParentHWND
 */
HWND Widget::getHWND()
{
  return m_HWND;
}

/**
 * Returns the HWND of the parent, or NULL if this widget doesn't have a parent.
 *
 * @see getHWND
 */
HWND Widget::getParentHWND()
{
  Widget* parent = getParent();
  return parent != NULL ? parent->getHWND(): NULL;
}

/**
 * Returns the Widget pointer from an internal field of the specified
 * HWND. This routine makes sense only when the HWND comes from a
 * Vaca's Widget. In other words, you should use this only if you known
 * that the HWND was created inside Vaca bounds.
 * 
 * @warning Old versions of Vaca uses the GWL_USERDATA field to get
 *          the pointer Widget pointer, now it uses a property through
 *          Win32 API's @c GetProp called @em "VacaAtom".
 *
 * @see getHWND
 */
Widget* Widget::fromHWND(HWND hwnd)
{
  // unbox the pointer
#ifdef USE_PROP
  // ScopedLock hold(vacaAtomMutex);
  return reinterpret_cast<Widget*>(::GetProp(hwnd, VACAATOM));
#else
  return reinterpret_cast<Widget*>(::GetWindowLongPtr(hwnd, GWL_USERDATA));
#endif
}

/**
 * Returns the global Vaca windows procedure that is used in the
 * WNDCLASS.
 *
 * @see globalWndProc, wndProc
 */
WNDPROC Widget::getGlobalWndProc()
{
  return Widget::globalWndProc;
}

// ===============================================================
// EVENTS
// ===============================================================

/**
 * It should calculates the preferred size for this widget.
 *
 * @param sz
 *     It's for input and output. You should put the preferred
 *     size in this value, but also you should read the input value to
 *     know if you must to fit the widget in some size. The possible
 *     values for @a sz are:
 *     - sz = Size(0, 0) to calculate the preferred size without restrictions.
 *     - sz = Size(width, 0) to calculate the preferred size with restricted &lt;= width.
 *     - sz = Size(0, height) to calculate the preferred size with restricted &lt;= height.
 */
void Widget::onPreferredSize(Size& sz)
{
  // there is a layout?
  if (m_layout != NULL) {
    // get the list of children
    Container children = getChildren();

    // calculate the preferred size through the layout manager
    sz = m_layout->getPreferredSize(this, children, sz);
  }
  // else do nothing...
}

/**
 * Called then the WM_PAINT event is received. The default
 * implementation calls the Graphics::noPaint to know that this widget
 * doesn't paint the surface. If you override this method, remember:
 * (1) to draw inside the Widget::getClientBounds limits, and (2)
 * don't call the base method onPaint().
 *
 * @warning Don't try to override the onPaint of a system control
 *          (like Button, Edit, etc.). You must to use the
 *          CustomButton, CustomEdit, etc. to do that.
 *
 * @code
 * class MyWidget : public Panel
 * {
 * public:
 *   ...
 *   virtual void onPaint(Graphics& g)
 *   {
 *     Rect rc = getClientBounds();
 *     g.drawEllipse(rc);
 *   }
 * };
 * @endcode
 *
 * @see onReflectedDrawItem
 */
void Widget::onPaint(Graphics& g)
{
  g.noPaint();
}

/**
 * Called when the user changes the size of the widget/frame (a
 * WM_SIZE message is received from Win32).
 */
void Widget::onResize(const Size& sz)
{
  Resize(sz);

  // Don't call layout() here (the Layout::mRelayoutWidgets collection is
  // useful to layout recursively). The first trigger of the layout()
  // method is from Frame::onResize().
}

/**
 * The mouse enters in the Widget.
 */
void Widget::onMouseEnter(MouseEvent& ev)
{
  MouseEnter(ev);
}

/**
 * The mouse leaves the Widget.
 */
void Widget::onMouseLeave()
{
  MouseLeave();
}

/**
 * The mouse is inside the Widget and the user press a mouse's button.
 */
void Widget::onMouseDown(MouseEvent& ev)
{
  MouseDown(ev);
}

/**
 * The mouse is inside the Widget and the user release a mouse's
 * button.
 */
void Widget::onMouseUp(MouseEvent& ev)
{
  MouseUp(ev);
}

/**
 * The user made double click over the widget. The default
 * implementation calls onMouseDown(), so it's like a single click
 * (converts double-clicks to single-clicks).
 *
 * @warning If you override this event, don't call the base implementation.
 *
 * @see onMouseDown
 */
void Widget::onDoubleClick(MouseEvent& ev)
{
  // if there aren't slots in DoubleClick...
  if (DoubleClick.empty()) {
    // ...by default onDoubleClick to onMouseDown (double-click to
    // single-click).
    onMouseDown(ev);
  }
  // if not fire DoubleClick signal
  else
    DoubleClick(ev);
}

/**
 * The mouse is moving inside the Widget's client area.
 * <p>
 * If you capture the mouse, you should use the System::getCursorPos
 * function to get the cursor position when it's outside the widget's
 * client area.
 */
void Widget::onMouseMove(MouseEvent& ev)
{
  MouseMove(ev);
}

/**
 * The mouse is inside the Widget and the user spin the mouse's
 * wheel. Event called when the WM_MOUSEWHEEL message is received.
 */
void Widget::onMouseWheel(MouseEvent& ev)
{
  MouseWheel(ev);
}

/**
 * Event generated when the user press ESC in a drag-and-drop operation
 * for example (equivalent to the WM_CANCELMODE message of Win32).
 */
void Widget::onCancelMode()
{
  CancelMode();
}

/**
 * Set the mouse's cursor depending of its position. If you override
 * this method, you shouldn't call Widget::onSetCursor if you don't
 * want the default behaviour.
 *
 * @param hitTest Where the mouse is.
 */
void Widget::onSetCursor(WidgetHitTest hitTest)
{
  // if we have a m_baseWndProc, use it
  if (m_baseWndProc != NULL) {
    CallWindowProc(m_baseWndProc, m_HWND, WM_SETCURSOR, m_wparam, m_lparam);
  }
  // if we aren't in the client area, maybe the defWndProc known more
  // about the cursor (like the cursors in Frame to resize the it)
  else if (hitTest != WidgetHitTest::Client) {
    defWndProc(WM_SETCURSOR, m_wparam, m_lparam);
  }
  // finally, we are in the client area, we can use the normal arrow
  else {
    setCursor(Cursor(SysCursor::Arrow));
  }
}

/**
 * The user presses a key. Event called when the WM_KEYUP message is
 * received.
 */
void Widget::onKeyUp(KeyEvent& ev)
{
  KeyUp(ev);
}

// The user releases a key. Event called when the WM_KEYDOWN message
// is received.
void Widget::onKeyDown(KeyEvent& ev)
{
  KeyDown(ev);
}

// The user releases a key. Event called when the WM_CHAR message is
// received.
void Widget::onKeyTyped(KeyEvent& ev)
{
  KeyTyped(ev);
}

/**
 * TODO docme
 */
void Widget::onGotFocus(Event& ev)
{
  GotFocus(ev);
}

/**
 * TODO docme
 */
void Widget::onLostFocus(Event& ev)
{
  LostFocus(ev);
}

/**
 * Called when an action/command by ID is activated by the user, this
 * can be a menu item or an accelerator.
 *
 * Win32 details: When a WM_COMMAND message is received by ID (like a
 * menu or accelerator command) this method is invoked.
 *
 * @param actionId
 *     Identifier of the action that was activated (by some
 *     method like ).
 *
 * @return
 *     It should returns true if the @a actionId was used.
 *
 * @warning Don't confuse with onReflectedCommand(): onActionById is used
 *          to handle command notifications that come directly from
 *          accelarators or menus, not from controls. Notifications
 *          by controls are controled by them via onReflectedCommand,
 *          onReflectedNotify, or onReflectedDrawItem.
 */
bool Widget::onActionById(int actionId)
{
  return false;
}

/**
 * TODO docme (WM_WINDOWPOSCHANGING)
 */
void Widget::onBeforePosChange()
{
}

/**
 * TODO docme (WM_WINDOWPOSCHANGED)
 */
void Widget::onAfterPosChange()
{
}

/**
 * TODO docme
 */
void Widget::onScroll(Orientation orient, int code)
{
}

/**
 * TODO docme
 */
void Widget::onDropFiles(DropFilesEvent& ev)
{
  DropFiles(ev);
}

/**
 * This method can be used to handle command notifications
 * (WM_COMMAND) reflected from the parent.
 *
 * @param id
 *     Identifier of the control or menu item.
 * 
 * @param code
 *     Notification code.
 * 
 * @param lResult
 *     Result to return by the wndProc() method.
 *
 * @warning Don't confuse with onActionById(): onReflectedCommand is used to handle
 *          commands that this widget by self generated, were sent to the
 *          parent, and finally were reflected to this widget again by
 *          the parent.
 */
bool Widget::onReflectedCommand(int id, int code, LRESULT& lResult)
{
  return false;
}

/**
 * This method can be used to handle notifications (WM_NOTIFY)
 * reflected from the parent.
 *
 * @param lpnmhdr
 *     Information about the notification message.
 * 
 * @param lResult
 *     Result to return by the wndProc() method.
 *
 * @return
 *     False if it doesn't use the notification.
 */
bool Widget::onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult)
{
  return false;
}

/**
 * Fired when the parent widget (m_parent) received the WM_DRAWITEM
 * message.
 *
 * @param g
 *     Graphics context to draw the item (you can obtain the HDC from
 *     this argument).
 * 
 * @param lpDrawItem
 *     Instance of the Win32 structure DRAWITEMSTRUCT to get information
 *     about the item to draw.
 *
 * @see onPaint
 */
bool Widget::onReflectedDrawItem(Graphics& g, LPDRAWITEMSTRUCT lpDrawItem)
{
  return false;
}

/**
 * Adds a child to this widget.
 *
 * @param child
 *     The child to add inside the widget.
 *
 * @param setParent
 *     If it's true the Win32 SetParent wil be used.
 */
void Widget::addChild(Widget* child, bool setParent)
{
  assert(::IsWindow(m_HWND));
  assert(child != NULL);
  assert(child->m_HWND != NULL);
  assert(child->m_parent == NULL);

  m_children.push_back(child);
  child->m_parent = this;

  if (setParent) {
    child->addStyle(Style(WS_CHILD, 0));
    ::SetParent(child->m_HWND, m_HWND);
    // sendMessage(WM_UPDATEUISTATE, UIS_SET..., 0);
  }
}

/**
 * Removes a child from this widget.
 *
 * @param child
 *     The child that we want to remove from the parent.
 *
 * @param setParent
 *     If it's true the Win32 SetParent will be used.
 */
void Widget::removeChild(Widget* child, bool setParent)
{
  assert(::IsWindow(m_HWND));
  assert(child != NULL);
  assert(child->m_HWND != NULL);
  assert(child->m_parent == this);

  remove_element_from_container(m_children, child);

  if (setParent) {
    ::SetParent(child->m_HWND, NULL);
    child->removeStyle(Style(WS_CHILD, 0));
  }

  child->m_parent = NULL;
}

/**
 * It creates the HWND (m_HWND) to be used in the Widget. The only way
 * to customize this routine is overloading the Widget::createHWND()
 * method, calling Widget::Widget() with NULL as @c className, and
 * finally calling Widget::create() from the constructor of your own
 * widget class (for example MdiClient::MdiClient()).
 *
 * This method MUST BE called in one constructor (in Widget::Widget() or
 * in your own class if you used a NULL className in Widget's
 * constructor).
 *
 * @see createHWND, @ref TN002
 */
void Widget::create(const WidgetClassName& className, Widget* parent, Style style)
{
  assert(m_HWND == NULL);
  assert(parent == NULL || parent->m_HWND != NULL);

  // all parents must to have the WS_EX_CONTROLPARENT style to avoid
  // infinite loops in Win32 dialog boxes (for more information see
  // the 'src/msw/window.cpp' file of 'wxWidgets' library)
  if ((parent != NULL) &&
      (parent->getStyle().extended & WS_EX_CONTROLPARENT) != 0)
    parent->addStyle(Style(0, WS_EX_CONTROLPARENT));

  // create the HWND handler
//   {
//     ScopedLock hold(outsideWidgetMutex);
//     assert(outsideWidget == NULL);

//     outsideWidget = this;
//     m_HWND = createHWND(className, parent, style);
//     outsideWidget = NULL;
//   }
  {
//     ThreadDataForWidget* data = getThreadData();
//     assert(data->outsideWidget == NULL);

//     data->outsideWidget = this;
//     m_HWND = createHWND(className, parent, style);
//     data->outsideWidget = NULL;
    Widget* outsideWidget = __vaca_get_outside_widget();
    assert(outsideWidget == NULL);
    
    __vaca_set_outside_widget(this);
    m_HWND = createHWND(className.toLPCTSTR(), parent, style);
    __vaca_set_outside_widget(NULL);
  }

  if (m_HWND == NULL)
    throw CreateWidgetException("Error creating widget of class \"" +
				String(className.toLPCTSTR()) + "\"");

  subClass();

  // add the widget to its parent
  if (parent != NULL)
    parent->addChild(this, false);
}

/**
 * @brief Does the classic Win32 subclassing replacing GWLP_WNDPROC.
 * 
 * In the GWLP_WNDPROC property of all HWND is the WNDPROC procedure
 * that is called each time a message is arrived/processed by the Win32
 * message-queue.
 *
 * @see getGlobalWndProc, wndProc, @ref TN002
 */
void Widget::subClass()
{
  assert(::IsWindow(m_HWND));

  // set the GWL_WNDPROC to globalWndProc
  m_baseWndProc = reinterpret_cast<WNDPROC>
    (SetWindowLongPtr(m_HWND,
		      GWLP_WNDPROC,
		      reinterpret_cast<LONG_PTR>(getGlobalWndProc())));
  if (m_baseWndProc == getGlobalWndProc())
    m_baseWndProc = NULL;

  // box the pointer
#ifdef USE_PROP
  {
    // ScopedLock hold(vacaAtomMutex);
    SetProp(m_HWND, VACAATOM, reinterpret_cast<HANDLE>(this));
  }
#else
  LONG_PTR oldData = SetWindowLongPtr(m_HWND, GWL_USERDATA, reinterpret_cast<LONG_PTR>(this));
  assert(oldData == 0);
#endif

  // TODO get the font from the hwnd

  // set the default font of the widget
  setFont(Font::getDefault());
}

/**
 * This method should create the HWND handler for the Widget. This
 * routine is called from Widget::create() method, so if you overloaded
 * this, you should use the Widget constructor with a NULL className,
 * and then use the Widget::create method in your own class's
 * constructor. E.g:
 * 
 * @code
 * MdiChild::MdiChild(const String& title,
 *                    MdiClient* parent,
 *                    Style style)
 *   : Frame(NULL, "", NULL, NoStyle) // <- className = NULL, "create" isn't called
 * {
 *   ...
 *   create("Vaca.MdiChild", parent, style); // we call "create" manually
 * }
 * @endcode
 *
 * @see create, @ref TN002
 */
HWND Widget::createHWND(LPCTSTR className, Widget* parent, Style style)
{
  return CreateWindowEx(style.extended, className, _T(""),
			style.regular,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			parent ? parent->getHWND(): (HWND)NULL,
			(HMENU)NULL,
			Application::getHINSTANCE(),
			reinterpret_cast<LPVOID>(this));
}

/**
 * The customized window procedure for this particular widget.  This
 * is called from Widget::globalWndProc().  Should return true when
 * the defWndProc() doesn't need to be called.
 *
 * This method is called to intercept any message after the creation
 * of the widget, and before the destruction. To intercept messages
 * outside that range (before creation, and after disposition), you
 * should customize defWndProc().
 *
 * This method mainly converts a message to a event:
 * <ul>
 *   <li><tt>WM_PAINT</tt> -&gt; onPaint()</li>
 *   <li><tt>WM_SIZE</tt> -&gt; onResize()</li>
 *   <li><tt>WM_SETCURSOR</tt> -&gt; onSetCursor()</li>
 *   <li><tt>WM_*BUTTONDOWN</tt> -&gt; onMouseDown()</li>
 *   <li><tt>WM_*BUTOTNUP</tt> -&gt; onMouseUp()</li>
 *   <li><tt>WM_*BUTOTNDBLCLK</tt> -&gt; onDoubleClick()</li>
 *   <li><tt>WM_MOUSEMOVE</tt> -&gt; onMouseMove()</li>
 *   <li><tt>WM_MOUSEWHEEL</tt> -&gt; onMouseWheel()</li>
 *   <li><tt>WM_MOUSELEAVE</tt> -&gt; onMouseLeave()</li>
 *   <li><tt>WM_CANCELMODE</tt> -&gt; onCancelMode()</li>
 *   <li><tt>WM_KEYDOWN</tt> -&gt; onKeyDown()</li>
 *   <li><tt>WM_KEYUP</tt> -&gt; onKeyUp()</li>
 *   <li><tt>WM_CHAR</tt> -&gt; onKeyChar()</li>
 *   <li><tt>WM_SETFOCUS</tt> -&gt; onGotFocus()</li>
 *   <li><tt>WM_KILLFOCUS</tt> -&gt; onLostFocus()</li>
 * </ul>
 *
 * For reflection, it does:
 * <ul>
 * <li>When <tt>WM_COMMAND</tt> is received, the onReflectedCommand() event
 *     <b>of the child</b> is called when it's a WM_COMMAND from a control, or the
 *     onActionById() event <b>of this widget</b> is called when the command
 *     come from a menu or an accelerator.</li>
 * <li>When <tt>WM_NOTIFY</tt> is received, the onReflectedNotify() event <b>of the
 *     child</b> is called.</li>
 * <li>When <tt>WM_DRAWITEM</tt> is received, the onReflectedDrawItem() event <b>of the
 *     child</b> is called.</li>
 * </ul>
 *
 * How to extend widget::wndProc method?
 *
 * @code
 * class MyWidget : public Widget {
 *   ...
 * protected:
 *   virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
 *   {
 *     if (Widget::wndProc(message, wParam, lParam, lResult))
 *       return true;
 *
 *     // ...Your code here...
 *
 *     return false;
 *   }
 *   ...
 * }
 * @endcode
 *
 * @see globalWndProc, getGlobalWndProc, defWndProc
 */
bool Widget::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
  bool ret = false;

  switch (message) {

    case WM_ERASEBKGND:
      // see TN011
      if (m_baseWndProc == NULL) {
	HDC hdc = reinterpret_cast<HDC>(wParam);
	// erase background only when the widget doesn't use
	// double-buffering
	if (!m_doubleBuffered) {
	  Graphics g(hdc);
	  Brush brush(getBgColor());

	  g.fillRect(brush, g.getClipBounds());
	}
	lResult = TRUE;
	ret = true;
      }
      // a Custom... widget and double-buffering is activated? ok, we
      // shouldn't send WM_ERASEBKGND to the original WNDPROC
      else if (m_doubleBuffered) {
	lResult = TRUE;
	ret = true;
      }
      break;

    case WM_PAINT:
      // see TN011
      if (m_baseWndProc == NULL) {
 	PAINTSTRUCT ps;
	bool painted = false;
	HDC hdc = ::BeginPaint(m_HWND, &ps);

	if (!::IsRectEmpty(&ps.rcPaint)) {
	  Graphics g(hdc);
	  painted = doPaint(g);
	}
	
	::EndPaint(m_HWND, &ps);

	if (painted) {
	  lResult = TRUE;
	  ret = true;
	}
      }
      break;

    case WM_DRAWITEM: {
      LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
      Widget* child = Widget::fromHWND(lpDrawItem->hwndItem);
      HDC hdc = lpDrawItem->hDC;
      bool painted = false;

      if (!::IsRectEmpty(&lpDrawItem->rcItem)) {
	Graphics g(hdc);
	painted = child->onReflectedDrawItem(g, lpDrawItem);
      }
      
      if (painted) {
	lResult = TRUE;
	ret = true;
      }
      break;
    }

    case WM_SIZE: {
      Size sz(LOWORD(lParam), HIWORD(lParam));
      onResize(sz);
      break;
    }

    case WM_SETCURSOR:
      if (hasMouseAbove()) {
	WidgetHitTest hitTest = WidgetHitTest::Error;
	switch (LOWORD(lParam)) {
	  case HTERROR: hitTest = WidgetHitTest::Error; break;
	  case HTTRANSPARENT: hitTest = WidgetHitTest::Transparent; break;
	  case HTNOWHERE: hitTest = WidgetHitTest::Nowhere; break;
	  case HTCLIENT: hitTest = WidgetHitTest::Client; break;
	  case HTCAPTION: hitTest = WidgetHitTest::Caption; break;
	  case HTSYSMENU: hitTest = WidgetHitTest::SystemMenu; break;
	    // case HTGROWBOX: hitTest = WidgetHitTest::Size; break;
	  case HTSIZE: hitTest = WidgetHitTest::Size; break;
	  case HTMENU: hitTest = WidgetHitTest::Menu; break;
	  case HTHSCROLL: hitTest = WidgetHitTest::HorizontalScroll; break;
	  case HTVSCROLL: hitTest = WidgetHitTest::VerticalScroll; break;
	  case HTMINBUTTON: hitTest = WidgetHitTest::MinimizeButton; break;
	  case HTMAXBUTTON: hitTest = WidgetHitTest::MaximizeButton; break;
	    //case HTREDUCE: hitTest = WidgetHitTest::MinimizeButton; break;
	    //case HTZOOM: hitTest = WidgetHitTest::MaximizeButton; break;
	  case HTLEFT: hitTest = WidgetHitTest::Left; break;
	  case HTRIGHT: hitTest = WidgetHitTest::Right; break;
	  case HTTOP: hitTest = WidgetHitTest::Top; break;
	  case HTTOPLEFT: hitTest = WidgetHitTest::TopLeft; break;
	  case HTTOPRIGHT: hitTest = WidgetHitTest::TopRight; break;
	  case HTBOTTOM: hitTest = WidgetHitTest::Bottom; break;
	  case HTBOTTOMLEFT: hitTest = WidgetHitTest::BottomLeft; break;
	  case HTBOTTOMRIGHT: hitTest = WidgetHitTest::BottomRight; break;
	  case HTBORDER: hitTest = WidgetHitTest::Border; break;
	  case HTOBJECT: hitTest = WidgetHitTest::Object; break;
	  case HTCLOSE: hitTest = WidgetHitTest::Close; break;
	  case HTHELP: hitTest = WidgetHitTest::Help; break;
	}
	onSetCursor(hitTest);
	lResult = TRUE;
	ret = true;
      }
      break;

//     case WM_WINDOWPOSCHANGING:
//       {
// 	LPWINDOWPOS lpwp = reinterpret_cast<LPWINDOWPOS>(lParam);
// 	Size sz = getBounds().getSize();
// 	if (lpwp->cx != sz.w ||
// 	    lpwp->cy != sz.h)
// 	  resize(sz);
//       }
//       break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN: {
      MouseEvent
	ev(this,				           // widget
	   Point(&MAKEPOINTS(lParam)),			   // pt
	   1,						   // clicks
	   wParam,					   // flags
	   message == WM_LBUTTONDOWN ? MouseButtons::Left: // button
	   message == WM_RBUTTONDOWN ? MouseButtons::Right:
	   message == WM_MBUTTONDOWN ? MouseButtons::Middle:
				       MouseButtons::None);

      onMouseDown(ev);
      break;
    }

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP: {
      MouseEvent
	ev(this,				         // widget
	   Point(&MAKEPOINTS(lParam)),			 // pt
	   1,					         // clicks
	   wParam,					 // flags
	   message == WM_LBUTTONUP ? MouseButtons::Left: // button
	   message == WM_RBUTTONUP ? MouseButtons::Right:
	   message == WM_MBUTTONUP ? MouseButtons::Middle:
				     MouseButtons::None);

      onMouseUp(ev);
      break;
    }

    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK: {
      MouseEvent
	ev(this,				             // widget
	   Point(&MAKEPOINTS(lParam)),			     // pt
	   2,					             // clicks
	   wParam,				             // flags
	   message == WM_LBUTTONDBLCLK ? MouseButtons::Left: // button
	   message == WM_RBUTTONDBLCLK ? MouseButtons::Right:
	   message == WM_MBUTTONDBLCLK ? MouseButtons::Middle:
					 MouseButtons::None);
      onDoubleClick(ev);
      break;
    }

    case WM_MOUSEMOVE: {
      MouseEvent
	ev(this,				    // widget
	   Point(&MAKEPOINTS(lParam)),		    // pt
	   0,					    // clicks
	   wParam,				    // flags
	   MouseButtons::None);			    // button

      if (!m_hasMouse) {
	onMouseEnter(ev);
	m_hasMouse = true;

	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;// | TME_HOVER
	tme.hwndTrack = m_HWND;
	_TrackMouseEvent(&tme);
      }

      onMouseMove(ev);
      break;
    }

// #ifdef WM_MOUSEWHEEL
    case WM_MOUSEWHEEL: {
      // the lParam specifies the coordinates of the cursor relative
      // to the screen (not to client area)
      Point clientOrigin = getAbsoluteClientBounds().getOrigin();
      MouseEvent
	ev(this,				   // widget
	   Point(&MAKEPOINTS(lParam)) - clientOrigin, // pt
	   0,					   // clicks
	   LOWORD(wParam),			   // flags
	   MouseButtons::None,			   // button
	   ((short)HIWORD(wParam)) / WHEEL_DELTA); // delta

      onMouseWheel(ev);
      break;
    }
// #endif

//       // TODO think about this
//     case WM_MOUSEHOVER: {
//       MouseEvent ev(this, Point(&MAKEPOINTS(lParam)), 0,
// 		    MouseButtons::None, wParam);
//       onMouseHover(ev);
//     }

    case WM_MOUSELEAVE: {
      // TODO ask for m_hasMouse?
//       if (m_hasMouse) {
	m_hasMouse = false;
	onMouseLeave();
//       }
	break;
    }

    case WM_CANCELMODE:
      onCancelMode();
      break;
      
    case WM_KEYDOWN: {
      KeyEvent ev(this, Keys::fromMessageParams(wParam, lParam));
      onKeyDown(ev);
      if (ev.isConsumed()) {
	lResult = false;
	ret = true;
      }
      break;
    }

    case WM_KEYUP: {
      KeyEvent ev(this, Keys::fromMessageParams(wParam, lParam));
      onKeyUp(ev);
      if (ev.isConsumed()) {
	lResult = false;
	ret = true;
      }
      break;
    }

    case WM_CHAR: {
      KeyEvent ev(this, Keys::None, wParam); // TODO complete Keys
      // KeyEvent ev(this, Keys::fromMessageParams(wParam, lParam));
      onKeyTyped(ev);
      if (ev.isConsumed()) {
	lResult = false;
	ret = true;
      }
      break;
    }

    case WM_COMMAND:
      if (reinterpret_cast<HWND>(lParam) != NULL) {
	HWND hwndCtrl = reinterpret_cast<HWND>(lParam);
	Widget* child = Widget::fromHWND(hwndCtrl);
	if (child != NULL) {
	  Container refContainer;
	  ref_widget(child, refContainer);

	  ret = child->onReflectedCommand(LOWORD(wParam), // id
					  HIWORD(wParam), // code
					  lResult);

	  unref_widget(refContainer);
	}
      }
      // accelerator or a menu
      else if (HIWORD(wParam) == 1 ||
	       HIWORD(wParam) == 0) {
	if (onActionById(LOWORD(wParam))) {
	  // ...onActionById returns true when processed the command
	  lResult = 0;		// processed
	  ret = true;
	}
      }
      break;

    case WM_NOTIFY: {
      LPNMHDR lpnmhdr = reinterpret_cast<LPNMHDR>(lParam);
      Widget* child = Widget::fromHWND(lpnmhdr->hwndFrom);
      if (child != NULL) {
	Container refContainer;
	ref_widget(child, refContainer);

	ret = child->onReflectedNotify(lpnmhdr, lResult);

	unref_widget(refContainer);
      }
      break;
    }

    case WM_SYSCOMMAND: // TODO
      // Beep(400, 100);
      break;

      /*
    case WM_MENUSELECT: {
      int index = LOWORD(wParam);
      int flags = HIWORD(wParam);
      HMENU hmenu = reinterpret_cast<HMENU>(lParam);

      if (flags & MF_MOUSESELECT) {
	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA;

	if (GetMenuItemInfo(hmenu, index, TRUE, &mii)) {
	  MenuItem* menuitem = reinterpret_cast<MenuItem *>(mii.dwItemData);
	  if (menuitem != NULL) {
	    ItemEvent(menuitem->onSelected, this, menuitem).fire();
	  }
	}
      }
    }
      */

    case WM_SETFOCUS: {
      Event ev(this);
      onGotFocus(ev);
      break;
    }

    case WM_KILLFOCUS: {
      Event ev(this);
      onLostFocus(ev);
      break;
    }

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC: {
      HDC hdc = reinterpret_cast<HDC>(wParam);
      HWND hwndCtrl = reinterpret_cast<HWND>(lParam);
      Widget* child = Widget::fromHWND(hwndCtrl);
      if (child != NULL) {
	COLORREF fgColor = child->getFgColor().getColorRef();
	COLORREF bgColor = child->getBgColor().getColorRef();

	SetTextColor(hdc, fgColor);
	SetBkColor(hdc, bgColor);

	HBRUSH hBrush = Graphics::findHBRUSH(BS_SOLID, 0, bgColor);
	lResult = reinterpret_cast<LRESULT>(hBrush);
	ret = true;
      }
      break;
    }

    case WM_WINDOWPOSCHANGING:
      onBeforePosChange();
      break;

    case WM_WINDOWPOSCHANGED:
      onAfterPosChange();
      break;

    case WM_VSCROLL:
    case WM_HSCROLL:
      {
	Orientation orient = (message == WM_VSCROLL) ? Orientation::Vertical:
						       Orientation::Horizontal;
	  
	// Note: onScroll() doesn't receive the nPos=HIWORD(wParam)
	// because it has a limit of 16-bits.  Instead you should use
	// GetScrollInfo to get the position of the scroll-bar, it has
	// the full 32 bits
	
	// reflect the message? (it's useful for the "Slider" widget)
	if (lParam != 0) {
	  Widget* child = Widget::fromHWND(reinterpret_cast<HWND>(lParam));
	  if (child != NULL) {
	    Container refContainer;
	    ref_widget(child, refContainer);

	    child->onScroll(orient, LOWORD(wParam));

	    unref_widget(refContainer);
	  }
	}

	onScroll(orient, LOWORD(wParam));
      }
      break;

    case WM_DROPFILES: {
      // TODO
      // DragQueryPoint(hDrop);
      HDROP hDrop = reinterpret_cast<HDROP>(wParam);
      std::vector<String> files;
      int index, count, length;

      count = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
      for (index=0; index<count; ++index) {
	length = DragQueryFile(hDrop, index, NULL, 0);
	if (length > 0) {
	  LPTSTR lpstr = new TCHAR[length+1];
	  DragQueryFile(hDrop, index, lpstr, length+1);
	  files.push_back(lpstr);
	  delete[] lpstr;
	}
      }

      DragFinish(hDrop);

      // generate the event
      DropFilesEvent ev(this, files);
      onDropFiles(ev);
      break;
    }

  }

  return ret;
}

/**
 * Calls the default window procedure (m_defWndProc that points to
 * Win32 DefWindowProc by default). If m_baseWndProc isn't NULL, it's
 * called instead.
 */
LRESULT Widget::defWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  if (m_baseWndProc != NULL)
    return CallWindowProc(m_baseWndProc, m_HWND, message, wParam, lParam);
  else
    return m_defWndProc(m_HWND, message, wParam, lParam);
}

/**
 * TODO docme
 */
bool Widget::doPaint(Graphics& g)
{
  bool painted = false;

  // use double-buffering technique?
  if (m_doubleBuffered) {
    // get the clip bounds
    Rect clipBounds = g.getClipBounds();
    // isn't it empty?
    if (!clipBounds.isEmpty()) {
      // create the image for double-buffering (of the size of
      // the clipping bounds)
      Image image(clipBounds.getSize(), g);
      // get the Graphics to draw in the image
      Graphics* imageG = image.getGraphics();
      // background brush
      Brush bgBrush(getBgColor());

      // setup clipping region
      Region clipRegion;
      g.getClipRegion(clipRegion);
      clipRegion.offset(-clipBounds.x, -clipBounds.y);
      imageG->setClipRegion(clipRegion);

      // special coordinates transformation (to make
      // transparent the "imageG" to onPaint)
      SetViewportOrgEx(imageG->getHDC(), -clipBounds.x, -clipBounds.y, NULL);
      
      // clear the background of the image
      imageG->fillRect(bgBrush, clipBounds);

      // configure defaults
      imageG->setFont(getFont());
      imageG->setColor(getFgColor()); // TODO remove this

      // paint on imageG
      onPaint(*imageG);
      painted = imageG->wasPainted();

      // restore the viewport origin (so drawImage works fine)
      SetViewportOrgEx(imageG->getHDC(), 0, 0, NULL);

      // bit transfer from image to graphics device
      g.drawImage(image, clipBounds.getOrigin());
    }
  }
  // draw directly to the screen
  else {
    // configure defaults
    g.setFont(getFont());
    g.setColor(getFgColor());

    // paint on g
    onPaint(g);
    painted = g.wasPainted();
  }

  return painted;
}

/**
 * TODO docme
 */
void Widget::setDefWndProc(WNDPROC proc)
{
  m_defWndProc = proc;
}

/**
 * TODO docme
 */
void Widget::setDestroyHWNDProc(void (*proc)(HWND))
{
  m_destroyHWNDProc = proc;
}

/**
 * This routine is called before to dispatch the message.
 *
 * In Win32, it's used by Dialog to call IsDialogMessage for example.
 *
 * @return
 *    true if the message was translated and sent, so the GUI
 *    thread doesn't need to dispatch it.
 */
bool Widget::preTranslateMessage(MSG& msg)
{
  if (m_parent != NULL)
    return m_parent->preTranslateMessage(msg);
  else
    return false;
}

/**
 * Sends a message to the HWND, like Win32 SendMessage.
 */
LRESULT Widget::sendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
  assert(::IsWindow(m_HWND));
  return ::SendMessage(m_HWND, message, wParam, lParam);
}

/**
 * The global procedure for Win32 used in all registered WNDCLASSes.
 * It's unique goal is to get the Widget pointer from HWND userdata
 * using Widget::fromHWND, and to call its wndProc method.
 */
LRESULT CALLBACK Widget::globalWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  Widget* widget = Widget::fromHWND(hwnd);

#ifdef REPORT_MESSAGES
  String preString =
    "Message "+String::fromInt(message)+" for " +
    String::fromInt(reinterpret_cast<int>(widget != NULL ? widget:
							   // getThreadData()->outsideWidget
							   __vaca_get_outside_widget()
					  ), 16, 8);
  
  String msgString = "Unknown";
  switch (message) {
    case WM_ACTIVATE: msgString = "WM_ACTIVATE"; break;
    case WM_ACTIVATEAPP: msgString = "WM_ACTIVATEAPP"; break;
    case WM_APP: msgString = "WM_APP"; break;
    case WM_ASKCBFORMATNAME: msgString = "WM_ASKCBFORMATNAME"; break;
    case WM_CANCELJOURNAL: msgString = "WM_CANCELJOURNAL"; break;
    case WM_CANCELMODE: msgString = "WM_CANCELMODE"; break;
    case WM_CAPTURECHANGED: msgString = "WM_CAPTURECHANGED"; break;
    case WM_CHANGECBCHAIN: msgString = "WM_CHANGECBCHAIN"; break;
    case WM_CHAR: msgString = "WM_CHAR"; break;
    case WM_CHARTOITEM: msgString = "WM_CHARTOITEM"; break;
    case WM_CHILDACTIVATE: msgString = "WM_CHILDACTIVATE"; break;
    case WM_CLEAR: msgString = "WM_CLEAR"; break;
    case WM_CLOSE: msgString = "WM_CLOSE"; break;
    case WM_COMMAND: msgString = "WM_COMMAND"; break;
    case WM_COMMNOTIFY: msgString = "WM_COMMNOTIFY"; break;
    case WM_COMPACTING: msgString = "WM_COMPACTING"; break;
    case WM_COMPAREITEM: msgString = "WM_COMPAREITEM"; break;
    case WM_CONTEXTMENU: msgString = "WM_CONTEXTMENU"; break;
    case WM_COPY: msgString = "WM_COPY"; break;
    case WM_COPYDATA: msgString = "WM_COPYDATA"; break;
    case WM_CREATE: msgString = "WM_CREATE"; break;
    case WM_CTLCOLORBTN: msgString = "WM_CTLCOLORBTN"; break;
    case WM_CTLCOLORDLG: msgString = "WM_CTLCOLORDLG"; break;
    case WM_CTLCOLOREDIT: msgString = "WM_CTLCOLOREDIT"; break;
    case WM_CTLCOLORLISTBOX: msgString = "WM_CTLCOLORLISTBOX"; break;
    case WM_CTLCOLORMSGBOX: msgString = "WM_CTLCOLORMSGBOX"; break;
    case WM_CTLCOLORSCROLLBAR: msgString = "WM_CTLCOLORSCROLLBAR"; break;
    case WM_CTLCOLORSTATIC: msgString = "WM_CTLCOLORSTATIC"; break;
    case WM_CUT: msgString = "WM_CUT"; break;
    case WM_DEADCHAR: msgString = "WM_DEADCHAR"; break;
    case WM_DELETEITEM: msgString = "WM_DELETEITEM"; break;
    case WM_DESTROY: msgString = "WM_DESTROY"; break;
    case WM_DESTROYCLIPBOARD: msgString = "WM_DESTROYCLIPBOARD"; break;
    case WM_DEVICECHANGE: msgString = "WM_DEVICECHANGE"; break;
    case WM_DEVMODECHANGE: msgString = "WM_DEVMODECHANGE"; break;
    case WM_DISPLAYCHANGE: msgString = "WM_DISPLAYCHANGE"; break;
    case WM_DRAWCLIPBOARD: msgString = "WM_DRAWCLIPBOARD"; break;
    case WM_DRAWITEM: msgString = "WM_DRAWITEM"; break;
    case WM_DROPFILES: msgString = "WM_DROPFILES"; break;
    case WM_ENABLE: msgString = "WM_ENABLE"; break;
    case WM_ENDSESSION: msgString = "WM_ENDSESSION"; break;
    case WM_ENTERIDLE: msgString = "WM_ENTERIDLE"; break;
    case WM_ENTERMENULOOP: msgString = "WM_ENTERMENULOOP"; break;
    case WM_ENTERSIZEMOVE: msgString = "WM_ENTERSIZEMOVE"; break;
    case WM_ERASEBKGND: msgString = "WM_ERASEBKGND"; break;
    case WM_EXITMENULOOP: msgString = "WM_EXITMENULOOP"; break;
    case WM_EXITSIZEMOVE: msgString = "WM_EXITSIZEMOVE"; break;
    case WM_FONTCHANGE: msgString = "WM_FONTCHANGE"; break;
    case WM_GETDLGCODE: msgString = "WM_GETDLGCODE"; break;
    case WM_GETFONT: msgString = "WM_GETFONT"; break;
    case WM_GETHOTKEY: msgString = "WM_GETHOTKEY"; break;
    case WM_GETICON: msgString = "WM_GETICON"; break;
    case WM_GETMINMAXINFO: msgString = "WM_GETMINMAXINFO"; break;
    case WM_GETTEXT: msgString = "WM_GETTEXT"; break;
    case WM_GETTEXTLENGTH: msgString = "WM_GETTEXTLENGTH"; break;
    case WM_HELP: msgString = "WM_HELP"; break;
    case WM_HOTKEY: msgString = "WM_HOTKEY"; break;
    case WM_HSCROLL: msgString = "WM_HSCROLL"; break;
    case WM_HSCROLLCLIPBOARD: msgString = "WM_HSCROLLCLIPBOARD"; break;
    case WM_ICONERASEBKGND: msgString = "WM_ICONERASEBKGND"; break;
    case WM_INITDIALOG: msgString = "WM_INITDIALOG"; break;
    case WM_INITMENU: msgString = "WM_INITMENU"; break;
    case WM_INITMENUPOPUP: msgString = "WM_INITMENUPOPUP"; break;
    case WM_INPUTLANGCHANGE: msgString = "WM_INPUTLANGCHANGE"; break;
    case WM_INPUTLANGCHANGEREQUEST: msgString = "WM_INPUTLANGCHANGEREQUEST"; break;
    case WM_KEYDOWN: msgString = "WM_KEYDOWN"; break;
    case WM_KEYUP: msgString = "WM_KEYUP"; break;
    case WM_KILLFOCUS: msgString = "WM_KILLFOCUS"; break;
    case WM_LBUTTONDBLCLK: msgString = "WM_LBUTTONDBLCLK"; break;
    case WM_LBUTTONDOWN: msgString = "WM_LBUTTONDOWN"; break;
    case WM_LBUTTONUP: msgString = "WM_LBUTTONUP"; break;
    case WM_MBUTTONDBLCLK: msgString = "WM_MBUTTONDBLCLK"; break;
    case WM_MBUTTONDOWN: msgString = "WM_MBUTTONDOWN"; break;
    case WM_MBUTTONUP: msgString = "WM_MBUTTONUP"; break;
    case WM_MDIACTIVATE: msgString = "WM_MDIACTIVATE"; break;
    case WM_MDICASCADE: msgString = "WM_MDICASCADE"; break;
    case WM_MDICREATE: msgString = "WM_MDICREATE"; break;
    case WM_MDIDESTROY: msgString = "WM_MDIDESTROY"; break;
    case WM_MDIGETACTIVE: msgString = "WM_MDIGETACTIVE"; break;
    case WM_MDIICONARRANGE: msgString = "WM_MDIICONARRANGE"; break;
    case WM_MDIMAXIMIZE: msgString = "WM_MDIMAXIMIZE"; break;
    case WM_MDINEXT: msgString = "WM_MDINEXT"; break;
    case WM_MDIREFRESHMENU: msgString = "WM_MDIREFRESHMENU"; break;
    case WM_MDIRESTORE: msgString = "WM_MDIRESTORE"; break;
    case WM_MDISETMENU: msgString = "WM_MDISETMENU"; break;
    case WM_MDITILE: msgString = "WM_MDITILE"; break;
    case WM_MEASUREITEM: msgString = "WM_MEASUREITEM"; break;
    case WM_MENUCHAR: msgString = "WM_MENUCHAR"; break;
    case WM_MENUCOMMAND: msgString = "WM_MENUCOMMAND"; break;
    case WM_MENUDRAG: msgString = "WM_MENUDRAG"; break;
    case WM_MENUGETOBJECT: msgString = "WM_MENUGETOBJECT"; break;
    case WM_MENURBUTTONUP: msgString = "WM_MENURBUTTONUP"; break;
    case WM_MENUSELECT: msgString = "WM_MENUSELECT"; break;
    case WM_MOUSEACTIVATE: msgString = "WM_MOUSEACTIVATE"; break;
    case WM_MOUSEHOVER: msgString = "WM_MOUSEHOVER"; break;
    case WM_MOUSELEAVE: msgString = "WM_MOUSELEAVE"; break;
    case WM_MOUSEMOVE: msgString = "WM_MOUSEMOVE"; break;
    case WM_MOUSEWHEEL: msgString = "WM_MOUSEWHEEL"; break;
    case WM_MOVE: msgString = "WM_MOVE"; break;
    case WM_MOVING: msgString = "WM_MOVING"; break;
    case WM_NCACTIVATE: msgString = "WM_NCACTIVATE"; break;
    case WM_NCCALCSIZE: msgString = "WM_NCCALCSIZE"; break;
    case WM_NCCREATE: msgString = "WM_NCCREATE"; break;
    case WM_NCDESTROY: msgString = "WM_NCDESTROY"; break;
    case WM_NCHITTEST: msgString = "WM_NCHITTEST"; break;
    case WM_NCLBUTTONDBLCLK: msgString = "WM_NCLBUTTONDBLCLK"; break;
    case WM_NCLBUTTONDOWN: msgString = "WM_NCLBUTTONDOWN"; break;
    case WM_NCLBUTTONUP: msgString = "WM_NCLBUTTONUP"; break;
    case WM_NCMBUTTONDBLCLK: msgString = "WM_NCMBUTTONDBLCLK"; break;
    case WM_NCMBUTTONDOWN: msgString = "WM_NCMBUTTONDOWN"; break;
    case WM_NCMBUTTONUP: msgString = "WM_NCMBUTTONUP"; break;
    case WM_NCMOUSEHOVER: msgString = "WM_NCMOUSEHOVER"; break;
    case WM_NCMOUSELEAVE: msgString = "WM_NCMOUSELEAVE"; break;
    case WM_NCMOUSEMOVE: msgString = "WM_NCMOUSEMOVE"; break;
    case WM_NCPAINT: msgString = "WM_NCPAINT"; break;
    case WM_NCRBUTTONDBLCLK: msgString = "WM_NCRBUTTONDBLCLK"; break;
    case WM_NCRBUTTONDOWN: msgString = "WM_NCRBUTTONDOWN"; break;
    case WM_NCRBUTTONUP: msgString = "WM_NCRBUTTONUP"; break;
    case WM_NCXBUTTONDBLCLK: msgString = "WM_NCXBUTTONDBLCLK"; break;
    case WM_NCXBUTTONDOWN: msgString = "WM_NCXBUTTONDOWN"; break;
    case WM_NCXBUTTONUP: msgString = "WM_NCXBUTTONUP"; break;
    case WM_NEXTDLGCTL: msgString = "WM_NEXTDLGCTL"; break;
    case WM_NEXTMENU: msgString = "WM_NEXTMENU"; break;
    case WM_NOTIFY: msgString = "WM_NOTIFY"; break;
    case WM_NOTIFYFORMAT: msgString = "WM_NOTIFYFORMAT"; break;
    case WM_NULL: msgString = "WM_NULL"; break;
    case WM_PAINT: msgString = "WM_PAINT"; break;
    case WM_PAINTCLIPBOARD: msgString = "WM_PAINTCLIPBOARD"; break;
    case WM_PAINTICON: msgString = "WM_PAINTICON"; break;
    case WM_PALETTECHANGED: msgString = "WM_PALETTECHANGED"; break;
    case WM_PALETTEISCHANGING: msgString = "WM_PALETTEISCHANGING"; break;
    case WM_PARENTNOTIFY: msgString = "WM_PARENTNOTIFY"; break;
    case WM_PASTE: msgString = "WM_PASTE"; break;
    case WM_POWER: msgString = "WM_POWER"; break;
    case WM_POWERBROADCAST: msgString = "WM_POWERBROADCAST"; break;
    case WM_PRINT: msgString = "WM_PRINT"; break;
    case WM_PRINTCLIENT: msgString = "WM_PRINTCLIENT"; break;
    case WM_QUERYDRAGICON: msgString = "WM_QUERYDRAGICON"; break;
    case WM_QUERYENDSESSION: msgString = "WM_QUERYENDSESSION"; break;
    case WM_QUERYNEWPALETTE: msgString = "WM_QUERYNEWPALETTE"; break;
    case WM_QUERYOPEN: msgString = "WM_QUERYOPEN"; break;
    case WM_QUEUESYNC: msgString = "WM_QUEUESYNC"; break;
    case WM_QUIT: msgString = "WM_QUIT"; break;
    case WM_RBUTTONDBLCLK: msgString = "WM_RBUTTONDBLCLK"; break;
    case WM_RBUTTONDOWN: msgString = "WM_RBUTTONDOWN"; break;
    case WM_RBUTTONUP: msgString = "WM_RBUTTONUP"; break;
    case WM_RENDERALLFORMATS: msgString = "WM_RENDERALLFORMATS"; break;
    case WM_RENDERFORMAT: msgString = "WM_RENDERFORMAT"; break;
    case WM_SETCURSOR: msgString = "WM_SETCURSOR"; break;
    case WM_SETFOCUS: msgString = "WM_SETFOCUS"; break;
    case WM_SETFONT: msgString = "WM_SETFONT"; break;
    case WM_SETHOTKEY: msgString = "WM_SETHOTKEY"; break;
    case WM_SETICON: msgString = "WM_SETICON"; break;
    case WM_SETREDRAW: msgString = "WM_SETREDRAW"; break;
    case WM_SETTEXT: msgString = "WM_SETTEXT"; break;
    case WM_SETTINGCHANGE: msgString = "WM_SETTINGCHANGE"; break;
    case WM_SHOWWINDOW: msgString = "WM_SHOWWINDOW"; break;
    case WM_SIZE: msgString = "WM_SIZE"; break;
    case WM_SIZECLIPBOARD: msgString = "WM_SIZECLIPBOARD"; break;
    case WM_SIZING: msgString = "WM_SIZING"; break;
    case WM_SPOOLERSTATUS: msgString = "WM_SPOOLERSTATUS"; break;
    case WM_STYLECHANGED: msgString = "WM_STYLECHANGED"; break;
    case WM_STYLECHANGING: msgString = "WM_STYLECHANGING"; break;
    case WM_SYNCPAINT: msgString = "WM_SYNCPAINT"; break;
    case WM_SYSCHAR: msgString = "WM_SYSCHAR"; break;
    case WM_SYSCOLORCHANGE: msgString = "WM_SYSCOLORCHANGE"; break;
    case WM_SYSCOMMAND: msgString = "WM_SYSCOMMAND"; break;
    case WM_SYSDEADCHAR: msgString = "WM_SYSDEADCHAR"; break;
    case WM_SYSKEYDOWN: msgString = "WM_SYSKEYDOWN"; break;
    case WM_SYSKEYUP: msgString = "WM_SYSKEYUP"; break;
    case WM_TCARD: msgString = "WM_TCARD"; break;
      // case WM_THEMECHANGED: msgString = "WM_THEMECHANGED"; break;
    case WM_TIMECHANGE: msgString = "WM_TIMECHANGE"; break;
    case WM_TIMER: msgString = "WM_TIMER"; break;
    case WM_UNDO: msgString = "WM_UNDO"; break;
    case WM_UNINITMENUPOPUP: msgString = "WM_UNINITMENUPOPUP"; break;
    case WM_USER: msgString = "WM_USER"; break;
    case WM_USERCHANGED: msgString = "WM_USERCHANGED"; break;
    case WM_VKEYTOITEM: msgString = "WM_VKEYTOITEM"; break;
    case WM_VSCROLL: msgString = "WM_VSCROLL"; break;
    case WM_VSCROLLCLIPBOARD: msgString = "WM_VSCROLLCLIPBOARD"; break;
    case WM_WINDOWPOSCHANGED: msgString = "WM_WINDOWPOSCHANGED"; break;
    case WM_WINDOWPOSCHANGING: msgString = "WM_WINDOWPOSCHANGING"; break;
    case WM_XBUTTONDBLCLK: msgString = "WM_XBUTTONDBLCLK"; break;
    case WM_XBUTTONDOWN: msgString = "WM_XBUTTONDOWN"; break;
    case WM_XBUTTONUP: msgString = "WM_XBUTTONUP"; break;
  }
  VACA_TRACE((preString + " ("+ msgString + ")\n").c_str());
#endif
  
  if (widget != NULL) {
    LRESULT lResult;
    bool used = false;

    Container refContainer;
    ref_widget(widget, refContainer);

    WPARAM old_wparam = widget->m_wparam;
    LPARAM old_lparam = widget->m_lparam;
    widget->m_wparam = wParam;
    widget->m_lparam = lParam;

    // window procedures
    used = widget->wndProc(message, wParam, lParam, lResult);
    if (!used)
      lResult = widget->defWndProc(message, wParam, lParam);

    widget->m_wparam = old_wparam;
    widget->m_lparam = old_lparam;

    unref_widget(refContainer);

    return lResult;
  }
  else {
//     widget = getThreadData()->outsideWidget;
    widget = __vaca_get_outside_widget();
    if (widget != NULL) {
      widget->m_HWND = hwnd;
      return widget->defWndProc(message, wParam, lParam);
    }
  }

  // never should be here...

  VACA_TRACE("------------ LOST MESSAGE: %p %d %d %d ---------------\n", hwnd, message, wParam, lParam);
  Beep(900, 100);

  return FALSE;
}
