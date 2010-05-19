// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
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
// * Neither the name of the author nor the names of its contributors
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

#include "Vaca/Frame.h"
#include "Vaca/Application.h"
#include "Vaca/Register.h"
#include "Vaca/CloseEvent.h"
#include "Vaca/Debug.h"
#include "Vaca/Menu.h"
#include "Vaca/MenuItemEvent.h"
#include "Vaca/AnchorLayout.h"
#include "Vaca/Event.h"
#include "Vaca/Point.h"
#include "Vaca/System.h"
#include "Vaca/Mdi.h"
#include "Vaca/Thread.h"
#include "Vaca/Icon.h"
#include "Vaca/ChildEvent.h"
#include "Vaca/CommandEvent.h"
#include "Vaca/PreferredSizeEvent.h"
#include "Vaca/win32.h"

using namespace Vaca;

/**
   Creates a frame using the FrameClass.  Also, remember that by
   default a Frame hasn't a Layout manager.

   @see onClose()
*/
Frame::Frame(const String& title, Widget* parent, Style style)
  : Widget(FrameClass::getClassName(), parent, style)
{
  initialize(title);
}

/**
   Creates a frame with a custom WNDCLASS. @a className can be NULL if
   you want to call Widget::create() by your self.
*/
Frame::Frame(const WidgetClassName& className, const String& title, Widget* parent, Style style)
  : Widget(className, parent, style)
{
  initialize(title);
}

Frame::Frame(HWND handle)
  : Widget(handle)
{
  initialize(getText());
}

void Frame::initialize(const String& title)
{
  m_menuBar = NULL;
  m_counted = false;

  // we can set the title of the window now if we have the HWND (for
  // example, HWND could be NULL here if we come from a Dialog's
  // contructor)
  if (::IsWindow(getHandle()))
    setText(title);
}

Frame::~Frame()
{
  delete m_menuBar;

  if (m_counted) {
    m_counted = false;

    CurrentThread::details::removeFrame(this);
  }
}

bool Frame::preTranslateMessage(Message& message)
{
  MSG& msg(*(LPMSG)message);

  if (m_menuBar != NULL && isEnabled()) {
    // TODO accelerators
    if (msg.message == WM_KEYDOWN) {
      Keys::Type keys = Keys::fromMessageParams(msg.wParam, msg.lParam);
      MenuItem* menuItem = m_menuBar->checkShortcuts(keys);

      if (menuItem != NULL) {
	// update the menuItem status before (onUpdate stuff)
	updateMenuItem(menuItem);

	// is menuItem enabled?
	if (menuItem->isEnabled()) {
	  // ok, we can do the action
	  CommandEvent ev(menuItem, menuItem->getId());
	  onCommand(ev);
	}

	return true;
      }
    }
  }

  return Widget::preTranslateMessage(message);
}

/**
   Adds space for the non-client size.

   @see getNonClientSize
*/
void Frame::onPreferredSize(PreferredSizeEvent& ev)
{
  Size ncSize = getNonClientSize();

  if (ev.fitInWidth() || ev.fitInHeight()) {
    ev.setPreferredSize(max_value(0, ev.fitInWidth() - ncSize.w),
			max_value(0, ev.fitInHeight() - ncSize.h));
  }

  Widget::onPreferredSize(ev);

  ev.setPreferredSize(ev.getPreferredSize() + ncSize);
}

/**
   Calls the #layout member function.
*/
void Frame::onResize(ResizeEvent& ev)
{
  layout();
  Widget::onResize(ev);
}

/**
   Converts a WM_COMMAND notification from a menus to a MenuItem#onClick event.
*/
void Frame::onCommand(CommandEvent& ev)
{
  if (!ev.isConsumed()) {
    // use menu bar
    if (m_menuBar != NULL) {
      MenuItem* menuItem = m_menuBar->getMenuItemById(ev.getCommandId());

      VACA_TRACE("Frame::onCommand(%d), menuItem=%p\n", ev.getCommandId(), menuItem);

      if (menuItem != NULL) {
	MenuItemEvent ev(menuItem);
	menuItem->onClick(ev);
      }
    }
  }
  Widget::onCommand(ev);
}

void Frame::onActivate(Event& ev)
{
  Activate(ev);
}

void Frame::onDeactivate(Event& ev)
{
  Deactivate(ev);
}

/**
   Event called when the Frame want to be closed.

   The default implementation fire the signal Frame#Close,
   and hide the widget (#setVisible(false)). If you cancel
   the @a ev event, the window'll not hidden.

   The next example shows how to avoid to hide the Frame when the user
   press the close button:
   @code
   class MyFrame : public Frame
   {
   public:
     ...
     virtual void onClose(WidgetEvent& ev) {
       Frame::onClose(ev); // it fires the Close signal
       ev.cancel();        // cancel the event (don't hide the frame)
     }
   }
   @endcode

   @win32
     This event is generated by the @msdn{WM_CLOSE} Message.
   @endwin32
*/
void Frame::onClose(CloseEvent& ev)
{
  // fire Close signal
  Close(ev);
}

/**
   @param edge From where the user is resizing.
*/
void Frame::onResizing(CardinalDirection dir, Rect& rc)
{
  Resizing(dir, rc);
}

void Frame::onUpdateIndicators()
{
  Widget::onUpdateIndicators();

  // update menu-bar
  if (m_menuBar != NULL) {
    MenuItemList children = m_menuBar->getMenuItems();

    // update menus
    for (MenuItemList::iterator it=children.begin(); it!=children.end(); ++it) {
      MenuItem* menuItem = *it;
      updateMenuItem(menuItem);
    }

    ::DrawMenuBar(getHandle());
  }
}

/**
   Hides or shows the window. If visible is true, the window'll be
   shown, if it's false the window'll be hidden.
*/
void Frame::setVisible(bool visible)
{
  HWND hwnd = getHandle();
  assert(::IsWindow(hwnd));

//   bool oldState = ::IsWindowVisible(hwnd) ? true: false;
//   bool oldState = isVisible();

  // synchronize all the group
  WidgetList group = getSynchronizedGroup();
  for (WidgetList::iterator it=group.begin(); it!=group.end(); ++it)
    (*it)->setVisible(visible);

  // Show the window
  if (visible) {
    int swCmd = SW_SHOWNORMAL;

    if ((getStyle() & Frame::Styles::InitiallyMinimized) == Frame::Styles::InitiallyMinimized)
      swCmd = SW_MINIMIZE;
    else if ((getStyle() & Frame::Styles::InitiallyMaximized) == Frame::Styles::InitiallyMaximized)
      swCmd = SW_MAXIMIZE;

    ::ShowWindow(hwnd, swCmd);
    ::UpdateWindow(hwnd);
    if (m_menuBar != NULL)
      ::DrawMenuBar(hwnd);

    // layout children
    layout();

    // increment the frame's counter
    if (!m_counted) {
      m_counted = true;

      CurrentThread::details::addFrame(this);
    }
  }
  // Hide the window. It looks simple, but the order of the commands
  // is critical: when the active window is hidden, another window (of
  // other application!!) is activated. We can fix it activating first
  // the parent, and then hide the non-active window.
  else {
    // activate the parent
    if (getParent() != NULL)
      ::SetActiveWindow(getParent()->getHandle());

    // then hide this non-active window
    ::ShowWindow(hwnd, SW_HIDE);

    // decrement the frame's counter
    if (m_counted) {
      m_counted = false;

      CurrentThread::details::removeFrame(this);
    }
  }
}

/**
   Returns the window's menu bar. You can't delete it, the Frame
   destructor does this for you. If you want to delete the current
   menu bar, you should do something like this:

   @code
     ...
     MenuBar* oldMenuBar = myFrame->setMenuBar(NULL);
     if (oldMenuBar != NULL)
       delete oldMenuBar;
     ...
   @endcode

   Or, if you are sure that the Frame has a MenuBar, you can do:

   @code
     ...
     delete myFrame->setMenuBar(NULL);
     ...
   @endcode

   @see setMenuBar
*/
MenuBar* Frame::getMenuBar()
{
  return m_menuBar;
}

/**
   Changes the window's menu bar. The @a menuBar pointer is deleted
   automatically only if you don't setMenuBar(NULL) in some place
   before the Frame's destructor.

   @return The old MenuBar (you should delete it). All Frames start
	   without a menu bar (=NULL).

   @see getMenuBar, MdiFrame::setMenuBar, MenuBar::isAutoDelete, @ref page_tn_010
*/
MenuBar* Frame::setMenuBar(MenuBar* menuBar)
{
  HWND hwnd = getHandle();
  HMENU hmenu = menuBar->getHandle();

  assert(hwnd != NULL && hmenu != NULL);

  ::SetMenu(hwnd, hmenu); // TODO check errors

  MenuBar* oldMenuBar = m_menuBar;

  if (m_menuBar) m_menuBar->setFrame(NULL);
  m_menuBar = menuBar;
  if (m_menuBar) m_menuBar->setFrame(this);

  return oldMenuBar;
}

/**
   Sets the Icon used in the title bar.

   @win32
     It uses @msdn{WM_SETICON} message.
   @endwin32

   @see #setBigIcon, #setIcon
*/
void Frame::setSmallIcon(const Icon& icon)
{
  m_smallIcon = icon;
  sendMessage(WM_SETICON, ICON_SMALL,
	      reinterpret_cast<LPARAM>(icon.getHandle()));
}

/**
   Sets the Icon used in the ALT+TAB dialog box.

   @win32
     It uses @msdn{WM_SETICON} message.
   @endwin32

   @see #setSmallIcon, #setIcon
*/
void Frame::setBigIcon(const Icon& icon)
{
  m_bigIcon = icon;
  sendMessage(WM_SETICON, ICON_BIG,
	      reinterpret_cast<LPARAM>(icon.getHandle()));
}

/**
   Set the big icon (32x32) and the small icon (16x16) of the Frame
   using the ICON resource speficied by @a iconId.

   @see #setSmallIcon, #setBigIcon
*/
void Frame::setIcon(ResourceId iconId)
{
  setSmallIcon(Icon(iconId, Size(32, 32)));
  setBigIcon(Icon(iconId, Size(16, 16)));
}

/**
   Returns the Size of non-client area of the frame. It's useful to
   known how much space of the rectangle passed to setBounds will be
   used in non-client area.

   @see getBounds
*/
Size Frame::getNonClientSize()
{
  HWND hwnd = getHandle();
  assert(::IsWindow(hwnd));

  Rect clientRect(0, 0, 1, 1);
  RECT nonClientRect = convert_to<RECT>(clientRect);

  ::AdjustWindowRectEx(&nonClientRect,
		       GetWindowLong(hwnd, GWL_STYLE),
		       m_menuBar ? true: false,
		       GetWindowLong(hwnd, GWL_EXSTYLE));

  return convert_to<Rect>(nonClientRect).getSize() - clientRect.getSize();
}

bool Frame::isLayoutFree() const
{
  return true;
}

/**
   You can override this member function to return true, so the Frame's
   enabled-and-visible-state will keep synchronized to the
   enabled-and-visible-state of its parent.

   @see getSynchronizedGroup
*/
bool Frame::keepSynchronized()
{
  return false;
}

void Frame::updateMenuItem(MenuItem* menuItem)
{
  assert(menuItem != NULL);

  MenuItemEvent ev(menuItem);
  menuItem->onUpdate(ev);
}

/**
   Returns the collection of frames that are synchronized to its
   parent (see keepSynchronized). If this routine returns true, the
   enabled-and-visible-state of the Frame will be synchronized with
   the state of its parent (for example DockFrame).

   @see keepSynchronized
*/
WidgetList Frame::getSynchronizedGroup()
{
  WidgetList children = getChildren();
  WidgetList container;

  for (WidgetList::iterator it=children.begin(); it!=children.end(); ++it) {
    Frame* child = dynamic_cast<Frame*>(*it);
    if (child != NULL && child->keepSynchronized())
      container.push_back(child);
  }

  return container;
}

/**
   Converts messages to events.

   It converts the following messages to events:
   @li @c WM_ACTIVATE -&gt; onActivate()
   @li @c WM_CLOSE -&gt; onClose()
   @li @c WM_SIZING -&gt; onResizing()

   When the event generated by WM_CLOSE message isn't cancelled, the
   Frame is automatically hidden.
*/
bool Frame::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
  if (Widget::wndProc(message, wParam, lParam, lResult))
    return true;

  switch (message) {

    case WM_CLOSE: {
      // Fire onClose event
      CloseEvent ev(this);
      onClose(ev);

      // Default behaviour: when the event isn't cancelled, we must to
      // hide the Frame
      if (!ev.isCanceled())
	setVisible(false);

      lResult = 0;
      return true;
      break;
    }

    case WM_ACTIVATE:
      if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE) {
	Event ev(this);
	onActivate(ev);
      }
      else if (wParam == WA_INACTIVE) {
	Event ev(this);
	onDeactivate(ev);
      }
      break;

    case WM_INITMENU:
    case WM_INITMENUPOPUP: {
      HMENU hmenu = reinterpret_cast<HMENU>(wParam);
      if (hmenu == NULL)
	break;

      Menu* menu = NULL;

      MENUINFO mi;
      mi.cbSize = sizeof(MENUINFO);
      mi.fMask = MIM_MENUDATA;
      if (::GetMenuInfo(hmenu, &mi))
	menu = reinterpret_cast<Menu* >(mi.dwMenuData);

      if (menu != NULL) {
	MenuItemList children = menu->getMenuItems();

	// Update menus
	for (MenuItemList::iterator it=children.begin(); it!=children.end(); ++it) {
	  MenuItem* menuItem = *it;
	  updateMenuItem(menuItem);
	}
      }
      break;
    }

    case WM_SIZING: {
      LPRECT lprc = reinterpret_cast<LPRECT>(lParam);
      Rect rc(convert_to<Rect>(*lprc));
      CardinalDirection dir;

      switch (wParam) {
	case WMSZ_TOP:         dir = CardinalDirection::North;     break;
	case WMSZ_TOPRIGHT:    dir = CardinalDirection::Northeast; break;
	case WMSZ_RIGHT:       dir = CardinalDirection::East;      break;
	case WMSZ_BOTTOMRIGHT: dir = CardinalDirection::Southeast; break;
	case WMSZ_BOTTOM:      dir = CardinalDirection::South;     break;
	case WMSZ_BOTTOMLEFT:  dir = CardinalDirection::Southwest; break;
	case WMSZ_LEFT:        dir = CardinalDirection::West;      break;
	case WMSZ_TOPLEFT:     dir = CardinalDirection::Northwest; break;
      }

      onResizing(dir, rc);
      *lprc = convert_to<RECT>(rc);
      lResult = TRUE;
      return true;
    }

    case WM_ENABLE: {
      WidgetList group = getSynchronizedGroup();
      HWND hParam  = reinterpret_cast<HWND>(lParam);

      // synchronize all the group
      for (WidgetList::iterator it=group.begin(); it!=group.end(); ++it) {
	HWND hwndChild = (*it)->getHandle();

	if (hwndChild != hParam)
	  EnableWindow(hwndChild, static_cast<BOOL>(wParam));
      }

      lResult = defWndProc(WM_ENABLE, wParam, lParam);
      return true;
    }

    case WM_NCACTIVATE:
      if (lParam == static_cast<LPARAM>(-1)) {
	lResult = defWndProc(WM_NCACTIVATE, wParam, lParam);
	return true;
      }
      else {
	Frame* owner = keepSynchronized() ? dynamic_cast<Frame*>(getParent()): this;

	// This can happend in the last WM_NCACTIVATE message (when
	// the widget is destroyed)
// 	if (owner == reinterpret_cast<Frame*>(NULL))
// 	  return false;
	assert(owner != NULL);

	bool keepActive = wParam != 0;
	bool syncGroup = true;
	HWND hParam = reinterpret_cast<HWND>(lParam);

	WidgetList group = owner->getSynchronizedGroup();
	group.push_back(owner);

	// if the other window to be activated/desactivated belong to
	// the synchronized group, we don't need to
	// synchronize/repaint all the group
	for (WidgetList::iterator it=group.begin(); it!=group.end(); ++it) {
	  Widget* child = *it;
	  if (child->getHandle() == hParam) {
	    keepActive = true;
	    syncGroup = false;
	    break;
	  }
	}

	// synchronize the group
	if (syncGroup) {
	  for (WidgetList::iterator it=group.begin(); it!=group.end(); ++it) {
	    Widget* child = *it;
	    if ((child->getHandle() != getHandle()) &&
		(child->getHandle() != hParam))
	      child->sendMessage(WM_NCACTIVATE, keepActive, static_cast<LPARAM>(-1));
	  }
	}

	lResult = defWndProc(WM_NCACTIVATE, keepActive, lParam);
	return true;
      }
      break;

  }

  return false;
}
