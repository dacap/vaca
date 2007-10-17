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
 * default a Frame hasn't a Layout manager. Frames are automatically
 * disposed when them are closed.
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
  mMenuBar = NULL;
  mCounted = false;

  if (getHWND() != NULL)
    setText(title);
}

Frame::~Frame()
{
  if (mMenuBar != NULL && mMenuBar->isAutoDelete()) {
    delete mMenuBar;
    mMenuBar = NULL;
  }

  for (std::vector<Command *>::iterator it = mCommands.begin();
       it != mCommands.end();
       ++it) {
    Command *command = *it;
    delete command;
  }

  // see TN002
  dispose();
}

bool Frame::preTranslateMessage(MSG &msg)
{
  if (mMenuBar != NULL) {
    // TODO accelerators
    if (msg.message == WM_KEYDOWN) {
      Keys::Type keys = Keys::fromMessageParams(msg.wParam, msg.lParam);
      MenuItem *menuItem = mMenuBar->checkShortcuts(keys);

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

void Frame::onDestroy()
{
  clearDockAreas();

  // Application::instance()->_closeFrame(this);
  if (mCounted) {
    mCounted = false;

    REMOVE_FRAME(this);
  }

  Widget::onDestroy();
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
  if (mMenuBar != NULL) {
    MenuItem *menuItem = mMenuBar->getMenuItemById(id);

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
  for (std::vector<Command *>::iterator it = mCommands.begin();
       it != mCommands.end();
       ++it) {
    if ((*it)->getId() == id)
      (*it)->onAction();
  }

  return false;
}

// void Frame::onKeyDown(KeyEvent &ev)
// {
//   if (mMenuBar != NULL) {
//     MenuItem *menuItem = mMenuBar->checkShortcuts(ev.getKeys());
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
 * By default when a Frame is closed, it's disposed. If onClose(), or
 * some slot in Close, cancels the event @a ev, the window is not
 * disposed.
 *
 * Example of how to hide the Frame (but not to destroy it) when the
 * user press the close button:
 *
 * @code
 * class MyFrame : public Frame
 * {
 * public:
 *   ...
 *   virtual void onClose(WidgetEvent &ev) {
 *     Frame::onClose(ev); // it fires the Close signal
 *
 *     setVisible(false);  // hide the window
 *     ev.cancel();        // cancel the event (don't dispose the frame)
 *   }
 * }
 * @endcode
 *
 * The above method is used by Dialog class to avoid dispose the
 * dialog when it's closed.
 *
 * @warning After the onClose event, if you don't cancel the @a ev,
 * the Frame'll be disposed automatically through
 * @ref Widget::dispose "dispose()".
 */
void Frame::onClose(CloseEvent &ev)
{
  // fire close signal
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
  assert(hwnd != NULL);

//   bool oldState = ::IsWindowVisible(hwnd) ? true: false;
  bool oldState = isVisible();

  // Show the window
  if (visible) {
    int swCmd = SW_SHOWNORMAL;

    if ((getStyle() & InitiallyMinimizedFrameStyle) == InitiallyMinimizedFrameStyle)
      swCmd = SW_MINIMIZE;
    else if ((getStyle() & InitiallyMaximizedFrameStyle) == InitiallyMaximizedFrameStyle)
      swCmd = SW_MAXIMIZE;
    
    ::ShowWindow(hwnd, swCmd);
    ::UpdateWindow(hwnd);
    if (mMenuBar != NULL)
      ::DrawMenuBar(hwnd);

    // layout children
    layout();

    // increment the frame's counter
    if (!mCounted) {
      mCounted = true;

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
    if (mCounted) {
      mCounted = false;

      REMOVE_FRAME(this);
    }
  }
}

/**
 * Hides the window, closes it, destroys it, but doesn't delete it. If
 * you use setVisible(false) you just are hidding the window. One time
 * you use the close method, you can't do a setVisible(true) because
 * the windows doesn't exist anymore.
 */
// bool Frame::close()
// {
//   HWND hwnd = getHWND();

//   WidgetEvent ev(this);
//   sigClose(ev);
//   if (!ev.isCanceled())
//     dispose();

//   return res;
// }

/**
 * Returns the window's menu bar. You can't delete it, the Frame
 * destructor does this for you.
 *
 * @see setMenuBar
 */
MenuBar *Frame::getMenuBar()
{
  return mMenuBar;
}

/**
 * Changes the window's menu bar. The @a menuBar pointer is deleted
 * automatically only if it "menuBar->isAutoDelete()" is true.
 *
 * @see getMenuBar, MdiFrame::setMenuBar, MenuBar::isAutoDelete
 */
void Frame::setMenuBar(MenuBar *menuBar)
{
  // delete the old menu bar
  if (mMenuBar != NULL && mMenuBar->isAutoDelete()) {
    delete mMenuBar;
    mMenuBar = NULL;
  }

  // change to the new menu bar
  HWND hwnd = getHWND();
  HMENU hmenu = menuBar->getHMENU();
  
  assert(hwnd != NULL && hmenu != NULL);

  ::SetMenu(hwnd, hmenu); // TODO check errors

  mMenuBar = menuBar;
}

/**
 * Sets the Icon used in the title bar (if @a bigIcon is false) or in the
 * ALT+TAB dialog box (is @a bigIcon is true) (Win32 WM_SETICON message).
 */
void Frame::setIcon(Icon *icon, bool bigIcon)
{
  sendMessage(WM_SETICON, bigIcon ? ICON_BIG: ICON_SMALL,
	      reinterpret_cast<LPARAM>(icon->getHICON()));
}

/**
 * Set the big icon (32x32) and the small icon (16x16) of the Frame
 * using the ICON resource speficied by @a iconId.
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
  assert(hwnd != NULL);

  Rect clientRect(0, 0, 1, 1);
  RECT nonClientRect = clientRect;

  ::AdjustWindowRectEx(&nonClientRect,
		       GetWindowLong(hwnd, GWL_STYLE),
		       mMenuBar ? true: false,
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

  for (std::vector<DockArea *>::iterator it=mDockAreas.begin(); it!=mDockAreas.end(); ++it) {
    DockArea *dockArea = *it;
    Size dockSize = dockArea->preferredSize();

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

void Frame::addCommand(Command *command)
{
  mCommands.push_back(command);
}

void Frame::removeCommand(Command *command)
{
  remove_element_from_container(mCommands, command);
}

/**
 * 
 * @param dockArea It'll be automatically deleted in dispose(). (if you don't want
 *                 it, you should use removeDockArea)
 *
 * @see @ref TN010
 */
void Frame::addDockArea(DockArea *dockArea)
{
  mDockAreas.push_back(dockArea);
}

/**
 * Remove the @a dockArea from the Frame. You should delete the @a
 * dockArea pointer.
 */
void Frame::removeDockArea(DockArea *dockArea)
{
  remove_element_from_container(mDockAreas, dockArea);
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
  clearDockAreas();

  addDockArea(new BandedDockArea(TopSide, this));
  addDockArea(new BandedDockArea(BottomSide, this));
  addDockArea(new BandedDockArea(LeftSide, this));
  addDockArea(new BandedDockArea(RightSide, this));
}

/**
 * Clears all the registered DockAreas.
 */
void Frame::clearDockAreas()
{
  for (std::vector<DockArea *>::iterator it=mDockAreas.begin(); it!=mDockAreas.end(); ++it) {
    DockArea *dockArea = *it;

    dockArea->setVisible(false);
    removeChild(dockArea, false);
    delete dockArea;
  }

  mDockAreas.clear();
}

/**
 * Returns the collection of registered DockAreas in the Frame.
 */
std::vector<DockArea *> Frame::getDockAreas()
{
  return mDockAreas;
}

/**
 * Returns the first DockArea in the specified @a side. Returns NULL
 * if there aren't registered a DockArea for the specified @a side.
 */
DockArea *Frame::getDockArea(Side side)
{
  for (std::vector<DockArea *>::iterator it=mDockAreas.begin(); it!=mDockAreas.end(); ++it) {
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
  if (!mDockAreas.empty())
    return mDockAreas.front();
  else
    return NULL;
}

Size Frame::preferredSize()
{
  return getNonClientSize() + Widget::preferredSize();
}

Size Frame::preferredSize(const Size &fitIn)
{
  Size ncSize(getNonClientSize());

  return
    ncSize +
    Widget::preferredSize(Size(VACA_MAX(0, fitIn.w - ncSize.w),
			       VACA_MAX(0, fitIn.h - ncSize.h)));
}

void Frame::layout()
{
  Widget::layout();

  // get the bounding rectangle where is the layout
  Rect clientRect = getClientBounds();
  Rect layoutRect = getLayoutBounds();

  // put the dock areas arround the layout bounds
  for (std::vector<DockArea *>::iterator it=mDockAreas.begin(); it!=mDockAreas.end(); ++it) {
    DockArea *dockArea = *it;
    // Size dockSize = dockArea->getBounds().getSize();
    Size dockSize = dockArea->preferredSize();

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

bool Frame::wantArrowCursor()
{
  return true;
}

void Frame::updateMenuItem(MenuItem *menuItem)
{
  assert(menuItem != NULL);
  
  MenuItemEvent ev(menuItem);
  menuItem->onUpdate(ev);

  CommandState cmdState;

  for (std::vector<Command *>::iterator it = mCommands.begin();
       it != mCommands.end();
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
 * find the Menu of the mMenuBar, known only its HMENU.
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
 * Frame is automatically disposed.
 */
bool Frame::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
  if (Widget::wndProc(message, wParam, lParam, lResult))
    return true;

  switch (message) {

    case WM_CLOSE: {
      CloseEvent ev(this);
      onClose(ev);
      if (!ev.isCanceled())
	dispose();

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

#if 0
    case WM_MENUCOMMAND: {
//       HMENU hmenu = reinterpret_cast<HMENU>(lParam);
//       if (hmenu == NULL)
// 	break;
      HMENU hmenu = (HMENU)lParam;
      if (hmenu == NULL)
	break;

      // work arround for Win95, Win98, WinMe...
      if (System::isWin95_98_Me()) {
	// get the selected index (yes, in Win98 it's the
	// HIWORD(wParam), in Win2K it's the whole wParam)
	int index = HIWORD(wParam);

	Menu *menu = getMenuByHMENU(hmenu);

	VACA_TRACE("hmenu=%p menu=%p index=%d\n", hmenu, menu, index);

	if (menu != NULL) {
	  MenuItem *menuItem = menu->getMenuItem(index);

	  assert(menuItem != NULL);

	  MenuItemEvent ev(*menuItem);
	  menuItem->onAction(ev);
	}
      }
      // in Win2K, WinXP, we can use MENUITEMINFO
      else if (System::isWinNT_2K_XP()) {
	int index = wParam;

	VACA_TRACE("hmenu=%p index=%d\n", hmenu, index);

	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA | MIIM_ID;
	if (GetMenuItemInfo(hmenu, index, TRUE, &mii)) {
	  MenuItem *menuItem = reinterpret_cast<MenuItem *>(mii.dwItemData);

	  if (menuItem != NULL) {
	    MenuItemEvent ev(*menuItem);
	    menuItem->onAction(ev);
	  }
	  else {
	    // we must create the WM_COMMAND because we was notified
	    // about a menu (or sysmenu) action through its
	    // position, not by its ID

	    MdiFrame *mdiFrame = dynamic_cast<MdiFrame *>(this);
	    assert(mdiFrame != NULL);

	    // this is from the MdiListMenu? we must to activate
	    // other MdiChild...
	    if (mii.wID >= VACA_IDM_FIRST_MDICHILD) {
	      // count how many items has IDs (from end to begin)
	      int i, lastIndex = GetMenuItemCount(hmenu)-1;
	      for (i=lastIndex; i>=0 && GetMenuItemID(hmenu, i) != 0; i--);
	      int itemsWithIds = lastIndex - i;

	      // the last menu item was the selected item?
	      if (itemsWithIds == 10 &&
		  mii.wID == GetMenuItemID(hmenu, lastIndex)) {
		// show the "More windows" dialog
		mdiFrame->onMoreWindows();
	      }
	      // send the command message to the frame
	      else {
		// get the child with the ID of the menu
		MdiChild *mdiChild = mdiFrame->getMdiClient().getChildById(mii.wID);
		assert(mdiChild != NULL);

		// select that child
		sendMessage(WM_COMMAND,
			    MAKEWPARAM(mii.wID, 0),
			    reinterpret_cast<LPARAM>(mdiChild->getHWND()));
	      }

	      // 	      MENUINFO mi;
	      // 	      mi.cbSize = sizeof(MENUINFO);
	      // 	      mi.fMask = MIM_MENUDATA
	      // 	      if (GetMenuInfo(hmenu, &mi)) {
	      // 		Menu *menu = reinterpret_cast<Menu *>(mi.dwMenuData);
	      // 		if (menu != NULL &&
	      // 		    menu->isMdiList()) {
		  
	      // 		}
	      // 	      }
	    }
	    // this is called when a MdiChild is maximized and some
	    // syscommand is used, so we must to generate the
	    // WM_SYSCOMMAND message because we are notified by
	    // position, not by ID
	    else {
// 	      VACA_TRACE("SYSCOMMAND %d 0x%x\n", mii.wID, mii.wID);

	      MdiChild *mdiChild = mdiFrame->getMdiClient().getActive();
	      assert(mdiChild != NULL);

	      mdiChild->sendMessage(WM_SYSCOMMAND, mii.wID, 0);
	    }
	  }
	}
      }
      break;
    }
#endif

    case WM_INITMENU:
    case WM_INITMENUPOPUP: {
      HMENU hmenu = reinterpret_cast<HMENU>(wParam);
      if (hmenu == NULL)
	break;

      Menu *menu = NULL;

#if 0
      // work arround for Win95, Win98, WinMe...
      if (System::isWin95_98_Me()) {
	menu = mMenuBar != NULL ? mMenuBar->getMenuByHMENU(hmenu): NULL;
      }
      // in Win2K, WinXP, we can use MENUITEMINFO
      else if (System::isWinNT_2K_XP()) {
#endif
	MENUINFO mi;
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_MENUDATA;
	if (::GetMenuInfo(hmenu, &mi))
	  menu = reinterpret_cast<Menu *>(mi.dwMenuData);
#if 0
      }
#endif

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

	for (std::vector<DockArea *>::iterator it=mDockAreas.begin(); it!=mDockAreas.end(); ++it) {
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

  }

  return false;
}
