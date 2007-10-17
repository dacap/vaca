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
#include "Vaca/Mutex.h"
#include "Vaca/ScopedLock.h"
#include "Vaca/BandedDockArea.h"
#include "Vaca/Icon.h"
#include "Vaca/Command.h"

using namespace Vaca;

// the "frm" isn't used (TODO check getCurrent != NULL)
#define ADD_FRAME(frm)		Thread::getCurrent()->addFrame()
#define REMOVE_FRAME(frm)	Thread::getCurrent()->removeFrame()

// #define ADD_FRAME(frm)
//       visibleFramesMutex.lock();
//       visibleFrames.push_back(frm);
//       visibleFramesMutex.unlock();

// #define REMOVE_FRAME(frm)
//     visibleFramesMutex.lock();
//     remove_element_from_container(visibleFrames, frm);
//     visibleFramesMutex.unlock();

// static Mutex visibleFramesMutex; // used to access "visibleFrames"
// static std::list<Frame *> visibleFrames; // visible frames of all threads

/**
 * Creates a frame using the FrameClass.  Also, remember that by
 * default a Frame hasn't a Layout manager.
 *
 * @see onClose()
 */
Frame::Frame(const String &title, Widget *parent, Style style)
  : Widget(FrameClass::getClassName(), parent, style)
{
  initialize(title);
}

/**
 * Creates a frame with a custom WNDCLASS. @a className can be NULL if
 * you want to call Widget::create() by your self.
 */
Frame::Frame(LPCTSTR className, const String &title, Widget *parent, Style style)
  : Widget(className, parent, style)
{
  initialize(title);
}

void Frame::initialize(const String &title)
{
  m_menuBar = NULL;
  m_counted = false;

  if (::IsWindow(getHWND()))
    setText(title);
}

Frame::~Frame()
{
  if (m_menuBar != NULL//  && m_menuBar->isAutoDelete()
      ) {
    delete m_menuBar;
    m_menuBar = NULL;
  }

  for (std::vector<Command *>::iterator it = m_commands.begin();
       it != m_commands.end();
       ++it) {
    Command *command = *it;
    delete command;
  }

  deleteDockAreas();

  if (m_counted) {
    m_counted = false;

    REMOVE_FRAME(this);
  }
}

bool Frame::preTranslateMessage(MSG &msg)
{
  if (m_menuBar != NULL) {
    // TODO accelerators
    if (msg.message == WM_KEYDOWN) {
      Keys::Type keys = Keys::fromMessageParams(msg.wParam, msg.lParam);
      MenuItem *menuItem = m_menuBar->checkShortcuts(keys);

      if (menuItem != NULL) {
	// update the menuItem status before (onUpdate stuff)
	updateMenuItem(menuItem);

	// is menuItem enabled?
	if (menuItem->isEnabled()) {
	  // ok, we can do the action
	  onIdAction(menuItem->getId());
	}

	return true;
      }
    }
  }

  return Widget::preTranslateMessage(msg);
}

void Frame::onPreferredSize(Size &sz)
{
  Size ncSize = getNonClientSize();

  if (sz.w > 0 || sz.h > 0) {
    sz = Size(VACA_MAX(0, sz.w - ncSize.w),
	      VACA_MAX(0, sz.h - ncSize.h));
  }

  Widget::onPreferredSize(sz);
  sz += ncSize;
}

/**
 * Calls the layout() method.
 */
void Frame::onResize(const Size &sz)
{
  layout();
}

/**
 * Convert a WM_COMMAND notification from a menus to a
 * MenuItem::onAction event.
 */
bool Frame::onIdAction(int id)
{
  if (Widget::onIdAction(id))
    return true;

  // use menu bar
  if (m_menuBar != NULL) {
    MenuItem *menuItem = m_menuBar->getMenuItemById(id);

    VACA_TRACE("Frame::onIdAction(%d), menuItem=%p\n", id, menuItem);

    if (menuItem != NULL) {
      MenuItemEvent ev(menuItem);
      menuItem->onAction(ev);
    }
    else {
      // TODO ToolBars
      // TODO Mdi stuff
    }
  }

  // call commands with the given ID
  for (std::vector<Command *>::iterator it = m_commands.begin();
       it != m_commands.end();
       ++it) {
    if ((*it)->getId() == id)
      (*it)->onAction();
  }

  return false;
}

// void Frame::onKeyDown(KeyEvent &ev)
// {
//   if (m_menuBar != NULL) {
//     MenuItem *menuItem = m_menuBar->checkShortcuts(ev.getKeys());
//     if (menuItem != NULL)
//       menuItem->Action();
//   }
// }

void Frame::onActivate(Event &ev)
{
  Activate(ev);
}

void Frame::onDeactivate(Event &ev)
{
  Deactivate(ev);
}

/**
 * Event called when the Frame want to be closed (WM_CLOSE). The
 * default implementation fire the signal Frame::Close.
 *
 * By default when a Frame is closed, it'll be hidden. If you cancel
 * the @a ev event, the window'll not hidden.
 *
 * Example of how to avoid to hide the Frame when the user press the
 * close button:
 *
 * @code
 * class MyFrame : public Frame
 * {
 * public:
 *   ...
 *   virtual void onClose(WidgetEvent &ev) {
 *     Frame::onClose(ev); // it fires the Close signal
 *     ev.cancel();        // cancel the event (don't hide the frame)
 *   }
 * }
 * @endcode
 */
void Frame::onClose(CloseEvent &ev)
{
  // fire Close signal
  Close(ev);
}

/**
 * @param edge Can be one of the following values:
 * @li WMSZ_LEFT
 * @li WMSZ_RIGHT
 * @li WMSZ_TOP
 * @li WMSZ_TOPLEFT
 * @li WMSZ_TOPRIGHT
 * @li WMSZ_BOTTOM
 * @li WMSZ_BOTTOMLEFT
 * @li WMSZ_BOTTOMRIGHT
 */
void Frame::onResizing(int edge, Rect &rc)
{
  // do nothing
}

#if 0
/**
 * Returns true if the windows is currently visible by the user.
 */
bool Frame::isVisible()
{
  // this is visibility for frame, for this frame, I don't want to known about the visibility of the parents
  return (getStyle() & WS_VISIBLE) == WS_VISIBLE ? true: false;
}
#endif

/**
 * Hides or shows the window. If visible is true, the window'll be
 * shown, if it's false the window'll be hidden.
 */
void Frame::setVisible(bool visible)
{
  HWND hwnd = getHWND();
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

    if ((getStyle() & InitiallyMinimizedFrameStyle) == InitiallyMinimizedFrameStyle)
      swCmd = SW_MINIMIZE;
    else if ((getStyle() & InitiallyMaximizedFrameStyle) == InitiallyMaximizedFrameStyle)
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

      ADD_FRAME(this);
    }
  }
  // Hide the window. It looks simple, but the order of the commands
  // is critical: when the active window is hidden, another window (of
  // other application!!) is activated. We can fix it activating first
  // the parent, and then hide the non-active window.
  else {
    // activate the parent
    if (getParent() != NULL)
      ::SetActiveWindow(getParent()->getHWND());

    // then hide this non-active window
    ::ShowWindow(hwnd, SW_HIDE);

    // decrement the frame's counter
    if (m_counted) {
      m_counted = false;

      REMOVE_FRAME(this);
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
 *   MenuBar *oldMenuBar = myFrame->setMenuBar(NULL);
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
MenuBar *Frame::getMenuBar()
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
 * @see getMenuBar, MdiFrame::setMenuBar, MenuBar::isAutoDelete, @ref TN010
 */
MenuBar *Frame::setMenuBar(MenuBar *menuBar)
{
  HWND hwnd = getHWND();
  HMENU hmenu = menuBar->getHMENU();
  
  assert(hwnd != NULL && hmenu != NULL);

  ::SetMenu(hwnd, hmenu); // TODO check errors

  MenuBar *oldMenuBar = m_menuBar;
  m_menuBar = menuBar;
  return oldMenuBar;
}

/**
 * Sets the Icon used in the title bar (if @a bigIcon is false) or in
 * the ALT+TAB dialog box (is @a bigIcon is true) (it's like the Win32
 * WM_SETICON message).
 * 
 * @see setIcon(int)
 */
void Frame::setIcon(Icon *icon, bool bigIcon)
{
  sendMessage(WM_SETICON, bigIcon ? ICON_BIG: ICON_SMALL,
	      reinterpret_cast<LPARAM>(icon->getHICON()));
}

/**
 * Set the big icon (32x32) and the small icon (16x16) of the Frame
 * using the ICON resource speficied by @a iconId.
 *
 * @see setIcon(Icon *, bool)
 */
void Frame::setIcon(int iconId)
{
  SharedIcon bigIcon(iconId, Size(32, 32));
  SharedIcon smallIcon(iconId, Size(16, 16));

  setIcon(&bigIcon, true);
  setIcon(&smallIcon, false);
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
  HWND hwnd = getHWND();
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
Rect Frame::getLayoutBounds()
{
  Rect rc = Widget::getLayoutBounds();

  for (std::vector<DockArea *>::iterator it=m_dockAreas.begin(); it!=m_dockAreas.end(); ++it) {
    DockArea *dockArea = *it;
    Size dockSize = dockArea->getPreferredSize();

    switch (dockArea->getSide()) {
      case LeftSide:
	rc.x += dockSize.w;
	rc.w -= dockSize.w;
	break;
      case TopSide:
	rc.y += dockSize.h;
	rc.h -= dockSize.h;
	break;
      case RightSide:
	rc.w -= dockSize.w;
	break;
      case BottomSide:
	rc.h -= dockSize.h;
	break;
    }
  }

  return rc;
}

/** 
 * Adds a new command to the Frame. The command is deleted
 * automatically in the Frame's destructor. If you want to avoid this,
 * you should to call removeCommand before.
 * 
 * @param command 
 *
 * @see @ref TN010
 */
void Frame::addCommand(Command *command)
{
  m_commands.push_back(command);
}

/** 
 * 
 * 
 * @param command 
 */
void Frame::removeCommand(Command *command)
{
  remove_element_from_container(m_commands, command);
}

/**
 * 
 * @param dockArea It'll be automatically deleted in Frame's
 *                 destructor or deleteDockAreas. (if you don't
 *                 want it, you should use removeDockArea).
 *
 * @see @ref TN010
 */
void Frame::addDockArea(DockArea *dockArea)
{
  m_dockAreas.push_back(dockArea);
}

/**
 * Remove the @a dockArea from the Frame. You should delete
 * the @a dockArea pointer.
 */
void Frame::removeDockArea(DockArea *dockArea)
{
  remove_element_from_container(m_dockAreas, dockArea);
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

  addDockArea(new BandedDockArea(TopSide, this));
  addDockArea(new BandedDockArea(BottomSide, this));
  addDockArea(new BandedDockArea(LeftSide, this));
  addDockArea(new BandedDockArea(RightSide, this));
}

/**
 * Deletes all the registered DockAreas.
 */
void Frame::deleteDockAreas()
{
  for (std::vector<DockArea *>::iterator it=m_dockAreas.begin(); it!=m_dockAreas.end(); ++it) {
    DockArea *dockArea = *it;

    dockArea->setVisible(false);
    // removeChild(dockArea, false);
    removeChild(dockArea, true);
    delete dockArea;
  }

  m_dockAreas.clear();
}

/**
 * Returns the collection of registered DockAreas in the Frame.
 */
std::vector<DockArea *> Frame::getDockAreas()
{
  return m_dockAreas;
}

/**
 * Returns the first DockArea in the specified @a side. Returns NULL
 * if there aren't registered a DockArea for the specified @a side.
 */
DockArea *Frame::getDockArea(Side side)
{
  for (std::vector<DockArea *>::iterator it=m_dockAreas.begin(); it!=m_dockAreas.end(); ++it) {
    DockArea *dockArea = *it;

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
DockArea *Frame::getDefaultDockArea()
{
  if (!m_dockAreas.empty())
    return m_dockAreas.front();
  else
    return NULL;
}

// Size Frame::preferredSize()
// {
//   return getNonClientSize() + Widget::preferredSize();
// }

// Size Frame::preferredSize(const Size &fitIn)
// {
//   Size ncSize(getNonClientSize());

//   return
//     ncSize +
//     Widget::preferredSize(Size(VACA_MAX(0, fitIn.w - ncSize.w),
// 			       VACA_MAX(0, fitIn.h - ncSize.h)));
// }

void Frame::layout()
{
  Widget::layout();

  // get the bounding rectangle where is the layout
  Rect clientRect = getClientBounds();
  Rect layoutRect = getLayoutBounds();

  // put the dock areas arround the layout bounds
  for (std::vector<DockArea *>::iterator it=m_dockAreas.begin(); it!=m_dockAreas.end(); ++it) {
    DockArea *dockArea = *it;
    // Size dockSize = dockArea->getBounds().getSize();
    Size dockSize = dockArea->getPreferredSize();

    switch (dockArea->getSide()) {

      case LeftSide:
	dockArea->setBounds(Rect(Point(clientRect.x, layoutRect.y),
				 Size(dockSize.w, layoutRect.h)));
	break;

      case TopSide:
	dockArea->setBounds(Rect(Point(clientRect.x, clientRect.y),
				 Size(clientRect.w, dockSize.h)));
	break;

      case RightSide:
	dockArea->setBounds(Rect(Point(layoutRect.x+layoutRect.w, layoutRect.y),
				 Size(dockSize.w, layoutRect.h)));
	break;

      case BottomSide:
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

void Frame::updateMenuItem(MenuItem *menuItem)
{
  assert(menuItem != NULL);
  
  MenuItemEvent ev(menuItem);
  menuItem->onUpdate(ev);

  CommandState cmdState;

  for (std::vector<Command *>::iterator it = m_commands.begin();
       it != m_commands.end();
       ++it) {
    Command *command = *it;
    if (command->getId() == menuItem->getId())
      command->onUpdate(cmdState);
  }

  if (cmdState.getText()   != NULL) menuItem->setText   (*cmdState.getText());
  if (cmdState.isEnabled() != NULL) menuItem->setEnabled(*cmdState.isEnabled());
  if (cmdState.isChecked() != NULL) menuItem->setChecked(*cmdState.isChecked());
  if (cmdState.isRadio()   != NULL) menuItem->setRadio  (*cmdState.isRadio());
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
    Frame *child = dynamic_cast<Frame *>(*it);
    if (child != NULL && child->keepSynchronized())
      container.push_back(child);
  }

  return container;
}

// int Frame::getFramesCount()
// {
//   // return framesCount;
//   ScopedLock lock(visibleFramesMutex);
//   int count = visibleFrames.size();
//   return count;
// }

// int Frame::getVisibleFramesByThread(int threadId)
// {
//   ScopedLock lock(visibleFramesMutex);
//   int count = 0;

//   for (std::list<Frame *>::iterator it=visibleFrames.begin();
//        it!=visibleFrames.end(); ++it)
//     if ((*it)->getThreadOwnerId() == threadId)
//       count++;

//   return count;
// }

/**
 * Used when WM_INITMENU or WM_INITMENUPOPUP message is received to
 * find the Menu of the m_menuBar, known only its HMENU.
 */
// Menu *Frame::getMenuByHMENU(HMENU hmenu)
// {
//   MenuBar *menuBar = getMenuBar();
//   Menu *lastMenu = NULL;

//   std::stack<Menu *> stack;
//   stack.push(menuBar);

//   while (!stack.empty()) {
//     lastMenu = stack.top();
//     if (lastMenu->getHMENU() == hmenu)
//       return lastMenu;

//     stack.pop();

//     Menu::Container subMenus = lastMenu->getMenuItems();
//     for (Menu::Container::iterator it=subMenus.begin(); it!=subMenus.end(); ++it) {
//       if ((*it)->isMenu())
// 	stack.push(static_cast<Menu *>(*it));
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
 *
 * This method converts next messages to events:
 * @li @c WM_ACTIVATE -&gt; onActivate()
 * @li @c WM_CLOSE -&gt; onClose()
 * @li @c WM_SIZING -&gt; onResizing()
 *
 * When the event generated by WM_CLOSE message isn't cancelled, the
 * Frame is automatically hidden.
 */
bool Frame::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
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

      Menu *menu = NULL;

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
	  menu = reinterpret_cast<Menu *>(mi.dwMenuData);
// #if 0
//       }
// #endif

      if (menu != NULL) {
	Menu::Container children = menu->getMenuItems();

	// update menus
	for (Menu::Container::iterator it=children.begin(); it!=children.end(); ++it) {
	  MenuItem *menuItem = *it;
	  updateMenuItem(menuItem);
	}
      }
      break;
    }

    case WM_SIZING: {
      LPRECT lprc = reinterpret_cast<LPRECT>(lParam);
      Rect rc(lprc);
      onResizing(wParam, rc);
      *lprc = rc;
      lResult = TRUE;
      return true;
    }
	
      /*
	case WM_NCCALCSIZE:
	if (wParam) {
	LPNCCALCSIZE_PARAMS lpncsp = (LPNCCALCSIZE_PARAMS)lParam;
	int left, top, right, bottom;

	lResult = DefWindowProc(getHWND(), message, wParam, lParam);

	left = top = right = bottom = 0;

	for (std::vector<DockArea *>::iterator it=m_dockAreas.begin(); it!=m_dockAreas.end(); ++it) {
	DockArea *dockArea = *it;
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
	HWND hwndChild = (*it)->getHWND();

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
	Frame *owner = keepSynchronized() ? dynamic_cast<Frame *>(getParent()): this;

	// this can happend in the last WM_NCACTIVATE message (when
	// the widget is destroyed)
// 	if (owner == reinterpret_cast<Frame *>(NULL))
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
	  Widget *child = *it;
	  if (child->getHWND() == hParam) {
	    keepActive = true;
	    syncGroup = false;
	    break;
	  }
	}

	// synchronize the group
	if (syncGroup) {
	  for (Container::iterator it=group.begin(); it!=group.end(); ++it) {
	    Widget *child = *it;
	    if ((child->getHWND() != getHWND()) &&
		(child->getHWND() != hParam))
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
