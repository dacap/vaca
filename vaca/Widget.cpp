// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifdef VACA_WINDOWS
  #include <windows.h>
#endif

#include "vaca/Widget.h"
#include "vaca/WidgetClass.h"
#include "vaca/Brush.h"
#include "vaca/Constraint.h"
#include "vaca/Cursor.h"
#include "vaca/Debug.h"
#include "vaca/Dialog.h"
#include "vaca/DropFilesEvent.h"
#include "vaca/Font.h"
#include "vaca/Frame.h"
#include "vaca/Image.h"
#include "vaca/KeyEvent.h"
#include "vaca/Layout.h"
#include "vaca/MouseEvent.h"
#include "vaca/PaintEvent.h"
#include "vaca/Point.h"
#include "vaca/Region.h"
#include "vaca/System.h"
#include "vaca/Mutex.h"
#include "vaca/ScopedLock.h"
#include "vaca/Command.h"
#include "vaca/ScrollInfo.h"
#include "vaca/ScrollEvent.h"
#include "vaca/FocusEvent.h"
#include "vaca/CommandEvent.h"
#include "vaca/ResizeEvent.h"
#include "vaca/PreferredSizeEvent.h"
#include "vaca/SetCursorEvent.h"
#include "vaca/LayoutEvent.h"
#include "vaca/win32.h"

#include <iterator>

// uncomment this if you want message reporting in the "vaca.log"
// #define REPORT_MESSAGES

using namespace vaca;

#define VACA_ATOM (reinterpret_cast<LPCTSTR>(MAKELPARAM(atom, 0)))

static Mutex atomMutex; // used to access atom
static volatile ATOM atom = 0;

// Default callback to destroy a HWND
static void Widget_DestroyHandleProc(HWND hwnd)
{
  ::DestroyWindow(hwnd);
}

// Creates the "VacaAtom" (the property name to put the "Widget*"
// pointer in the HWNDs)
static void create_atom()
{
  ScopedLock hold(atomMutex);
  if (atom == 0) {
    atom = GlobalAddAtom(L"VacaAtom");
    assert(atom != 0);
  }
}

// ============================================================
// CTOR & DTOR
// ============================================================

/**
   Creates a new widget with the specified class name.

   You can specify WidgetClassName#None as @a className if you want
   to call #create member function by your self (see @ref page_tn_002).

   @param className
       Must be a registered WNDCLASS for Windows.

   @param parent
       It is a Widget (generally a Frame) that will act as the container
       parent of the new widget.

   @param style
       Specifies the @em style that this widget should follow. A style
       can be an aspect, behavior or property of the widget.
       There are styles for all widgets (like Widget::Styles::Focusable or
       Widget::Styles::Visible) and styles for particular widgets (like
       TextEdit::Styles::ReadOnly for TextEdit).
*/
Widget::Widget(const WidgetClassName& className, Widget* parent, Style style)
{
  initialize();

  // Do we create with the specified "className"?
  if (className != WidgetClassName::None)
    create(className, parent, style);
}

/**
   Creates a user control.

   This is an empty widget without behavior, so you can use this
   routine to create your own user control (a completelly customized
   widget).

   @param parent
       Container where we have to put the add widget.

   @param style
  	Style for the widget.
*/
Widget::Widget(Widget* parent, Style style)
{
  initialize();

  create(WidgetClass::getClassName(), parent, style);
}

/**
   Creates a widget from an existent Win32 handle that is not
   currently managed by Vaca.

   @warning A HWND can be wrapped just one time. You cannot create
   a Widget from an existent Vaca Widget.

   @win32
     This constructor is only for Windows platform.
   @endwin32

   @throw CreateWidgetException
     If the @a handle is NULL. This can occur if a dialog is initialized
     with a resource ID and the resource could not be loaded.
     Also this exception will be thrown if the widget is already subclassed
     by Vaca. This means that you cannot do something like the following example:
     @code
       Button button(L"OK", parent);
       Widget wrapper(button.getHandle()); // throws CreateWidgetException
     @endcode
*/
Widget::Widget(HWND handle)
{
  initialize();

  // the handle must be a valid Win32 handle
  if (handle == NULL || !::IsWindow(handle))
    throw CreateWidgetException(format_string(L"Cannot create a widget with an invalid handle."));

  // avoid double-subclassing
  if (fromHandle(handle) != NULL)
    throw CreateWidgetException(format_string(L"Cannot subclass two times the same widget."));

  // calibrate members so this special Widget acts like a "HWND wrapper"
  m_handle            = handle;
  m_defWndProc        = NULL; // m_baseWndProc will be used
  m_destroyHandleProc = NULL; // to avoid destroying this widget in dtor

  // subclass the handle
  subClass();

  assert(m_baseWndProc != NULL);

  // add the widget to its parent
  HWND parent_handle = ::GetParent(handle);
  Widget* parent = parent_handle ? Widget::fromHandle(parent_handle): NULL;
  if (parent != NULL)
    parent->addChildWin32(this, false);
}

void Widget::initialize()
{
  create_atom();

  m_handle            = NULL;
  m_parent            = NULL;
  m_fgColor           = System::getColor(COLOR_WINDOWTEXT);
  m_bgColor           = System::getColor(COLOR_3DFACE);
  m_constraint        = NULL;
  m_layout            = NULL;
  m_baseWndProc       = NULL;
  m_hasMouse          = false;
  m_deleteAfterEvent  = false;
  m_doubleBuffered    = false;
  m_preferredSize     = NULL;
  m_defWndProc        = ::DefWindowProc;
  m_destroyHandleProc = Widget_DestroyHandleProc;
  m_hbrush            = NULL;
}

/**
   Destroys the widget.

   @warning
     If the widget contains children, they will be automatically
     deleted.

   @win32
     It calls @msdn{DestroyWindow} function.
   @endwin32

   @see @ref page_tn_002, setDestroyHandleProc
*/
Widget::~Widget()
{
  assert(::IsWindow(m_handle));

  // Lost the focus. WARNING: if we do not make this, Dialogs will die
  // suddenly in an infinite loop when TAB key is pressed. It seems
  // like Win32 cannot handle dialog boxes, the keyboard focus, and
  // destroyed HWND at the same time
  if (hasFocus())
    releaseFocus();

  // hide the window (only if we will call DestroyWindow)
  if (m_destroyHandleProc)
    ::ShowWindow(m_handle, SW_HIDE);

  if (getParent() != NULL) {
    // this is very important!!! we cannot set the parent of the HWND:
    // MdiChild needs the parent HWND to send WM_MDIDESTROY
    getParent()->removeChildWin32(this, false); // <-- false means: do not call Win32's SetParent
  }

  // delete all children (this is the case for children added using
  // "new" operator that were not deleted)
  WidgetList children = getChildren(); // we need a copy of the children's list because...
  for (WidgetList::iterator
	 it = children.begin(); it != children.end(); ++it) {
    // ...each destructor will modify the m_children collection
    delete (*it);
  }

  m_constraint = NULL;		// unref the constraint
  m_layout = NULL;		// unref the layout manager
  delete m_preferredSize;	// delete the preferred size

  // restore the old window-procedure
  if (m_baseWndProc != NULL)
    SetWindowLongPtr(m_handle, GWLP_WNDPROC,
		     reinterpret_cast<LONG_PTR>(m_baseWndProc));

  if (m_hbrush)
    DeleteObject(m_hbrush);

  // call the procedure to destroy the HWND handler
  if (m_destroyHandleProc)
    (*m_destroyHandleProc)(m_handle);
  else
    RemoveProp(m_handle, VACA_ATOM);

  m_handle = NULL;
}

// ============================================================
// PARENT & CHILDREN RELATIONSHIP
// ============================================================

/**
   Returns the root widget (ancestor of all widgets).

   The root widget should be the main containing Frame.

   @note This routine never returns NULL (at least it will returns
	 this same widget).
*/
Widget* Widget::getRoot()
{
  Widget* root = this;

  while (root->m_parent != NULL)
    root = root->m_parent;

  return root;
}

/**
   Returns the parent of the widget.

   @win32
     This member function does not use the @msdn{GetParent}.
	 Widget has a m_parent member to hold its parent.
   @endwin32
*/
Widget* Widget::getParent() const
{
  return m_parent;
}

Widget* Widget::getFirstChild() const
{
  return !m_children.empty() ? m_children.front(): NULL;
}

Widget* Widget::getLastChild() const
{
  return !m_children.empty() ? m_children.back(): NULL;
}

Widget* Widget::getPreviousSibling() const
{
  if (m_parent) {
    WidgetList::iterator it = std::find(m_parent->m_children.begin(),
					m_parent->m_children.end(), this);

    if (it != m_parent->m_children.end()) {
      if (it != m_parent->m_children.begin())
	return *(--it);
    }
  }
  return NULL;
}

Widget* Widget::getNextSibling() const
{
  if (m_parent) {
    WidgetList::iterator it = std::find(m_parent->m_children.begin(),
					m_parent->m_children.end(), this);

    if (it != m_parent->m_children.end()) {
      if ((++it) != m_parent->m_children.end())
	return *it;
    }
  }
  return NULL;
}

/**
   Returns the collection of children. The returned list is a
   copy of the original, so you can do with it what you want,
   in other words, does not matter if you add or remove elements
   from them, the original list of children will not be modified.
*/
WidgetList Widget::getChildren() const
{
#if 0				// do not use this alternative (it
				// does not work to synchronize Frame)
  assert(::IsWindow(m_handle));

  WidgetList container;

  HWND hwndChild = ::GetWindow(m_handle, GW_CHILD);
  if (hwndChild != NULL) {
    do {
      Widget* widget = Widget::fromHandle(hwndChild);
      if (widget != NULL)
	container.push_back(widget);
    } while ((hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT)) != NULL);
  }

  return container;
#else
  return m_children;
#endif
}

/**
   Adds a child to this widget.

   @param child
     The child to add inside the widget.
*/
void Widget::addChild(Widget* child)
{
  addChildWin32(child, true);
}

/**
   Removes a child from this widget.

   @param child
       The child that we want to remove from the parent.
*/
void Widget::removeChild(Widget* child)
{
  removeChildWin32(child, true);
}

bool Widget::hasChild(const Widget* child) const
{
  return std::find(m_children.begin(), m_children.end(), child) != m_children.end();
}

bool Widget::hasDescendant(const Widget* descendant) const
{
  WidgetList remaining;

  remaining.push_back((Widget*)this);
  while (!remaining.empty()) {
    Widget* widget = remaining.back();
    remaining.pop_back();

    if (widget->hasChild(descendant))
      return true;

    std::copy(m_children.begin(), m_children.end(), std::back_inserter(remaining));
  }

  return false;
}

/**
   Moves the widget before the specified @a sibling.

   @note This routine changes the z-order of children, so the tab-order is
	 affected.

   @param sibling
     The widget will take the position before @a sibling.
     After using this routine, if you call Widget#getNextSibling
     you will get this @sibling argument.
     This parameter can be NULL (so the widget will take the last position).
*/
void Widget::moveBeforeWidget(Widget* sibling)
{
  assert(this != sibling);

  assert(m_parent != NULL);
  remove_from_container(m_parent->m_children, this);

  if (sibling != NULL) {
    WidgetList::iterator it =
      std::find(m_parent->m_children.begin(),
		m_parent->m_children.end(), sibling);

    assert(it != m_parent->m_children.end());

    m_parent->m_children.insert(it, this);
  }
  else {
    m_parent->m_children.push_back(this);
  }

  // Fix HWND handles
  if (getPreviousSibling())
    ::SetWindowPos(getHandle(), getPreviousSibling()->getHandle(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  else
    ::SetWindowPos(getHandle(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

  assert(getNextSibling() == sibling);
}

/**
   Moves the widget after the specified @a sibling.

   @note This routine changes the z-order of children, so the tab-order is
   affected.

   @param sibling
     The widget will take the position after @a sibling.
     After using this routine, if you call Widget#getPreviousSibling
     you will get this @sibling argument.
     This parameter can be NULL (so the widget will take the first position).
*/
void Widget::moveAfterWidget(Widget* sibling)
{
  assert(this != sibling);

  assert(m_parent != NULL);
  remove_from_container(m_parent->m_children, this);

  if (sibling != NULL) {
    WidgetList::iterator it =
      std::find(m_parent->m_children.begin(),
		m_parent->m_children.end(), sibling);

    assert(it != m_parent->m_children.end());

    m_parent->m_children.insert(++it, this);
  }
  else
    m_parent->m_children.insert(m_parent->m_children.begin(), this);

  // Fix HWND handles
  if (getPreviousSibling())
    ::SetWindowPos(getHandle(), getPreviousSibling()->getHandle(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  else
    ::SetWindowPos(getHandle(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

  assert(getPreviousSibling() == sibling);
}

// ===============================================================
// LAYOUT & CONSTRAINT
// ===============================================================

/**
   Returns the current Layout that arranges the widget's children.

   For most widgets, the Layout will be NULL. But for widgets like
   Frame you should use #setLayout to change the Layout manager.

   @see setLayout, getConstraint, @ref page_tn_011
*/
LayoutPtr Widget::getLayout()
{
  return m_layout;
}

/**
   Changes the current layout manager to arrange widget's children.

   @see getLayout, setConstraint
*/
void Widget::setLayout(LayoutPtr layout)
{
  m_layout = layout;
}

/**
   Returns the widget's constraint.

   This constraint is used by the parent widget's Layout to know how
   to lay the widget. For example, the AnchorLayout uses a Anchor
   constraint to know what side of a widget is anchored.

   @see setConstraint, getLayout
*/
ConstraintPtr Widget::getConstraint()
{
  return m_constraint;
}

/**
   Modifies the widget's constraint.

   @see getConstraint, setLayout
*/
void Widget::setConstraint(ConstraintPtr constraint)
{
  m_constraint = constraint;
}

/**
   Arranges the position/size of children widgets.

   This member function is called from Widget#onResize, so when the
   Widget is shown for first time or it is resized, the
   children are automatically positioned.
*/
void Widget::layout()
{
  LayoutEvent ev(this, getClientBounds());
  onLayout(ev);
}

/**
   Returns true if the widget is layout-free: widget's bounds are not
   controled by the layout manager of the parent.

   The default implementation looks for the VisibleStyle (if it is
   hidden, it is free of layout). You should override this member function
   and return true if your widget set its position by itself (like a
   StatusBar).

   @see getLayout, setLayout, onParentLayout
*/
bool Widget::isLayoutFree() const
{
  // A widget is free of layout if it's hidden
  return ((getStyle().regular & WS_VISIBLE) == WS_VISIBLE) ? false: true;
}

// ===============================================================
// TEXT & FONT
// ===============================================================

/**
   Returns the widget's text, label, or frame's title.

   @win32
     It uses @msdn{GetWindowTextLength} and @msdn{GetWindowText}.
   @endwin32

   @see setText
*/
String Widget::getText() const
{
  assert(::IsWindow(m_handle));

  int len = ::GetWindowTextLength(m_handle);
  if (!len)
    return L"";
  else {
    Char* buf = new Char[len+1];
    ::GetWindowText(m_handle, buf, len+1);
    String str(buf);
    delete[] buf;
    return str;
  }
}

/**
   Changes the widget's text, label, or frame's title.

   @win32
     It uses the @msdn{SetWindowText}.
   @endwin32
*/
void Widget::setText(const String& str)
{
  assert(::IsWindow(m_handle));
  ::SetWindowText(m_handle, str.c_str());
}

/**
   Returns the current font used to paint the Widget.
*/
Font Widget::getFont() const
{
  return m_font;
}

/**
   Sets the font of the widget.
*/
void Widget::setFont(Font font)
{
  m_font = font;
  sendMessage(WM_SETFONT, reinterpret_cast<WPARAM>(m_font.getHandle()), TRUE);
}

// ===============================================================
// COMMAND
// ===============================================================

CommandId Widget::getId() const
{
  assert(::IsWindow(m_handle));
  return ::GetWindowLong(m_handle, GWL_ID);
}

void Widget::setId(CommandId id)
{
  assert(::IsWindow(m_handle));
  ::SetWindowLong(m_handle, GWL_ID, id);
}

/**
   Returns the command with the specified ID looking in this widget.

   The command is searched in this instance if the Widget is a
   CommandsClient (a @c dynamic_cast is used for this).
*/
Command* Widget::getCommandById(CommandId id)
{
  // if this widget is a CommandsClient instance
  if (CommandsClient* cc = dynamic_cast<CommandsClient*>(this)) {
    if (Command* cmd = cc->getCommandById(id))
      return cmd;
  }
  return NULL;
}

/**
   Returns the command with the specified ID searching in all the
   CommandsClient that this widget can know: itself, the parent
   (ancestors), and the Application instance.

   The command is searched in this instance if the Widget is a
   CommandsClient (a @c dynamic_cast is used for this), then it looks
   for the parent (using this same member function), and finally in the
   @link Application#getInstance Application instance@endlink (if the
   Application is a CommandsClient).
*/
Command* Widget::findCommandById(CommandId id)
{
  Widget* widget = this;

  while (widget != NULL) {
    if (Command* cmd = getCommandById(id))
      return cmd;
    widget = widget->getParent();
  }

  // check if the application is a CommandsClient
  if (CommandsClient* cc = dynamic_cast<CommandsClient*>(Application::getInstance())) {
    if (Command* cmd = cc->getCommandById(id))
      return cmd;
  }

  return NULL;
}

// ===============================================================
// WIDGET STYLE
// ===============================================================

/**
   Returns the current Widget style.
*/
Style Widget::getStyle() const
{
  assert(::IsWindow(m_handle));

  return Style(::GetWindowLong(m_handle, GWL_STYLE),
	       ::GetWindowLong(m_handle, GWL_EXSTYLE));
}

/**
   Returns true if the widget has all the specified styles.

   @param style
     Set of styles to see if the widget has.
*/
bool Widget::hasStyle(Style style) const
{
  return (getStyle() & style) == style;
}

/**
   Replaces all the styles of the Widget with the new ones specified
   in @a style parameter.

   @win32
     It uses @msdn{SetWindowLong} to setup the @msdn{GWL_STYLE} and
     @msdn{GWL_EXSTYLE}.
   @endwin32

   @see addStyle, removeStyle
*/
void Widget::setStyle(Style style)
{
  assert(::IsWindow(m_handle));

  ::SetWindowLong(m_handle, GWL_STYLE, style.regular);
  ::SetWindowLong(m_handle, GWL_EXSTYLE, style.extended);

  // TODO MSDN says to do this after SetWindowLong
//   SetWindowPos(mWND, NULL, 0, 0, 0, 0,
// 	       SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

/**
   Adds styles to the widget.

   @see setStyle
*/
void Widget::addStyle(Style style)
{
  assert(::IsWindow(m_handle));

  setStyle(getStyle() + style);
}

/**
   Removes styles from the widget.

   @see setStyle
*/
void Widget::removeStyle(Style style)
{
  assert(::IsWindow(m_handle));

  setStyle(getStyle() - style);
}

// ===============================================================
// SIZE & POSITION
// ===============================================================

/**
   Gets the dimensions of the entire bounding rectangle that enclose
   the Widget.

   The bounds are relative to the upper-left corner of the widget's
   bounds, or absolute to the screen if this Widget hasn't parent.

   @see getClientBounds, getAbsoluteBounds
*/
Rect Widget::getBounds() const
{
  assert(::IsWindow(m_handle));

  RECT rc;
  ::GetWindowRect(m_handle, &rc);

  if (m_parent != NULL &&
      (::GetWindowLong(m_handle, GWL_STYLE) & WS_CAPTION) == 0) {
    POINT pt1 = { rc.left, rc.top };
    POINT pt2 = { rc.right, rc.bottom };

    ::ScreenToClient(m_parent->m_handle, &pt1);
    ::ScreenToClient(m_parent->m_handle, &pt2);

    return Rect(pt1.x, pt1.y, pt2.x-pt1.x, pt2.y-pt1.y);
  }
  else {
    return convert_to<Rect>(rc);
  }
}

/**
   Gets the dimensions of the bounding rectangle relative to the
   upper-left corner of the screen (absolute position in the screen).

   @see getAbsoluteClientBounds, getBounds
*/
Rect Widget::getAbsoluteBounds() const
{
  assert(::IsWindow(m_handle));

  RECT rc;
  ::GetWindowRect(m_handle, &rc);

  return convert_to<Rect>(rc);
}

/**
   Gets the client bounds.

   It is the area which you should use to draw the widget content.

   @win32
     It's like @msdn{GetClientRect}.
   @endwin32

   @see getBounds, getAbsoluteClientBounds
*/
Rect Widget::getClientBounds() const
{
  RECT rc;
  assert(::IsWindow(m_handle));
  ::GetClientRect(m_handle, &rc);
  return convert_to<Rect>(rc);
}

/**
   Gets the client bounds relative to the upper-left corner of the
   screen.

   @see getAbsoluteBounds, getClientBounds
*/
Rect Widget::getAbsoluteClientBounds() const
{
  RECT rc = convert_to<RECT>(getClientBounds());
  ::MapWindowPoints(m_handle, NULL, (POINT*)&rc, 2);
  return convert_to<Rect>(rc);
}

/**
   Sets the boundary rectangle for the widget.


   The rectangle must be in absolute coordinates for widgets without
   parent, and relative to the parent's client area otherwise.

   @see Widget#setBounds(int,int,int,int), getBounds
*/
void Widget::setBounds(const Rect& rc)
{
  assert(::IsWindow(m_handle));

  /* TODO remove this
  ::SetWindowPos(m_handle, NULL,
		 rc.left, rc.top, rc.getWidth(), rc.getHeight(),
		 SWP_NOZORDER | SWP_NOACTIVATE);
  */

  ::MoveWindow(m_handle, rc.x, rc.y, rc.w, rc.h, TRUE);
}

/**
   Sets the boundary rectangle for the widget.


   The rectangle must be in absolute coordinates for widgets without
   parent, and relative to the parent's client area otherwise.

   @see Widget#setBounds(const Rect&), getBounds
*/
void Widget::setBounds(int x, int y, int w, int h)
{
  setBounds(Rect(x, y, w, h));
}

/**
   Moves the center of the widget to the center of the parent window
   or the screen if does not have a parent.

   The center of the widget is Widget::getBounds().getCenter(),
   this member function moves the widget to the center of its parent
   or the work-area bounds.

   @see setOrigin, setBounds, System#getWorkAreaBounds
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
   Sets the position of the widget.

   This member function does not affect the size of the widget. The coordinates
   are relative to the parent's client area. If the widget does not
   have a parent, the position is an absolute screen position.

   @see setOrigin(int,int), setBounds, center
*/
void Widget::setOrigin(const Point& pt)
{
  Size sz = getBounds().getSize();

  setBounds(Rect(pt, sz));
}

/**
   Sets the position of the widget.

   This member function does not affect the size of the widget. The coordinates
   are relative to the parent's client area. If the widget does not
   have a parent, the position is an absolute screen position.

   @see setOrigin(const Point& pt)
*/
void Widget::setOrigin(int x, int y)
{
  setOrigin(Point(x, y));
}

/**
   Sets the size of the widget.

   It does not affect the origin position.

   @see setSize(int,int)
*/
void Widget::setSize(const Size& sz)
{
  Point pt = getBounds().getOrigin();

  setBounds(Rect(pt, sz));
}

/**
   Sets the size of the widget.

   It does not affect the origin position.

   @see setSize(const Size&)
*/
void Widget::setSize(int w, int h)
{
  setSize(Size(w, h));
}

/**
   Returns the preferred size of the Widget.

   It checks if the preferred size is static (it means when it was
   set through #setPreferredSize before) or if it is dynamic (this is
   the default and is when the #onPreferredSize is used to determined
   the preferred size).

   In another words, if you do not use #setPreferredSize to set a
   <em>static preferred size</em> for the widget then #onPreferredSize
   will be used to calculate it.

   @see setPreferredSize, onPreferredSize, #getPreferredSize(const Size &)
*/
Size Widget::getPreferredSize()
{
  if (m_preferredSize != NULL)
    return *m_preferredSize;
  else {
    PreferredSizeEvent ev(this, Size(0, 0));
    onPreferredSize(ev);
    return ev.getPreferredSize();
  }
}

/**
   Returns the preferred size trying to fit in the specified size.
   Remember that if you use #setPreferredSize this routine will
   return the static size which you specified manually.

   @param fitIn
       This can have both attributes (width and height) in
       zero, which means that it'll behave same as #getPreferredSize().
       If the width is great than zero the #onPreferredSize will try to
       fit in that width (this is useful to fit @link vaca::Label Label@endlink
       or @link vaca::Edit Edit@endlink controls in a specified width and
       calculate the height it could occupy).

   @see getPreferredSize
*/
Size Widget::getPreferredSize(const Size& fitIn)
{
  if (m_preferredSize != NULL)
    return *m_preferredSize;
  else {
    PreferredSizeEvent ev(this, fitIn);
    onPreferredSize(ev);
    return ev.getPreferredSize();
  }
}

/**
   Sets a fixed preferred size specified by the user.
   Widget::getPreferredSize() will return this value if it's setted.
*/
void Widget::setPreferredSize(const Size& fixedSize)
{
  delete m_preferredSize;
  m_preferredSize = new Size(fixedSize);
}

void Widget::setPreferredSize(int fixedWidth, int fixedHeight)
{
  setPreferredSize(Size(fixedWidth, fixedHeight));
}

// ===============================================================
// REFRESH ISSUES
// ===============================================================

/**
   Returns true if the double-buffering technique is activated
   in this widget.

   @see setDoubleBuffered
*/
bool Widget::isDoubleBuffered()
{
  return m_doubleBuffered;
}

/**
   Sets if you want or not to use a double-buffering technique to
   draw the entire widget content.

   With double-buffering technique you can avoid @wikipedia{Flicker_(screen),flickering effect}.

   @see isDoubleBuffered
*/
void Widget::setDoubleBuffered(bool doubleBuffered)
{
  m_doubleBuffered = doubleBuffered;
}

/**
   Validates the entire widget.

   It removes all paint messages from the message queue, because a
   validated widget is like a widget that does not need to be
   repainted.

   @see invalidate
*/
void Widget::validate()
{
  assert(::IsWindow(m_handle));
  ::ValidateRect(m_handle, NULL);
}

/**
   Validates a part of the widget.

   This means that the specified rectangle does not need to be
   repainted.

   @see invalidate(bool)
*/
void Widget::validate(const Rect& _rc)
{
  assert(::IsWindow(m_handle));

  RECT rc = convert_to<RECT>(_rc);
  ::ValidateRect(m_handle, &rc);
}

/**
   Invalidates the entire client area.

   All the widget will be repainted through an #onPaint event.

   @param eraseBg
       true means that the background should be erased
       (with a WM_ERASEBKGND message).

   @see validate, invalidate(const Rect&, bool), update
*/
void Widget::invalidate(bool eraseBg)
{
  assert(::IsWindow(m_handle));
  ::InvalidateRect(m_handle, NULL, eraseBg);
}

/**
   Invalidates the specified rectangle.

   When the next round of <em>paint messages</em> is processed by the
   operating system the specified area will be redrawn. This means
   that the area will not be repainted immediately (you should call
   #update member function to do this).

   @param rc
       Area to repaint.

   @param eraseBg
       true means that the background should be erased with
       the background color specified by #getBgColor (with a
       WM_ERASEBKGND message for example).

   @see invalidate(bool), #update
*/
void Widget::invalidate(const Rect& _rc, bool eraseBg)
{
  RECT rc = convert_to<RECT>(_rc);

  assert(::IsWindow(m_handle));
  ::InvalidateRect(m_handle, &rc, eraseBg);
}

/**
   Flushes the widget invalidated area to redraw it now.

   This routine should be used if you want to show the changes of the
   widget immediately without to wait for the message queue to be
   processed.

   @win32
     It's like @msdn{UpdateWindow}.
   @endwin32
*/
void Widget::update()
{
  assert(::IsWindow(m_handle));
  ::UpdateWindow(m_handle);
}

/**
   Refreshes the state of indicators that could be inside this
   widget.

   An indicator is a MenuItem or a ToolButton which its state changes
   depending the current context of the Application. The current
   context gives to you different states for every Command, so we have
   to call Command#isEnabled for every visible indicator associated to
   a Command. This means that updateIndicators refresh the state of
   the visible ToolBar's buttons and MenuBar's menu items.

   Note: menu items in sub-menus or popup-menus are updated when they
   are shown.

   @see MenuBar, ToolBar, MenuItem, ToolButton
*/
void Widget::updateIndicators()
{
  onUpdateIndicators();
}

// ===============================================================
// COMMON PROPERTIES
// ===============================================================

/**
   Returns true if this widget is visible.

   If this widget or some of its parents has the visibility state,
   this routine returns true.

   @see Frame#setVisible
*/
bool Widget::isVisible()
{
  assert(::IsWindow(m_handle));

  return ::IsWindowVisible(m_handle) != FALSE;
}

/**
   Changes the visibility of this widget.

   The children are hidden too.
*/
void Widget::setVisible(bool visible)
{
  assert(::IsWindow(m_handle));

  if (visible)
    ::ShowWindow(m_handle, SW_SHOW);
  else {
    if (hasFocus())
      releaseFocus();

    ::ShowWindow(m_handle, SW_HIDE);
  }
}

/**
   Returns true if the widget is enabled.

   @win32
     This member function calls @msdn{IsWindowEnabled}.
   @endwin32
*/
bool Widget::isEnabled()
{
  assert(::IsWindow(m_handle));

  return ::IsWindowEnabled(m_handle) != FALSE;
}

/**
   Changes the enable-state of the widget.

   @win32
     If @a state is true removes the @msdn{WS_DISABLED} style,
     if @a state is false adds the @msdn{WS_DISABLED} style.
     It's like to call the @msdn{EnableWindow}.
   @endwin32
*/
void Widget::setEnabled(bool state)
{
  assert(::IsWindow(m_handle));

//   if (::GetFocus() == m_handle)
//     ::SetFocus(NULL);
  if (hasFocus())
    releaseFocus();

  ::EnableWindow(m_handle, state ? TRUE: FALSE);
}

/**
   Gets the foreground color. The foreground color generally is the
   font color to be used.

   @return
       The current foreground color for the widget.

   @see setFgColor
*/
Color Widget::getFgColor()
{
  return m_fgColor;
}

/**
   Gets the background color.

   @return
       The current background color for the widget.

   @see setBgColor
*/
Color Widget::getBgColor()
{
  return m_bgColor;
}

/**
   Sets the foreground color. This color is used when WM_CTLCOLOR*
   messages are received. The default value is the System::getColor(COLOR_WINDOWTEXT).

   @see getFgColor
*/
void Widget::setFgColor(const Color& color)
{
  m_fgColor = color;
}

/**
   Sets the background color. This color is used on WM_ERASEBKGND and
   WM_CTLCOLOR* messages. The default value is the
   System::getColor(COLOR_3DFACE).

   @see getBgColor
*/
void Widget::setBgColor(const Color& color)
{
  m_bgColor = color;
}

// ======================================================================
// Based on code of J Brown (http://www.catch22.net/)

// Prototypes for Win2000/XP API
typedef BOOL (WINAPI* GLWAProc)(HWND hwnd, COLORREF* pcrKey, BYTE* pbAlpha, DWORD* pdwFlags);
typedef BOOL (WINAPI* SLWAProc)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

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
   Returns the widget opacity. If the current OS does not support
   translucent windows, it will always be 255.

   @return A value from 0 (completelly transparent) to 255 (completelly opaque)
*/
int Widget::getOpacity()
{
  assert(::IsWindow(m_handle));

  HMODULE hUser32 = GetModuleHandle(L"USER32.DLL");
  GLWAProc pGLWA;
  if (hUser32 != NULL)
    pGLWA = (GLWAProc)GetProcAddress(hUser32, "GetLayeredWindowAttributes");
  else
    pGLWA = NULL;

  if (pGLWA == NULL) {
    return 255;
  }
  else if (GetWindowLong(m_handle, GWL_EXSTYLE) & WS_EX_LAYERED) {
    BYTE opacity;
    pGLWA(m_handle, NULL, &opacity, NULL);
    return opacity;
  }
  else {
    return 255;
  }
}

/**
   Sets the alpha property of the widget. It uses the
   SetLayeredWindowAttributes, if it's available, if not, does
   nothing.

   @param opacity
       A value from 0 (completelly transparent) to 255 (completelly
       opaque)
*/
void Widget::setOpacity(int opacity)
{
  assert(::IsWindow(m_handle));
  assert(opacity >= 0 && opacity < 256);

  HMODULE  hUser32 = GetModuleHandle(L"USER32.DLL");
  SLWAProc pSLWA = (SLWAProc)GetProcAddress(hUser32, "SetLayeredWindowAttributes");

  // The version of Windows running does not support translucent windows!
  if (pSLWA == 0) {
    // do nothing
    return;
  }
  else if (opacity == 255) {
    SetWindowLong(m_handle, GWL_EXSTYLE,
		  GetWindowLong(m_handle, GWL_EXSTYLE) & (~WS_EX_LAYERED));
  }
  else {
    SetWindowLong(m_handle, GWL_EXSTYLE,
  		  GetWindowLong(m_handle, GWL_EXSTYLE) | WS_EX_LAYERED);

    pSLWA(m_handle, 0, opacity, LWA_ALPHA);
  }
}

// ===============================================================
// FOCUS & MOUSE
// ===============================================================

/**
   Sets the keyboard focus to this Widget.
*/
void Widget::requestFocus()
{
  assert(::IsWindow(m_handle));
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
			reinterpret_cast<WPARAM>(m_handle),
			MAKELPARAM(TRUE, 0));
  else
    ::SetFocus(m_handle);
#else
  ::SetFocus(m_handle);
#endif
}

/**
   Frees the keyboard focus.
*/
void Widget::releaseFocus()
{
  assert(::IsWindow(m_handle));
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
  if (m_handle == ::GetFocus()) {
    ::SetFocus(NULL);
  }
#endif
}

/**
   Captures the mouse to receive its events even when it's outside the
   widget.

   You must call Widget#releaseMouse when you are done with the mouse.

   @see releaseMouse
*/
void Widget::captureMouse()
{
  assert(::IsWindow(m_handle));

  ::SetCapture(m_handle);
}

/**
   Releases the previously captured mouse.

   @see captureMouse
*/
void Widget::releaseMouse()
{
  assert(::IsWindow(m_handle));

  if (m_handle == ::GetCapture())
    ::ReleaseCapture();
}

/**
   Returns true if the widget has the focus.
*/
bool Widget::hasFocus()
{
  assert(::IsWindow(m_handle));
  return m_handle == ::GetFocus();
}

/**
   Returns true if the widget has the mouse.

   If a widget captured the mouse, then this routine will return true
   anyway until you release the mouse.

   @see hasCapture, hasMouseAbove
*/
bool Widget::hasMouse()
{
  return m_hasMouse;
}

/**
   Returns true if the widget has the mouse above.

   It does not matter if the widget has the capture status.

   @see hasMouse
*/
bool Widget::hasMouseAbove()
{
  return ::WindowFromPoint(convert_to<POINT>(System::getCursorPos())) == m_handle;
}

/**
   Returns true if the widget has captured the mouse.

   @see hasMouse
*/
bool Widget::hasCapture()
{
  assert(::IsWindow(m_handle));
  return m_handle == ::GetCapture();
}

// ===============================================================
// SCROLL
// ===============================================================

/**
   Retrieves the scroll information in the specified @a orientation.

   @param orientation
     Gets the information of the scroll bar in this direction:
     @li Orientation::Horizontal
     @li Orientation::Vertical

   @see #setScrollInfo, #getScrollPos
*/
ScrollInfo Widget::getScrollInfo(Orientation orientation) const
{
  assert(::IsWindow(m_handle));

  int fnBar = (orientation == Orientation::Horizontal) ? SB_HORZ: SB_VERT;

  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask = SIF_RANGE | SIF_PAGE;

  ::GetScrollInfo(m_handle, fnBar, &si);

  return ScrollInfo(si.nMin, si.nMax, si.nPage);
}

/**
   Sets the scroll information in the specified @a orientation.

   @param orientation
     Sets the information for the scroll bar in this direction:
     @li Orientation::Horizontal
     @li Orientation::Vertical

   @param scrollInfo
     New scroll information.

   @see #getScrollInfo, #setScrollPos
*/
void Widget::setScrollInfo(Orientation orientation, const ScrollInfo& scrollInfo)
{
  assert(::IsWindow(m_handle));

  int fnBar = (orientation == Orientation::Horizontal) ? SB_HORZ: SB_VERT;

  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask  = SIF_RANGE | SIF_PAGE;
  si.nMin = scrollInfo.getMinPos();
  si.nMax = scrollInfo.getMaxPos();
  si.nPage = scrollInfo.getPageSize();

  ::SetScrollInfo(m_handle, fnBar, &si, TRUE);
}

/**
   Retrieves the current scroll position in the specified @a orientation.

   @param orientation
     Returns the information of the scroll bar in this direction:
     @li Orientation::Horizontal
     @li Orientation::Vertical

   @see #setScrollPos, #getScrollInfo
*/
int Widget::getScrollPos(Orientation orientation) const
{
  assert(::IsWindow(m_handle));

  int fnBar = (orientation == Orientation::Horizontal) ? SB_HORZ: SB_VERT;

  SCROLLINFO si;
  si.cbSize = sizeof (si);
  si.fMask  = SIF_POS;
  ::GetScrollInfo(m_handle, fnBar, &si);
  return si.nPos;
}

/**
   Sets the current scroll position in the specified @a orientation.

   @param orientation
     Sets the position for the scroll bar in this direction:
     @li Orientation::Horizontal
     @li Orientation::Vertical

   @see #getScrollPos, #setScrollInfo
*/
void Widget::setScrollPos(Orientation orientation, int pos)
{
  assert(::IsWindow(m_handle));

  int fnBar = (orientation == Orientation::Horizontal) ? SB_HORZ: SB_VERT;

  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask  = SIF_ALL;
  ::GetScrollInfo(m_handle, fnBar, &si);

  si.fMask = SIF_POS;
  si.nPos = clamp_value(pos,
			si.nMin,
			si.nMax - max_value(static_cast<int>(si.nPage) - 1, 0));
  ::SetScrollInfo(m_handle, fnBar, &si, TRUE);
}

/**
   Retrieves the current scroll point.

   It is like this:
   @code
   Point pt = Point(widget->getScrollPos(Orientation::Horizontal),
		    widget->getScrollPos(Orientation::Vertical));
   @endcode

   @see #setScrollPoint, #getScrollPos
*/
Point Widget::getScrollPoint() const
{
  return Point(getScrollPos(Orientation::Horizontal),
	       getScrollPos(Orientation::Vertical));
}

/**
   Sets the current scroll point.

   It is like this:
   @code
   widget->setScrollPos(Orientation::Horizontal, pt.x);
   widget->setScrollPos(Orientation::Vertical, pt.y);
   @endcode

   @see #getScrollPoint, #setScrollPos
*/
void Widget::setScrollPoint(const Point& pt)
{
  setScrollPos(Orientation::Horizontal, pt.x);
  setScrollPos(Orientation::Vertical, pt.y);
}

/**
   @todo docme
*/
void Widget::hideScrollBar(Orientation orientation)
{
  assert(::IsWindow(m_handle));

  int fnBar = (orientation == Orientation::Horizontal) ? SB_HORZ: SB_VERT;

  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask  = SIF_RANGE;
  si.nMin = si.nMax = 0;
  ::SetScrollInfo(getHandle(), fnBar, &si, TRUE);
}

/**
   @todo docme

   @win32
     ScrollWindowEx
   @endwin32
*/
void Widget::scrollRect(const Rect& _rc, const Point& delta)
{
  assert(::IsWindow(m_handle));

  RECT rc = convert_to<RECT>(_rc);
  ScrollWindowEx(m_handle,
		 delta.x, delta.y,
		 &rc, &rc, NULL, NULL,
		 SW_ERASE | SW_INVALIDATE);
}

/**
   This routine is called before to dispatch the message.

   You can override this virtual member function to receive custom
   messages from another threads.

   @win32
     For example, it is used by Dialog to call @msdn{IsDialogMessage}.
   @endwin32

   @return
      True if the message was translated and sent, so the GUI
      thread does not need to dispatch it.
*/
bool Widget::preTranslateMessage(Message& message)
{
  if (m_parent != NULL)
    return m_parent->preTranslateMessage(message);
  else
    return false;
}

/**
   Enqueues a message to the widget to be processed in its own thread.

   @see @ref page_tn_015, @link page_examples Threads example@endlink
*/
void Widget::enqueueMessage(const Message& message)
{
  ::PostMessage(getHandle(),
		message.m_msg.message,
		message.m_msg.wParam,
		message.m_msg.lParam);
}

/**
   Returns the HWND of this Widget. This cannot be NULL.

   @see fromHandle, getParentHandle
*/
HWND Widget::getHandle() const
{
  assert(!m_handle || ::IsWindow(m_handle));
  return m_handle;
}

/**
   Returns the HWND of the parent, or NULL if this widget does not have a parent.

   @see getHandle
*/
HWND Widget::getParentHandle() const
{
  Widget* parent = getParent();
  return parent != NULL ? parent->getHandle(): NULL;
}

/**
   Returns the Widget pointer from an internal field of the specified
   HWND. This routine makes sense only when the HWND comes from a
   Vaca's Widget. In other words, you should use this only if you known
   that the HWND was created inside Vaca bounds.

   @win32
     Old versions of Vaca uses the GWL_USERDATA field to get
     the Widget, now it uses a property called @em "VacaAtom"
     (through @msdn{GetProp} function).
   @endwin32

   @see getHandle
*/
Widget* Widget::fromHandle(HWND hwnd)
{
  // unbox the pointer...

  // ScopedLock hold(atomMutex); <-- is it necessary?
  return reinterpret_cast<Widget*>(::GetProp(hwnd, VACA_ATOM));
}

/**
   Returns the global Vaca windows procedure that is used in the
   WNDCLASS.

   @see globalWndProc, wndProc
*/
WNDPROC Widget::getGlobalWndProc()
{
  return Widget::globalWndProc;
}

// ===============================================================
// EVENTS
// ===============================================================

/**
   Calculates the preferred size for the widget.

   The default implementation get the preferred size of the current
   layout manager. Also, if there exists layout-free widgets inside
   this parent (like a StatusBar), they preferred-sizes are
   accumulated.

   @see Layout#getPreferredSize,
*/
void Widget::onPreferredSize(PreferredSizeEvent& ev)
{
  Size sz;

  // there is a layout?
  if (m_layout != NULL) {
    // get the list of children
    WidgetList children = getChildren();

    // calculate the preferred size through the layout manager
    sz = m_layout->getPreferredSize(this, children, ev.fitInSize());
  }

  // Search for layout-free widgets
  for (WidgetList::iterator
	 it=m_children.begin(); it!=m_children.end(); ++it) {
    Widget* child = *it;
    if (child->isLayoutFree()) {
      PreferredSizeEvent ev2(this, Size(0, 0));
      child->onPreferredSize(ev2);
      sz += ev2.getPreferredSize();
    }
  }

  ev.setPreferredSize(sz);
}

/**
   Event generated to arrange the position and size of children widgets.

   The default implementation calls the Layout#layout member function of the
   Widget's layout manager, but you can override this event to obtain
   your own behavior (you can avoid to use layout manager at all if
   you want).

   When #isLayoutFree is overloaded to return true, #onLayout must to
   arrange the widget by itself in its parent bounds (e.g. StatusBar
   changes its own size and position to the bottom of the parent
   bounds).

   @see getLayout, setLayout.
*/
void Widget::onLayout(LayoutEvent& ev)
{
  // Call onLayout() member function for layout-free children (they know
  // how to layout theirself). Generally, these widgets will modify the
  // bounds of LayoutEvent
  for (WidgetList::iterator
	 it=m_children.begin(); it!=m_children.end(); ++it) {
    Widget* child = *it;
    if (child->isLayoutFree())
      child->onLayout(ev);
  }

  // Now we can use the layout manager with the bounds of LayoutEvent
  if (m_layout != NULL && !m_children.empty())
    m_layout->layout(this, m_children, ev.getBounds());
}

/**
   Called when the widget have to be painted in the screen.

   If you override this member function, remember: (1) to draw inside the
   Widget#getClientBounds limits, and (2) do not call the base member
   function Widget#onPaint.

   @warning You cannot override the #onPaint of a system control
	    (like Button, Edit, etc.). You must to use CustomButton,
	    CustomEdit, etc. to do that.

   Overriding:
   @code
   class MyWidget : public Widget
   {
   protected:
     virtual void onPaint(PaintEvent& ev)
     {
       Graphics& g = ev.getGraphics();
       Rect bounds = getClientBounds();
       Rect clip = g.getClipBounds();
       ...
     }
   };
   @endcode

   @win32
     Called then the @msdn{WM_PAINT} event is received.
   @endwin32

   @see onReflectedDrawItem
*/
void Widget::onPaint(PaintEvent& ev)
{
  // Do nothing
}

/**
   Called when the user changes the size of the widget/frame.

   @win32
     This event is generated when a @msdn{WM_SIZE} message is received.
   @endwin32
*/
void Widget::onResize(ResizeEvent& ev)
{
  Resize(ev);

  // Do not call layout() here. The first trigger of layout()
  // member function is from Frame::onResize()
}

/**
   The mouse enters in the Widget.

   @win32
     This event is generated when the first @msdn{WM_MOUSEMOVE} message is received.
   @endwin32
*/
void Widget::onMouseEnter(MouseEvent& ev)
{
  MouseEnter(ev);
}

/**
   The mouse leaves the Widget.

   The @a ev contains the mouse coordinate outside the client bounds
   of the widgets (it is relative to client bounds, so you could obtain
   negative values for x and/or y).

   @win32
     This event is generated when @msdn{WM_MOUSELEAVE} message is received.
   @endwin32
*/
void Widget::onMouseLeave(MouseEvent& ev)
{
  MouseLeave(ev);
}

/**
   The mouse is inside the Widget and the user press a mouse's button.

   @win32
     This event is generated when @msdn{WM_LBUTTONDOWN},
     @msdn{WM_MBUTTONDOWN}, or @msdn{WM_RBUTTONDOWN} message is received.
   @endwin32
*/
void Widget::onMouseDown(MouseEvent& ev)
{
  MouseDown(ev);
}

/**
   The mouse is inside the Widget and the user release a mouse's
   button.

   @win32
     This event is generated when @msdn{WM_LBUTTONUP},
     @msdn{WM_MBUTTONUP}, or @msdn{WM_RBUTTONUP} message is received.
   @endwin32
*/
void Widget::onMouseUp(MouseEvent& ev)
{
  MouseUp(ev);
}

/**
   The mouse is moving inside the Widget's client area.

   If you capture the mouse, you should use the System#getCursorPos
   function to get the cursor position when it is outside the widget's
   client area.

   Overriding:
   @code
   class MyWidget : public Widget
   {
     ...
   protected:
     virtual void onMouseMove(MouseEvent& ev)
     {
       if (!ev.isConsumed() && (...this event is useful for us...)) {
	  ...
	  ev.consume();
       }
       Widget::onMouseMove(ev);
     }
   };
   @endcode
*/
void Widget::onMouseMove(MouseEvent& ev)
{
  MouseMove(ev);
}

/**
   The mouse is inside the Widget and the user spin the mouse's
   wheel.

   Overriding:
   @code
   class MyWidget : public Widget
   {
     ...
   protected:
     virtual void onMouseWheel(MouseEvent& ev)
     {
       if (!ev.isConsumed() && (...this event is useful for us...)) {
	  ...
	  ev.consume();
       }
       Widget::onMouseWheel(ev);
     }
   };
   @endcode

   @win32
     This event is generated when @msdn{WM_MOUSEWHEEL} message is received.
   @endwin32
*/
void Widget::onMouseWheel(MouseEvent& ev)
{
  MouseWheel(ev);
}

/**
   Called when the user does double-click over the widget.

   If the event is not consumed (by a slot in #DoubleClick signal or
   overriding this member function), the double-click will be converted
   to a #onMouseDown event.

   Overriding:
   @code
   class MyWidget : public Widget
   {
   protected:
     virtual void onDoubleClick(MouseEvent& ev)
     {
       if (!ev.isConsumed() && (...this event is useful for us...)) {
	  ...
	  ev.consume(); // <-- consume the event so it is
			//     not converted to MouseDown
       }
       Widget::onDoubleClick(ev);
     }
   };
   @endcode

   @see onMouseDown
*/
void Widget::onDoubleClick(MouseEvent& ev)
{
  // fire DoubleClick signal
  DoubleClick(ev);
}

/**
   Requests the mouse cursor to be used in the area
   specified by @a SetCursorEvent#getWidgetHit.

   This event is generated for everytime the mouse moves over the
   widget.

   @todo fix this behavior: If the widget captured the mouse, this event is not
   generated anymore until the capture is not released.
*/
void Widget::onSetCursor(SetCursorEvent& ev)
{
  if (!ev.isConsumed()) {
    if (!m_baseWndProc && ev.getWidgetHit() == WidgetHit::Client) {
      ev.setCursor(Cursor(SysCursor::Arrow));
    }
  }
}

/**
   Event generated when the user presses a key.

   @param ev
     Information about the pressed key.

   @win32
     If KeyEvent#getKeyCode is not 0 the received message was @msdn{WM_KEYDOWN},
     otherwise KeyEvent#getCharCode is not 0 and the message was
     @msdn{WM_CHAR}.
   @endwin32
*/
void Widget::onKeyDown(KeyEvent& ev)
{
  KeyDown(ev);
}

/**
   Event generated when the user releases a key.

   @param ev
     Information about the released key.

   @win32
     This event is generated when @msdn{WM_KEYUP} message is received.
   @endwin32
*/
void Widget::onKeyUp(KeyEvent& ev)
{
  KeyUp(ev);
}

/**
   Event generated when the widget gets the keyboard-focus.

   @win32
     This event is generated when @msdn{WM_SETFOCUS} message is received.
   @endwin32
*/
void Widget::onFocusEnter(FocusEvent& ev)
{
  FocusEnter(ev);
}

/**
   Event generated when the widget losts the keyboard-focus.

   @win32
     This event is generated when @msdn{WM_KILLFOCUS} message is received.
   @endwin32
*/
void Widget::onFocusLeave(FocusEvent& ev)
{
  FocusLeave(ev);
}

/**
   Event generated when a command by ID is activated by the user, this
   can be a menu item or an accelerator.

   @param commandId
       Identifier of the command that was activated.

   @return
       It should returns true if the @a commandId was used.

   @win32
     Do not confuse with #onReflectedCommand: onCommand is used
     to handle command notifications that come directly from
     accelarators or menus, not from Win32's
     controls. Notifications by Win32's controls are handled
     via onReflectedCommand, onReflectedNotify, or
     onReflectedDrawItem.
   @endwin32

   @see Command
*/
void Widget::onCommand(CommandEvent& ev)
{
  if (!ev.isConsumed()) {
    CommandId id = ev.getCommandId();

    if (Command* cmd = getCommandById(id)) {
      if (cmd->isEnabled()) {
	cmd->execute();
	ev.consume();
	return;
      }
    }

    if (getParent() != NULL) {
      getParent()->onCommand(ev);
    }
    else {
      // check if the application is a CommandsClient
      if (CommandsClient* cc = dynamic_cast<CommandsClient*>(Application::getInstance())) {
	if (Command* cmd = cc->getCommandById(id)) {
	  if (cmd->isEnabled()) {
	    cmd->execute();
	    ev.consume();
	  }
	}
      }
    }
  }
}

/**
   Event generated to update the state of indicators.

   @see updateIndicators
*/
void Widget::onUpdateIndicators()
{
  for (WidgetList::iterator
	 it=m_children.begin(); it!=m_children.end(); ++it) {
    Widget* child = *it;
    child->updateIndicators();
  }
}

/**
   Generated when the user move the scroll of this widget.
*/
void Widget::onScroll(ScrollEvent& ev)
{
  // do nothing
}

/**
   When the user drops files to the widget this event is generated.

   @warning This event is generated only if the widget has the
	    Widget::Styles::AcceptFiles style.

   @see Widget::Styles::AcceptFiles
*/
void Widget::onDropFiles(DropFilesEvent& ev)
{
  DropFiles(ev);
}

/**
   This member function can be used to handle command notifications
   (@msdn{WM_COMMAND}) reflected from the parent.

   @param id
     Identifier of the control or menu item.

   @param code
     Notification code.

   @param lResult
     Result to be returned by the #wndProc member function.

   @win32
     You should not confuse this member function with #onCommand. #onReflectedCommand
	 is used to handle commands that this widget by self generated, were sent to the
     parent, and finally were reflected to this widget again by the parent.
   @endwin32
*/
bool Widget::onReflectedCommand(int id, int code, LRESULT& lResult)
{
  return false;
}

/**
   This member function can be used to handle notifications (@msdn{WM_NOTIFY})
   reflected from the parent.

   @param lpnmhdr
     Information about the notification message.

   @param lResult
     Result to be returned by the #wndProc member function.

   @return
     False if it does not use the notification.
*/
bool Widget::onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult)
{
  return false;
}

/**
   Fired when the parent widget received the @msdn{WM_DRAWITEM} message.

   @param g
     Graphics context to draw the item (you can obtain the HDC from
     this argument).

   @param lpDrawItem
     Instance of the Win32 structure DRAWITEMSTRUCT to get information
     about the item to draw.

   @see onPaint
*/
bool Widget::onReflectedDrawItem(Graphics& g, LPDRAWITEMSTRUCT lpDrawItem)
{
  return false;
}

/**
   Adds a child to this widget.

   @param child
     The child to add inside the widget.

   @param setParent
     If it's true the Win32's SetParent will be used.

   @internal
*/
void Widget::addChildWin32(Widget* child, bool setParent)
{
  assert(::IsWindow(m_handle));
  assert(child != NULL);
  assert(child->m_handle != NULL);
  assert(child->m_parent == NULL);

  m_children.push_back(child);
  child->m_parent = this;

  if (setParent) {
    child->addStyle(Style(WS_CHILD, 0));
    ::SetParent(child->m_handle, m_handle);
    // sendMessage(WM_UPDATEUISTATE, UIS_SET..., 0);
  }
}

/**
   Removes a child from this widget.

   @param child
       The child that we want to remove from the parent.

   @param setParent
       If it's true the Win32's SetParent will be used.

   @internal
*/
void Widget::removeChildWin32(Widget* child, bool setParent)
{
  assert(::IsWindow(m_handle));
  assert(child != NULL);
  assert(child->m_handle != NULL);
  assert(child->m_parent == this);

  remove_from_container(m_children, child);

  if (setParent) {
    invalidate(child->getBounds(), true);
    child->removeStyle(Style(WS_VISIBLE, 0));

    ::SetParent(child->m_handle, NULL);
    child->removeStyle(Style(WS_CHILD, 0));
  }

  child->m_parent = NULL;
}

/**
   It creates the handle to be used in the Widget.

   The only way to customize this routine is overloading the Widget#createHandle
   member function, calling Widget#Widget with WidgetClassName#None as @c className,
   and finally calling Widget#create from the constructor of your own
   widget class (for example as MdiClient#MdiClient does).

   This member function @em "must be" called in one constructor (in Widget#Widget
   or in your own class if you used a WidgetClassName#None as @c className
   in Widget's constructor).

   @throw CreateWidgetException
     If the handle couldn't be created through the #createHandle member
	 function.

   @win32
     The widget handle is the @msdn{HWND}.
   @endwin32

   @see createHandle, @ref page_tn_002, #m_handle
*/
void Widget::create(const WidgetClassName& className, Widget* parent, Style style)
{
  assert(m_handle == NULL);
  assert(parent == NULL || parent->m_handle != NULL);

  // all parents must to have the WS_EX_CONTROLPARENT style to avoid
  // infinite loops in Win32's dialog boxes (for more information see
  // the 'src/msw/window.cpp' file of 'wxWidgets' library)
  if ((parent != NULL) &&
      (parent->getStyle().extended & WS_EX_CONTROLPARENT) != 0) {
    parent->addStyle(Style(0, WS_EX_CONTROLPARENT));
  }

  // fixup styles...
  // remove child style if you don't specify a parent
  if (parent == NULL && style.regular & WS_CHILD) {
    style.regular &= ~WS_CHILD;
  }
  else if (parent != NULL && !(style.regular & WS_CHILD)) {
    style.regular |= WS_CHILD;
  }

  // create the HWND handler
  {
    Widget* outsideWidget = CurrentThread::details::getOutsideWidget();
    assert(outsideWidget == NULL);

    CurrentThread::details::setOutsideWidget(this);
    m_handle = createHandle(className.c_str(), parent, style);
    CurrentThread::details::setOutsideWidget(NULL);
  }

  if (m_handle == NULL || !::IsWindow(m_handle))
    throw CreateWidgetException(format_string(L"Error creating widget of class \"%s\"",
					      className.c_str()));

  subClass();

  // add the widget to its parent
  if (parent != NULL)
    parent->addChildWin32(this, false);
}

/**
   @brief Does the classic Win32's subclassing replacing @msdn{GWLP_WNDPROC}.

   In the @msdn{GWLP_WNDPROC} property of all @msdn{HWND} is the @msdn{WNDPROC}
   procedure that is called each time a message is arrived/processed by the Win32's
   message-queue. So this member function changes that procedure by #globalWndProc
   to hook all messages that come from Windows.

   @see getGlobalWndProc, wndProc, @ref page_tn_002, #m_baseWndProc
*/
void Widget::subClass()
{
  assert(::IsWindow(m_handle));

  // set the GWL_WNDPROC to globalWndProc
  m_baseWndProc = reinterpret_cast<WNDPROC>
    (SetWindowLongPtr(m_handle,
		      GWLP_WNDPROC,
		      reinterpret_cast<LONG_PTR>(getGlobalWndProc())));
  if (m_baseWndProc == getGlobalWndProc())
    m_baseWndProc = NULL;

  // box the pointer...
  SetProp(m_handle, VACA_ATOM, reinterpret_cast<HANDLE>(this));

  // TODO get the font from the hwnd

  // set the default font of the widget
  setFont(m_font);
}

/**
   This member function creates the @msdn{HWND} handler for the Widget.

   It is called inside Widget#create, so if you overload
   #createHandle, you must to call the Widget's constructor with
   WidgetClassName#None as @c className, and then use
   Widget#create in your own class's constructor. An example
   is the MdiChild class, which overwrites the #createHandle member
   function, so it does something like the following code:
   @code
   MdiChild::MdiChild(const String& title,
		      MdiClient* parent,
		      Style style)
     // className=WidgetClassName::None so "create" isn't called
     : Frame(WidgetClassName::None, "", NULL, NoStyle)
   {
     ...
     // we call "create" manually here
     create(L"Vaca.MdiChild", parent, style);
   }

   // it is invoked from "create" from the MdiChild's constructor
   HWND MdiChild::createHandle(...) { ... }
   @endcode

   @see create, @ref page_tn_002
*/
HWND Widget::createHandle(LPCTSTR className, Widget* parent, Style style)
{
  return CreateWindowEx(style.extended, className, L"",
			style.regular,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			parent ? parent->getHandle(): (HWND)NULL,
			(HMENU)NULL,
			Application::getHandle(),
			reinterpret_cast<LPVOID>(this));
}

/**
   The customized Win32 window procedure for Vaca widgets.

   @win32
   This is called from Widget#globalWndProc. It should returns true when
   the #defWndProc does not need to be called.

   This member function is called to intercept any message after the
   creation of the widget, and before the destruction. To intercept
   messages outside that range (before creation, and after
   disposition), you should customize #defWndProc.

   This member function mainly converts a message to a event:
   <ul>
     <li><tt>WM_ERASEBKGND</tt> -&gt; Clears the background with #getBgColor.</li>
     <li><tt>WM_PAINT</tt> -&gt; Calls #onPaint event.</li>
     <li><tt>WM_SIZE</tt> -&gt; Calls #onResize event.</li>
     <li><tt>WM_SETCURSOR</tt> -&gt; Calls #onSetCursor event.</li>
     <li><tt>WM_*BUTTONDOWN</tt> -&gt; Calls #onMouseDown event.</li>
     <li><tt>WM_*BUTOTNUP</tt> -&gt; Calls #onMouseUp event.</li>
     <li><tt>WM_*BUTOTNDBLCLK</tt> -&gt; Calls #onDoubleClick event.</li>
     <li><tt>WM_MOUSEMOVE</tt> -&gt; Calls #onMouseMove event.</li>
     <li><tt>WM_MOUSEWHEEL</tt> -&gt; Calls #onMouseWheel event.</li>
     <li><tt>WM_MOUSELEAVE</tt> -&gt; Calls #onMouseLeave event.</li>
     <li><tt>WM_CHAR</tt> -&gt; Calls #onKeyDown event (use the KeyEvent#getCharCode).</li>
     <li><tt>WM_KEYDOWN</tt> -&gt; Calls #onKeyDown event (use the KeyEvent#getKeyCode).</li>
     <li><tt>WM_KEYUP</tt> -&gt; Calls #onKeyUp event.</li>
     <li><tt>WM_SETFOCUS</tt> -&gt; Calls #onFocusEnter event.</li>
     <li><tt>WM_KILLFOCUS</tt> -&gt; Calls #onFocusLeave event.</li>
     <li><tt>WM_CTLCOLOR*</tt> -&gt; Returns a HBRUSH with the #getBgColor.</li>
     <li><tt>WM_VSCROLL</tt> -&gt; Calls #onScroll event.</li>
     <li><tt>WM_HSCROLL</tt> -&gt; Calls #onScroll event.</li>
     <li><tt>WM_DROPFILES</tt> -&gt; Calls #onDropFiles event.</li>
   </ul>

   For reflection, it does:
   <ul>
   <li>When <tt>WM_COMMAND</tt> is received, the #onReflectedCommand event
       <b>of the child</b> is called (if it is a WM_COMMAND from a control),
       then the #onCommand() event <b>of this widget</b> (in any case/source: control,
       menu item, accelerator, etc.).</li>
   <li>When <tt>WM_NOTIFY</tt> is received, the #onReflectedNotify event <b>of the
       child</b> is called.</li>
   <li>When <tt>WM_DRAWITEM</tt> is received, the #onReflectedDrawItem event <b>of the
       child</b> is called.</li>
   </ul>

   How to extend widget#wndProc member function?

   @code
   class MyWidget : public Widget {
     ...
   protected:
     virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
     {
       if (Widget::wndProc(message, wParam, lParam, lResult))
	 return true;

       // ...Your code here...

       return false;
     }
     ...
   }
   @endcode

   @return
     True if the message was used and lResult contains a valid value to be
     returned by #globalWndProc.

   @see globalWndProc, getGlobalWndProc, defWndProc
   @endwin32
*/
bool Widget::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
  bool ret = false;

  switch (message) {

    case WM_ERASEBKGND:
      if (m_baseWndProc == NULL) {
	HDC hdc = reinterpret_cast<HDC>(wParam);
	// erase background only when the widget does not use
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
      // should not send WM_ERASEBKGND to the original WNDPROC
      else if (m_doubleBuffered) {
	lResult = TRUE;
	ret = true;
      }
      break;

    case WM_PAINT:
      // if this is not a wrapped widget (like BUTTON, EDIT, etc.)...
      if (m_baseWndProc == NULL) {
	// ...we have to paint its content through an explicit onPaint event

	PAINTSTRUCT ps;
	bool painted = false;
	HDC hdc = ::BeginPaint(m_handle, &ps);

	if (!::IsRectEmpty(&ps.rcPaint)) {
	  Graphics g(hdc);
	  painted = doPaint(g);
	}

	::EndPaint(m_handle, &ps);

	if (painted) {
	  lResult = TRUE;
	  ret = true;
	}
      }
      break;

    case WM_DRAWITEM: {
      LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
      Widget* child = Widget::fromHandle(lpDrawItem->hwndItem);
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
      ResizeEvent ev(this, Size(LOWORD(lParam), HIWORD(lParam)));
      onResize(ev);
      break;
    }

    case WM_SETCURSOR:
      if (hasMouseAbove()) {
	WidgetHit hitTest = WidgetHit::Error;
	switch (LOWORD(lParam)) {
	  case HTERROR: hitTest = WidgetHit::Error; break;
	  case HTTRANSPARENT: hitTest = WidgetHit::Transparent; break;
	  case HTNOWHERE: hitTest = WidgetHit::Nowhere; break;
	  case HTCLIENT: hitTest = WidgetHit::Client; break;
	  case HTCAPTION: hitTest = WidgetHit::Caption; break;
	  case HTSYSMENU: hitTest = WidgetHit::SystemMenu; break;
	    // case HTGROWBOX: hitTest = WidgetHit::Size; break;
	  case HTSIZE: hitTest = WidgetHit::Size; break;
	  case HTMENU: hitTest = WidgetHit::Menu; break;
	  case HTHSCROLL: hitTest = WidgetHit::HorizontalScroll; break;
	  case HTVSCROLL: hitTest = WidgetHit::VerticalScroll; break;
	  case HTMINBUTTON: hitTest = WidgetHit::MinimizeButton; break;
	  case HTMAXBUTTON: hitTest = WidgetHit::MaximizeButton; break;
	    //case HTREDUCE: hitTest = WidgetHit::MinimizeButton; break;
	    //case HTZOOM: hitTest = WidgetHit::MaximizeButton; break;
	  case HTLEFT: hitTest = WidgetHit::Left; break;
	  case HTRIGHT: hitTest = WidgetHit::Right; break;
	  case HTTOP: hitTest = WidgetHit::Top; break;
	  case HTTOPLEFT: hitTest = WidgetHit::TopLeft; break;
	  case HTTOPRIGHT: hitTest = WidgetHit::TopRight; break;
	  case HTBOTTOM: hitTest = WidgetHit::Bottom; break;
	  case HTBOTTOMLEFT: hitTest = WidgetHit::BottomLeft; break;
	  case HTBOTTOMRIGHT: hitTest = WidgetHit::BottomRight; break;
	  case HTBORDER: hitTest = WidgetHit::Border; break;
	  case HTOBJECT: hitTest = WidgetHit::Object; break;
	  case HTCLOSE: hitTest = WidgetHit::Close; break;
	  case HTHELP: hitTest = WidgetHit::Help; break;
	}

	Point pt(System::getCursorPos() - getAbsoluteClientBounds().getOrigin());
	SetCursorEvent ev(this, pt, hitTest);
	onSetCursor(ev);

	if (ev.isConsumed()) {
	  lResult = TRUE;
	  ret = true;
	}
      }
      break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN: {
      MouseEvent
	ev(this,					   // widget
	   convert_to<Point>(MAKEPOINTS(lParam)),	   // mouse position
	   1,						   // clicks
	   wParam,					   // flags
	   message == WM_LBUTTONDOWN ? MouseButton::Left:  // button
	   message == WM_RBUTTONDOWN ? MouseButton::Right:
	   message == WM_MBUTTONDOWN ? MouseButton::Middle:
				       MouseButton::None);

      onMouseDown(ev);
      break;
    }

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP: {
      MouseEvent
	ev(this,					 // widget
	   convert_to<Point>(MAKEPOINTS(lParam)),	 // mouse position
	   1,						 // clicks
	   wParam,					 // flags
	   message == WM_LBUTTONUP ? MouseButton::Left:  // button
	   message == WM_RBUTTONUP ? MouseButton::Right:
	   message == WM_MBUTTONUP ? MouseButton::Middle:
				     MouseButton::None);

      onMouseUp(ev);
      break;
    }

    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK: {
      MouseEvent
	ev(this,					     // widget
	   convert_to<Point>(MAKEPOINTS(lParam)),	     // mouse position
	   2,						     // clicks
	   wParam,					     // flags
	   message == WM_LBUTTONDBLCLK ? MouseButton::Left:  // button
	   message == WM_RBUTTONDBLCLK ? MouseButton::Right:
	   message == WM_MBUTTONDBLCLK ? MouseButton::Middle:
					 MouseButton::None);
      onDoubleClick(ev);

      // if the double-click was not consumed, we can convert it to
      // mouse-down event
      if (!ev.isConsumed())
	onMouseDown(ev);
      break;
    }

    case WM_MOUSEMOVE: {
      MouseEvent
	ev(this,				    // widget
	   convert_to<Point>(MAKEPOINTS(lParam)),   // mouse position
	   0,					    // clicks
	   wParam,				    // flags
	   MouseButton::None);			    // button

      if (!m_hasMouse) {
	m_hasMouse = true;
	onMouseEnter(ev);

	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = m_handle;
	_TrackMouseEvent(&tme);
      }

      onMouseMove(ev);

      // WM_SETCURSOR is not generated when we capture the mouse
      if (hasCapture()) {
	SetCursorEvent ev2(this, ev.getPoint(), WidgetHit::Client);
	onSetCursor(ev2);
      }
      break;
    }

    case WM_MOUSEWHEEL: {
      // the lParam specifies the coordinates of the cursor relative
      // to the screen (not to client area)
      Point clientOrigin = getAbsoluteClientBounds().getOrigin();
      MouseEvent
	ev(this,				   // widget
	   convert_to<Point>(MAKEPOINTS(lParam)) - clientOrigin, // mouse position
	   0,					   // clicks
	   LOWORD(wParam),			   // flags
	   MouseButton::None,			   // button
	   ((short)HIWORD(wParam)) / WHEEL_DELTA); // delta

      onMouseWheel(ev);
      break;
    }

    case WM_MOUSELEAVE: {
      Point clientOrigin = getAbsoluteClientBounds().getOrigin();
      MouseEvent
	ev(this,				   // widget
	   System::getCursorPos() - clientOrigin,  // pt
	   0,					   // clicks
	   0,					   // flags
	   MouseButton::None);			   // button

      m_hasMouse = false;
      onMouseLeave(ev);
      break;
    }

    case WM_KEYDOWN: {
      KeyEvent ev(this, Keys::fromMessageParams(wParam, lParam), 0);
      onKeyDown(ev);
      if (ev.isConsumed()) {
	lResult = false;
	ret = true;
      }
      break;
    }

    case WM_CHAR: {
      KeyEvent ev(this, 0, wParam);
      onKeyDown(ev);
      if (ev.isConsumed()) {
	lResult = false;
	ret = true;
      }
      break;
    }

    case WM_KEYUP: {
      KeyEvent ev(this,
		  Keys::fromMessageParams(wParam, lParam),
		  MapVirtualKey(wParam, 2));
      onKeyUp(ev);
      if (ev.isConsumed()) {
	lResult = false;
	ret = true;
      }
      break;
    }

    case WM_COMMAND: {
      if (reinterpret_cast<HWND>(lParam) != NULL) {
	HWND hwndCtrl = reinterpret_cast<HWND>(lParam);
	Widget* child = Widget::fromHandle(hwndCtrl);
	if (child != NULL) {
	  MakeWidgetRef ref(child);
	  ret = child->onReflectedCommand(LOWORD(wParam), // id
					  HIWORD(wParam), // code
					  lResult);
	}
      }

      CommandEvent ev(this, static_cast<CommandId>(LOWORD(wParam)));
      onCommand(ev);
      if (!ret && ev.isConsumed()) {
	// ...onCommand returns true when processed the command
	lResult = 0;		// processed
	ret = true;
      }
      break;
    }

    case WM_NOTIFY: {
      LPNMHDR lpnmhdr = reinterpret_cast<LPNMHDR>(lParam);
      Widget* child = Widget::fromHandle(lpnmhdr->hwndFrom);
      if (child != NULL) {
	MakeWidgetRef ref(child);
	ret = child->onReflectedNotify(lpnmhdr, lResult);
      }
      break;
    }

    case WM_SETFOCUS: {
      FocusEvent ev(this, wParam == 0 ? NULL: Widget::fromHandle((HWND)wParam), this);
      onFocusEnter(ev);
      break;
    }

    case WM_KILLFOCUS: {
      FocusEvent ev(this, this, wParam == 0 ? NULL: Widget::fromHandle((HWND)wParam));
      onFocusLeave(ev);
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
      Widget* child = Widget::fromHandle(hwndCtrl);
      if (child != NULL) {
	COLORREF fgColor = convert_to<COLORREF>(child->getFgColor());
	COLORREF bgColor = convert_to<COLORREF>(child->getBgColor());

	SetTextColor(hdc, fgColor);
	SetBkColor(hdc, bgColor);

	if (m_hbrush)
	  DeleteObject(m_hbrush);

	{
	  LOGBRUSH lb;

	  lb.lbStyle = BS_SOLID;
	  lb.lbColor = bgColor;
	  lb.lbHatch = 0;

	  m_hbrush = CreateBrushIndirect(&lb);
	}

	lResult = reinterpret_cast<LRESULT>(m_hbrush);
	ret = true;
      }
      break;
    }

    case WM_VSCROLL:
    case WM_HSCROLL:
      {
	Orientation orien = (message == WM_VSCROLL) ? Orientation::Vertical:
						      Orientation::Horizontal;
	ScrollRequest req;
	int fnBar = (orien == Orientation::Horizontal) ? SB_HORZ: SB_VERT;
	int pos;
	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	::GetScrollInfo(m_handle, fnBar, &si);

	// convert the scroll request code to ScrollRequest
	switch (LOWORD(wParam)) {
	  case SB_LINELEFT:
	    req = ScrollRequest::LineBackward;
	    pos = si.nPos - 1;
	    break;
	  case SB_LINERIGHT:
	    req = ScrollRequest::LineForward;
	    pos = si.nPos + 1;
	    break;
	  case SB_PAGELEFT:
	    req = ScrollRequest::PageBackward;
	    pos = si.nPos - si.nPage;
	    break;
	  case SB_PAGERIGHT:
	    req = ScrollRequest::PageForward;
	    pos = si.nPos + si.nPage;
	    break;
	  case SB_THUMBPOSITION:
	    req = ScrollRequest::BoxPosition;
	    pos = si.nPos;
	    break;
	  case SB_THUMBTRACK:
	    req = ScrollRequest::BoxTracking;
	    pos = si.nTrackPos;
	    break;
	  case SB_LEFT:
	    req = ScrollRequest::FullBackward;
	    pos = si.nMin;
	    break;
	  case SB_RIGHT:
	    req = ScrollRequest::FullForward;
	    pos = si.nMax;
	    break;
	  case SB_ENDSCROLL:
	    req = ScrollRequest::EndScroll;
	    pos = si.nPos;
	    break;
	}

	pos = clamp_value(pos,
			  si.nMin,
			  si.nMax - max_value(static_cast<int>(si.nPage) - 1, 0));

	// Note: onScroll() does not receive the nPos=HIWORD(wParam)
	// because it has a limit of 16-bits.  Instead you should use
	// GetScrollInfo to get the position of the scroll-bar, it has
	// the full 32 bits
	ScrollEvent ev(this, orien, req, pos);

	// reflect the message? (it is useful for the "Slider" widget)
	if (lParam != 0) {
	  Widget* child = Widget::fromHandle(reinterpret_cast<HWND>(lParam));
	  if (child != NULL) {
	    MakeWidgetRef ref(child);
	    child->onScroll(ev);
	  }
	}

	onScroll(ev);
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
	  Char* lpstr = new Char[length+1];
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
   Calls the default Win32's window procedure.

   The default window procedure is #m_baseWndProc if it is not @c NULL, or
   #m_defWndProc otherwise. Even if #m_defWndProc was changed with #setDefWndProc
   but #m_baseWndProc is not @c NULL, #m_defWndProc is completelly ignored anyway.

   @internal

   @see setDefWndProc, m_defWndProc, m_baseWndProc
*/
LRESULT Widget::defWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  if (m_baseWndProc != NULL)
    return CallWindowProc(m_baseWndProc, m_handle, message, wParam, lParam);
  else {
    assert(m_defWndProc != NULL);
    return m_defWndProc(m_handle, message, wParam, lParam);
  }
}

/**
   Paints the widgets calling the #onPaint event.

   This member function check the value of #m_doubleBuffered to do the
   double-buffering technique (draw in a Graphics of a temporary
   Image, and then copy its content to @a g).

   @param g Where to draw.

   @internal
*/
bool Widget::doPaint(Graphics& g)
{
  bool painted = false;

  // use double-buffering technique?
  if (m_doubleBuffered) {
    // get the clip bounds
    Rect clipBounds = g.getClipBounds();
    // is not it empty?
    if (!clipBounds.isEmpty()) {
      // create the image for double-buffering (of the size of
      // the clipping bounds)
      Image image(clipBounds.getSize(), g);
      // get the Graphics to draw in the image
      Graphics& imageG = image.getGraphics();
      // background brush
      Brush bgBrush(getBgColor());

      // setup clipping region
      Region clipRegion;
      g.getClipRegion(clipRegion);
      clipRegion.offset(-clipBounds.x, -clipBounds.y);
      imageG.setClipRegion(clipRegion);

      // special coordinates transformation (to make the "imageG"
      // graphics transparent to "onPaint" member function)
      SetViewportOrgEx(imageG.getHandle(), -clipBounds.x, -clipBounds.y, NULL);

      // clear the background of the image
      imageG.fillRect(bgBrush, clipBounds);

      // configure defaults
      imageG.setFont(getFont());

      // paint on imageG
      PaintEvent ev(this, imageG);
      onPaint(ev);
      painted = ev.isPainted();

      // restore the viewport origin (so drawImage works fine)
      SetViewportOrgEx(imageG.getHandle(), 0, 0, NULL);

      // bit transfer from image to graphics device
      g.drawImage(image, clipBounds.getOrigin());
    }
  }
  // draw directly to the screen
  else {
    // configure defaults
    g.setFont(getFont());

    // paint on g
    PaintEvent ev(this, g);
    onPaint(ev);
    painted = ev.isPainted();
  }

  return painted;
}

/**
   Sets the default procedure to be called by #defWndProc when a
   message is not processed.

   @internal

   @see defWndProc, m_defWndProc
*/
void Widget::setDefWndProc(WNDPROC proc)
{
  m_defWndProc = proc;
}

/**
   Sets the destroy-procedure associated with this widget.

   @win32
     Each widget has a procedure to be called when it is destroyed, it is
     the last procedure to be called in #~Widget() destructor. By
     default it is @msdn{DestroyWindow}.
   @endwin32

   @see ~Widget
*/
void Widget::setDestroyHandleProc(void (*proc)(HWND))
{
  m_destroyHandleProc = proc;
}

/**
   Sends a message to the widget.

   @win32
     It uses the @msdn{SendMessage} routine to send a message
     to the wrapped @msdn{HWND}.
   @endwin32
*/
LRESULT Widget::sendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
  assert(::IsWindow(m_handle));
  return ::SendMessage(m_handle, message, wParam, lParam);
}

/**
   The global procedure for Win32 used in all registered WNDCLASSes.

   It is unique goal is to get the Widget pointer from HWND
   using #fromHandle, and to call its #wndProc member function.
*/
LRESULT CALLBACK Widget::globalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Widget* widget = Widget::fromHandle(hwnd);

#ifdef REPORT_MESSAGES
  String preString =
    format_string(L"Message %d for %p", msg,
		  (widget != NULL ? widget: CurrentThread::details::getOutsideWidget()));

  String msgString = L"Unknown";
  switch (msg) {
    case WM_ACTIVATE: msgString = L"WM_ACTIVATE"; break;
    case WM_ACTIVATEAPP: msgString = L"WM_ACTIVATEAPP"; break;
    case WM_APP: msgString = L"WM_APP"; break;
    case WM_ASKCBFORMATNAME: msgString = L"WM_ASKCBFORMATNAME"; break;
    case WM_CANCELJOURNAL: msgString = L"WM_CANCELJOURNAL"; break;
    case WM_CANCELMODE: msgString = L"WM_CANCELMODE"; break;
    case WM_CAPTURECHANGED: msgString = L"WM_CAPTURECHANGED"; break;
    case WM_CHANGECBCHAIN: msgString = L"WM_CHANGECBCHAIN"; break;
    case WM_CHAR: msgString = L"WM_CHAR"; break;
    case WM_CHARTOITEM: msgString = L"WM_CHARTOITEM"; break;
    case WM_CHILDACTIVATE: msgString = L"WM_CHILDACTIVATE"; break;
    case WM_CLEAR: msgString = L"WM_CLEAR"; break;
    case WM_CLOSE: msgString = L"WM_CLOSE"; break;
    case WM_COMMAND: msgString = L"WM_COMMAND"; break;
    case WM_COMMNOTIFY: msgString = L"WM_COMMNOTIFY"; break;
    case WM_COMPACTING: msgString = L"WM_COMPACTING"; break;
    case WM_COMPAREITEM: msgString = L"WM_COMPAREITEM"; break;
    case WM_CONTEXTMENU: msgString = L"WM_CONTEXTMENU"; break;
    case WM_COPY: msgString = L"WM_COPY"; break;
    case WM_COPYDATA: msgString = L"WM_COPYDATA"; break;
    case WM_CREATE: msgString = L"WM_CREATE"; break;
    case WM_CTLCOLORBTN: msgString = L"WM_CTLCOLORBTN"; break;
    case WM_CTLCOLORDLG: msgString = L"WM_CTLCOLORDLG"; break;
    case WM_CTLCOLOREDIT: msgString = L"WM_CTLCOLOREDIT"; break;
    case WM_CTLCOLORLISTBOX: msgString = L"WM_CTLCOLORLISTBOX"; break;
    case WM_CTLCOLORMSGBOX: msgString = L"WM_CTLCOLORMSGBOX"; break;
    case WM_CTLCOLORSCROLLBAR: msgString = L"WM_CTLCOLORSCROLLBAR"; break;
    case WM_CTLCOLORSTATIC: msgString = L"WM_CTLCOLORSTATIC"; break;
    case WM_CUT: msgString = L"WM_CUT"; break;
    case WM_DEADCHAR: msgString = L"WM_DEADCHAR"; break;
    case WM_DELETEITEM: msgString = L"WM_DELETEITEM"; break;
    case WM_DESTROY: msgString = L"WM_DESTROY"; break;
    case WM_DESTROYCLIPBOARD: msgString = L"WM_DESTROYCLIPBOARD"; break;
    case WM_DEVICECHANGE: msgString = L"WM_DEVICECHANGE"; break;
    case WM_DEVMODECHANGE: msgString = L"WM_DEVMODECHANGE"; break;
    case WM_DISPLAYCHANGE: msgString = L"WM_DISPLAYCHANGE"; break;
    case WM_DRAWCLIPBOARD: msgString = L"WM_DRAWCLIPBOARD"; break;
    case WM_DRAWITEM: msgString = L"WM_DRAWITEM"; break;
    case WM_DROPFILES: msgString = L"WM_DROPFILES"; break;
    case WM_ENABLE: msgString = L"WM_ENABLE"; break;
    case WM_ENDSESSION: msgString = L"WM_ENDSESSION"; break;
    case WM_ENTERIDLE: msgString = L"WM_ENTERIDLE"; break;
    case WM_ENTERMENULOOP: msgString = L"WM_ENTERMENULOOP"; break;
    case WM_ENTERSIZEMOVE: msgString = L"WM_ENTERSIZEMOVE"; break;
    case WM_ERASEBKGND: msgString = L"WM_ERASEBKGND"; break;
    case WM_EXITMENULOOP: msgString = L"WM_EXITMENULOOP"; break;
    case WM_EXITSIZEMOVE: msgString = L"WM_EXITSIZEMOVE"; break;
    case WM_FONTCHANGE: msgString = L"WM_FONTCHANGE"; break;
    case WM_GETDLGCODE: msgString = L"WM_GETDLGCODE"; break;
    case WM_GETFONT: msgString = L"WM_GETFONT"; break;
    case WM_GETHOTKEY: msgString = L"WM_GETHOTKEY"; break;
    case WM_GETICON: msgString = L"WM_GETICON"; break;
    case WM_GETMINMAXINFO: msgString = L"WM_GETMINMAXINFO"; break;
    case WM_GETTEXT: msgString = L"WM_GETTEXT"; break;
    case WM_GETTEXTLENGTH: msgString = L"WM_GETTEXTLENGTH"; break;
    case WM_HELP: msgString = L"WM_HELP"; break;
    case WM_HOTKEY: msgString = L"WM_HOTKEY"; break;
    case WM_HSCROLL: msgString = L"WM_HSCROLL"; break;
    case WM_HSCROLLCLIPBOARD: msgString = L"WM_HSCROLLCLIPBOARD"; break;
    case WM_ICONERASEBKGND: msgString = L"WM_ICONERASEBKGND"; break;
    case WM_INITDIALOG: msgString = L"WM_INITDIALOG"; break;
    case WM_INITMENU: msgString = L"WM_INITMENU"; break;
    case WM_INITMENUPOPUP: msgString = L"WM_INITMENUPOPUP"; break;
    case WM_INPUTLANGCHANGE: msgString = L"WM_INPUTLANGCHANGE"; break;
    case WM_INPUTLANGCHANGEREQUEST: msgString = L"WM_INPUTLANGCHANGEREQUEST"; break;
    case WM_KEYDOWN: msgString = L"WM_KEYDOWN"; break;
    case WM_KEYUP: msgString = L"WM_KEYUP"; break;
    case WM_KILLFOCUS: msgString = L"WM_KILLFOCUS"; break;
    case WM_LBUTTONDBLCLK: msgString = L"WM_LBUTTONDBLCLK"; break;
    case WM_LBUTTONDOWN: msgString = L"WM_LBUTTONDOWN"; break;
    case WM_LBUTTONUP: msgString = L"WM_LBUTTONUP"; break;
    case WM_MBUTTONDBLCLK: msgString = L"WM_MBUTTONDBLCLK"; break;
    case WM_MBUTTONDOWN: msgString = L"WM_MBUTTONDOWN"; break;
    case WM_MBUTTONUP: msgString = L"WM_MBUTTONUP"; break;
    case WM_MDIACTIVATE: msgString = L"WM_MDIACTIVATE"; break;
    case WM_MDICASCADE: msgString = L"WM_MDICASCADE"; break;
    case WM_MDICREATE: msgString = L"WM_MDICREATE"; break;
    case WM_MDIDESTROY: msgString = L"WM_MDIDESTROY"; break;
    case WM_MDIGETACTIVE: msgString = L"WM_MDIGETACTIVE"; break;
    case WM_MDIICONARRANGE: msgString = L"WM_MDIICONARRANGE"; break;
    case WM_MDIMAXIMIZE: msgString = L"WM_MDIMAXIMIZE"; break;
    case WM_MDINEXT: msgString = L"WM_MDINEXT"; break;
    case WM_MDIREFRESHMENU: msgString = L"WM_MDIREFRESHMENU"; break;
    case WM_MDIRESTORE: msgString = L"WM_MDIRESTORE"; break;
    case WM_MDISETMENU: msgString = L"WM_MDISETMENU"; break;
    case WM_MDITILE: msgString = L"WM_MDITILE"; break;
    case WM_MEASUREITEM: msgString = L"WM_MEASUREITEM"; break;
    case WM_MENUCHAR: msgString = L"WM_MENUCHAR"; break;
    case WM_MENUCOMMAND: msgString = L"WM_MENUCOMMAND"; break;
    case WM_MENUDRAG: msgString = L"WM_MENUDRAG"; break;
    case WM_MENUGETOBJECT: msgString = L"WM_MENUGETOBJECT"; break;
    case WM_MENURBUTTONUP: msgString = L"WM_MENURBUTTONUP"; break;
    case WM_MENUSELECT: msgString = L"WM_MENUSELECT"; break;
    case WM_MOUSEACTIVATE: msgString = L"WM_MOUSEACTIVATE"; break;
    case WM_MOUSEHOVER: msgString = L"WM_MOUSEHOVER"; break;
    case WM_MOUSELEAVE: msgString = L"WM_MOUSELEAVE"; break;
    case WM_MOUSEMOVE: msgString = L"WM_MOUSEMOVE"; break;
    case WM_MOUSEWHEEL: msgString = L"WM_MOUSEWHEEL"; break;
    case WM_MOVE: msgString = L"WM_MOVE"; break;
    case WM_MOVING: msgString = L"WM_MOVING"; break;
    case WM_NCACTIVATE: msgString = L"WM_NCACTIVATE"; break;
    case WM_NCCALCSIZE: msgString = L"WM_NCCALCSIZE"; break;
    case WM_NCCREATE: msgString = L"WM_NCCREATE"; break;
    case WM_NCDESTROY: msgString = L"WM_NCDESTROY"; break;
    case WM_NCHITTEST: msgString = L"WM_NCHITTEST"; break;
    case WM_NCLBUTTONDBLCLK: msgString = L"WM_NCLBUTTONDBLCLK"; break;
    case WM_NCLBUTTONDOWN: msgString = L"WM_NCLBUTTONDOWN"; break;
    case WM_NCLBUTTONUP: msgString = L"WM_NCLBUTTONUP"; break;
    case WM_NCMBUTTONDBLCLK: msgString = L"WM_NCMBUTTONDBLCLK"; break;
    case WM_NCMBUTTONDOWN: msgString = L"WM_NCMBUTTONDOWN"; break;
    case WM_NCMBUTTONUP: msgString = L"WM_NCMBUTTONUP"; break;
    case WM_NCMOUSEHOVER: msgString = L"WM_NCMOUSEHOVER"; break;
    case WM_NCMOUSELEAVE: msgString = L"WM_NCMOUSELEAVE"; break;
    case WM_NCMOUSEMOVE: msgString = L"WM_NCMOUSEMOVE"; break;
    case WM_NCPAINT: msgString = L"WM_NCPAINT"; break;
    case WM_NCRBUTTONDBLCLK: msgString = L"WM_NCRBUTTONDBLCLK"; break;
    case WM_NCRBUTTONDOWN: msgString = L"WM_NCRBUTTONDOWN"; break;
    case WM_NCRBUTTONUP: msgString = L"WM_NCRBUTTONUP"; break;
    case WM_NCXBUTTONDBLCLK: msgString = L"WM_NCXBUTTONDBLCLK"; break;
    case WM_NCXBUTTONDOWN: msgString = L"WM_NCXBUTTONDOWN"; break;
    case WM_NCXBUTTONUP: msgString = L"WM_NCXBUTTONUP"; break;
    case WM_NEXTDLGCTL: msgString = L"WM_NEXTDLGCTL"; break;
    case WM_NEXTMENU: msgString = L"WM_NEXTMENU"; break;
    case WM_NOTIFY: msgString = L"WM_NOTIFY"; break;
    case WM_NOTIFYFORMAT: msgString = L"WM_NOTIFYFORMAT"; break;
    case WM_NULL: msgString = L"WM_NULL"; break;
    case WM_PAINT: msgString = L"WM_PAINT"; break;
    case WM_PAINTCLIPBOARD: msgString = L"WM_PAINTCLIPBOARD"; break;
    case WM_PAINTICON: msgString = L"WM_PAINTICON"; break;
    case WM_PALETTECHANGED: msgString = L"WM_PALETTECHANGED"; break;
    case WM_PALETTEISCHANGING: msgString = L"WM_PALETTEISCHANGING"; break;
    case WM_PARENTNOTIFY: msgString = L"WM_PARENTNOTIFY"; break;
    case WM_PASTE: msgString = L"WM_PASTE"; break;
    case WM_POWER: msgString = L"WM_POWER"; break;
    case WM_POWERBROADCAST: msgString = L"WM_POWERBROADCAST"; break;
    case WM_PRINT: msgString = L"WM_PRINT"; break;
    case WM_PRINTCLIENT: msgString = L"WM_PRINTCLIENT"; break;
    case WM_QUERYDRAGICON: msgString = L"WM_QUERYDRAGICON"; break;
    case WM_QUERYENDSESSION: msgString = L"WM_QUERYENDSESSION"; break;
    case WM_QUERYNEWPALETTE: msgString = L"WM_QUERYNEWPALETTE"; break;
    case WM_QUERYOPEN: msgString = L"WM_QUERYOPEN"; break;
    case WM_QUEUESYNC: msgString = L"WM_QUEUESYNC"; break;
    case WM_QUIT: msgString = L"WM_QUIT"; break;
    case WM_RBUTTONDBLCLK: msgString = L"WM_RBUTTONDBLCLK"; break;
    case WM_RBUTTONDOWN: msgString = L"WM_RBUTTONDOWN"; break;
    case WM_RBUTTONUP: msgString = L"WM_RBUTTONUP"; break;
    case WM_RENDERALLFORMATS: msgString = L"WM_RENDERALLFORMATS"; break;
    case WM_RENDERFORMAT: msgString = L"WM_RENDERFORMAT"; break;
    case WM_SETCURSOR: msgString = L"WM_SETCURSOR"; break;
    case WM_SETFOCUS: msgString = L"WM_SETFOCUS"; break;
    case WM_SETFONT: msgString = L"WM_SETFONT"; break;
    case WM_SETHOTKEY: msgString = L"WM_SETHOTKEY"; break;
    case WM_SETICON: msgString = L"WM_SETICON"; break;
    case WM_SETREDRAW: msgString = L"WM_SETREDRAW"; break;
    case WM_SETTEXT: msgString = L"WM_SETTEXT"; break;
    case WM_SETTINGCHANGE: msgString = L"WM_SETTINGCHANGE"; break;
    case WM_SHOWWINDOW: msgString = L"WM_SHOWWINDOW"; break;
    case WM_SIZE: msgString = L"WM_SIZE"; break;
    case WM_SIZECLIPBOARD: msgString = L"WM_SIZECLIPBOARD"; break;
    case WM_SIZING: msgString = L"WM_SIZING"; break;
    case WM_SPOOLERSTATUS: msgString = L"WM_SPOOLERSTATUS"; break;
    case WM_STYLECHANGED: msgString = L"WM_STYLECHANGED"; break;
    case WM_STYLECHANGING: msgString = L"WM_STYLECHANGING"; break;
    case WM_SYNCPAINT: msgString = L"WM_SYNCPAINT"; break;
    case WM_SYSCHAR: msgString = L"WM_SYSCHAR"; break;
    case WM_SYSCOLORCHANGE: msgString = L"WM_SYSCOLORCHANGE"; break;
    case WM_SYSCOMMAND: msgString = L"WM_SYSCOMMAND"; break;
    case WM_SYSDEADCHAR: msgString = L"WM_SYSDEADCHAR"; break;
    case WM_SYSKEYDOWN: msgString = L"WM_SYSKEYDOWN"; break;
    case WM_SYSKEYUP: msgString = L"WM_SYSKEYUP"; break;
    case WM_TCARD: msgString = L"WM_TCARD"; break;
      // case WM_THEMECHANGED: msgString = L"WM_THEMECHANGED"; break;
    case WM_TIMECHANGE: msgString = L"WM_TIMECHANGE"; break;
    case WM_TIMER: msgString = L"WM_TIMER"; break;
    case WM_UNDO: msgString = L"WM_UNDO"; break;
    case WM_UNINITMENUPOPUP: msgString = L"WM_UNINITMENUPOPUP"; break;
    case WM_USER: msgString = L"WM_USER"; break;
    case WM_USERCHANGED: msgString = L"WM_USERCHANGED"; break;
    case WM_VKEYTOITEM: msgString = L"WM_VKEYTOITEM"; break;
    case WM_VSCROLL: msgString = L"WM_VSCROLL"; break;
    case WM_VSCROLLCLIPBOARD: msgString = L"WM_VSCROLLCLIPBOARD"; break;
    case WM_WINDOWPOSCHANGED: msgString = L"WM_WINDOWPOSCHANGED"; break;
    case WM_WINDOWPOSCHANGING: msgString = L"WM_WINDOWPOSCHANGING"; break;
    case WM_XBUTTONDBLCLK: msgString = L"WM_XBUTTONDBLCLK"; break;
    case WM_XBUTTONDOWN: msgString = L"WM_XBUTTONDOWN"; break;
    case WM_XBUTTONUP: msgString = L"WM_XBUTTONUP"; break;
  }
  VACA_TRACE(convert_to<std::string>(preString + L" ("+ msgString + L")\n").c_str());
#endif

  if (widget != NULL) {
    LRESULT lResult;
    bool used = false;

    MakeWidgetRef ref(widget);

    // window procedures
    used = widget->wndProc(msg, wParam, lParam, lResult);
    if (!used)
      lResult = widget->defWndProc(msg, wParam, lParam);

    return lResult;
  }
  else {
    widget = CurrentThread::details::getOutsideWidget();
    if (widget != NULL) {
      assert(hwnd != NULL);
      widget->m_handle = hwnd;
      return widget->defWndProc(msg, wParam, lParam);
    }
  }

  // never should be here...

  VACA_TRACE("------------ LOST MESSAGE: %p %d %d %d ---------------\n", hwnd, msg, wParam, lParam);
  Beep(900, 100);

  return FALSE;
}

// ============================================================
// REFERENCES
// ============================================================

/**
   Increments the reference-counter of @a widget and all the parents, filling
   the #m_container with all the widgets referenced.
*/
MakeWidgetRef::MakeWidgetRef(Widget* widget)
{
  while (widget != NULL) {
    m_container.push_back(widget);
    widget->ref();
    widget = widget->getParent();
  }
}

/**
   Decrements the reference-counter of all the widget in #m_container.
*/
MakeWidgetRef::~MakeWidgetRef()
{
  for (std::vector<Widget*>::iterator
	 it=m_container.begin(); it!=m_container.end(); ++it) {
    (*it)->unref();
    safeDelete(*it);
  }
}

/**
   Checks if @a widget should be deleted right now, and if it is, deletes it.
   Do not use this function, it is an internal routine that uses Vaca.

   @internal
*/
void MakeWidgetRef::safeDelete(Widget* widget)
{
  // is "delete-after-event" flag activated and is the widget unreferenced?
  if (widget->m_deleteAfterEvent && widget->getRefCount() == 0) {
    // ok, so we can delete it...
    widget->m_deleteAfterEvent = false;
    delete widget;
  }
}

/**
   Safe way to delete a widget from memory.

   It deletes the specified widget if it is not referenced, or defer
   its deletion for a secure point of deletion (e.g. when it is
   completelly unreferenced after an event is processed).

   @see @ref page_tn_006
*/
void vaca::delete_widget(Widget* widget)
{
  // is unreferenced?
  if (widget->getRefCount() == 0)
    delete widget;
  // is referenced, we defer the delete operation for "safeDelete"
  else {
    assert(widget->getRefCount() > 0);

    // hide the window
    ShowWindow(widget->m_handle, SW_HIDE);

    // delete after event flag
    widget->m_deleteAfterEvent = true;
  }
}
