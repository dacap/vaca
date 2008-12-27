// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
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

#include "Vaca/Mdi.h"
#include "Vaca/Application.h"
#include "Vaca/Debug.h"
#include "Vaca/Event.h"
#include "Vaca/ClientLayout.h"
#include "Vaca/Menu.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// MdiChild


static void MdiChild_DestroyHWNDProc(HWND hwnd)
{
  // we must be limited to use only the HWND...
  HWND hClient = ::GetParent(hwnd);
  assert(hClient != NULL);
  ::SendMessage(hClient, WM_MDIDESTROY, reinterpret_cast<WPARAM>(hwnd), 0);
}

/**
 * Creates a new MDI child window.
 *
 * @warning All MdiChilds are created visible and activated (and this
 *          can't be changed, see @ref page_tn_005 for more information).
 */
MdiChild::MdiChild(const String& title, MdiClient* parent, Style style)
  : Frame(WidgetClassName::None, "", NULL, Widget::Styles::None)
{
  setDefWndProc(DefMDIChildProc);
  setDestroyHWNDProc(MdiChild_DestroyHWNDProc);

  create(MdiChildClass::getClassName(), parent, style);
  setText(title);
  initialize();
}

/**
 * Creates a new MDI child window.
 *
 * @warning All MdiChilds are created visible and activated (and this
 *          can't be changed, see @ref page_tn_005 for more information).
 */
MdiChild::MdiChild(const String& title, MdiFrame* parent, Style style)
  : Frame(WidgetClassName::None, "", NULL, Widget::Styles::None)
{
  setDefWndProc(DefMDIChildProc);
  setDestroyHWNDProc(MdiChild_DestroyHWNDProc);

  create(MdiChildClass::getClassName(), parent->getMdiClient(), style);
  setText(title);
  initialize();
}

MdiChild::~MdiChild()
{
}

void MdiChild::initialize()
{
  // see TN005
  ::PostMessage(getHandle(), WM_MDIACTIVATE, 0, reinterpret_cast<LPARAM>(getHandle()));
  ::PostMessage(getHandle(), WM_SETFOCUS, 0, 0);

  Rect rc = getClientBounds();
  ::PostMessage(getHandle(), WM_SIZE, 0, MAKELPARAM(rc.w, rc.h));

  // refresh the MdiListMenu (see TN007)
  if (getParent() != NULL)
    getParent()->sendMessage(WM_MDIREFRESHMENU, 0, 0);
}

// // Intercept commands from @c VACA_IDM_MDICHILD to activate this MdiChild.
// bool MdiChild::onReflectedCommand(int it, int code, LRESULT& lResult)
// {
//   if (Frame::onReflectedCommand(id, code, lResult))
//     return true;

//   // Note: why just don't call the DefFrameProc with WM_COMMAND? it
//   // looks like it doesn't work fine with menus that has
//   // MNS_NOTIFYBYPOS enabled

//   // the user selects a MdiChild from the MdiListMenu?
//   if (commandCode == 0) {
//     MdiClient* parent = static_cast<MdiClient*>(getParent());
//     assert(parent != NULL);

//     parent->activate(this);
//     return true;
//   }

//   return false;
// }

/**
 * Intercepts the WM_MDIACTIVATE message to generate the
 * Frame::onActivate and Frame::onDeactivate events.
 */
bool MdiChild::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
  if (Frame::wndProc(message, wParam, lParam, lResult))
    return true;

  switch (message) {

    case WM_MDIACTIVATE: {
      if (getHandle() == reinterpret_cast<HWND>(wParam)) {
	Event ev(this);
	onDeactivate(ev);
      }
      else if (getHandle() == reinterpret_cast<HWND>(lParam)) {
	Event ev(this);
	onActivate(ev);
      }
      break;
    }

  }

  return false;
}

// Don't try to do this alternative (using WM_MDICREATE), it doesn't
// fix the TN005 problem (also it doesn't support the extended styles WS_EX)
#if 0				// don't remove this code, must be for
				// historical purposes
HWND MdiChild::createHandle(LPCTSTR className, Widget* parent, Style style)
{
  HWND hClient = parent->getHandle();
  assert(hClient != NULL);

  MDICREATESTRUCT mdics;
  mdics.szClass = className;
  mdics.szTitle = _T("");
  mdics.hOwner = Application::getHandle();
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

//////////////////////////////////////////////////////////////////////
// MdiClient
 

/**
 * Container for MdiChild windows.
 */
MdiClient::MdiClient(Widget* parent, Style style)
  : Widget(WidgetClassName::None, NULL, Widget::Styles::None) // className must be NULL to avoid calling the Widget::createHandle
{
  // now Widget::create calls MdiClient::createHandle because MdiClient is constructed
  create(WidgetClassName(_T("MDICLIENT")), parent, style);
}

MdiClient::~MdiClient()
{
}

/**
 * Calls @c CreateWindowEx using a @c CLIENTCREATESTRUCT.
 */
HWND MdiClient::createHandle(LPCTSTR className, Widget* parent, Style style)
{
  CLIENTCREATESTRUCT ccs;

  //ccs.hWindowMenu = GetSubMenu(GetMenu(m_hwnd), 1);
  ccs.hWindowMenu = NULL;//GetMenu(parent->m_hwnd);
  ccs.idFirstChild = VACA_FIRST_MDICHILD;

  return CreateWindowEx(style.extended, className, _T(""),
			style.regular,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			parent ? parent->getHandle(): (HWND)NULL,
			(HMENU)NULL,
			Application::getHandle(),
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
MdiChild* MdiClient::getActive()
{
  HWND hwnd = getHandle();
  assert(::IsWindow(hwnd));
  HWND hwndChild = reinterpret_cast<HWND>(sendMessage(WM_MDIGETACTIVE, 0, 0));
  if (hwndChild != NULL)
    return dynamic_cast<MdiChild*>(Widget::fromHandle(hwndChild));
  else
    return NULL;
}

/**
 * 
 * @see activateNext, activatePrevious
 */
void MdiClient::activate(MdiChild* mdiChild)
{
  HWND hChildWnd = mdiChild->getHandle();
  assert(hChildWnd != NULL);

  HWND hwnd = getHandle();
  assert(::IsWindow(hwnd));

  sendMessage(WM_MDIACTIVATE, reinterpret_cast<WPARAM>(hChildWnd), 0);
}

/**
 * 
 * @see activatePrevious, activate
 */
void MdiClient::activateNext(MdiChild* mdiChild)
{
  HWND hChildWnd = mdiChild != NULL ? mdiChild->getHandle(): NULL;
  HWND hwnd = getHandle();
  assert(::IsWindow(hwnd));

  sendMessage(WM_MDINEXT, reinterpret_cast<WPARAM>(hChildWnd), FALSE);
}

/**
 * 
 * @see activateNext, activate
 */
void MdiClient::activatePrevious(MdiChild* mdiChild)
{
  HWND hChildWnd = mdiChild != NULL ? mdiChild->getHandle(): NULL;
  HWND hwnd = getHandle();
  assert(::IsWindow(hwnd));

  sendMessage(WM_MDINEXT, reinterpret_cast<WPARAM>(hChildWnd), TRUE);
}

/**
 * Returns a MdiChild by its ID.
 */
MdiChild* MdiClient::getChildById(int wID)
{
  Container children = getChildren();

  for (Container::iterator it=children.begin();
       it!=children.end(); ++it) {
    HWND hChild = (*it)->getHandle();
    assert(hChild != NULL);

    if (GetWindowLong(hChild, GWL_ID) == wID)
      return static_cast<MdiChild*>(*it);
  }

  return NULL;
}

bool MdiClient::preTranslateMessage(Message& message)
{
  if (Widget::preTranslateMessage(message))
    return true;

  if (TranslateMDISysAccel(getHandle(), (LPMSG)message))
    return true;

  return false;
}

//////////////////////////////////////////////////////////////////////
// MdiFrame


static LRESULT WINAPI MdiFrame_defWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  Widget* widget = Widget::fromHandle(hwnd);
  if (widget != NULL) {
    MdiFrame* frame = dynamic_cast<MdiFrame*>(widget);
    MdiClient* client = frame != NULL ? frame->getMdiClient(): NULL;
    HWND hclient = client != NULL ? client->getHandle(): NULL;

    return ::DefFrameProc(hwnd, hclient, message, wParam, lParam);
  }

  // ::GetWindow(hwnd, GW_CHILD);
  return ::DefFrameProc(hwnd, NULL, message, wParam, lParam);
}

/**
 * Creates a new MdiFrame that contains a MdiClient in its client
 * bounds.
 *
 * MdiFrame start with a ClientLayout to arrange the MdiClient to the
 * full client area, but you can change the layout manager.
 */
MdiFrame::MdiFrame(const String& title, Widget* parent, Style style, bool customMdiClient)
  : Frame(WidgetClassName::None, title)
  , m_mdiClient(NULL)
{
  setDefWndProc(MdiFrame_defWndProc);
  
  create(FrameClass::getClassName(), parent, style);
  setText(title);
  setLayout(new ClientLayout);

  if (!customMdiClient)
    m_mdiClient = new MdiClient(this);
}

// MdiFrame::MdiFrame(Widget* parent, Style style)
//   : Frame(NULL, String(), parent, style)
//   , m_mdiClient(NULL)
// {
//   create(FrameClass::getClassName(), parent, style);
//   setLayout(new ClientLayout);

//   m_mdiClient = new MdiClient(this);
// }

MdiFrame::~MdiFrame()
{
  delete m_mdiClient;
}

MdiClient* MdiFrame::getMdiClient()
{
//   assert(m_mdiClient != NULL);

  return m_mdiClient;
}

/** 
 * 
 * 
 * @param mdiClient It's deleted automatically in the destructor of
 *                  MdiClient. To avoid this you should do a
 *                  setMdiClient(NULL) before.
 *
 * @see @ref page_tn_010
 */
MdiClient* MdiFrame::setMdiClient(MdiClient *mdiClient)
{
//   assert(m_mdiClient == NULL);

  MdiClient* oldMdiClient = m_mdiClient;
  m_mdiClient = mdiClient;
  return oldMdiClient;
}

/**
 * Why it's overrided? Because when the frame's menu bar changes, the
 * m_mdiClient want a WM_MDISETMENU message to known the new hmenu
 * (@ref MenuBar#getHandle). TODO Why it's necessary?
 *
 * @see Frame::setMenuBar
 */
MenuBar* MdiFrame::setMenuBar(MenuBar* menubar)
{
  assert(m_mdiClient != NULL);
  
  // call super class implementation
  MenuBar* oldMenuBar = Frame::setMenuBar(menubar);

  // we must send the WM_MDISETMENU message to change the frame menu
  // and the window menu...

  HMENU hFrameMenu, hWindowMenu;

  hFrameMenu = menubar->getHandle();
  assert(hFrameMenu != NULL);

  MdiListMenu* mdiListMenu = menubar->getMdiListMenu();
  if (mdiListMenu != NULL) {
    hWindowMenu = mdiListMenu->getHandle();
    assert(hWindowMenu != NULL);
  }
  else {
    hWindowMenu = NULL;
  }

  m_mdiClient->sendMessage(WM_MDISETMENU,
			  reinterpret_cast<WPARAM>(hFrameMenu),
			  reinterpret_cast<WPARAM>(hWindowMenu));

  DrawMenuBar(getHandle());

  return oldMenuBar;
}

/**
 * Sends the WM_MDIREFRESHMENU to the m_mdiClient window.
 */
void MdiFrame::refreshMenuBar()
{
  assert(m_mdiClient != NULL);

  m_mdiClient->sendMessage(WM_MDIREFRESHMENU, 0, 0);
  DrawMenuBar(getHandle());
}

/**
 * This wndProc stops the message WM_SIZE because the DefFrameProc
 * (see MdiFrame::defWndProc) makes changes in the m_mdiClient, and we
 * don't want to modify the behaviour of the current layout manager.
 */
bool MdiFrame::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
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
// LRESULT MdiFrame::defWndProc(UINT message, WPARAM wParam, LPARAM lParam)
// {
//   return DefFrameProc(getHandle(),
// 		      m_mdiClient != NULL ? m_mdiClient->getHandle(): NULL,
// 		      message, wParam, lParam);
// }
