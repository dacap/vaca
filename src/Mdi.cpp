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
#include "Vaca/MDI.h"
#include "Vaca/Application.h"
#include "Vaca/Debug.h"
#include "Vaca/WidgetEvent.h"
#include "Vaca/ClientLayout.h"
#include "Vaca/Menu.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// MdiChild


/**
 * Creates a new MDI child window.
 *
 * @warning All MdiChilds are created visible and activated (and this
 *          can't be changed, see @ref TN005 for more information).
 */
MdiChild::MdiChild(const String &title, MdiClient *parent, Style style)
  : Frame(NULL, "", NULL, NoStyle)
{
  create(MdiChildClass::getClassName(), parent, style);
  setText(title);
  initialize();
}

/**
 * Creates a new MDI child window.
 *
 * @warning All MdiChilds are created visible and activated (and this
 *          can't be changed, see @ref TN005 for more information).
 */
MdiChild::MdiChild(const String &title, MdiFrame *parent, Style style)
  : Frame(NULL, "", NULL, NoStyle)
{
  create(MdiChildClass::getClassName(), &parent->getMdiClient(), style);
  setText(title);
  initialize();
}

MdiChild::~MdiChild()
{
  // see TN002
  dispose();
}

void MdiChild::initialize()
{
  // see TN005
  ::PostMessage(getHwnd(), WM_MDIACTIVATE, 0, reinterpret_cast<LPARAM>(getHwnd()));
  ::PostMessage(getHwnd(), WM_SETFOCUS, 0, 0);

  Rect rc = getClientBounds();
  ::PostMessage(getHwnd(), WM_SIZE, 0, MAKELPARAM(rc.w, rc.h));

  // refresh the MdiListMenu (see TN007)
  if (getParent() != NULL)
    getParent()->sendMessage(WM_MDIREFRESHMENU, 0, 0);
}

/**
 * Intercept commands from @c VACA_IDM_MDICHILD to activate this
 * MdiChild.
 */
bool MdiChild::onCommand(int commandCode, LRESULT &lResult)
{
  if (Frame::onCommand(commandCode, lResult))
    return true;
  else {
    // Note: why just don't call the DefFrameProc with WM_COMMAND? it
    // looks like it doesn't work fine with menus that has
    // MNS_NOTIFYBYPOS enabled

    // the user selects a MdiChild from the MdiListMenu?
    if (commandCode == 0) {
      MdiClient *parent = static_cast<MdiClient *>(getParent());
      VACA_ASSERT(parent != NULL);

      parent->activate(this);
      return true;
    }

    return false;
  }
}

/**
 * Intercepts the WM_MDIACTIVATE message to generate the
 * Frame::onActivate and Frame::onDeactivate events.
 */
bool MdiChild::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
  if (Frame::wndProc(message, wParam, lParam, lResult))
    return true;

  switch (message) {

    case WM_MDIACTIVATE: {
      if (getHwnd() == (HWND)wParam) {
	WidgetEvent ev(this);
	onDeactivate(ev);
      }
      else if (getHwnd() == (HWND)lParam) {
	WidgetEvent ev(this);
	onActivate(ev);
      }
      break;
    }

  }

  return false;
}

/**
 * It uses the Win32 DefMDIChildProc to process messages.
 */
LRESULT MdiChild::defWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  return ::DefMDIChildProc(getHwnd(), message, wParam, lParam);
}

// Don't try to do this alternative (using WM_MDICREATE), it doesn't
// fix the TN005 problem (also it doesn't support the extended styles WS_EX)
#if 0				// don't remove this code, must be for
				// historical purposes
HWND MdiChild::createHwnd(LPCTSTR className, Widget *parent, Style style)
{
  HWND hClient = parent->getHwnd();
  VACA_ASSERT(hClient != NULL);

  MDICREATESTRUCT mdics;
  mdics.szClass = className;
  mdics.szTitle = _T("");
  mdics.hOwner = Application::getHinstance();
  mdics.x = CW_USEDEFAULT;
  mdics.y = CW_USEDEFAULT;
  mdics.cx = CW_USEDEFAULT;
  mdics.cy = CW_USEDEFAULT;
  mdics.style = style.regular & ~(WS_VISIBLE | WS_MAXIMIZE);
  mdics.lParam = 0;

  HWND hwnd =
    reinterpret_cast<HWND>(::SendMessage(hClient, WM_MDICREATE, 0, reinterpret_cast<LPARAM>(&mdics)));

  return hwnd;
}
#endif

/**
 * Doesn't call DestroyWindow, it sends a WM_MDIDESTROY message to the
 * MdiClient parent to destroy the MdiChild.
 */
void MdiChild::destroyHwnd(HWND hwnd)
{
  // this can't be used
  // ...getParent()->sendMessage(WM_MDIDESTROY, reinterpret_cast<WPARAM>(getHwnd()), 0);

  // we must be limited to use only the HWND...
  HWND hClient = ::GetParent(hwnd);
  VACA_ASSERT(hClient != NULL);
  ::SendMessage(hClient, WM_MDIDESTROY, reinterpret_cast<WPARAM>(hwnd), 0);
}

//////////////////////////////////////////////////////////////////////
// MdiClient
 

/**
 * Container for MdiChild windows.
 */
MdiClient::MdiClient(Widget *parent)
  : Widget(NULL, NULL, NoStyle) // className must be NULL to avoid calling the Widget::createHwnd
{
  // now Widget::create calls MdiClient::createHwnd because MdiClient is constructed
  create(_T("MDICLIENT"), parent, MdiClientStyle);
}

/**
 * Calls @c CreateWindowEx using a @c CLIENTCREATESTRUCT.
 */
HWND MdiClient::createHwnd(LPCTSTR className, Widget *parent, Style style)
{
  CLIENTCREATESTRUCT ccs;

  //ccs.hWindowMenu = GetSubMenu(GetMenu(m_hwnd), 1);
  ccs.hWindowMenu = NULL;//GetMenu(parent->m_hwnd);
  ccs.idFirstChild = VACA_FIRST_MDICHILD;

  return CreateWindowEx(style.extended, className, _T(""),
			style.regular,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			parent ? parent->getHwnd(): (HWND)NULL,
			(HMENU)NULL,
			Application::getHinstance(),
			(LPVOID)&ccs);
}

/**
 * 
 * @see tileHorizontal, tileVertical
 */
void MdiClient::cascade()
{
  sendMessage(WM_MDICASCADE, 0, 0);
}

/**
 * 
 * @see tileVertical, cascade
 */
void MdiClient::tileHorizontal()
{
  sendMessage(WM_MDITILE, MDITILE_HORIZONTAL, 0);
}

/**
 * 
 * @see tileHorizontal, cascade
 */
void MdiClient::tileVertical()
{
  sendMessage(WM_MDITILE, MDITILE_VERTICAL, 0);
}

void MdiClient::arrangeIcons()
{
  sendMessage(WM_MDIICONARRANGE, 0, 0);
}

/**
 * 
 * @see activate, activateNext, activatePrevious
 */
MdiChild *MdiClient::getActive()
{
  HWND hwnd = getHwnd();
  VACA_ASSERT(hwnd != NULL);
  HWND hwndChild = reinterpret_cast<HWND>(sendMessage(WM_MDIGETACTIVE, 0, 0));
  if (hwndChild != NULL)
    return dynamic_cast<MdiChild *>(Widget::fromHwnd(hwndChild));
  else
    return NULL;
}

/**
 * 
 * @see activateNext, activatePrevious
 */
void MdiClient::activate(MdiChild *mdiChild)
{
  HWND hChildWnd = mdiChild->getHwnd();
  VACA_ASSERT(hChildWnd != NULL);

  HWND hwnd = getHwnd();
  VACA_ASSERT(hwnd != NULL);

  sendMessage(WM_MDIACTIVATE, reinterpret_cast<WPARAM>(hChildWnd), 0);
}

/**
 * 
 * @see activatePrevious, activate
 */
void MdiClient::activateNext(MdiChild *mdiChild)
{
  HWND hChildWnd = mdiChild != NULL ? mdiChild->getHwnd(): NULL;
  HWND hwnd = getHwnd();
  VACA_ASSERT(hwnd != NULL);

  sendMessage(WM_MDINEXT, reinterpret_cast<WPARAM>(hChildWnd), FALSE);
}

/**
 * 
 * @see activateNext, activate
 */
void MdiClient::activatePrevious(MdiChild *mdiChild)
{
  HWND hChildWnd = mdiChild != NULL ? mdiChild->getHwnd(): NULL;
  HWND hwnd = getHwnd();
  VACA_ASSERT(hwnd != NULL);

  sendMessage(WM_MDINEXT, reinterpret_cast<WPARAM>(hChildWnd), TRUE);
}

/**
 * Returns a MdiChild by its ID.
 */
MdiChild *MdiClient::getChildById(int wID)
{
  Container children = getChildren();

  for (Container::iterator it=children.begin();
       it!=children.end(); ++it) {
    HWND hChild = (*it)->getHwnd();
    VACA_ASSERT(hChild != NULL);

    if (GetWindowLong(hChild, GWL_ID) == wID)
      return static_cast<MdiChild *>(*it);
  }

  return NULL;
}

bool MdiClient::preTranslateMessage(MSG &msg)
{
  if (Widget::preTranslateMessage(msg))
    return true;

  if (TranslateMDISysAccel(getHwnd(), &msg))
    return true;

  return false;
}

//////////////////////////////////////////////////////////////////////
// MdiFrame


/**
 * Creates a new MdiFrame that contains a MdiClient in its client
 * bounds.
 *
 * MdiFrame start with a ClientLayout to arrange the mMdiClient to the
 * full client area, but you can change the layout manager.
 */
MdiFrame::MdiFrame(const String &title, Widget *parent, Style style, bool customMdiClient)
  : Frame(NULL, title)
  , mMdiClient(NULL)
{
  create(FrameClass::getClassName(), parent, style);
  setText(title);
  setLayout(new ClientLayout);

  if (!customMdiClient)
    mMdiClient = new MdiClient(this);
}

// MdiFrame::MdiFrame(Widget *parent, Style style)
//   : Frame(NULL, String(), parent, style)
//   , mMdiClient(NULL)
// {
//   create(FrameClass::getClassName(), parent, style);
//   setLayout(new ClientLayout);

//   mMdiClient = new MdiClient(this);
// }

MdiFrame::~MdiFrame()
{
  VACA_ASSERT(mMdiClient != NULL);
  
  delete mMdiClient;
}

MdiClient &MdiFrame::getMdiClient()
{
  VACA_ASSERT(mMdiClient != NULL);

  return *mMdiClient;
}

void MdiFrame::setMdiClient(MdiClient *mdiClient)
{
  VACA_ASSERT(mMdiClient == NULL);

  mMdiClient = mdiClient;
}

/**
 * Why it's overrided? Because when the frame's menu bar changes, the
 * mMdiClient want a WM_MDISETMENU message to known the new hmenu
 * (MenuBar::getHmenu). TODO Why it's necessary?
 *
 * @see Frame::setMenuBar
 */
void MdiFrame::setMenuBar(MenuBar *menubar)
{
  VACA_ASSERT(mMdiClient != NULL);
  
  // call super class implementation
  Frame::setMenuBar(menubar);

  // we must send the WM_MDISETMENU message to change the frame menu
  // and the window menu...

  HMENU hFrameMenu, hWindowMenu;

  hFrameMenu = menubar->getHmenu();
  VACA_ASSERT(hFrameMenu != NULL);

  MdiListMenu *mdiListMenu = menubar->getMdiListMenu();
  if (mdiListMenu != NULL) {
    hWindowMenu = mdiListMenu->getHmenu();
    VACA_ASSERT(hWindowMenu != NULL);
  }
  else {
    hWindowMenu = NULL;
  }

  mMdiClient->sendMessage(WM_MDISETMENU,
			  reinterpret_cast<WPARAM>(hFrameMenu),
			  reinterpret_cast<WPARAM>(hWindowMenu));

  DrawMenuBar(getHwnd());
}

/**
 * Sends the WM_MDIREFRESHMENU to the mMdiClient window.
 */
void MdiFrame::refreshMenuBar()
{
  VACA_ASSERT(mMdiClient != NULL);

  mMdiClient->sendMessage(WM_MDIREFRESHMENU, 0, 0);
  DrawMenuBar(getHwnd());
}

/**
 * This wndProc stops the message WM_SIZE because the DefFrameProc
 * (see MdiFrame::defWndProc) makes changes in the mMdiClient, and we
 * don't want to modify the behaviour of the current layout manager.
 */
bool MdiFrame::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
  if (Frame::wndProc(message, wParam, lParam, lResult))
    return true;

  switch (message) {

    // avoid passing this message to DefFrameProc
    case WM_SIZE:
      // the Layout manager take care of this message
      lResult = FALSE;
      return true;

  }

  return false;
}

/**
 * Uses the DefFrameProc.
 */
LRESULT MdiFrame::defWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  return DefFrameProc(getHwnd(),
		      mMdiClient != NULL ? mMdiClient->getHwnd(): NULL,
		      message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
// "More Windows" dialog

#if 0
#include "Vaca/Dialog.h"
#include "Vaca/ListBox.h"
#include "Vaca/Panel.h"
#include "Vaca/Button.h"
#include "Vaca/BoxLayout.h"
#include "Vaca/BoxConstraint.h"
#include "Vaca/Bind.h"
#include "Vaca/System.h"

// define this if you want to activate MdiChilds when the selection in
// the ListBox changes...
// #define ACTIVATE_ON_LISTCHANGE

#ifdef ACTIVATE_ON_LISTCHANGE
static void activateSelectedChildren(MdiClient *mdiClient,
				     ListBox *listBox,
				     Widget::Container *mdiChildren)
{
  int index = listBox->getSelectedItem();

  VACA_ASSERT(index >= 0 && index < mdiChildren->size());

  mdiClient->activate(static_cast<MdiChild *>((*mdiChildren)[index]));
}
#endif

/**
 * Shows the "More Windows" dialog. You can show your how dialog, but
 * remeber that you must to activate the selected MdiChild.
 *
 * @deprecated It was useful only when WM_MENUCOMMAND was used, right
 *             now it's deprecated, but it'll be back for customize the
 *             default "More Windows" dialog of Win32
 */
void MdiFrame::onMoreWindows()
{
  Dialog dlg("More Windows", this, DialogStyle + ResizableFrameStyle);
  ListBox listBox(&dlg);
  Panel bottom(&dlg);
  Panel dummy(&bottom);
  Button okButton("&OK", &bottom);
  Button cancelButton("&Cancel", &bottom);

  // list-box of children
  Widget::Container mdiChildren = mMdiClient->getChildren();
  MdiChild *activeChild = mMdiClient->getActive();
  int index = 0;

  for (Widget::Container::iterator
	 it=mdiChildren.begin();
       it!=mdiChildren.end(); ++it, ++index) {
    listBox.addItem((*it)->getText());

    // by default select the current active MdiChild in the ListBox
    if (*it == activeChild)
      listBox.setCurrentItem(index);
  }

  // default button
  okButton.setDefault(true);

  // layout
  dlg.setLayout(new VBoxLayout(false));
  bottom.setLayout(new HBoxLayout(false, 0));
  listBox.setConstraint(new BoxConstraint(true));
  dummy.setConstraint(new BoxConstraint(true));

  // size & position
  dlg.setSize(System::getWorkAreaBounds().getSize()
	      .createIntersect(Size(256, 256).createUnion(dlg.preferredSize())));
  dlg.center();

  // bindings
  listBox.Action.connect(boost::bind(&Dialog::defaultOkAction, &dlg));
#ifdef ACTIVATE_ON_LISTCHANGE
  listBox.SelChange.connect(boost::bind(activateSelectedChildren,
					&mMdiClient, &listBox, &mdiChildren));
#endif
  okButton.Action.connect(boost::bind(&Dialog::defaultOkAction, &dlg));
  cancelButton.Action.connect(boost::bind(&Dialog::defaultCancelAction, &dlg));

  // do modal
  if (dlg.doModal()) {
    int index = listBox.getCurrentItem();

    VACA_ASSERT(index >= 0 && index < mdiChildren.size());

    mMdiClient->activate(static_cast<MdiChild *>(mdiChildren[index]));
  }
}
#endif
