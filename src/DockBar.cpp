// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

#include "Vaca/DockBar.hpp"
#include "Vaca/DockFrame.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/Application.hpp"
#include "Vaca/MouseEvent.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/System.hpp"

#define DEF_GRIPPER_SIZE 8

using namespace Vaca;

// Information about the DockBar when the user is dragging it.
struct DockBar::DragInfo
{
  bool startDocked;  // The drag start with the DockBar docked.
  Rect startRect;    // The rectangle where the DockBar was before dragging.
  Rect currentRect;  // Current drag rectangle (where to draw the XORed feedback rectangle).
  Point begMousePos; // The position of the mouse when the drag start.
  Point oldMousePos; // Last mouse position visited
  bool oldCtrlState; // Old state of the Control key
  DockInfo* dockIn;  // Where we could dock this bar
  DockArea* inArea;  // If we enter in a DockArea we should keep us inside it
  Point anchorPos;   // Offset where the cursor start in the client area of the DockBar
};

static HHOOK dragHook = NULL;
static bool isControlPressed = false;
static HWND hwndDockWnd = NULL;

// based on the code of James Brown (http://www.catch22.net)
static LRESULT CALLBACK dragHookProc(int code, WPARAM wParam, LPARAM lParam)
{
  ULONG state = (ULONG)lParam;

  if (code < 0) 
    return CallNextHookEx(dragHook, code, wParam, lParam);

  if (wParam == VK_CONTROL) {
    if (state & 0x80000000)
      isControlPressed = FALSE;
    else
      isControlPressed = TRUE;

    // the cursor position isn't gotten from lParam
    // and the control state isn't gotten from wParam
    SendMessage(hwndDockWnd, WM_MOUSEMOVE, 0, 0);
    
    return -1;
  }

  if (wParam == VK_ESCAPE) {
    PostMessage(hwndDockWnd, WM_CANCELMODE, 0, 0);
    return 0;
  }

  return CallNextHookEx(dragHook, code, wParam, lParam);
}


// Creates a new DockBar
DockBar::DockBar(const String& title, Frame* parent, Style style)
  : Widget(DockBarClass::getClassName(), parent, style)
{
  setText(title);

  m_dockArea = NULL;
  m_drag = NULL;
  m_dockFrame = NULL;
//   m_dockFrameGarbage = NULL;
  m_owner = parent;
  m_dockInfo = NULL;

  m_fullDrag = false;
  m_floatingGripper = false;

//   BOOL param = FALSE;
//   SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, reinterpret_cast<PVOID>(&param), 0);
//   m_fullDrag = param;

  setVisible(false);

  if (parent != NULL)
    parent->removeChild(this, true);
}

DockBar::~DockBar()
{
//   dispose();

  assert(m_drag == NULL);

  // undock, and delete DockFrame or DockFrameGarbage
  cleanUp();

  // delete DockInfo
  if (m_dockInfo != NULL) {
    delete m_dockInfo;
    m_dockInfo = NULL;
  }
  
}

/**
 * You can use setVisible() to hide or show the DockBar. If you hide
 * the DockBar, this method deletes the #Vaca::DockFrame if it exist, or
 * remove the DockBar from the #Vaca::DockArea. All is automatic.
 */
void DockBar::setVisible(bool visible)
{
  Widget::setVisible(visible);

  if (!visible)
    cleanUp();
  else {
    if (m_dockArea == NULL && m_dockFrame == NULL)
      // TODO save the old state, and dock when is necessary
      makeFloat(NULL);
  }
}

/**
 * Leaves some space for the gripper (using measureGripper()).
 */
Rect DockBar::getLayoutBounds()
{
  Rect rc = Widget::getLayoutBounds();

  if (isGripperVisible(isDocked(),
		       isDocked() ? m_dockArea->getSide(): Side())) {
    Side gripperSide = getGripperSide(isDocked(),
				      isDocked() ? m_dockArea->getSide(): Side());
    Size gripperSize = measureGripper(isDocked(),
				      isDocked() ? m_dockArea->getSide(): Side());

    switch (gripperSide) {
      case Side::Left:
	rc.x += gripperSize.w;
	rc.w -= gripperSize.w;
	break;
      case Side::Right:
	rc.w -= gripperSize.w;
	break;
      case Side::Top:
	rc.y += gripperSize.h;
	rc.h -= gripperSize.h;
	break;
      case Side::Bottom:
	rc.h -= gripperSize.h;
	break;
    }
  }

  return rc;
}

/*
 * Sets the drag full tool bar mode. If @a state is false, only a
 * border is moved when the tool bar is dragged.  By default the full
 * drag mode is disabled.
 */
void DockBar::setFullDrag(bool state)
{
  m_fullDrag = state;
}

bool DockBar::isFullDrag()
{
  return m_fullDrag;
}

/**
 * If @a state is true, the gripper is drawn on floating mode too. By
 * default, the gripper is not drawn in floating mode.
 */
void DockBar::setFloatingGripper(bool state)
{
  m_floatingGripper = state;

  if (isFloating()) {
    m_dockFrame->setSize(m_dockFrame->getPreferredSize());
    m_dockFrame->invalidate(true);
  }
}

/**
 * Default value: false.
 */
bool DockBar::isFloatingGripper()
{
  return m_floatingGripper;
}

/**
 * Returns true if the bar is docked in a DockArea.
 */
bool DockBar::isDocked()
{
  return m_dockArea != NULL;
}

/**
 * Returns true if the bar is floating in a DockFrame.
 */
bool DockBar::isFloating()
{
  return m_dockFrame != NULL;
}

void DockBar::dockIn(DockArea* dockArea)
{
  assert(dockArea != NULL);
  
  // does it have DockInfo?
  if (m_dockInfo == NULL) {
    // create the default one
    m_dockInfo = dockArea->createDefaultDockInfo(this);
  }

  makeDock(dockArea, m_dockInfo);
}

void DockBar::floatOut()
{
  makeFloat(NULL);
}

Frame* DockBar::getOwnerFrame()
{
  return m_owner;
}

DockArea* DockBar::getDockArea()
{
  return m_dockArea;
}

DockFrame* DockBar::getDockFrame()
{
  return m_dockFrame;
}

DockInfo* DockBar::getDockInfo()
{
  return m_dockInfo;
}

Size DockBar::getDockedSize(Side side)
{
  return getBounds().getSize();
}

Size DockBar::getFloatingSize()
{
  return getBounds().getSize();
}

void DockBar::makeDock(DockArea* dockArea, DockInfo* dockInfo)
{
  bool redock = false;

  // redock in the same dock area?
  if (m_dockArea != NULL && dockArea == m_dockArea) {
    Widget::setVisible(false);
    
    redock = true;
    m_dockArea->onRedock(this, m_drag->dockIn);
    m_dockArea->removeDockBar(this);
  }
  // hide the DockBar (disposing the DockFrame if exists)
  else {
    setVisible(false);
  }

  // set the new DockInfo?
  if (m_dockInfo != dockInfo) {
    if (m_dockInfo != NULL)
      delete m_dockInfo;

    m_dockInfo = dockInfo;
  }

  // set the new DockArea
  m_dockArea = dockArea;
  m_dockArea->addDockBar(this);

  setVisible(true);

  onDocking();

  m_dockArea->getParent()->layout();
}

void DockBar::makeFloat(const Rect* rect)
{
  Size floatingSize = getFloatingSize();

  if (m_dockFrame == NULL) {
    setVisible(false);

    m_dockFrame = new DockFrame(this, m_owner);
    m_dockFrame->addChild(this, true);
    m_dockFrame->Close.connect(&DockBar::onDockFrameClose, this);
    // m_dockFrame->Destroy.connect(&DockBar::onDockFrameDestroy, this);

    setVisible(true);

    m_dockFrame->layout();
  }

  if (rect != NULL) {
    if (m_drag != NULL && m_drag->startDocked)
      m_dockFrame->setBounds(Rect(rect->getOrigin(),
				 m_dockFrame->getNonClientSize() + floatingSize));
    else
      m_dockFrame->setBounds(*rect);
  }
  else {
    m_dockFrame->setBounds(Rect(m_owner->getAbsoluteClientBounds().getOrigin(),
			       m_dockFrame->getNonClientSize() + floatingSize));
  }

  m_dockFrame->setVisible(true);

  focusOwner();
  onFloating();
}

// void DockBar::onDestroy()
// {
//   assert(m_drag == NULL);

//   // undock, and delete DockFrame or DockFrameGarbage
//   cleanUp();

//   // delete DockInfo
//   if (m_dockInfo != NULL) {
//     delete m_dockInfo;
//     m_dockInfo = NULL;
//   }

// //   Widget::onDestroy();
// }

void DockBar::onPreferredSize(Size& sz)
{
  Widget::onPreferredSize(sz);
  sz += measureGripper(isDocked(),
		       isDocked() ? m_dockArea->getSide(): Side());
}

/**
 * Called when m_dockFrame is closed by its "Close" button.
 */
void DockBar::onDockFrameClose(CloseEvent& ev)
{
  Widget::setVisible(false);

  m_dockFrame->removeChild(this, true);
//   m_dockFrame->Destroy.disconnect_all_slots();
  // TODO XXXX check this
//   m_dockFrame->deleteAfterEvent();
  delete_widget(m_dockFrame);
  m_dockFrame = NULL;
}

/**
 * Called when m_dockFrame is destroyed by its parent. In this case
 * m_dockFrameGarbage holds the m_dockFrame pointer to be deleted,
 * because the m_dockFrame is in its disposition process, we can't
 * delete right now the m_dockFrame, so we must to hold on the pointer
 * to postpone its deletion.
 */
// void DockBar::onDockFrameDestroy()
// {
//   assert(m_dockFrame != NULL);

//   if (getParent() == m_dockFrame) {
// //     m_dockFrame->removeChild(this, false);
//     m_dockFrame->removeChild(this, true);
    
//     // we can't delete m_dockFrame here (we come from its signal),
//     // we must to postpone its deletetion
//     m_dockFrameGarbage = m_dockFrame;
//     m_dockFrame = NULL;
//   }
// }

/**
 * Calls paintGripper().
 */
void DockBar::onPaint(Graphics& g)
{
  paintGripper(g);
}

/**
 * When the gripper is visible in the floating state
 * (m_floatingGripper == true), we must to repaint the DockBar's
 * gripper when the DockBar is resized.
 */
void DockBar::onResize(const Size& sz)
{
  Widget::onResize(sz);

  if (m_floatingGripper)
    invalidate(true);
}

void DockBar::onMouseDown(MouseEvent& ev)
{
  // we are dragging?
  if (m_drag != NULL) {
    // cancel drag
    if (ev.getButton() == MouseButtons::Right) {
      if (!m_fullDrag) {
	ScreenGraphics g;
	cleanFeedbackShape(g);
      }

      endDrag();
    }

    // do nothing
    return;
  }

  // start dragging?
  if (ev.getButton() == MouseButtons::Left) {
    captureMouse();

    if (hasCapture()) {
      beginDrag();

      if (!m_fullDrag) {
	ScreenGraphics g;
	drawFeedbackShape(g);
      }
    }
  }
}

void DockBar::onMouseMove(MouseEvent& ev)
{
  if (hasCapture()) {
    assert(m_drag != NULL);

    //////////////////////////////////////////////////////////////////////
    // does the mouse or the control key change?
    Point newPos = System::getCursorPos();
    bool newCtrl = System::getKeyState(Keys::ControlKey);

    if (m_drag->oldMousePos == newPos &&
	m_drag->oldCtrlState == newCtrl)
      return;

    m_drag->oldMousePos = newPos;
    m_drag->oldCtrlState = newCtrl;
    //////////////////////////////////////////////////////////////////////

    ScreenGraphics g;

    if (!m_fullDrag)
      cleanFeedbackShape(g);

    if (m_drag->dockIn != NULL)
      delete m_drag->dockIn;
    m_drag->dockIn = calcDestination(m_drag->currentRect);

    if (m_fullDrag)
      dragBar();
    else
      drawFeedbackShape(g);
  }
}

void DockBar::onMouseUp(MouseEvent& ev)
{
  if (hasCapture()) {
    assert(m_drag != NULL);

    if (!m_fullDrag) {
      ScreenGraphics g;
      cleanFeedbackShape(g);

      dragBar();
    }

    endDrag();
  }
}

void DockBar::onDoubleClick(MouseEvent& ev)
{
  assert(m_owner != NULL);

  // is floating? dock it in the last dock position remembered
  if (isFloating()) {
    DockArea* dockArea = NULL;

    // the DockBar has DockInfo, so use it
    if (m_dockInfo != NULL)
      dockArea = m_owner->getDockArea(m_dockInfo->getSide());
    // create a default DockInfo
    else {
      dockArea = m_owner->getDefaultDockArea();
      if (dockArea != NULL)
	m_dockInfo = dockArea->createDefaultDockInfo(this);
    }

    if (dockArea != NULL && m_dockInfo != NULL)
      dockIn(dockArea);
    // else { do nothing, we can't dock the tool-bar, so it should
    // continue floating }
  }
}

void DockBar::onCancelMode()
{
  if (m_drag != NULL) {
    if (!m_fullDrag) {
      ScreenGraphics g;
      cleanFeedbackShape(g);
    }
    else {
      // TODO
    }

    endDrag();
  }
}

/**
 * Event called when the DockBar is docked in a new DockArea. You can
 * use getDockArea() method to known where the DockBar is docked.
 */
void DockBar::onDocking()
{
  assert(getDockArea() != NULL);
}

/**
 * Event called when the DockBar was docked and now is floating in
 * some DockFrame.
 */
void DockBar::onFloating()
{
}

/**
 * When the DockBar is floating, and its DockFrame container is
 * resized (DockFrame::onResizing()), this event is fired.
 */
void DockBar::onResizingFrame(DockFrame* frame, int edge, Rect& rc)
{
}

void DockBar::paintGripper(Graphics& g)
{
  if (isGripperVisible(isDocked(), 
		       isDocked() ? m_dockArea->getSide():
				    Side())) {
    Color topLeft = System::getColor(COLOR_3DHIGHLIGHT);
    Color bottomRight = System::getColor(COLOR_3DSHADOW);
    Rect rc = getClientBounds();
    Side gripperSide = getGripperSide(isDocked(), 
				      isDocked() ? m_dockArea->getSide():
						   Side());

    switch (gripperSide) {

      case Side::Left:
	g.draw3dRect(rc.x, rc.y, 3, rc.h, topLeft, bottomRight);
	g.draw3dRect(rc.x+3, rc.y, 3, rc.h, topLeft, bottomRight);
	break;

      case Side::Top:
	g.draw3dRect(rc.x, rc.y, rc.w, 3, topLeft, bottomRight);
	g.draw3dRect(rc.x, rc.y+3, rc.w, 3, topLeft, bottomRight);
	break;

      case Side::Right:
	g.draw3dRect(rc.x+rc.w-3, rc.y, 3, rc.h, topLeft, bottomRight);
	g.draw3dRect(rc.x+rc.w-6, rc.y, 3, rc.h, topLeft, bottomRight);
	break;

      case Side::Bottom:
	g.draw3dRect(rc.x, rc.y+rc.h-3, rc.w, 3, topLeft, bottomRight);
	g.draw3dRect(rc.x, rc.y+rc.h-6, rc.w, 3, topLeft, bottomRight);
	break;

    }
  }
}

/**
 * @return This must return Size(gripperWidth, 0),
 *         Size(0, gripperHeight), or Size(0, 0) if the gripper is hidden
 *         (use isGripperVisible() to know that).
 */
Size DockBar::measureGripper(bool docked, Side dockSide)
{
  if (isGripperVisible(docked, dockSide)) {
    Side gripperSide = getGripperSide(docked, dockSide);

    if (gripperSide == Side::Left ||
	gripperSide == Side::Right)
      return Size(DEF_GRIPPER_SIZE, 0);
    else
      return Size(0, DEF_GRIPPER_SIZE);
  }
  return Size(0, 0);
}

Side DockBar::getGripperSide(bool docked, Side dockSide)
{
  if (docked) {
    if (dockSide == Side::Left ||
	dockSide == Side::Right)
      return Side::Top;
  }
  else {
    Size sz = getClientBounds().getSize();
    if (sz.h >= sz.w)
      return Side::Top;
  }
  return Side::Left;
}

bool DockBar::isGripperVisible(bool docked, Side dockSide)
{
  if (docked || isFloatingGripper())
    return true;
  else
    return false;
}

// creates the DragInfo to start dragging this DockBar...
void DockBar::beginDrag()
{
  assert(m_drag == NULL);
  
  // install the keyboard hook (to hook the CONTROL key changes, and
  // the ESC key)
  assert(hwndDockWnd == NULL && dragHook == NULL);
  hwndDockWnd = getHWND();
  dragHook = SetWindowsHookEx(WH_KEYBOARD, dragHookProc, NULL, ::GetCurrentThreadId()); 

//   if (m_dockArea != NULL)
//     m_dockArea->onBeginDockBarDrag(this);

  // if the DockBar is floating, bring the DockFrame to top
  if (m_dockFrame != NULL)
    m_dockFrame->bringToTop();

  //////////////////////////////////////////////////////////////////////
  // create the DragInfo...
  m_drag = new DragInfo;
  m_drag->startDocked = isDocked();
  m_drag->currentRect =
    m_dockFrame != NULL ? m_dockFrame->getAbsoluteBounds():
			 this->getAbsoluteBounds().inflate(getNonClientSizeForADockFrame());
  m_drag->startRect    = m_drag->currentRect;
  m_drag->begMousePos  = System::getCursorPos();
  m_drag->oldMousePos  = System::getCursorPos();
  m_drag->oldCtrlState = System::getKeyState(Keys::ControlKey);

  // calculate the position of the anchor point (offset where
  // the cursor is in the client area)
  Point anchor;
  Rect bounds = getAbsoluteClientBounds();
  anchor = m_drag->begMousePos - bounds.getOrigin();
  anchor.x = anchor.x < 0 ? 0: (anchor.x >= bounds.w ? bounds.w-1: anchor.x);
  anchor.y = anchor.y < 0 ? 0: (anchor.y >= bounds.h ? bounds.h-1: anchor.y);
  m_drag->anchorPos = anchor;

  Rect dummy;
  m_drag->inArea = NULL;
  m_drag->dockIn = calcDestination(dummy);
  m_drag->inArea = m_dockArea;
  // ...done.
  //////////////////////////////////////////////////////////////////////
}

// moves the DockBar to the place that indicates the current m_drag information
void DockBar::dragBar()
{
  // inside a dock bar
  if (m_drag->dockIn != NULL) {
    makeDock(m_owner->getDockArea(m_drag->dockIn->getSide()),
	     m_drag->dockIn);

    // now "m_dockInfo" is "m_drag->dockIn", so avoid to delete it in endDrag()
    assert(m_dockInfo == m_drag->dockIn);
    m_drag->dockIn = NULL;
  }
  // floating
  else {
    makeFloat(&m_drag->currentRect);
  }
}

// destroys the DragInfo (m_drag)
void DockBar::endDrag()
{
  assert(m_drag != NULL);
  
  // destroy the DragInfo...
  if (m_drag->dockIn != NULL)
    delete m_drag->dockIn;
  delete m_drag;
  m_drag = NULL;

  // we don't use anymore the keyboard hook
  if (dragHook != NULL) {
    UnhookWindowsHookEx(dragHook);
    dragHook = NULL;
  }

  assert(hwndDockWnd == getHWND());
  hwndDockWnd = NULL;

      // end dragging
//     if (m_dockArea != NULL)
//       m_dockArea->onEndDockBarDrag(this, m_drag->dockIn);

  releaseMouse();
}

// used to hide the DockBar, mainly deletes the m_dockFrame and removes
// this DockBar from m_dockArea (if it isn't NULL)
void DockBar::cleanUp()
{
  // undock
  if (m_dockArea != NULL) {
    if (m_dockArea->getHWND() != NULL) {
      m_dockArea->removeDockBar(this);
      m_dockArea->getParent()->layout();
    }

    m_dockArea = NULL;
  }

  // delete the DockFrame
  if (m_dockFrame != NULL) {
    m_dockFrame->Close.disconnectAll();
    // m_dockFrame->Destroy.disconnectAll();

    if (m_dockFrame->getHWND() != NULL)
      m_dockFrame->removeChild(this, true);

    delete_widget(m_dockFrame);
    m_dockFrame = NULL;
  }

  // delete garbage
//   if (m_dockFrameGarbage != NULL) {
//     delete m_dockFrameGarbage;
//     m_dockFrameGarbage = NULL;
//   }
}

/**
 * Focus the owner window.
 */
void DockBar::focusOwner()
{
  if (m_owner != NULL)
    m_owner->requestFocus();
}

DockInfo* DockBar::calcDestination(Rect& rc)
{
  Point cursor = System::getCursorPos();
  Point pt = m_drag->startRect.getOrigin() + cursor - m_drag->begMousePos;
  Size sz = !m_drag->startDocked ? m_drag->startRect.getSize():
				  getFloatingSize() + getNonClientSizeForADockFrame();

  rc = Rect(pt, sz);

  if (!System::getKeyState(Keys::ControlKey) && (m_owner != NULL)) {
    // for each dock area
    std::vector<DockArea*> dockAreas = m_owner->getDockAreas();
    std::vector<DockArea*>::reverse_iterator it;

    // add the inArea, it has priority
    if (m_drag->inArea != NULL)
      dockAreas.push_back(m_drag->inArea);

    for (it=dockAreas.rbegin(); it!=dockAreas.rend(); ++it) {
      DockArea* dockArea = *it;

      // we can dock here?
      if (dockArea->hitTest(this,		      // this dock bar
			    cursor,		      // cursor position
			    m_drag->anchorPos,         // anchor position
			    m_drag->inArea == dockArea // from inside?
			    )) {
	// return a new DockInfo
	return dockArea->createDockInfo(this, cursor, m_drag->anchorPos);
      }
    }
  }

  return NULL;
}

void DockBar::drawFeedbackShape(Graphics& g)
{
  if (m_drag->dockIn != NULL)
    m_drag->inArea = xorFeedbackShape(g);
  else
    xorFeedbackShape(g);
}

void DockBar::cleanFeedbackShape(Graphics& g)
{
  xorFeedbackShape(g);
}

DockArea* DockBar::xorFeedbackShape(Graphics& g)
{
  assert(m_drag != NULL);

  // clean docked shape
  if (m_drag->dockIn != NULL) {
    DockArea* dockArea = m_owner->getDockArea(m_drag->dockIn->getSide());

    assert(dockArea != NULL);
    
    dockArea->drawXorDockInfoShape(g, m_drag->dockIn);

    return dockArea;
  }
  // clean floating rectangle
  else {
    g.drawXorFrame(m_drag->currentRect);
    return NULL;
  }
}

Size DockBar::getNonClientSizeForADockFrame()
{
  Rect clientRect(0, 0, 1, 1);
  RECT nonClientRect = clientRect;
  Style style = DockFrameStyle;

  ::AdjustWindowRectEx(&nonClientRect, style.regular, false, style.extended);

  return Rect(&nonClientRect).getSize() - clientRect.getSize();
}
