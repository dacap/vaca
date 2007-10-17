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

#include "stdvaca.h"
#include "Vaca/Widget.h"
#include "Vaca/Point.h"
#include "Vaca/Debug.h"
#include "Vaca/Frame.h"
#include "Vaca/Layout.h"
#include "Vaca/Constraint.h"
#include "Vaca/System.h"
#include "Vaca/Font.h"
#include "Vaca/Thread.h"
#include "Vaca/Mutex.h"
#include "Vaca/Cursor.h"
#include "Vaca/MouseEvent.h"
#include "Vaca/KeyEvent.h"
#include "Vaca/DropFilesEvent.h"
#include "Vaca/Image.h"

// comment this to use the old behaviour (using GWL_USERDATA to store
// the "Widget" pointer)
#define USE_PROP

// uncomment this if you want message reporting in the "vaca.log"
// #define REPORT_MESSAGES

using namespace Vaca;

static Mutex outsideWidgetMutex;     // used to access outsideWidget
static Widget *outsideWidget = NULL; // widget used to call createHWND

#ifdef USE_PROP
#define VACAATOM (reinterpret_cast<LPCTSTR>(MAKELPARAM(vacaAtom, 0)))

static Mutex vacaAtomMutex; // used to access vacaAtom
static ATOM vacaAtom = 0;
#endif

/**
 * Creates a new widget with the specified @a className.
 *
 * You can specify a NULL @a className, if you want to call create()
 * method by your self (@ref TN002).
 *
 * You can't create widgets from a HWND, so you can't treat an
 * existent HWND like a Widget.
 *
 * @param className Must be a registered WNDCLASS for Windows.
 * @param parent It's a Widget (generally a Frame) that will contain this widget.
 * @param style A set of @c WS_ and @c WS_EX values.
 */
Widget::Widget(LPCTSTR className, Widget *parent, Style style)
{
  vacaAtomMutex.lock();
  if (vacaAtom == 0) {
    vacaAtom = GlobalAddAtom(_T("VacaAtom"));
    assert(vacaAtom != 0);
  }
  vacaAtomMutex.unlock();

  mHWND = NULL;
  mParent = NULL;
  mFgColor = System::getColor(COLOR_WINDOWTEXT);
  mBgColor = System::getColor(COLOR_3DFACE);
  mConstraint = NULL;
  mLayout = NULL;
  mSuperWndProc = NULL;
  mDisposed = false;
  mHasMouse = false;
  mDeleteAfterEvent = false;
  mDoubleBuffering = false;
  mCriticalInner = 0;
  mFont = NULL;
//   mHbrush = NULL;

  if (className != NULL)
    create(className, parent, style);
}

/**
 * Deletes the widget. It calls the dispose() method to destroy the
 * widget if it wasn't destroyed yet.
 *
 * @see @ref TN002
 */
Widget::~Widget()
{
//   if (mHbrush != NULL)
//     DeleteObject(mHbrush);

  dispose();
}

/**
 * Returns the parent of the widget. Generally, the parent will be a
 * Frame window. This method doesn't use the GetParent routine of Win32.
 */
Widget *Widget::getParent()
{
  return mParent;
}

/**
 * Returns the collection of children.
 */
Widget::Container Widget::getChildren()
{
  return mChildren;
}

/**
 * Returns the current Layout (Widget::mLayout) that arranges the
 * widget's children (Widget::mChildren). For most widgets, generally
 * it'll NULL, but for a Frame you should use setLayout to change the
 * Layout manager.
 *
 * @warning You can't delete the returned pointer (use "delete setLayout(NULL)" instead).
 *
 * @see setLayout, getConstraint, @ref TN010
 */
Layout *Widget::getLayout()
{
  return mLayout;
}

/**
 * Changes the current Layout (Widget::mLayout) to arrange widget's
 * children (Widget::mChildren). The @a layout pointer'll be deleted
 * automatically by dipose() method. If you change the Layout, you
 * must delete the old pointer returned.
 *
 * @code
 * {
 *   {
 *     Frame frame(...);
 *     frame.setLayout(new MyLayout());
 *     delete frame.setLayout(new MyBetterLayout());
 *   }
 *   // here MyBetterLayout doesn't exist (was automatically deleted)
 * }
 * @endcode
 *
 * Another example:
 *
 * @code
 * {
 *   Layout myLayout;
 *   Frame frame(...);
 *   frame.setLayout(&myLayout);
 *   frame.setLayout(NULL); // <-- very important!!! to avoid double deletion
 * }
 * @endcode
 *
 * @return The old Layout manager (you should delete it).
 *
 * @see getLayout, setConstraint, @ref TN010
 */
Layout *Widget::setLayout(Layout *layout)
{
  Layout *oldLayout = mLayout;
  mLayout = layout;
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
Constraint *Widget::getConstraint()
{
  return mConstraint;
}

/**
 * Modifies the widget's constraint. The "constraint" pointer'll be
 * deleted automatically by the Widget::~Widget destructor. If you
 * change the Constraint, you must delete the old pointer returned
 * (it's the same behavior of setLayout()).
 *
 * @see getConstraint, setLayout, @ref TN010
 */
Constraint *Widget::setConstraint(Constraint *constraint)
{
  Constraint *oldConstraint = mConstraint;
  mConstraint = constraint;
  return oldConstraint;
}

/**
 * Returns the widget's text, label, or frame's title. It uses
 * GetWindowTextLength and GetWindowText functions of Win32.
 *
 * @see setText
 */
String Widget::getText()
{
  assert(mHWND != NULL);

  int len = ::GetWindowTextLength(mHWND);
  if (!len)
    return String("");
  else {
    LPTSTR buf = (LPTSTR)new _TCHAR[len+1];
    ::GetWindowText(mHWND, buf, len+1);
    String str = String(buf);
    delete buf;
    return str;
  }
}

/**
 * Changes the widget's text, label, or frame's title. It uses the
 * SetWindowText of Win32.
 */
void Widget::setText(const String &str)
{
  assert(mHWND != NULL);
  ::SetWindowText(mHWND, str.c_str());
}

/**
 * Returns the current font used to paint the Widget. If you don't use
 * Widget::setFont, the default font is used (Font::getDefault).
 */
Font &Widget::getFont()
{
  return *mFont;
}

/**
 * Sets the font of the widget. The font should be the same life cycle
 * of the Widget. The font'll not be automatically deleted. If you use
 * Font::assign or Font::operator= methods for @a font, remember to
 * recall this routine with the same @a font.
 *
 * @see Font::assign, Font::operator=
 */
void Widget::setFont(Font &font)
{
  mFont = &font;
  sendMessage(WM_SETFONT, reinterpret_cast<WPARAM>(mFont->getHFONT()), TRUE);
}

/**
 * Returns the current Widget style.
 */
Style Widget::getStyle()
{
  assert(mHWND != NULL);

  return Style(GetWindowLong(mHWND, GWL_STYLE),
	       GetWindowLong(mHWND, GWL_EXSTYLE));
}

/**
 * Replaces all the styles of the Widget with the new ones specified
 * in @a style parameter.
 */
void Widget::setStyle(Style style)
{
  assert(mHWND != NULL);

  SetWindowLong(mHWND, GWL_STYLE, style.regular);
  SetWindowLong(mHWND, GWL_EXSTYLE, style.extended);
}

/**
 * Adds styles to the widget.
 */
void Widget::addStyle(Style style)
{
  assert(mHWND != NULL);

  setStyle(getStyle() + style);
}

/**
 * Removes styles from the widget.
 */
void Widget::removeStyle(Style style)
{
  assert(mHWND != NULL);

  setStyle(getStyle() - style);
}


bool Widget::getDoubleBuffering()
{
  return mDoubleBuffering;
}

void Widget::setDoubleBuffering(bool useDoubleBuffering)
{
  mDoubleBuffering = useDoubleBuffering;
}

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
  assert(mHWND != NULL);

  RECT rc;
  ::GetWindowRect(mHWND, &rc);

  if (mParent != NULL &&
      (GetWindowLong(mHWND, GWL_STYLE) & WS_CAPTION) == 0) {
    POINT pt1 = Point(rc.left, rc.top);
    POINT pt2 = Point(rc.right, rc.bottom);

    ::ScreenToClient(mParent->mHWND, &pt1);
    ::ScreenToClient(mParent->mHWND, &pt2);

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
  assert(mHWND != NULL);

  RECT rc;
  ::GetWindowRect(mHWND, &rc);

  return Rect(&rc);
}

/**
 * Gets the client bounds. This is like GetClientRect for most
 * cases.
 *
 * @warning The top-left corner not always is (0,0), see the reimplementations.
 *
 * @see getBounds, getAbsoluteClientBounds
 */
Rect Widget::getClientBounds()
{
  RECT rc;
  assert(mHWND != NULL);
  ::GetClientRect(mHWND, &rc);
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
  ::MapWindowPoints(mHWND, NULL, (POINT *)&rc, 2);
  return Rect(&rc);
}

/**
 * Returns the area where the Layout must arrange the collection of
 * children.
 *
 * @see getClientBounds
 */
Rect Widget::getLayoutBounds()
{
  return getClientBounds();
}

/**
 * Sets the boundary rectangle for the widget.
 *
 * @see getBounds
 */
void Widget::setBounds(const Rect &rc)
{
  assert(mHWND != NULL);

  /* TODO remove this
  ::SetWindowPos(mHWND, NULL,
		 rc.left, rc.top, rc.getWidth(), rc.getHeight(),
		 SWP_NOZORDER | SWP_NOACTIVATE);
  */

  ::MoveWindow(mHWND, rc.x, rc.y, rc.w, rc.h, TRUE);
}

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
void Widget::setOrigin(const Point &pt)
{
  Size sz = getBounds().getSize();

  setBounds(Rect(pt, sz));
}

void Widget::setOrigin(int x, int y)
{
  setOrigin(Point(x, y));
}

/**
 * Sets the size of the widget. It doesn't affect the origin position.
 *
 * @see setSize(int,int)
 */
void Widget::setSize(const Size &sz)
{
  Point pt = getBounds().getOrigin();
  
  setBounds(Rect(pt, sz));
}

void Widget::setSize(int w, int h)
{
  setSize(Size(w, h));
}

void Widget::validate()
{
  assert(mHWND != NULL);
  ValidateRect(mHWND, NULL);
}

void Widget::validate(const Rect &rc)
{
  assert(mHWND != NULL);

  RECT rc2 = rc;
  ValidateRect(mHWND, &rc2);
}

/**
 * Invalidates the entire client area.
 *
 * @param eraseBg true means that the background should be erased
 *        (with a WM_ERASEBKGND message).
 */
void Widget::invalidate(bool eraseBg)
{
  assert(mHWND != NULL);
  InvalidateRect(mHWND, NULL, eraseBg);
}

void Widget::invalidate(const Rect &rc, bool eraseBg)
{
  RECT rc2 = rc;

  assert(mHWND != NULL);
  InvalidateRect(mHWND, &rc2, eraseBg);
}

/**
 * Flush the Widget invalidated area to redraw it now. It's like
 * Win32's UpdateWindow.
 */
void Widget::update()
{
  assert(mHWND != NULL);
  UpdateWindow(mHWND);
}

/**
 * Returns true if this widget is visible. If this widget or some of
 * its parents has the visibility state, this routine returns
 * true.
 *
 * @see Frame::setVisible
 */
bool Widget::isVisible()
{
  assert(mHWND != NULL);

  return IsWindowVisible(mHWND) != FALSE;
}

void Widget::setVisible(bool visible)
{
  assert(mHWND != NULL);

  if (visible)
    ShowWindow(mHWND, SW_SHOW);
  else
    ShowWindow(mHWND, SW_HIDE);
}

/**
 * Returns true if the widget is enabled. It's like to ask if the
 * widget hasn't the WS_DISABLED style. Really, this method calls the
 * Win32 IsWindowEnabled function.
 */
bool Widget::isEnabled()
{
  assert(mHWND != NULL);

  return IsWindowEnabled(mHWND) != FALSE;
}

/**
 * Changes the enable-state of the widget. If @a state is true removes
 * the WS_DISABLED style, if @a state is false adds the WS_DISABLED
 * style. It's like to call the Win32 EnableWindow.
 */
void Widget::setEnabled(bool state)
{
  assert(mHWND != NULL);

  EnableWindow(mHWND, state ? TRUE: FALSE);
}

/**
 * Gets the foreground color.
 *
 * @return The current foreground color for the widget.
 *
 * @see setFgColor
 */
Color Widget::getFgColor()
{
  return mFgColor;
}

/**
 * Gets the background color.
 *
 * @return The current background color for the widget.
 *
 * @see setBgColor
 */
Color Widget::getBgColor()
{
  return mBgColor;
}

/**
 * Sets the foreground color. This color is used when WM_CTLCOLOR*
 * messages are received. The default value is the
 * Color(GetSysColor(COLOR_WINDOWTEXT)).
 *
 * @see getFgColor
 */
void Widget::setFgColor(Color color)
{
  mFgColor = color;
}

/**
 * Sets the background color. This color is used on WM_ERASEBKGND and
 * WM_COLORCTL* messages. The default value is the
 * Color(GetSysColor(COLOR_3DFACE)).
 *
 * @see getBgColor
 */
void Widget::setBgColor(Color color)
{
  mBgColor = color;
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
 */
int Widget::getOpacity()
{
  assert(mHWND != NULL);

  HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
  GLWAProc pGLWA;
  if (hUser32 != NULL)
    pGLWA = (GLWAProc)GetProcAddress(hUser32, "GetLayeredWindowAttributes");
  else
    pGLWA = NULL;

  if (pGLWA == NULL) {
    return 255;
  }
  else if (GetWindowLong(mHWND, GWL_EXSTYLE) & WS_EX_LAYERED) {
    BYTE opacity;
    pGLWA(mHWND, NULL, &opacity, NULL);
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
 */
void Widget::setOpacity(int opacity)
{
  assert(mHWND != NULL);
  assert(opacity >= 0 && opacity < 256);

  HMODULE  hUser32 = GetModuleHandle(_T("USER32.DLL"));
  SLWAProc pSLWA = (SLWAProc)GetProcAddress(hUser32, "SetLayeredWindowAttributes");

  // The version of Windows running does not support translucent windows!
  if (pSLWA == 0) {
    // do nothing
    return;
  }
  else if (opacity == 255) {
    SetWindowLong(mHWND, GWL_EXSTYLE,
		  GetWindowLong(mHWND, GWL_EXSTYLE) & (~WS_EX_LAYERED));
  }
  else {
    SetWindowLong(mHWND, GWL_EXSTYLE,
  		  GetWindowLong(mHWND, GWL_EXSTYLE) | WS_EX_LAYERED);

    pSLWA(mHWND, 0, opacity, LWA_ALPHA);
  }
}


// Cursor Widget::getCursor()
// {
//   // TODO
//   return Cursor::Arrow;
// }

/**
 * You should use this method only inside onSetCursor() event.
 */
void Widget::setCursor(const Cursor &cursor)
{
  // getHCURSOR can be NULL, like the Cursor(NoCursor)
  ::SetCursor(const_cast<Cursor *>(&cursor)->getHCURSOR());
}

/**
 * Sets the focus to this Widget.
 */
void Widget::focus()
{
  assert(mHWND != NULL);
  SetFocus(mHWND);
}

/**
 * Captures the mouse to receive its events even when it's outside the
 * widget (you must call Widget::releaseCapture when you are done with
 * the mouse).
 */
void Widget::acquireCapture()
{
  assert(mHWND != NULL);
  SetCapture(mHWND);
}

/**
 * Releases the previously captured mouse (Widget::acquireCapture).
 */
void Widget::releaseCapture()
{
  assert(mHWND != NULL);
  assert(mHWND == GetCapture());

  ReleaseCapture();
}

/**
 * Returns true if the widget has captured the mouse.
 */
bool Widget::hasCapture()
{
  assert(mHWND != NULL);

  return mHWND == GetCapture();
}

/**
 * Returns true if the widget has the mouse.
 */
bool Widget::hasMouse()
{
  return mHasMouse;
}

/**
 * Sends this window to the top.
 */
void Widget::bringToTop()
{
  assert(mHWND != NULL);
  BringWindowToTop(mHWND);
}

/**
 * Shows a message box, locking the widget. It's like call Win32's
 * MessageBox using the Widget's mHWND (Widget::getHWND).
 *
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
 * @param text Text shown in the center of the dialog box. It can contains "\n" to break lines.
 * @param title Title for the dialog box.
 * @param flags What kind of message box to show.
 * \n It should be one of these values:
 *    \n &ndash; @c MB_OK
 *    \n &ndash; @c MB_YESNO
 *    \n &ndash; @c MB_YESNOCANCEL
 *    \n &ndash; @c MB_RETRYCANCEL
 *    \n &ndash; @c MB_ABORTRETRYIGNORE
 *    \n &ndash; @c MB_CANCELTRYCONTINUE
 * \n Optionally combined with one of these ones:
 *    \n &ndash; @c MB_ICONWARNING
 *    \n &ndash; @c MB_ICONERROR
 *    \n &ndash; @c MB_ICONQUESTION
 *    \n &ndash; @c MB_ICONINFORMATION
 * \n More optionally combined with one of these other ones:
 *    \n &ndash; @c MB_DEFBUTTON1
 *    \n &ndash; @c MB_DEFBUTTON2
 *    \n &ndash; @c MB_DEFBUTTON3
 * @return It can returns:
 *         @li @c IDOK for @c MB_OK
 *         @li @c IDYES or @c IDNO for @c MB_YESNO
 *         @li @c IDYES, @c IDNO or @c IDCANCEL for @c MB_YESNOCANCEL
 *         @li @c IDRETRY or @c IDCANCEL for @c MB_RETRYCANCEL
 *         @li @c IDABORT, @c IDRETRY or @c IDIGNORE for @c MB_ABORTRETRYIGNORE
 *         @li @c IDCANCEL, @c IDTRYAGAIN or @c IDCONTINUE for @c MB_CANCELTRYCONTINUE
 */
int Widget::msgBox(String text, String title, int flags)
{
  return MessageBox(mHWND, text.c_str(), title.c_str(), flags);
}

/**
 * Returns the position of the mouse relative to the widget's client
 * area.
 *
 * @deprecated This doesn't work because getClientBounds isn't like GetClientRect.
 *
 * @see getAbsoluteCursorPos
 */
// Point Widget::getCursorPos()
// {
//   return getAbsoluteCursorPos() - getAbsoluteClientBounds().getOrigin();
// }

/**
 * Returns the absolute position of the mouse in the screen.
 *
 * @see getCursorPos
 * 
 * TODO maybe this method should be in the @c System class
 */
// Point Widget::getAbsoluteCursorPos()
// {
//   POINT pt;
//   GetCursorPos(&pt);
//   return Point(pt.x, pt.y);
// }

/**
 * Indicates that this widget must be deleted (with C++ "delete"
 * operator) after current event ends. It's a safe way to delete
 * the Event::getSource().
 *
 * @see @ref TN006
 */
void Widget::deleteAfterEvent()
{
  // we can't call deleteAfterEvent if we are outside an event
  assert(mCriticalInner > 0);

  mDeleteAfterEvent = true;
}

/**
 * Returns the ID of the Thread that creates this Widget.
 */
int Widget::getThreadOwnerId()
{
  assert(mHWND != NULL);

  return GetWindowThreadProcessId(mHWND, NULL);
}

/**
 * Returns the HWND of this Widget. This can't be NULL.
 *
 * @see fromHWND, getParentHWND
 */
HWND Widget::getHWND()
{
  return mHWND;
}

/**
 * Returns the HWND
 *
 * @see getHWND
 */
HWND Widget::getParentHWND()
{
  Widget *parent = getParent();
  return parent != NULL ? parent->getHWND(): NULL;
}

/**
 * Returns the Widget pointer from the GWL_USERDATA field of a
 * HWND. This routine makes sense only when the HWND comes from a
 * Widget. In other words, you should use this only if you known that
 * the HWND was created inside Vaca bounds.
 *
 * @see getHWND
 */
Widget *Widget::fromHWND(HWND hwnd)
{
  // unbox the pointer
#ifdef USE_PROP
  return reinterpret_cast<Widget *>(GetProp(hwnd, VACAATOM));
#else
  return reinterpret_cast<Widget *>(GetWindowLongPtr(hwnd, GWL_USERDATA));
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

/**
 * Should return the minimum size that the Widget can adopt in Layout
 * arrangement.
 *
 * @see maximumSize, preferredSize
 */
// Size Widget::minimumSize()
// {
//   if (mLayout != NULL)
//     return mLayout->minimumSize(this, mChildren);
//   else
//     return Size(0, 0);
// }

/**
 * Should return the preferred size that the Widget want.
 */
Size Widget::preferredSize()
{
  if (mLayout != NULL)
    return mLayout->preferredSize(this, mChildren, Size(0, 0));
  else
    return Size(0, 0);
}

Size Widget::preferredSize(const Size &fitIn)
{
  if (mLayout != NULL)
    return mLayout->preferredSize(this, mChildren, fitIn);
  else
    // TODO intersect with fitIn
    return preferredSize();
}

/**
 * Should return the maximum size that the Widget can adopt in Layout
 * arrangement.
 *
 * @see minimumSize, preferredSize
 */
// Size Widget::maximumSize()
// {
//   if (mLayout != NULL)
//     return mLayout->maximumSize(this, mChildren);
//   else
//     // TODO use std::numeric_limits<int>::max()
//     return Size(INT_MAX, INT_MAX);
// }

/**
 * Must arranges the children bounds. The default implementation calls
 * the Layout::layout method of the Widget's layout manager
 * (Widget::mLayout), but you can override this to obtain your own
 * behavior (you can avoid to use Layout manager if you want).
 *
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
  if (mLayout != NULL && !mChildren.empty())
    mLayout->layout(this, mChildren, getLayoutBounds());
}

/**
 * Returns true if the widget is layout-free, that means the Layout
 * shouldn't arrange this widget.
 *
 * @see getLayout, setLayout
 */
bool Widget::isLayoutFree()
{
  // a widget is free of layout if it's hidden
  return ((getStyle().regular & WS_VISIBLE) == WS_VISIBLE) ? false: true;
}

bool Widget::wantArrowCursor()
{
  return false;
}

/**
 * You can customize this method to return true, so the Widget's
 * enabled-state will keep synchronised to the enabled-state of its
 * parent.
 *
 * @see getEnabledSynchronisedGroup
 */
bool Widget::keepEnabledSynchronised()
{
  return false;
}

/**
 * Called when WM_DESTROY message come to wndProc. Calls
 * DestroyWindow, also destroys all children. To extend this method:
 *
 * @code
 * class MyWidget : public Widget {
 *   ...
 * protected:
 *   virtual void onDestroy() {
 *     // ...Your code here...
 *     Widget::onDestroy();
 *   }
 *   ...
 * public:
 *   ~MyWidget() {
 *     dispose(); // <-- This is very important!!!
 *   }
 *   ...
 * }
 * @endcode
 *
 * @warning If you override this method, you @b MUST to call dipose()
 *          in your destructor.
 *
 * @see dispose, destroyHWND, @ref TN002
 */
void Widget::onDestroy()
{
  assert(mHWND != NULL);

  if (getParent() != NULL) {
    // this is very important!!! we can't set the parent of the HWND:
    // this can generate ugly effects for MDI, and also
    // MdiChild::destroyHWND needs the parent HWND
    getParent()->removeChild(this, false);
  }

  // dispose children too
  while (!mChildren.empty()) {
    Widget *child = mChildren.front();
    child->dispose();
  }

  // delete the constraint
  if (mConstraint != NULL) {
    delete mConstraint;
    mConstraint = NULL;
  }

  // delete the layout manager
  if (mLayout != NULL) {
    delete mLayout;
    mLayout = NULL;
  }

  // assert again
  assert(mHWND != NULL);

  // restore the old window-procedure
  if (mSuperWndProc != NULL)
    SetWindowLongPtr(mHWND, GWLP_WNDPROC,
		     reinterpret_cast<LONG_PTR>(mSuperWndProc));
  
  // Please!!! don't remove the pointer from the widget
  // ...
  //   #ifdef USE_PROP
  //     RemoveProp(mHWND, VACAATOM);
  //   #else
  //     SetWindowLongPtr(mHWND, GWL_USERDATA, static_cast<LONG_PTR>(NULL));
  //   #endif
  // ...

  // call the destroyHWND
  destroyHWND(mHWND);

//   if (Frame::getVisibleFramesByThread(Thread::getCurrentId()) == 0)
  Thread *currentThread = Thread::getCurrent();
  if (currentThread != NULL &&	// == NULL could be if the application
				// hasn't an Application instance
      currentThread->hasFrames() == 0)
    PostQuitMessage(0);		// this thread doesn't have more
				// Frames to continue...

  mHWND = NULL;
}

/**
 * Called then the WM_PAINT event is received. The default
 * implementation calls the Graphics::noPaint to know that this widget
 * doesn't paint the surface. If you override this method, remember:
 * (1) to draw inside the Widget::getClientBounds limits, and (2)
 * don't call the base method onPaint().
 *
 * @code
 * class MyWidget : public Panel
 * {
 * public:
 *   ...
 *   virtual void onPaint(Graphics &g)
 *   {
 *     Rect rc = getClientBounds();
 *     g.drawEllipse(rc);
 *   }
 * };
 * @endcode
 *
 * @see onDrawItem
 */
void Widget::onPaint(Graphics &g)
{
  g.noPaint();
}

/**
 * Called when the WM_SIZE message is received.
 */
void Widget::onResize(const Size &sz)
{
  // Don't call layout() here (the Layout::mRelayoutWidgets member is
  // useful to layout recursively). The first trigger of the layout()
  // method is from Frame::onResize().
}

/**
 * The mouse enters in the Widget.
 */
void Widget::onMouseEnter(MouseEvent &ev)
{
  // do nothing
}

/**
 * The mouse leaves the Widget.
 */
void Widget::onMouseLeave()
{
  // do nothing
}

/**
 * The mouse is inside the Widget and the user press a mouse's button.
 */
void Widget::onMouseDown(MouseEvent &ev)
{
  // do nothing
}

/**
 * The mouse is inside the Widget and the user release a mouse's
 * button.
 */
void Widget::onMouseUp(MouseEvent &ev)
{
  // do nothing
}

/**
 * The user made double click over the widget.
 */
void Widget::onDoubleClick(MouseEvent &ev)
{
  // do nothing
}

/**
 * The mouse is inside the Widget and the user release a mouse's
 * button.
 */
void Widget::onMouseMove(MouseEvent &ev)
{
  // do nothing
}

// /**
//  * The mouse was for a short period of time above the widget.
//  */
// void Widget::onMouseHover(MouseHoverEvent &ev)
// {
// }

/**
 * The mouse is inside the Widget and the user spin the mouse's
 * wheel. Event called when the WM_MOUSEWHEEL message is received.
 */
void Widget::onMouseWheel(MouseEvent &ev)
{
  // do nothing
}

/**
 * WM_CANCELMODE.
 * 
 */
void Widget::onCancelMode()
{
  // do nothing
}

/**
 * Set the mouse's cursor depending of its position. If you override
 * this method, you shouldn't call Widget::onSetCursor if you don't
 * want the default behaviour.
 *
 * @param hitTest One of the following values:
 * @li HTERROR
 * @li HTTRANSPARENT
 * @li HTNOWHERE
 * @li HTCLIENT
 * @li HTCAPTION
 * @li HTSYSMENU
 * @li HTGROWBOX
 * @li HTSIZE
 * @li HTMENU
 * @li HTHSCROLL
 * @li HTVSCROLL
 * @li HTMINBUTTON
 * @li HTMAXBUTTON
 * @li HTREDUCE
 * @li HTZOOM
 * @li HTLEFT
 * @li HTSIZEFIRST
 * @li HTRIGHT
 * @li HTTOP
 * @li HTTOPLEFT
 * @li HTTOPRIGHT
 * @li HTBOTTOM
 * @li HTBOTTOMLEFT
 * @li HTBOTTOMRIGHT
 * @li HTSIZELAST
 * @li HTBORDER
 * @li HTOBJECT
 * @li HTCLOSE
 * @li HTHELP
 */
void Widget::onSetCursor(int hitTest)
{
  // if we have a mSuperWndProc, use it
  if (mSuperWndProc != NULL) {
    CallWindowProc(mSuperWndProc, mHWND, WM_SETCURSOR, mWparam, mLparam);
  }
  // if we aren't in the client area, maybe the defWndProc known more
  // about the cursor (like the cursors in Frame to resize the it)
  else if (hitTest != HTCLIENT) {
    defWndProc(WM_SETCURSOR, mWparam, mLparam);
  }
  // finally, we are in the client area, we can use the normal arrow
  else {
    setCursor(Cursor(ArrowCursor));
  }
}

/**
 * The user presses a key. Event called when the WM_KEYUP message is
 * received.
 */
void Widget::onKeyUp(KeyEvent &ev)
{
  KeyUp(ev);
}

/**
 * The user releases a key. Event called when the WM_KEYDOWN message
 * is received.
 */
void Widget::onKeyDown(KeyEvent &ev)
{
  KeyDown(ev);
}

void Widget::onGotFocus(Event &ev)
{
  GotFocus(ev);
}

void Widget::onLostFocus(Event &ev)
{
  LostFocus(ev);
}

/**
 * Called when a WM_COMMAND message is received by ID (like a menu or
 * accelerator command).
 *
 * @return It should returns true if the @a id was used.
 *
 * @warning Don't confuse with onCommand(): onIdAction is used to
 *          handle command notifications that come from accelarators
 *          or menus, not from controls. Notifications by controls are
 *          controled by them via onCommand, onNotify, or onDrawItem.
 */
bool Widget::onIdAction(int id)
{
  return false;
}

/**
 * (WM_WINDOWPOSCHANGING)
 * 
 */
void Widget::onBeforePosChange()
{
}

/**
 * (WM_WINDOWPOSCHANGED)
 * 
 */
void Widget::onAfterPosChange()
{
}

void Widget::onScroll(Orientation orientation, int code)
{
}

void Widget::onDropFiles(DropFilesEvent &ev)
{
  DropFiles(ev);
}

/**
 * This method can be used to handle command notifications
 * (WM_COMMAND) reflected from the parent.
 *
 * @warning Don't confuse with onIdAction(): onCommand is used to handle
 *          commands that this widget by self generated, sent to the parent,
 *          and finally was reflected to this widget again.
 */
bool Widget::onCommand(int id, int code, LRESULT &lResult)
{
  return false;
}

/**
 * This method can be used to handle notifications (WM_NOTIFY)
 * reflected from the parent.
 */
bool Widget::onNotify(LPNMHDR lpnmhdr, LRESULT &lResult)
{
  return false;
}

/**
 * Fired when the parent widget (mParent) received the WM_DRAWITEM
 * message.
 *
 * @see onPaint
 */
bool Widget::onDrawItem(Graphics &g, LPDRAWITEMSTRUCT lpDrawItem)
{
  return false;
}

// TODO timer
// void Widget::addTimer(Timer<Widget> *timer)
// {
//   assert(mHWND != NULL);
//   timer->setID(m_timerIDcount);
//   SetTimer(mHWND, m_timerIDcount, timer->getTimeOut(), NULL);
//   m_timerIDcount++;
// }

/**
 * Adds a child to this widget.
 *
 * @param setParent if it's true the Win32 SetParent wil be used.
 */
void Widget::addChild(Widget *child, bool setParent)
{
  assert(mHWND != NULL);
  assert(child != NULL);
  assert(child->mHWND != NULL);
  assert(child->mParent == NULL);

  mChildren.push_back(child);

  child->mParent = this;

  if (setParent) {
    child->addStyle(Style(WS_CHILD, 0));
    ::SetParent(child->mHWND, mHWND);
    // sendMessage(WM_UPDATEUISTATE, UIS_SET..., 0);
  }
}

/**
 * Removes a child from this widget.
 *
 * @param setParent if it's true the Win32 SetParent will be used.
 */
void Widget::removeChild(Widget *child, bool setParent)
{
  assert(mHWND != NULL);
  assert(child != NULL);
  assert(child->mHWND != NULL);
  assert(child->mParent == this);

  remove_element_from_container(mChildren, child);

  if (setParent) {
    ::SetParent(child->mHWND, NULL);
    child->removeStyle(Style(WS_CHILD, 0));
  }

  child->mParent = NULL;
}

/**
 * Returns the collection of widgets that are enabled-synchronised to
 * its parent (see keepEnabledSynchronised). If this routine returns
 * true, the enabled state of the Widget will be synchronised with the
 * enabled state of its parent (for example ToolBarFrame).
 *
 * @see keepEnabledSynchronised
 */
Widget::Container Widget::getEnabledSynchronisedGroup()
{
  Container container;
  for (Container::iterator it=mChildren.begin(); it!=mChildren.end(); ++it) {
    Widget *child = *it;
    if (child->keepEnabledSynchronised())
      container.push_back(child);
  }
  return container;
}

// Widget::Container Widget::getLayoutFreeGroup()
// {
//   Container container;
//   for (Container::iterator it=mChildren.begin(); it!=mChildren.end(); ++it) {
//     Widget *child = *it;
//     if (child->isLayoutFree())
//       container.push_back(child);
//   }
//   return container;
// }

/**
 * It creates the HWND (mHWND) to be used in the Widget. The only way
 * to customize this routine is overloading the Widget::createHWND()
 * method, calling Widget::Widget() with NULL as @c className, and
 * finally calling Widget::create() from the constructor of your own
 * widget class (for example MdiClient::MdiClient()).
 *
 * This method MUST BE called in one constructor (in Widget::Widget() or
 * in your own class if you used a NULL className in Widget's
 * constructor).
 *
 * @see dispose, createHWND, @ref TN002
 */
void Widget::create(LPCTSTR className, Widget *parent, Style style)
{
  assert(mHWND == NULL);
  assert(parent == NULL || parent->mHWND != NULL);

  // create the HWND handler
  outsideWidgetMutex.lock();
    assert(outsideWidget == NULL);

    outsideWidget = this;
    mHWND = createHWND(className, parent, style);
    outsideWidget = NULL;

  outsideWidgetMutex.unlock();

  if (mHWND == NULL)
    throw CreateHWNDException();

  subClass();

  // add the widget to its parent
  if (parent != NULL)
    parent->addChild(this, false);
}

void Widget::subClass()
{
  assert(mHWND != NULL);

  // set the GWL_WNDPROC to globalWndProc
  mSuperWndProc = reinterpret_cast<WNDPROC>
    (SetWindowLongPtr(mHWND,
		      GWLP_WNDPROC,
		      reinterpret_cast<LONG_PTR>(getGlobalWndProc())));
  if (mSuperWndProc == getGlobalWndProc())
    mSuperWndProc = NULL;

  // box the pointer
#ifdef USE_PROP
  SetProp(mHWND, VACAATOM, reinterpret_cast<HANDLE>(this));
#else
  LONG_PTR oldData = SetWindowLongPtr(mHWND, GWL_USERDATA, reinterpret_cast<LONG_PTR>(this));
  assert(oldData == 0);
#endif

  // TODO get the font from the hwnd

  // set the default font of the widget
  setFont(Font::getDefault());
}

/**
 * Should create the HWND for the Widget. This routine is called from
 * Widget::create method. If you overloaded this, you should use the
 * Widget constructor with a NULL className, and then use the
 * Widget::create method in your own class's constructor (for example
 * MdiCliente::MdiClient).
 *
 * @see create, @ref TN002
 */
HWND Widget::createHWND(LPCTSTR className, Widget *parent, Style style)
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
 * Destroys the HWND. It's virtual because maybe there are other ways
 * to destroy the window (like the MdiChild::destroyHWND).
 *
 * @warning If you override this method, you @b MUST to call dipose()
 *          in your destructor.
 *
 * @warning You can't use Widget's methods in this point, you are
 *          limited to use the HWND data only (use the onDestroy event
 *          if you want to delete data members).
 *
 * @see dispose, onDestroy, @ref TN002
 */
void Widget::destroyHWND(HWND hwnd)
{
  DestroyWindow(mHWND);
}

/** 
 * The customized window procedure for this particular widget.  This
 * is called by Widget::globalWndProc().  Should return true when the
 * defWndProc() doesn't need to be called.
 *
 * This method is called to intercept any message after the creation
 * of the widget, and before the disposition. To intercept messages
 * outside that range (before creation, and after disposition), you
 * should override defWndProc().
 *
 * This method mainly converts a message to a event:
 * @li @c WM_PAINT -&gt; onPaint()
 * @li @c WM_SIZE -&gt; onResize()
 * @li @c WM_SETCURSOR -&gt; onSetCursor()
 * @li @c WM_*BUTTONDOWN -&gt; onMouseDown()
 * @li @c WM_*BUTOTNUP -&gt; onMouseUp()
 * @li @c WM_*BUTOTNDBLCLK -&gt; onDoubleClick()
 * @li @c WM_MOUSEMOVE -&gt; onMouseMove()
 * @li @c WM_MOUSEWHEEL -&gt; onMouseWheel()
 * @li @c WM_MOUSELEAVE -&gt; onMouseLeave()
 * @li @c WM_CANCELMODE -&gt; onCancelMode()
 * @li @c WM_KEYDOWN -&gt; onKeyDown()
 * @li @c WM_KEYUP -&gt; onKeyUp()
 * @li @c WM_SETFOCUS -&gt; onGotFocus()
 * @li @c WM_KILLFOCUS -&gt; onLostFocus()
 *
 * For reflection, it does:
 *
 * @li When @c WM_COMMAND is received, the onCommand() event <b>of the
 *     child</b> is called when it's a WM_COMMAND from a control, or the
 *     onIdAction() event <b>of this widget</b> is called when the command
 *     come from a menu or an accelerator.
 * @li When @c WM_NOTIFY is received, the onNotify() event <b>of the
 *     child</b> is called.
 * @li When @c WM_DRAWITEM is received, the onDrawItem() event <b>of the
 *     child</b> is called.
 *
 * How to extend widget::wndProc method?
 *
 * @code
 * class MyWidget : public Widget {
 *   ...
 * protected:
 *   virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
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
bool Widget::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
  bool ret = false;

  switch (message) {

    case WM_ERASEBKGND:
      // see TN011
      if (mSuperWndProc == NULL) {
	HDC hdc = reinterpret_cast<HDC>(wParam);
	// erase background only in when the widget doesn't use
	// double-buffering
	if (!mDoubleBuffering) {
	  Graphics g(hdc);
	  g.setColor(getBgColor());
	  g.fillRect(g.getClipBounds());
	  // TODO fill only the invalid region
// 	  RECT rc;
// 	  if (GetUpdateRect(mHWND, &rc, FALSE))
// 	    g.fillRect(Rect(&rc));
	}
	lResult = TRUE;
	ret = true;
      }
      break;

    case WM_PAINT:
      // see TN011
      if (mSuperWndProc == NULL) {
	PAINTSTRUCT ps;
	bool painted;
	HDC hdc = BeginPaint(mHWND, &ps);
	{
	  Graphics g(hdc);
	  painted = doPaint(g);
	}
	EndPaint(mHWND, &ps);

	if (painted) {
	  lResult = TRUE;
	  ret = true;
	}
      }
      break;

    case WM_DRAWITEM: {
      LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
      Widget *child = Widget::fromHWND(lpDrawItem->hwndItem);
      HDC hdc = lpDrawItem->hDC;
      bool painted;
      {
	Graphics g(hdc);
	painted = child->onDrawItem(g, lpDrawItem);
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
      if (WindowFromPoint(System::getCursorPos()) == mHWND) {
	onSetCursor(LOWORD(lParam));
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
	   Point(LOWORD(lParam), HIWORD(lParam)),	   // pt
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
	   Point(LOWORD(lParam), HIWORD(lParam)),	 // pt
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
	   Point(LOWORD(lParam), HIWORD(lParam)),	     // pt
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
	   Point(LOWORD(lParam), HIWORD(lParam)),   // pt
	   0,					    // clicks
	   wParam,				    // flags
	   MouseButtons::None);			    // button

      if (!mHasMouse) {
	onMouseEnter(ev);
	mHasMouse = true;

	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;// | TME_HOVER
	tme.hwndTrack = mHWND;
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
	   Point(LOWORD(lParam) - clientOrigin.x,
		 HIWORD(lParam) - clientOrigin.y), // pt
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
//       MouseEvent ev(this, Point(LOWORD(lParam), HIWORD(lParam)), 0,
// 		    MouseButtons::None, wParam);
//       onMouseHover(ev);
//     }

    case WM_MOUSELEAVE: {
      // TODO ask for mHasMouse?
//       if (mHasMouse) {
	mHasMouse = false;
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
      // sigKeyDown();
      if (ev.isConsumed()) {
	lResult = false;
	ret = true;
      }
      break;
    }

    case WM_KEYUP: {
      KeyEvent ev(this, Keys::fromMessageParams(wParam, lParam));
      onKeyUp(ev);
      // sigKeyUp();
      if (ev.isConsumed()) {
	lResult = false;
	ret = true;
      }
      break;
    }

    case WM_COMMAND:
      if (reinterpret_cast<HWND>(lParam) != NULL) {
	HWND hwndCtrl = reinterpret_cast<HWND>(lParam);
	Widget *ctrl = Widget::fromHWND(hwndCtrl);
	if (ctrl != NULL)
	  ret = ctrl->onCommand(LOWORD(wParam), // id
				HIWORD(wParam), // code
				lResult);
      }
      // accelerator or a menu
      else if (HIWORD(wParam) == 1 ||
	       HIWORD(wParam) == 0) {
	if (onIdAction(LOWORD(wParam))) {
	  // ...onIdAction returns true when processed the command
	  lResult = 0;		// processed
	  ret = true;
	}
      }
      break;

    case WM_NOTIFY: {
      LPNMHDR lpnmhdr = reinterpret_cast<LPNMHDR>(lParam);
      Widget *ctrl = Widget::fromHWND(lpnmhdr->hwndFrom);
      if (ctrl != NULL)
	ret = ctrl->onNotify(lpnmhdr, lResult);
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
	  MenuItem *menuitem = reinterpret_cast<MenuItem *>(mii.dwItemData);
	  if (menuitem != NULL) {
	    ItemEvent(menuitem->onSelected, this, menuitem).fire();
	  }
	}
      }
    }
      */

    case WM_ENABLE: {
      Container group = getEnabledSynchronisedGroup();
      HWND hParam  = reinterpret_cast<HWND>(lParam);

      // synchronise all enable-state group
      for (Container::iterator it=group.begin(); it!=group.end(); ++it) {
	HWND hwndChild = (*it)->getHWND();

	if (hwndChild != hParam)
	  EnableWindow(hwndChild, static_cast<BOOL>(wParam));
      }

      lResult = defWndProc(WM_ENABLE, wParam, lParam);
      ret = true;
      break;
    }

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

    case WM_NCACTIVATE:
      if (lParam == static_cast<LPARAM>(-1)) {
	lResult = defWndProc(WM_NCACTIVATE, wParam, lParam);
	ret = true;
      }
      else {
	Widget *owner = keepEnabledSynchronised() ? getParent(): this;
	
	// this can happend in the last WM_NCACTIVATE message (when
	// the widget is destroyed)
	if (owner == reinterpret_cast<Widget *>(NULL))
	  return false;

	bool keepActive = wParam != 0;
	bool syncGroup = true;
	HWND hParam  = reinterpret_cast<HWND>(lParam);

	Container group = owner->getEnabledSynchronisedGroup();
	group.push_back(owner);

	// if the other window to be activated/desactivated belong to
	// the synchronised enable-state group, we don't need to
	// synchronise/repaint all the group
	for (Container::iterator it=group.begin(); it!=group.end(); ++it) {
	  Widget *child = *it;
	  if (child->getHWND() == hParam) {
	    keepActive = true;
	    syncGroup = false;
	    break;
	  }
	}

	// synchronise the group
	if (syncGroup) {
	  for (Container::iterator it=group.begin(); it!=group.end(); ++it) {
	    Widget *child = *it;
	    if ((child->getHWND() != mHWND) && (child->getHWND() != hParam))
	      child->sendMessage(WM_NCACTIVATE, keepActive, static_cast<LPARAM>(-1));
	  }
	}

	lResult = defWndProc(WM_NCACTIVATE, keepActive, lParam);
	ret = true;
      }
      break;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC: {
      HDC hdc = reinterpret_cast<HDC>(wParam);
      HWND hwndCtrl = reinterpret_cast<HWND>(lParam);
      Widget *ctrl = Widget::fromHWND(hwndCtrl);
      if (ctrl != NULL) {
	COLORREF fgColor = ctrl->getFgColor().getColorRef();
	COLORREF bgColor = ctrl->getBgColor().getColorRef();

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
	Orientation orientation =
	  (message == WM_VSCROLL) ? Vertical:
				    Horizontal;
	  
	// Note: onScroll() doesn't receive the nPos=HIWORD(wParam)
	// because it has a limit of 16-bits.  Instead you should use
	// GetScrollInfo to get the position of the scroll-bar, it has
	// the full 32 bits
	
	// reflect the message? (it's useful for the "Slider" widget)
	if (lParam != 0) {
	  Widget *ctrl = Widget::fromHWND(reinterpret_cast<HWND>(lParam));
	  if (ctrl != NULL) {
	    ctrl->onScroll(orientation, LOWORD(wParam));
	  }
	}

	onScroll(orientation, LOWORD(wParam));
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
 * Calls the default window procedure (Win32 DefWindowProc) for the
 * generic Widgets. If mSuperWndProc isn't NULL, it's called instead.
 */
LRESULT Widget::defWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  if (mSuperWndProc != NULL)
    return CallWindowProc(mSuperWndProc, mHWND, message, wParam, lParam);
  else
    return DefWindowProc(mHWND, message, wParam, lParam);
}

bool Widget::doPaint(Graphics &g)
{
  bool painted = false;

  // use double buffering?
  if (mDoubleBuffering) {
    // get the clip bounds
    Rect clipBounds = g.getClipBounds();
    // isn't it empty?
    if (!clipBounds.isEmpty()) {
      // create the image for double-buffering (of the size of
      // the clipping bounds)
      Image image(g, clipBounds.getSize());
      // get the Graphics to draw in the image
      Graphics &g2(image.getGraphics());

      // special coordinates transformation (to make
      // transparent the "g2" to onPaint)
      SetViewportOrgEx(g2.getHDC(), -clipBounds.x, -clipBounds.y, NULL);

      // clear the background of the image
      g2.setColor(getBgColor());
      g2.fillRect(clipBounds);

      // configure defaults
      g2.setFont(getFont());
      g2.setColor(getFgColor());
      
      // paint on g2
      onPaint(g2);
      painted = g2.wasPainted();

      // restore the viewport origin (so drawImage works fine)
      SetViewportOrgEx(g2.getHDC(), 0, 0, NULL);

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

bool Widget::preTranslateMessage(MSG &msg)
{
//   for (Container::iterator it=mChildren.begin(); it!=mChildren.end(); ++it) {
//     Widget *child = *it;
//     if (child->preTranslateMessage(msg))
//       return true;
//   }

  if (mParent != NULL)
    return mParent->preTranslateMessage(msg);
  else
    return false;
}

/**
 * Sends a message to the HWND, like Win32 SendMessage.
 */
LRESULT Widget::sendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
  assert(mHWND != NULL);
  return ::SendMessage(mHWND, message, wParam, lParam);
}

bool Widget::isDisposedAscendent()
{
  if (mDisposed)
    return true;
  else if (mParent != NULL)
    return mParent->isDisposedAscendent();
  else
    return false;
}

/**
 * Destroys the widget. Is safe to call dispose() many times, because
 * it checks is the widget is already destroyed after destroy it.
 *
 * @warning You can't use other widget's methods after calling this
 *          one, because assertions will fail.
 *
 * @warning If you override the onDestroy method, you must call
 *          dispose() in your own destructor.
 *
 * @warning This method closes a window and destroys it. If you have a
 *          Frame and you want to hide it, you should use
 *          setVisible(false) and intercept the @ref Frame::onClose
 *          "onClose event".
 *
 * @see create, onDestroy, ~Widget(), @ref TN002
 */
void Widget::dispose()
{
  if (!mDisposed) {
#ifdef REPORT_MESSAGES
    VACA_TRACE("--------------- Dispose (%08x) --------------------\n", reinterpret_cast<int>(this));
#endif
    
    mDisposed = true;
    onDestroy();
  }
}

/**
 * The global procedure for Win32 used in all registered WNDCLASSes.
 * It's unique goal is to get the Widget pointer from HWND userdata
 * using Widget::fromHWND, and to call its wndProc method.
 */
LRESULT CALLBACK Widget::globalWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  Widget *widget = Widget::fromHWND(hwnd);

#ifdef REPORT_MESSAGES
  String preString =
    "Message "+String::fromInt(message)+" for " +
    String::fromInt(reinterpret_cast<int>(widget != NULL ? widget: outsideWidget), 16, 8);
  
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

    widget->mCriticalInner++;

    WPARAM oldWparam = widget->mWparam;
    LPARAM oldLparam = widget->mLparam;
    widget->mWparam = wParam;
    widget->mLparam = lParam;

    // AfterEvent must be empty
//     assert(widget->AfterEvent.empty());

    // call the wndProc() only if the widget isn't disposed
    if (!widget->mDisposed)   // TODO we need isDisposedAscendent()???
      used = widget->wndProc(message, wParam, lParam, lResult);

    // call the defWndProc()
    if (!used)
      lResult = widget->defWndProc(message, wParam, lParam);

//     // call all slots connected to AfterEvent
//     if (!widget->AfterEvent.empty()) {
//       widget->AfterEvent();
//       widget->AfterEvent.disconnect_all_slots();
//     }

    widget->mCriticalInner--;

    widget->mWparam = oldWparam;
    widget->mLparam = oldLparam;

    if (widget->mCriticalInner == 0) {
      // see TN006
      if (widget->mDeleteAfterEvent)
	delete widget;
    }

    return lResult;
  }
  else {
    outsideWidgetMutex.lock();
    widget = outsideWidget;
    outsideWidgetMutex.unlock();

    if (widget != NULL) {
      widget->mHWND = hwnd;
      return widget->defWndProc(message, wParam, lParam);
    }
  }

  VACA_TRACE("------------ LOST MESSAGE: %p %d %d %d ---------------\n", hwnd, message, wParam, lParam);
  Beep(900, 100);

  return FALSE;
}
