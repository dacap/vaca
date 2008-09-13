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

#include "Vaca/Menu.h"
#include "Vaca/MenuItemEvent.h"
#include "Vaca/Debug.h"
#include "Vaca/System.h"
#include "Vaca/Mdi.h"
#include "Vaca/ResourceException.h"
#include "Vaca/Mutex.h"
#include "Vaca/ScopedLock.h"
#include "Vaca/Command.h"

#include <stack>

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// MenuItem

MenuItem::MenuItem()
{
  m_parent = NULL;
  m_id = 0;
  m_enabled = true;
  m_checked = false;
}

/**
 * Creates a new menu item.
 *
 * @param text You can use the \& character to underline a letter (e.g.: "&New")
 * @param defaultShortcut The default keyboard shortcut for this menu item.
 * @param id You can use an specified CommandId for the menu.
 */
MenuItem::MenuItem(const String& text, CommandId id, Keys::Type defaultShortcut)
{
  m_parent = NULL;
  m_text = text;
  m_id = id;
  m_enabled = true;
  m_checked = false;

  if (defaultShortcut != Keys::None)
    addShortcut(defaultShortcut);
}

MenuItem::~MenuItem()
{
  Menu* parent = getParent();
  if (parent != NULL)
    parent->remove(this);
}

Menu* MenuItem::getParent()
{
  return m_parent;
}

Menu* MenuItem::getRoot()
{
  Menu* root = m_parent;
  if (root) {
    while (root->m_parent != NULL)
      root = root->m_parent;
  }
  return root;
}

CommandId MenuItem::getId()
{
  return m_id;
}

const String& MenuItem::getText()
{
  return m_text;
}

void MenuItem::setText(const String& text)
{
  m_text = text;

  if (m_parent != NULL) {
    MENUITEMINFO mii;

    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING;
    mii.dwTypeData = const_cast<LPTSTR>(text.c_str());

    SetMenuItemInfo(m_parent->getHandle(),
		    m_parent->getMenuItemIndex(this),
		    TRUE, &mii);
  }
}

void MenuItem::setId(CommandId id)
{
  m_id = id;

  if (m_parent != NULL) {
    MENUITEMINFO mii;

    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_ID;
    mii.wID = m_id;

    SetMenuItemInfo(m_parent->getHandle(),
		    m_parent->getMenuItemIndex(this),
		    TRUE, &mii);
  }
}

bool MenuItem::isEnabled()
{
  if (m_parent) {
    MENUITEMINFO mii;
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STATE;
    if (GetMenuItemInfo(m_parent->getHandle(),
  			m_parent->getMenuItemIndex(this),
  			TRUE, &mii)) {
      m_enabled = (mii.fState & (MFS_DISABLED | MFS_GRAYED)) == 0;
    }
  }
  return m_enabled;
}

void MenuItem::setEnabled(bool state)
{
  m_enabled = state;

  if (m_parent) {
    ::EnableMenuItem(m_parent->getHandle(),
		     m_parent->getMenuItemIndex(this),
		     MF_BYPOSITION | (m_enabled ? MF_ENABLED: MF_GRAYED));
  }
}

bool MenuItem::isChecked()
{
  if (m_parent) {
    MENUITEMINFO mii;
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STATE;
    if (GetMenuItemInfo(m_parent->getHandle(),
			m_parent->getMenuItemIndex(this),
			TRUE, &mii)) {
      m_checked = (mii.fState & MFS_CHECKED) != 0;
    }
  }
  return m_checked;
}

void MenuItem::setChecked(bool state)
{
  m_checked = state;

  if (m_parent) {
    ::CheckMenuItem(m_parent->getHandle(),
		    m_parent->getMenuItemIndex(this),
		    MF_BYPOSITION |
		    (state ? MF_CHECKED: MF_UNCHECKED));
  }
}

/**
 * Radio MenuItem groups are automatically delimited by MenuSeparator
 * or by the beginning and the end of the menu.
 */
void MenuItem::setRadio(bool state)
{
  if (state) {
    assert(m_parent != NULL);

    int index = m_parent->getMenuItemIndex(this);
    int count = m_parent->getItemCount();
    int first = index;
    int last = index;

    do {
      first--;
    } while (first >= 0 && !m_parent->getMenuItemByIndex(first)->isSeparator());
    first++;

    do {
      last++;
    } while (last < count && !m_parent->getMenuItemByIndex(last)->isSeparator());
    last--;

    ::CheckMenuRadioItem(m_parent->getHandle(), first, last, index, MF_BYPOSITION);
  }
  else {
    setChecked(false);
  }
}

void MenuItem::addShortcut(Keys::Type shortcut)
{
  assert(shortcut != Keys::None);

  m_shortcuts.push_back(shortcut);
}

MenuItem* MenuItem::checkShortcuts(Keys::Type pressedKey)
{
  for (std::vector<Keys::Type>::iterator
	 it=m_shortcuts.begin(); it!=m_shortcuts.end(); ++it) {
    if (pressedKey == (*it))
      return this;
  }
  return NULL;
}

bool MenuItem::isMenu() const { return false; }
bool MenuItem::isSeparator() const { return false; }
bool MenuItem::isMdiList() const { return false; }

/**
 * It's called when the menu item is selected. Also it's called when
 * some keyboard shortcut of this MenuItem is pressed
 * (MenuItem::m_shortcuts). Remember that onAction() is called only
 * after an onUpdate() and only if it leaves the MenuItem enabled (see
 * setEnabled() method),
 */
void MenuItem::onAction(MenuItemEvent& ev)
{
  // do nothing
}

/**
 * It's called when a menu is shown for first time. Also when the user
 * press a keyboard shortcut (MenuItem::m_shortcuts) it's called to
 * known if the item is available after execute onAction().
 *
 * Internally, when the WM_INITMENU message is received, a Frame calls
 * this event.
 *
 * The default implementation search for a Command with ID equal to
 * MenuItem#getId, so the Command#isEnabled routine will be called to
 * change the state of this MenuItem.
 */
void MenuItem::onUpdate(MenuItemEvent& ev)
{
  // does this menu item have an associated command-id?
  if (m_id != 0) {
    // search the command in the frame where is the menu-bar
    if (MenuBar* menuBar = dynamic_cast<MenuBar*>(getRoot())) {
      if (Frame* frame = menuBar->getFrame()) {
	if (Command* cmd = frame->findCommandById(m_id)) {
	  setEnabled(cmd->isEnabled());
	  return;
	}
      }
    }

    // check if the application is a CommandsClient
    if (CommandsClient* cc = dynamic_cast<CommandsClient*>(Application::getInstance())) {
      if (Command* cmd = cc->getCommandById(m_id))
	setEnabled(cmd->isEnabled());
    }
  }
}

//////////////////////////////////////////////////////////////////////
// MenuSeparator

MenuSeparator::MenuSeparator()
{
}

MenuSeparator::~MenuSeparator()
{
}

bool MenuSeparator::isSeparator() const
{
  return true;
}

//////////////////////////////////////////////////////////////////////
// Menu

Menu::Menu()
{
  m_HMENU = ::CreateMenu();
  VACA_TRACE("%p = CreateMenu()\n", m_HMENU);

  subClass();
}

Menu::Menu(const String& text)
  : MenuItem(text, 0)
{
  m_HMENU = ::CreatePopupMenu();
  VACA_TRACE("%p = CreatePopupMenu()\n", m_HMENU);

  subClass();
}

Menu::Menu(CommandId menuId)
{
  m_HMENU = ::LoadMenu(Application::getHandle(),
		       MAKEINTRESOURCE(menuId));

  if (m_HMENU == NULL)
    throw ResourceException("Can't load the menu resource " + String::fromInt(menuId));

  subClass();
}

Menu::Menu(HMENU hmenu)
{
  assert(hmenu != NULL);
  
  m_HMENU = hmenu;
  subClass();
}

Menu::~Menu()
{
  assert(m_HMENU != NULL);

  while (!m_container.empty()) {
    MenuItem* menuItem = m_container.front();
    delete menuItem;
  }

  DestroyMenu(m_HMENU);
}

void Menu::subClass()
{
  assert(m_HMENU != NULL);

  MENUINFO mi;
  mi.cbSize = sizeof(MENUINFO);
  mi.fMask = MIM_MENUDATA | MIM_STYLE;
  GetMenuInfo(m_HMENU, &mi);

  // Vaca doesn't use MNS_NOTIFYBYPOS
  assert((mi.dwStyle & MNS_NOTIFYBYPOS) == 0);

  // the menu can't have data
  assert(mi.dwMenuData == 0);

  // set the associated data with this Menu
  mi.fMask = MIM_MENUDATA;
  mi.dwMenuData = reinterpret_cast<ULONG_PTR>(this);
  SetMenuInfo(m_HMENU, &mi);

  // now we must sub-class all existent menu items
  int menuItemCount = GetMenuItemCount(m_HMENU);
  for (int itemIndex=0; itemIndex<menuItemCount; ++itemIndex) {
    TCHAR buf[4096];		// TODO buffer overflow
    MENUITEMINFO mii;

    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_FTYPE | MIIM_DATA | MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
    mii.dwTypeData = buf;
    mii.cch = sizeof(buf) / sizeof(TCHAR);

    BOOL res = GetMenuItemInfo(m_HMENU, itemIndex, TRUE, &mii);
    assert(res == TRUE);

    // the item can't have data
    assert(mii.dwItemData == 0);

    MenuItem* menuItem = NULL;

    switch (mii.fType) {

      case MFT_STRING:
	if (mii.hSubMenu != NULL) {
	  menuItem = new Menu(mii.hSubMenu);
	  menuItem->m_text = buf;
	}
	else {
	  menuItem = new MenuItem(buf, mii.wID, Keys::None);
	}
	break;

      case MFT_SEPARATOR:
	menuItem = new MenuSeparator();
	break;

	// TODO more MFT_ support
	// TODO MdiListMenu
      default:
	assert(false);		// TODO unsupported MENUITEM type
	break;
    }

    menuItem->m_parent = this;
    m_container.push_back(menuItem);
  }
}

/**
 * Adds a MenuItem at the end of the Menu.
 *
 * @warning The @a menuItem is deleted automatically if you don't
 *          remove it before to destroy the Menu.
 *
 * @see insert(int, MenuItem*), remove(MenuItem*)
 */
MenuItem* Menu::add(MenuItem* menuItem)
{
  insert(m_container.size(), menuItem);
  return menuItem;
}

/**
 * Adds a new MenuItem with the specified @a text and @a defaultShortcut
 * at the end of the Menu.
 *
 * @warning The returned MenuItem is deleted automatically when the
 *          Menu is destroyed.
 * @warning If you use the Menu::remove method with the returned
 *          MenuItem, you will be responsible to delete it.
 *
 * @see add(MenuItem*), remove(MenuItem*)
 */
MenuItem* Menu::add(const String& string, CommandId id, Keys::Type defaultShortcut)
{
  MenuItem* menuItem = add(new MenuItem(string, id, defaultShortcut));
  return menuItem;
}

/**
 * Adds a new MenuSeparator at the end of the Menu.
 *
 * @see add(MenuItem*), remove(MenuItem*)
 */
void Menu::addSeparator()
{
  MenuItem* menuItem = add(new MenuSeparator());
}

/**
 * Inserts a MenuItem at the @a index position of the Menu.
 * 
 * @warning The inserted @a menuItem is deleted automatically if you don't
 *          remove it before to destroy the Menu.
 *
 * @see remove(MenuItem*), insert(int, const String &)
 */
MenuItem* Menu::insert(int index, MenuItem* menuItem)
{
  assert(index >= 0 && index <= static_cast<int>(m_container.size()));

  Container::iterator it;
  int c = 0;

  for (it=m_container.begin(); it!=m_container.end(); ++it, ++c)
    if (c == index)
      break;

  String text(menuItem->getText());
  int len = text.size();
  LPTSTR buf = new TCHAR[len+1];
#ifdef UNICODE
  wcscpy(buf, text.c_str());
#else
  strcpy(buf, text.c_str());
#endif

  MENUITEMINFO mii;
  mii.cbSize = sizeof(MENUITEMINFO);
  mii.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_DATA;
  mii.dwItemData = reinterpret_cast<ULONG_PTR>(menuItem);
  mii.fState =
    (menuItem->isEnabled() ? MFS_ENABLED: MFS_DISABLED) |
    (menuItem->isChecked() ? MFS_CHECKED: 0);
  mii.wID = menuItem->getId();

  if (menuItem->isSeparator()) {
    mii.fType = MFT_SEPARATOR; 
  }
  else if (menuItem->isMenu()) {
    mii.fMask |= MIIM_STRING | MIIM_SUBMENU;
    mii.fType = MFT_STRING;
    Menu* menu = static_cast<Menu*>(menuItem);
    mii.hSubMenu = menu->getHandle();
    mii.dwTypeData = buf;
    mii.cch = len;
  }
  else {
    mii.fMask |= MIIM_STRING;
    mii.fType = MFT_STRING;
    mii.dwTypeData = buf;
    mii.cch = len; 
    // HBITMAP hbmpChecked; 
    // HBITMAP hbmpUnchecked; 
    // HBITMAP hbmpItem;
  }

  InsertMenuItem(m_HMENU, index, TRUE, &mii);

  delete[] buf;

  m_container.insert(it, menuItem);
  menuItem->m_parent = this;
  return menuItem;
}

/**
 * Inserts a new MenuItem with the specified @a text at the @a index
 * position of the Menu.
 *
 * @warning The returned MenuItem is deleted automatically when the
 *          Menu is destroyed.
 * @warning If you use the Menu::remove method with the returned
 *          MenuItem, you will be responsible to delete it.
 *
 * @see insert(int, MenuItem*), remove(MenuItem*)
 */
MenuItem* Menu::insert(int index, const String& text, CommandId id, Keys::Type defaultShortcut)
{
  MenuItem* menuItem = insert(index, new MenuItem(text, id, defaultShortcut));
  return menuItem;
}

/**
 * Inserts a new MenuSeparator at the @a index position of the Menu.
 */
void Menu::insertSeparator(int index)
{
  MenuItem* menuItem = insert(index, new MenuSeparator());
}

/**
 * Removes the @a menuItem from the menu.
 *
 * @warning You must to delete the specified @a menuItem after this
 *          (because it'll not be under the control of the Menu any more).
 *
 * @return The same pointer to the specified parameter @a menuItem
 */
MenuItem* Menu::remove(MenuItem* menuItem)
{
  assert(m_HMENU != NULL);

  // TODO check if this works
  ::RemoveMenu(m_HMENU, getMenuItemIndex(menuItem), MF_BYPOSITION);

  // see the "RemoveMenu Function" in the MSDN, you "must call the
  // DrawMenuBar function whenever a menu changes"
  // http://msdn.microsoft.com/en-us/library/ms647994(VS.85).aspx
  if (MenuBar* bar = dynamic_cast<MenuBar*>(getRoot())) {
    Frame* frame = bar->getFrame();
    ::DrawMenuBar(frame ? frame->getHandle(): NULL);
  }

  menuItem->m_parent = NULL;
  remove_from_container(m_container, menuItem);

  return menuItem;
}

/**
 * Removes the menu item that is the specified position by the @a
 * index.
 * 
 * @warning You should delete the returned pointer or save it
 *          to destroy it in the future.
 *
 * @code
 * Menu* menu = ...
 * MenuItem* menuItem = menu->getMenuItemByIndex(2);
 * menu->remove(menuItem);
 * delete menuItem;
 * @endcode
 *
 * It's the same as:
 * 
 * @code
 * delete menu->remove(2);
 * @endcode
 *
 * @see remove(MenuItem*)
 */
MenuItem* Menu::remove(int index)
{
  MenuItem* menuItem = getMenuItemByIndex(index);
  remove(menuItem);
  return menuItem;
}

MenuItem* Menu::getMenuItemByIndex(int index)
{
  return m_container[index];
}

MenuItem* Menu::getMenuItemById(CommandId id)
{
  if (id == 0)
    return NULL;

  MenuItem* menuItem = NULL;

  std::stack<MenuItem*> stack;
  stack.push(this);

  while (!stack.empty()) {
    menuItem = stack.top();
    if (menuItem->getId() == id)
      return menuItem;

    stack.pop();

    if (menuItem->isMenu()) {
      Menu::Container& subMenus(static_cast<Menu*>(menuItem)->m_container);

      for (Menu::Container::iterator it=subMenus.begin();
	   it!=subMenus.end(); ++it)
	stack.push(*it);
    }
  }

  return NULL;
}

int Menu::getMenuItemIndex(MenuItem* menuItem)
{
  Container::iterator it;
  int c = 0;

  for (it=m_container.begin(); it!=m_container.end(); ++it, ++c)
    if (*it == menuItem)
      return c;

  return -1;
}

int Menu::getItemCount()
{
  // TODO
  return m_container.size();
}

Menu::Container Menu::getMenuItems()
{
  return m_container;
}

MenuItem* Menu::checkShortcuts(Keys::Type pressedKey)
{
  for (Container::iterator it=m_container.begin(); it!=m_container.end(); ++it) {
    MenuItem* menuItem = (*it)->checkShortcuts(pressedKey);
    if (menuItem != NULL)
      return menuItem;
  }

  return NULL;
}

// PopupMenu* Menu::getPopupMenu()
// {
//   // TODO
//   return NULL;
// }

bool Menu::isMenu() const
{
  return true;
}

// Menu* Menu::getMenuByHMENU(HMENU hmenu)
// {
//   Menu* lastMenu = NULL;

//   std::stack<Menu*> stack;
//   stack.push(this);

//   while (!stack.empty()) {
//     lastMenu = stack.top();
//     if (lastMenu->getHandle() == hmenu)
//       return lastMenu;

//     stack.pop();

//     Menu::Container& subMenus(lastMenu->m_container);
//     for (Menu::Container::iterator it=subMenus.begin();
// 	 it!=subMenus.end(); ++it) {
//       if ((*it)->isMenu())
// 	stack.push(static_cast<Menu*>(*it));
//     }
//   }

//   return NULL;
// }

HMENU Menu::getHandle()
{
  return m_HMENU;
}

//////////////////////////////////////////////////////////////////////
// MenuBar

MenuBar::MenuBar()
  : Menu()
  , m_frame(NULL)
{
}

MenuBar::MenuBar(CommandId menuId)
  : Menu(menuId)
  , m_frame(NULL)
{
}

MenuBar::~MenuBar()
{
}

Frame* MenuBar::getFrame()
{
  return m_frame;
}

void MenuBar::setFrame(Frame* frame)
{
  m_frame = frame;
}

MdiListMenu* MenuBar::getMdiListMenu()
{
  std::stack<MenuItem*> stack;
  stack.push(this);

  while (!stack.empty()) {
    MenuItem* menuItem = stack.top();
    stack.pop();

    if (menuItem->isMdiList())
      return static_cast<MdiListMenu*>(menuItem);
    else if (menuItem->isMenu()) {
      Container container = static_cast<Menu*>(menuItem)->getMenuItems();
      for (Container::iterator it=container.begin();
	   it!=container.end(); ++it)
	stack.push(*it);
    }
  }

  return NULL;
}

//////////////////////////////////////////////////////////////////////
// MdiListMenu

MdiListMenu::MdiListMenu(const String& text)
 : Menu(text)
{
}

MdiListMenu::~MdiListMenu()
{
}

bool MdiListMenu::isMdiList() const
{
  return true;
}
