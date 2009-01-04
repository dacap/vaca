// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, 2009, David Capello
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
#include "Vaca/BandedDockArea.h"
#include "Vaca/StatusBar.h"

using namespace Vaca;

/**
 * Creates a frame using the FrameClass.  Also, remember that by
 * default a Frame hasn't a Layout manager.
 *
 * @see onClose()
 */
Frame::Frame(const String& title, Widget* parent, Style style)
  : Widget(FrameClass::getClassName(), parent, style)
{
  initialize(title);
}

/**
 * Creates a frame with a custom WNDCLASS. @a className can be NULL if
 * you want to call Widget::create() by your self.
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
  m_statusBar = NULL;
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

  deleteDockAreas();

  if (m_counted) {
    m_counted = false;

    Thread::removeFrame(this);
  }
}

bool Frame::preTranslateMessage(Message& message)
{
  MSG& msg(*(LPMSG)message);

  if (m_menuBar != NULL) {
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
	  onCommand(menuItem->getId());
	}

	return true;
      }
    }
  }

  return Widget::preTranslateMessage(message);
}

/**
 * Adds space for the non-client size.
 *
 * @see getNonClientSize
 */
void Frame::onPreferredSize(Size& sz)
{
  Size ncSize = getNonClientSize();

  if (sz.w > 0 || sz.h > 0) {
    sz = Size(max_value(0, sz.w - ncSize.w),
	      max_value(0, sz.h - ncSize.h));
  }

  Widget::onPreferredSize(sz);
  sz += ncSize;
}

/**
 * Calls the layout() method.
 */
void Frame::onResize(const Size& sz)
{
  Widget::onResize(sz);
  layout();
}

/**
 * Convert a WM_COMMAND notification from a menus to a
 * MenuItem::onAction event.
 */
bool Frame::onCommand(CommandId id)
{
  if (Widget::onCommand(id))
    return true;

  // use menu bar
  if (m_menuBar != NULL) {
    MenuItem* menuItem = m_menuBar->getMenuItemById(id);

    VACA_TRACE("Frame::onCommand(%d), menuItem=%p\n", id, menuItem);

    if (menuItem != NULL) {
      MenuItemEvent ev(menuItem);
      menuItem->onAction(ev);
    }
  }

  return false;
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
 * Event called when the Frame want to be closed.
 *
 * The default implementation fire the signal Frame#Close,
 * and hide the widget (#setVisible(false)). If you cancel
 * the @a ev event, the window'll not hidden.
 *
 * The next example shows how to avoid to hide the Frame when the user
 * press the close button:
 * @code
 * class MyFrame : public Frame
 * {
 * public:
 *   ...
 *   virtual void onClose(WidgetEvent& ev) {
 *     Frame::onClose(ev); // it fires the Close signal
 *     ev.cancel();        // cancel the event (don't hide the frame)
 *   }
 * }
 * @endcode
 *
 * @win32
 *   This event is generated by the @msdn{WM_CLOSE} Message.
 * @endwin32
 */
void Frame::onClose(CloseEvent& ev)
{
  // fire Close signal
  Close(ev);
}

/**
 * @param edge From where the user is resizing.
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
    Menu::Container children = m_menuBar->getMenuItems();

    // update menus
    for (Menu::Container::iterator it=children.begin(); it!=children.end(); ++it) {
      MenuItem* menuItem = *it;
      updateMenuItem(menuItem);
    }

    ::DrawMenuBar(getHandle());
  }

  // // update tool-bars
  // for (std::vector<DockArea*>::iterator it=m_dockAreas.begin(); it!=m_dockAreas.end(); ++it) {
  //   DockArea* dockArea = *it;
  //   Container dockBars = dockArea->getChildren();
  //   for (std::vector<Widget*>::iterator it=dockBars.begin(); it!=dockBars.end(); ++it) {
  //     (*it)->updateIndicators();
  //   }
  // }
}

void Frame::onRemoveChild(Widget* child)
{
  Widget::onRemoveChild(child);

  if (m_statusBar == child)
    m_statusBar = NULL;
}

/**
 * Hides or shows the window. If visible is true, the window'll be
 * shown, if it's false the window'll be hidden.
 */
void Frame::setVisible(bool visible)
{
  HWND hwnd = getHandle();
  assert(::IsWindow(hwnd));

//   bool oldState = ::IsWindowVisible(hwnd) ? true: false;
//   bool oldState = isVisible();

  // synchronize all the group
  Container group = getSynchronizedGroup();
  for (Container::iterator it=group.begin(); it!=group.end(); ++it)
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

    // search for the StatusBar widget in the Frame
    Widget::Container children = getChildren();
    m_statusBar = NULL;
    for (Widget::Container::iterator
	   it=children.begin(); it!=children.end(); ++it) {
      if (StatusBar* statusBar = dynamic_cast<StatusBar*>(*it)) {
	// a Frame can't have two StatusBar
	assert(!m_statusBar);
	m_statusBar = statusBar;
      }
    }

    // layout children
    layout();

    // increment the frame's counter
    if (!m_counted) {
      m_counted = true;

      Thread::addFrame(this);
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

      Thread::removeFrame(this);
    }
  }
}

/**
 * Returns the window's menu bar. You can't delete it, the Frame
 * destructor does this for you. If you want to delete the current
 * menu bar, you should do something like this:
 *
 * @code
 *   ...
 *   MenuBar* oldMenuBar = myFrame->setMenuBar(NULL);
 *   if (oldMenuBar != NULL)
 *     delete oldMenuBar;
 *   ...
 * @endcode
 *
 * Or, if you are sure that the Frame has a MenuBar, you can do:
 *
 * @code
 *   ...
 *   delete myFrame->setMenuBar(NULL);
 *   ...
 * @endcode
 *
 * @see setMenuBar
 */
MenuBar* Frame::getMenuBar()
{
  return m_menuBar;
}

/**
 * Changes the window's menu bar. The @a menuBar pointer is deleted
 * automatically only if you don't setMenuBar(NULL) in some place
 * before the Frame's destructor.
 *
 * @return The old MenuBar (you should delete it). All Frames start
 *         without a menu bar (=NULL).
 *
 * @see getMenuBar, MdiFrame::setMenuBar, MenuBar::isAutoDelete, @ref page_tn_010
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
 * Sets the Icon used in the title bar.
 *
 * @warning It's like the Win32 WM_SETICON message.
 * 
 * @see #setBigIcon, #setIcon
 */
void Frame::setSmallIcon(const Icon& icon)
{
  m_smallIcon = icon;
  sendMessage(WM_SETICON, ICON_SMALL,
	      reinterpret_cast<LPARAM>(icon.getHandle()));
}

/**
 * Sets the Icon used in the ALT+TAB dialog box.
 *
 * @warning It's like the Win32 WM_SETICON message.
 * 
 * @see #setSmallIcon, #setIcon
 */
void Frame::setBigIcon(const Icon& icon)
{
  m_bigIcon = icon;
  sendMessage(WM_SETICON, ICON_BIG,
	      reinterpret_cast<LPARAM>(icon.getHandle()));
}

/**
 * Set the big icon (32x32) and the small icon (16x16) of the Frame
 * using the ICON resource speficied by @a iconId.
 *
 * @see #setSmallIcon, #setBigIcon
 */
void Frame::setIcon(ResourceId iconId)
{
  setSmallIcon(Icon(iconId, Size(32, 32)));
  setBigIcon(Icon(iconId, Size(16, 16)));
}

/**
 * Returns the Size of non-client area of the frame. It's useful to
 * known how much space of the rectangle passed to setBounds will be
 * used in non-client area.
 *
 * @see getBounds
 */
Size Frame::getNonClientSize()
{
  HWND hwnd = getHandle();
  assert(::IsWindow(hwnd));

  Rect clientRect(0, 0, 1, 1);
  RECT nonClientRect = clientRect;

  ::AdjustWindowRectEx(&nonClientRect,
		       GetWindowLong(hwnd, GWL_STYLE),
		       m_menuBar ? true: false,
		       GetWindowLong(hwnd, GWL_EXSTYLE));

  return Rect(&nonClientRect).getSize() - clientRect.getSize();
}

/**
 * The area where the Layout must arrange the children don't include
 * the dock areas.
 *
 * @see Widget::getLayoutBounds
 */
Rect Frame::getLayoutBounds() const
{
  Rect rc = Widget::getLayoutBounds();

  // if the Frame has a StatusBar the layout-bounds are reduced in the bottom
  if (m_statusBar)
    rc.h -= m_statusBar->getPreferredSize().h;

  for (std::vector<DockArea*>::const_iterator
	 it=m_dockAreas.begin(); it!=m_dockAreas.end(); ++it) {
    DockArea* dockArea = *it;
    Size dockSize = dockArea->getPreferredSize();

    switch (dockArea->getSide()) {
      case Side::Left:
	rc.x += dockSize.w;
	rc.w -= dockSize.w;
	break;
      case Side::Top:
	rc.y += dockSize.h;
	rc.h -= dockSize.h;
	break;
      case Side::Right:
	rc.w -= dockSize.w;
	break;
      case Side::Bottom:
	rc.h -= dockSize.h;
	break;
    }
  }

  return rc;
}

/**
 * Adds a new DockArea in the Frame.
 * 
 * @param dockArea It'll be automatically deleted in Frame's
 *                 destructor or deleteDockAreas. (if you don't
 *                 want it, you should use removeDockArea).
 *
 * @see @ref page_tn_010
 */
void Frame::addDockArea(DockArea* dockArea)
{
  m_dockAreas.push_back(dockArea);
}

/**
 * Remove the @a dockArea from the Frame. You should delete
 * the @a dockArea pointer.
 */
void Frame::removeDockArea(DockArea* dockArea)
{
  remove_from_container(m_dockAreas, dockArea);
}

/**
 * Setups the default dock areas for the Frame window.
 *
 * @warning This isn't called automatically in the Frame's
 *          constructor.  If you want to use dockable ToolBars, you
 *          @b must to call this routine before (or you can setup your
 *          own DockAreas with addDockArea())
 */
void Frame::defaultDockAreas()
{
  deleteDockAreas();

  addDockArea(new BandedDockArea(Side::Top, this));
  addDockArea(new BandedDockArea(Side::Bottom, this));
  addDockArea(new BandedDockArea(Side::Left, this));
  addDockArea(new BandedDockArea(Side::Right, this));
}

/**
 * Deletes all the registered DockAreas.
 */
void Frame::deleteDockAreas()
{
  for (std::vector<DockArea*>::iterator it=m_dockAreas.begin(); it!=m_dockAreas.end(); ++it) {
    DockArea* dockArea = *it;
    removeChild(dockArea);
    delete dockArea;
  }

  m_dockAreas.clear();
}

/**
 * Returns the collection of registered DockAreas in the Frame.
 */
std::vector<DockArea*> Frame::getDockAreas()
{
  return m_dockAreas;
}

/**
 * Returns the first DockArea in the specified @a side. Returns NULL
 * if there aren't registered a DockArea for the specified @a side.
 */
DockArea* Frame::getDockArea(Side side)
{
  for (std::vector<DockArea*>::iterator it=m_dockAreas.begin(); it!=m_dockAreas.end(); ++it) {
    DockArea* dockArea = *it;

    if (dockArea->getSide() == side)
      return dockArea;
  }

  return NULL;
}

/**
 * Returns a default DockArea for the floating DockBar that want to be
 * docked by double-click by first-time. The default implementation
 * returns the first DockArea that you added using addDockArea(); if
 * you use defaultDockAreas(), it will be the bar from the top side.
 */
DockArea* Frame::getDefaultDockArea()
{
  if (!m_dockAreas.empty())
    return m_dockAreas.front();
  else
    return NULL;
}

void Frame::layout()
{
  Widget::layout();

  // get the bounding rectangle where is the layout
  Rect clientRect = getClientBounds();
  Rect layoutRect = getLayoutBounds();

  // if the Frame has a StatusBar the clientRect is reduced in the bottom
  if (m_statusBar) {
    Size pref = m_statusBar->getPreferredSize();

    m_statusBar->setBounds(Rect(clientRect.x,
    				clientRect.y+clientRect.h-pref.h,
    				clientRect.w,
				pref.h));

    clientRect.h -= pref.h;
  }

  // put the dock areas arround the layout bounds
  for (std::vector<DockArea*>::const_iterator
	 it=m_dockAreas.begin(); it!=m_dockAreas.end(); ++it) {
    DockArea* dockArea = *it;
    // Size dockSize = dockArea->getBounds().getSize();
    Size dockSize = dockArea->getPreferredSize();

    switch (dockArea->getSide()) {

      case Side::Left:
	dockArea->setBounds(Rect(Point(clientRect.x, layoutRect.y),
				 Size(dockSize.w, layoutRect.h)));
	break;

      case Side::Top:
	dockArea->setBounds(Rect(Point(clientRect.x, clientRect.y),
				 Size(clientRect.w, dockSize.h)));
	break;

      case Side::Right:
	dockArea->setBounds(Rect(Point(layoutRect.x+layoutRect.w, layoutRect.y),
				 Size(dockSize.w, layoutRect.h)));
	break;

      case Side::Bottom:
	dockArea->setBounds(Rect(Point(clientRect.x, layoutRect.y+layoutRect.h),
				 Size(clientRect.w, dockSize.h)));
	break;
    }

    // dockArea->onResize(dockArea->getBounds().getSize());
    dockArea->layout();
  }
}

bool Frame::isLayoutFree()
{
  return true;
}

/**
 * You can customize this method to return true, so the Frame's
 * enabled-and-visible-state will keep synchronized to the
 * enabled-and-visible-state of its parent.
 *
 * @see getSynchronizedGroup
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
 * Returns the collection of frames that are synchronized to its
 * parent (see keepSynchronized). If this routine returns true, the
 * enabled-and-visible-state of the Frame will be synchronized with
 * the state of its parent (for example DockFrame).
 *
 * @see keepSynchronized
 */
Widget::Container Frame::getSynchronizedGroup()
{
  Container children = getChildren();
  Container container;

  for (Container::iterator it=children.begin(); it!=children.end(); ++it) {
    Frame* child = dynamic_cast<Frame*>(*it);
    if (child != NULL && child->keepSynchronized())
      container.push_back(child);
  }

  return container;
}

// // Used when WM_INITMENU or WM_INITMENUPOPUP message is received to
// // find the Menu of the m_menuBar, known only its HMENU.
// Menu* Frame::getMenuByHMENU(HMENU hmenu)
// {
//   MenuBar* menuBar = getMenuBar();
//   Menu* lastMenu = NULL;

//   std::stack<Menu*> stack;
//   stack.push(menuBar);

//   while (!stack.empty()) {
//     lastMenu = stack.top();
//     if (lastMenu->getHandle() == hmenu)
//       return lastMenu;

//     stack.pop();

//     Menu::Container subMenus = lastMenu->getMenuItems();
//     for (Menu::Container::iterator it=subMenus.begin(); it!=subMenus.end(); ++it) {
//       if ((*it)->isMenu())
// 	stack.push(static_cast<Menu*>(*it));
//     }
//   }

//   return NULL;
// }

// #ifdef MINGW
// typedef struct {
//   RECT rgrc[3];
//   PWINDOWPOS lppos;
// } NCCALCSIZE_PARAMS, *LPNCCALCSIZE_PARAMS;
// #endif

/**
 * This method converts next messages to events:
 * @li @c WM_ACTIVATE -&gt; onActivate()
 * @li @c WM_CLOSE -&gt; onClose()
 * @li @c WM_SIZING -&gt; onResizing()
 *
 * When the event generated by WM_CLOSE message isn't cancelled, the
 * Frame is automatically hidden.
 */
bool Frame::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
  if (Widget::wndProc(message, wParam, lParam, lResult))
    return true;

  switch (message) {

    case WM_CLOSE: {
      // fire onClose event
      CloseEvent ev(this);
      onClose(ev);

      // default behaviour: when the event isn't cancelled, we must to
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

// #if 0
//       // work arround for Win95, Win98, WinMe...
//       if (System::isWin95_98_Me()) {
// 	menu = m_menuBar != NULL ? m_menuBar->getMenuByHMENU(hmenu): NULL;
//       }
//       // in Win2K, WinXP, we can use MENUITEMINFO
//       else if (System::isWinNT_2K_XP()) {
// #endif
	MENUINFO mi;
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_MENUDATA;
	if (::GetMenuInfo(hmenu, &mi))
	  menu = reinterpret_cast<Menu* >(mi.dwMenuData);
// #if 0
//       }
// #endif

      if (menu != NULL) {
	Menu::Container children = menu->getMenuItems();

	// update menus
	for (Menu::Container::iterator it=children.begin(); it!=children.end(); ++it) {
	  MenuItem* menuItem = *it;
	  updateMenuItem(menuItem);
	}
      }
      break;
    }

    case WM_SIZING: {
      LPRECT lprc = reinterpret_cast<LPRECT>(lParam);
      Rect rc(lprc);
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
      *lprc = rc;
      lResult = TRUE;
      return true;
    }
	
      /*
	case WM_NCCALCSIZE:
	if (wParam) {
	LPNCCALCSIZE_PARAMS lpncsp = (LPNCCALCSIZE_PARAMS)lParam;
	int left, top, right, bottom;

	lResult = DefWindowProc(getHandle(), message, wParam, lParam);

	left = top = right = bottom = 0;

	for (std::vector<DockArea*>::iterator it=m_dockAreas.begin(); it!=m_dockAreas.end(); ++it) {
	DockArea* dockArea = *it;
	// 	    Widget::Container widgets = dockArea->getChildren();
	Size sz = dockArea->preferredSize();
	    
	switch (dockArea->getSide()) {
	case LeftSide:   left   += sz.w; break;
	case TopSide:    top    += sz.h; break;
	case RightSide:  right  += sz.w; break;
	case BottomSide: bottom += sz.h; break;
	}
	}

	lpncsp->rgrc[0].left   += left;
	lpncsp->rgrc[0].top    += top;
	lpncsp->rgrc[0].right  -= right;
	lpncsp->rgrc[0].bottom -= bottom;

	return true;
	}
	break;
      */

    case WM_ENABLE: {
      Container group = getSynchronizedGroup();
      HWND hParam  = reinterpret_cast<HWND>(lParam);

      // synchronize all the group
      for (Container::iterator it=group.begin(); it!=group.end(); ++it) {
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

	// this can happend in the last WM_NCACTIVATE message (when
	// the widget is destroyed)
// 	if (owner == reinterpret_cast<Frame*>(NULL))
// 	  return false;
	assert(owner != NULL);

	bool keepActive = wParam != 0;
	bool syncGroup = true;
	HWND hParam = reinterpret_cast<HWND>(lParam);

	Container group = owner->getSynchronizedGroup();
	group.push_back(owner);

	// if the other window to be activated/desactivated belong to
	// the synchronized group, we don't need to
	// synchronize/repaint all the group
	for (Container::iterator it=group.begin(); it!=group.end(); ++it) {
	  Widget* child = *it;
	  if (child->getHandle() == hParam) {
	    keepActive = true;
	    syncGroup = false;
	    break;
	  }
	}

	// synchronize the group
	if (syncGroup) {
	  for (Container::iterator it=group.begin(); it!=group.end(); ++it) {
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
