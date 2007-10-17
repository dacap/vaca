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
#include "Vaca/Menu.h"
#include "Vaca/MenuItemEvent.h"
#include "Vaca/Debug.h"
#include "Vaca/System.h"
#include "Vaca/Mdi.h"
#include "Vaca/Mutex.h"
#include "Vaca/ResourceException.h"

using namespace Vaca;

static int mIdCounter = VACA_FIRST_AUTOID;

//////////////////////////////////////////////////////////////////////
// MenuItem

MenuItem::MenuItem()
{
  mAutoDelete = false;
  mParent = NULL;
  mId = 0;
}

/**
 * Creates a new menu item.
 *
 * @param text You can use the \& character to underline a letter (e.g.: "&New")
 * @param defaultShortcut The default keyboard shortcut for this menu item.
 * @param id You can use an specified ID for the menu, or leave blank
 *           to use a default generated ID. <b>Remember</b> this rule:
 *           if you specify an ID in just one MenuItem, <b>you must
 *           to</b> specify IDs in all MenuItems that you create. You
 *           can't use automatic IDs and manual IDs mixed,
 *           assertions'll fail.
 */
MenuItem::MenuItem(const String &text, Keys::Type defaultShortcut, int id)
{
  if (id < 0) {
    id = mIdCounter++;
  }
  else {
    // you can't use automatic and manual IDs mixed, use one option or
    // other, but don't use both in the same application
    assert(mIdCounter == VACA_FIRST_AUTOID);
  }

  // check overflow
  assert(mIdCounter < VACA_FIRST_MDICHILD);

  mAutoDelete = false;
  mParent = NULL;
  mText = text;
  mId = id;

  if (defaultShortcut != Keys::None)
    addShortcut(defaultShortcut);
}

MenuItem::~MenuItem()
{
}

bool MenuItem::isAutoDelete()
{
  return mAutoDelete;
}

Menu *MenuItem::getParent()
{
  return mParent;
}

int MenuItem::getId()
{
  return mId;
}

const String &MenuItem::getText()
{
  return mText;
}

void MenuItem::setText(const String &text)
{
  mText = text;

  if (mParent != NULL) {
    MENUITEMINFO mii;

    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING;
    mii.dwTypeData = const_cast<LPTSTR>(text.c_str());

    SetMenuItemInfo(mParent->getHMENU(),
		    mParent->getMenuItemIndex(this),
		    TRUE, &mii);
  }
}

bool MenuItem::isEnabled()
{
  assert(mParent != NULL);

  MENUITEMINFO mii;
  mii.cbSize = sizeof(MENUITEMINFO);
  mii.fMask = MIIM_STATE;
  if (GetMenuItemInfo(mParent->getHMENU(),
		      mParent->getMenuItemIndex(this),
		      TRUE, &mii)) {
    return (mii.fState & (MFS_DISABLED | MFS_GRAYED)) == 0;
  }

  return true;
}

void MenuItem::setEnabled(bool state)
{
  assert(mParent != NULL);

  ::EnableMenuItem(mParent->getHMENU(),
		   mParent->getMenuItemIndex(this),
		   MF_BYPOSITION | (state ? MF_ENABLED: MF_GRAYED));
}

bool MenuItem::isChecked()
{
  assert(mParent != NULL);

  MENUITEMINFO mii;
  mii.cbSize = sizeof(MENUITEMINFO);
  mii.fMask = MIIM_STATE;
  if (GetMenuItemInfo(mParent->getHMENU(),
		      mParent->getMenuItemIndex(this),
		      TRUE, &mii)) {
    return (mii.fState & MFS_CHECKED) != 0;
  }

  return false;
}

void MenuItem::setChecked(bool state)
{
  assert(mParent != NULL);

  ::CheckMenuItem(mParent->getHMENU(),
		  mParent->getMenuItemIndex(this),
		  MF_BYPOSITION |
		  (state ? MF_CHECKED: MF_UNCHECKED));
}

/**
 * Radio MenuItem groups are automatically delimited by MenuSeparator
 * or by the beginning and the end of the menu.
 */
void MenuItem::setRadio(bool state)
{
  if (state) {
    assert(mParent != NULL);

    int index = mParent->getMenuItemIndex(this);
    int count = mParent->getItemCount();
    int first = index;
    int last = index;

    do {
      first--;
    } while (first >= 0 && !mParent->getMenuItemByIndex(first)->isSeparator());
    first++;

    do {
      last++;
    } while (last < count && !mParent->getMenuItemByIndex(last)->isSeparator());
    last--;

    ::CheckMenuRadioItem(mParent->getHMENU(), first, last, index, MF_BYPOSITION);
  }
  else {
    setChecked(false);
  }
}

void MenuItem::addShortcut(Keys::Type shortcut)
{
  assert(shortcut != Keys::None);

  mShortcuts.push_back(shortcut);
}

MenuItem *MenuItem::checkShortcuts(Keys::Type pressedKey)
{
  for (std::vector<Keys::Type>::iterator it=mShortcuts.begin(); it!=mShortcuts.end(); ++it)
    if (pressedKey == (*it))
      return this;

  return NULL;
}

bool MenuItem::isMenu() const { return false; }
bool MenuItem::isSeparator() const { return false; }
bool MenuItem::isMdiList() const { return false; }

/**
 * It's called when the menu item is selected. Also it's called when
 * some keyboard shortcut of this MenuItem is pressed
 * (MenuItem::mShortcuts). Remember that onAction() is called only
 * after an onUpdate() and only if it leaves the MenuItem enabled (see
 * setEnabled() method),
 */
void MenuItem::onAction(MenuItemEvent &ev)
{
  Action(ev);
}

/**
 * It's called when a menu is shown for first time. Also when the user
 * press a keyboard shortcut (MenuItem::mShortcuts) it's called to
 * known if the item is available after execute onAction().
 *
 * Internally, when the WM_INITMENU message is received, a Frame calls
 * this event.
 *
 * The default implementation fires the MenuItem::Update signal.
 */
void MenuItem::onUpdate(MenuItemEvent &ev)
{
  Update(ev);
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
  mHMENU = ::CreateMenu();
  VACA_TRACE("%p = CreateMenu()\n", mHMENU);

  subClass();
}

Menu::Menu(const String &text)
  : MenuItem(text)
{
  mHMENU = ::CreatePopupMenu();
  VACA_TRACE("%p = CreatePopupMenu()\n", mHMENU);

  subClass();
}

Menu::Menu(int menuId)
{
  mHMENU = ::LoadMenu(Application::getHINSTANCE(),
		      MAKEINTRESOURCE(menuId));

  if (mHMENU == NULL)
    throw ResourceException();

  subClass();
}

Menu::Menu(HMENU hmenu)
{
  assert(hmenu != NULL);
  
  mHMENU = hmenu;
  subClass();
}

Menu::~Menu()
{
  assert(mHMENU != NULL);

  for (Container::iterator it=mContainer.begin(); it!=mContainer.end(); ++it) {
    MenuItem *menuItem = *it;
    if (menuItem->isAutoDelete())
      delete menuItem;
  }

  DestroyMenu(mHMENU);
}

void Menu::subClass()
{
  assert(mHMENU != NULL);

  MENUINFO mi;
  mi.cbSize = sizeof(MENUINFO);
  mi.fMask = MIM_MENUDATA | MIM_STYLE;
  GetMenuInfo(mHMENU, &mi);

  // Vaca doesn't use MNS_NOTIFYBYPOS
  assert((mi.dwStyle & MNS_NOTIFYBYPOS) == 0);

  // the menu can't have data
  assert(mi.dwMenuData == 0);

  // set the associated data with this Menu
  mi.fMask = MIM_MENUDATA;
  mi.dwMenuData = reinterpret_cast<ULONG_PTR>(this);
  SetMenuInfo(mHMENU, &mi);

  // now we must sub-class all existent menu items
  int menuItemCount = GetMenuItemCount(mHMENU);
  for (int itemIndex=0; itemIndex<menuItemCount; ++itemIndex) {
    TCHAR buf[4096];		// TODO buffer overflow
    MENUITEMINFO mii;

    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_FTYPE | MIIM_DATA | MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
    mii.dwTypeData = buf;
    mii.cch = sizeof(buf) / sizeof(TCHAR);

    BOOL res = GetMenuItemInfo(mHMENU, itemIndex, TRUE, &mii);
    assert(res == TRUE);

    // the item can't have data
    assert(mii.dwItemData == 0);

    MenuItem *menuItem = NULL;

    switch (mii.fType) {
      case MFT_STRING:
	if (mii.hSubMenu != NULL) {
	  menuItem = new Menu(mii.hSubMenu);
	  menuItem->mText = buf;
	}
	else {
	  menuItem = new MenuItem(buf, Keys::None, mii.wID);
	}
	break;
      case MFT_SEPARATOR:
	menuItem = new MenuSeparator();
	break;
      // TODO MdiListMenu
      default:
	assert(false);		// TODO unsupported MENUITEM type
	break;
    }

    menuItem->mParent = this;
    mContainer.push_back(menuItem);
  }
}

/**
 * Adds a MenuItem at the end of the Menu.
 *
 * @warning You must delete the @a menuItem.
 *
 * @see insert(int, MenuItem *), remove(MenuItem *)
 */
MenuItem *Menu::add(MenuItem *menuItem)
{
  insert(mContainer.size(), menuItem);
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
 * @see add(MenuItem *), remove(MenuItem *)
 */
MenuItem *Menu::add(const String &string, Keys::Type defaultShortcut)
{
  MenuItem *menuItem = add(new MenuItem(string, defaultShortcut));
  menuItem->mAutoDelete = true;
  return menuItem;
}

/**
 * Adds a new MenuSeparator at the end of the Menu.
 *
 * @see add(MenuItem *), remove(MenuItem *)
 */
void Menu::addSeparator()
{
  MenuItem *menuItem = add(new MenuSeparator());
  menuItem->mAutoDelete = true;
}

/**
 * Inserts a MenuItem at the @a index position of the Menu.
 *
 * @warning You must delete the @a menuItem.
 *
 * @see remove(MenuItem *), insert(int, const String &)
 */
MenuItem *Menu::insert(int index, MenuItem *menuItem)
{
  assert(index >= 0 && index <= static_cast<int>(mContainer.size()));

  Container::iterator it;
  int c = 0;

  for (it=mContainer.begin(); it!=mContainer.end(); ++it, ++c)
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
  mii.fState = MFS_ENABLED; 
  mii.wID = menuItem->getId(); 

  if (menuItem->isSeparator()) {
    mii.fType = MFT_SEPARATOR; 
  }
  else if (menuItem->isMenu()) {
    mii.fMask |= MIIM_STRING | MIIM_SUBMENU;
    mii.fType = MFT_STRING;
    Menu *menu = static_cast<Menu *>(menuItem);
    mii.hSubMenu = menu->getHMENU();
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

  InsertMenuItem(mHMENU, index, TRUE, &mii);

  if (buf != NULL)
    delete buf;

  mContainer.insert(it, menuItem);
  menuItem->mParent = this;
  return menuItem;
}

/**
 * Inserts a new MenuItem with the specified @a text and @a defaultShortcut
 * at the @a index position of the Menu.
 *
 * @warning The returned MenuItem is deleted automatically when the
 *          Menu is destroyed.
 * @warning If you use the Menu::remove method with the returned
 *          MenuItem, you will be responsible to delete it.
 *
 * @see insert(int, MenuItem *), remove(MenuItem *)
 */
MenuItem *Menu::insert(int index, const String &text)
{
  MenuItem *menuItem = insert(index, new MenuItem(text));
  menuItem->mAutoDelete = true;
  return menuItem;
}

/**
 * Inserts a new MenuSeparator at the @a index position of the Menu.
 */
void Menu::insertSeparator(int index)
{
  MenuItem *menuItem = insert(index, new MenuSeparator());
  menuItem->mAutoDelete = true;
}

void Menu::remove(MenuItem *menuItem)
{
  assert(mHMENU != NULL);

  // TODO check if this works
  RemoveMenu(mHMENU, getMenuItemIndex(menuItem), MF_BYPOSITION);

  menuItem->mParent = NULL;
  menuItem->mAutoDelete = false;
}

void Menu::remove(int index)
{
  remove(getMenuItemByIndex(index));
}

MenuItem *Menu::getMenuItemByIndex(int index)
{
  return mContainer[index];
}

MenuItem *Menu::getMenuItemById(int id)
{
  if (id == 0)
    return NULL;

  MenuItem *menuItem = NULL;

  std::stack<MenuItem *> stack;
  stack.push(this);

  while (!stack.empty()) {
    menuItem = stack.top();
    if (menuItem->getId() == id)
      return menuItem;

    stack.pop();

    if (menuItem->isMenu()) {
      Menu::Container &subMenus(static_cast<Menu *>(menuItem)->mContainer);

      for (Menu::Container::iterator it=subMenus.begin();
	   it!=subMenus.end(); ++it)
	stack.push(*it);
    }
  }

  return NULL;
}

int Menu::getMenuItemIndex(MenuItem *menuItem)
{
  Container::iterator it;
  int c = 0;

  for (it=mContainer.begin(); it!=mContainer.end(); ++it, ++c)
    if (*it == menuItem)
      return c;

  return -1;
}

int Menu::getItemCount()
{
  // TODO
  return mContainer.size();
}

Menu::Container Menu::getMenuItems()
{
  return mContainer;
}

MenuItem *Menu::checkShortcuts(Keys::Type pressedKey)
{
  for (Container::iterator it=mContainer.begin(); it!=mContainer.end(); ++it) {
    MenuItem *menuItem = (*it)->checkShortcuts(pressedKey);
    if (menuItem != NULL)
      return menuItem;
  }

  return NULL;
}

// PopupMenu *Menu::getPopupMenu()
// {
//   // TODO
//   return NULL;
// }

bool Menu::isMenu() const
{
  return true;
}

Menu *Menu::getMenuByHMENU(HMENU hmenu)
{
  Menu *lastMenu = NULL;

  std::stack<Menu *> stack;
  stack.push(this);

  while (!stack.empty()) {
    lastMenu = stack.top();
    if (lastMenu->getHMENU() == hmenu)
      return lastMenu;

    stack.pop();

    Menu::Container &subMenus(lastMenu->mContainer);
    for (Menu::Container::iterator it=subMenus.begin();
	 it!=subMenus.end(); ++it) {
      if ((*it)->isMenu())
	stack.push(static_cast<Menu *>(*it));
    }
  }

  return NULL;
}

HMENU Menu::getHMENU()
{
  return mHMENU;
}

//////////////////////////////////////////////////////////////////////
// MenuBar


MenuBar::MenuBar()
 : Menu()
{
}

MenuBar::MenuBar(int menuId)
  : Menu(menuId)
{
}

MenuBar::~MenuBar()
{
}

MdiListMenu *MenuBar::getMdiListMenu()
{
  std::stack<MenuItem *> stack;
  stack.push(this);

  while (!stack.empty()) {
    MenuItem *menuItem = stack.top();
    stack.pop();

    if (menuItem->isMdiList())
      return static_cast<MdiListMenu *>(menuItem);
    else {
      Container container = getMenuItems();
      for (Container::iterator it=container.begin();
	   it!=container.end(); ++it)
	stack.push(*it);
    }
  }

  return NULL;
}

//////////////////////////////////////////////////////////////////////
// MdiListMenu


MdiListMenu::MdiListMenu(const String &text)
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
