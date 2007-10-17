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
#include "Vaca/DockBar.h"
#include "Vaca/DockFrame.h"
#include "Vaca/Debug.h"
#include "Vaca/Application.h"
#include "Vaca/MouseEvent.h"
#include "Vaca/Debug.h"
#include "Vaca/System.h"
#include "Vaca/Bind.h"

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
  DockInfo *dockIn;  // Where we could dock this bar
  DockArea *inArea;  // If we enter in a DockArea we should keep us inside it
  Point anchorPos;   // Offset where the cursor start in the client area of the DockBar
};

static HHOOK dragHook = NULL;
static bool isControlPressed = false;
static HWND hwndDockWnd = NULL;

// based on the code of James Brown
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
DockBar::DockBar(const String &title, Frame *parent, Style style)
  : Widget(DockBarClass::getClassName(), parent, style)
{
  setText(title);

  mDockArea = NULL;
  mDrag = NULL;
  mDockFrame = NULL;
  mDockFrameGarbage = NULL;
  mOwner = parent;
  mDockInfo = NULL;

  mFullDrag = false;
  mFloatingGripper = false;

//   BOOL param = FALSE;
//   SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, reinterpret_cast<PVOID>(&param), 0);
//   mFullDrag = param;

  setVisible(false);

  if (parent != NULL)
    parent->removeChild(this, true);
}

DockBar::~DockBar()
{
  dispose();
}

/**
 * You can use setVisible() to hide or show the DockBar. If you hide
 * the DockBar, this method deletes the mDockFrame if it exist, or
 * remove the DockBar from the DockArea. All is automatic.
 */
void DockBar::setVisible(bool visible)
{
  Widget::setVisible(visible);

  if (!visible)
    cleanUp();
  else {
    if (mDockArea == NULL && mDockFrame == NULL)
      // TODO save the old state, and dock when is necessary
      makeFloat(NULL);
  }
}

Size DockBar::preferredSize()
{
  Size gripperSize = measureGripper(isDocked(),
				    isDocked() ? mDockArea->getSide(): LeftSide);

  return Widget::preferredSize() + gripperSize;
}

// Size DockBar::preferredSize(const Size &fitIn)
// {
//   Size gripperSize = measureGripper(isDocked(),
// 				    isDocked() ? mDockArea->getSide(): LeftSide);

//   return Widget::preferredSize(fitIn - gripperSize);
// }

/**
 * Leaves some space for the gripper (using measureGripper()).
 */
Rect DockBar::getLayoutBounds()
{
  Rect rc = Widget::getLayoutBounds();

  if (isGripperVisible(isDocked(),
		       isDocked() ? mDockArea->getSide(): LeftSide)) {
    Side gripperSide = getGripperSide(isDocked(),
				      isDocked() ? mDockArea->getSide(): LeftSide);
    Size gripperSize = measureGripper(isDocked(),
				      isDocked() ? mDockArea->getSide(): LeftSide);

    switch (gripperSide) {
      case LeftSide:
	rc.x += gripperSize.w;
	rc.w -= gripperSize.w;
	break;
      case RightSide:
	rc.w -= gripperSize.w;
	break;
      case TopSide:
	rc.y += gripperSize.h;
	rc.h -= gripperSize.h;
	break;
      case BottomSide:
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
  mFullDrag = state;
}

bool DockBar::isFullDrag()
{
  return mFullDrag;
}

/**
 * If @a state is true, the gripper is drawn on floating mode too. By
 * default, the gripper is not drawn in floating mode.
 */
void DockBar::setFloatingGripper(bool state)
{
  mFloatingGripper = state;

  if (isFloating()) {
    mDockFrame->setSize(mDockFrame->preferredSize());
    mDockFrame->invalidate(true);
  }
}

/**
 * Default value: false.
 */
bool DockBar::isFloatingGripper()
{
  return mFloatingGripper;
}

/**
 * Returns true if the bar is docked in a DockArea.
 */
bool DockBar::isDocked()
{
  return mDockArea != NULL;
}

/**
 * Returns true if the bar is floating in a DockFrame.
 */
bool DockBar::isFloating()
{
  return mDockFrame != NULL;
}

void DockBar::dockIn(DockArea *dockArea)
{
  assert(dockArea != NULL);
  
  // does it have DockInfo?
  if (mDockInfo == NULL) {
    // create the default one
    mDockInfo = dockArea->createDefaultDockInfo(this);
  }

  makeDock(dockArea, mDockInfo);
}

void DockBar::floatOut()
{
  makeFloat(NULL);
}

Frame *DockBar::getOwnerFrame()
{
  return mOwner;
}

DockArea *DockBar::getDockArea()
{
  return mDockArea;
}

DockFrame *DockBar::getDockFrame()
{
  return mDockFrame;
}

DockInfo *DockBar::getDockInfo()
{
  return mDockInfo;
}

Size DockBar::getDockedSize(Side side)
{
  return getBounds().getSize();
}

Size DockBar::getFloatingSize()
{
  return getBounds().getSize();
}

void DockBar::makeDock(DockArea *dockArea, DockInfo *dockInfo)
{
  bool redock = false;

  // redock in the same dock area?
  if (mDockArea != NULL && dockArea == mDockArea) {
    Widget::setVisible(false);
    
    redock = true;
    mDockArea->onRedock(this, mDrag->dockIn);
    mDockArea->removeDockBar(this);
  }
  // hide the DockBar (disposing the DockFrame if exists)
  else {
    setVisible(false);
  }

  // set the new DockInfo?
  if (mDockInfo != dockInfo) {
    if (mDockInfo != NULL)
      delete mDockInfo;

    mDockInfo = dockInfo;
  }

  // set the new DockArea
  mDockArea = dockArea;
  mDockArea->addDockBar(this);

  setVisible(true);

  onDocking();

  mDockArea->getParent()->layout();
}

void DockBar::makeFloat(const Rect *rect)
{
  Size floatingSize = getFloatingSize();

  if (mDockFrame == NULL) {
    setVisible(false);

    mDockFrame = new DockFrame(this, mOwner);
    mDockFrame->addChild(this, true);
    mDockFrame->Close.connect(Bind(&DockBar::onDockFrameClose, this));
    mDockFrame->Destroy.connect(Bind(&DockBar::onDockFrameDestroy, this));

    setVisible(true);

    mDockFrame->layout();
  }

  if (rect != NULL) {
    if (mDrag != NULL && mDrag->startDocked)
      mDockFrame->setBounds(Rect(rect->getOrigin(),
				 mDockFrame->getNonClientSize() + floatingSize));
    else
      mDockFrame->setBounds(*rect);
  }
  else {
    mDockFrame->setBounds(Rect(mOwner->getAbsoluteClientBounds().getOrigin(),
			       mDockFrame->getNonClientSize() + floatingSize));
  }

  mDockFrame->setVisible(true);

  focusOwner();
  onFloating();
}

void DockBar::onDestroy()
{
  assert(mDrag == NULL);

  // undock, and delete DockFrame or DockFrameGarbage
  cleanUp();

  // delete DockInfo
  if (mDockInfo != NULL) {
    delete mDockInfo;
    mDockInfo = NULL;
  }

  Widget::onDestroy();
}

/**
 * Called when mDockFrame is closed by its "Close" button.
 */
void DockBar::onDockFrameClose(CloseEvent &ev)
{
  Widget::setVisible(false);

  mDockFrame->removeChild(this, true);
  mDockFrame->Destroy.disconnect_all_slots();
  mDockFrame->deleteAfterEvent();
  mDockFrame = NULL;
}

/**
 * Called when mDockFrame is destroyed by its parent. In this case
 * mDockFrameGarbage hold the mDockFrame pointer to be deleted,
 * because the mDockFrame is in its disposition process, we can't
 * delete right now the mDockFrame, so we must to hold on the pointer
 * to postpone its deletion.
 */
void DockBar::onDockFrameDestroy()
{
  assert(mDockFrame != NULL);

  if (getParent() == mDockFrame) {
    mDockFrame->removeChild(this, false);
    // we can't delete mDockFrame here (we come from its signal),
    // we must to postpone its deletetion
    mDockFrameGarbage = mDockFrame;
    mDockFrame = NULL;
  }
}

/**
 * Calls paintGripper().
 */
void DockBar::onPaint(Graphics &g)
{
  paintGripper(g);
}

/**
 * When the gripper is visible in the floating state
 * (mFloatingGripper == true), we must to repaint the DockBar's
 * gripper when the DockBar is resized.
 */
void DockBar::onResize(const Size &sz)
{
  Widget::onResize(sz);

  if (mFloatingGripper)
    invalidate(true);
}

void DockBar::onMouseDown(MouseEvent &ev)
{
  // we are dragging?
  if (mDrag != NULL) {
    // cancel drag
    if (ev.getButton() == MouseButtons::Right) {
      if (!mFullDrag) {
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
    acquireCapture();

    if (hasCapture()) {
      beginDrag();

      if (!mFullDrag) {
	ScreenGraphics g;
	drawFeedbackShape(g);
      }
    }
  }
}

void DockBar::onMouseMove(MouseEvent &ev)
{
  if (hasCapture()) {
    assert(mDrag != NULL);

    //////////////////////////////////////////////////////////////////////
    // does the mouse or the control key change?
    Point newPos = System::getCursorPos();
    bool newCtrl = System::getKeyState(Keys::ControlKey);

    if (mDrag->oldMousePos == newPos &&
	mDrag->oldCtrlState == newCtrl)
      return;

    mDrag->oldMousePos = newPos;
    mDrag->oldCtrlState = newCtrl;
    //////////////////////////////////////////////////////////////////////

    ScreenGraphics g;

    if (!mFullDrag)
      cleanFeedbackShape(g);

    if (mDrag->dockIn != NULL)
      delete mDrag->dockIn;
    mDrag->dockIn = calcDestination(mDrag->currentRect);

    if (mFullDrag)
      dragBar();

    if (!mFullDrag)
      drawFeedbackShape(g);
  }
}

void DockBar::onMouseUp(MouseEvent &ev)
{
  if (hasCapture()) {
    assert(mDrag != NULL);

    if (!mFullDrag) {
      ScreenGraphics g;
      cleanFeedbackShape(g);

      dragBar();
    }

    endDrag();
  }
}

void DockBar::onDoubleClick(MouseEvent &ev)
{
  assert(mOwner != NULL);

  // is floating? dock it in the last dock position remembered
  if (isFloating()) {
    DockArea *dockArea = NULL;

    // the DockBar has DockInfo, so use it
    if (mDockInfo != NULL)
      dockArea = mOwner->getDockArea(mDockInfo->getSide());
    // create a default DockInfo
    else {
      dockArea = mOwner->getDefaultDockArea();
      if (dockArea != NULL)
	mDockInfo = dockArea->createDefaultDockInfo(this);
    }

    if (dockArea != NULL && mDockInfo != NULL)
      dockIn(dockArea);
    // else { do nothing, we can't dock the tool-bar, so it should
    // continue floating }
  }
}

void DockBar::onCancelMode()
{
  if (mDrag != NULL) {
    if (!mFullDrag) {
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
void DockBar::onResizingFrame(DockFrame *frame, int edge, Rect &rc)
{
}

void DockBar::paintGripper(Graphics &g)
{
  if (isGripperVisible(isDocked(), 
		       isDocked() ? mDockArea->getSide(): LeftSide)) {
    Color topLeft = System::getColor(COLOR_3DHIGHLIGHT);
    Color bottomRight = System::getColor(COLOR_3DSHADOW);
    Rect rc = getClientBounds();
    Side gripperSide = getGripperSide(isDocked(), 
				      isDocked() ? mDockArea->getSide(): LeftSide);

    switch (gripperSide) {

      case LeftSide:
	g.draw3dRect(rc.x, rc.y, 3, rc.h, topLeft, bottomRight);
	g.draw3dRect(rc.x+3, rc.y, 3, rc.h, topLeft, bottomRight);
	break;

      case TopSide:
	g.draw3dRect(rc.x, rc.y, rc.w, 3, topLeft, bottomRight);
	g.draw3dRect(rc.x, rc.y+3, rc.w, 3, topLeft, bottomRight);
	break;

      case RightSide:
	g.draw3dRect(rc.x+rc.w-3, rc.y, 3, rc.h, topLeft, bottomRight);
	g.draw3dRect(rc.x+rc.w-6, rc.y, 3, rc.h, topLeft, bottomRight);
	break;

      case BottomSide:
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

    if (gripperSide == LeftSide ||
	gripperSide == RightSide)
      return Size(DEF_GRIPPER_SIZE, 0);
    else
      return Size(0, DEF_GRIPPER_SIZE);
  }
  return Size(0, 0);
}

Side DockBar::getGripperSide(bool docked, Side dockSide)
{
  if (docked) {
    if (dockSide == LeftSide || dockSide == RightSide)
      return TopSide;
  }
  else {
    Size sz = getClientBounds().getSize();
    if (sz.h >= sz.w)
      return TopSide;
  }
  return LeftSide;
}

bool DockBar::isGripperVisible(bool docked, Side dockSide)
{
  if (docked || isFloatingGripper())
    return true;
  else
    return false;
}

void DockBar::beginDrag()
{
  assert(mDrag == NULL);
  
  // install the keyboard hook (to hook the CONTROL key changes, and
  // the ESC key)
  assert(hwndDockWnd == NULL && dragHook == NULL);
  hwndDockWnd = getHWND();
  dragHook = SetWindowsHookEx(WH_KEYBOARD, dragHookProc, NULL, Thread::getCurrentId()); 

//   if (mDockArea != NULL)
//     mDockArea->onBeginDockBarDrag(this);

  // if the DockBar is floating, bring the DockFrame to top
  if (mDockFrame != NULL)
    mDockFrame->bringToTop();

  //////////////////////////////////////////////////////////////////////
  // create the DragInfo...
  mDrag = new DragInfo;
  mDrag->startDocked = isDocked();
  mDrag->currentRect = mDockFrame != NULL ? mDockFrame->getAbsoluteBounds():
					    this->getAbsoluteBounds();
  mDrag->startRect    = mDrag->currentRect;
  mDrag->begMousePos  = System::getCursorPos();
  mDrag->oldMousePos  = System::getCursorPos();
  mDrag->oldCtrlState = System::getKeyState(Keys::ControlKey);

  // calculate the position of the anchor point (offset where
  // the cursor is in the client area)
  Point anchor;
  Rect bounds = getAbsoluteClientBounds();
  anchor = mDrag->begMousePos - bounds.getOrigin();
  anchor.x = anchor.x < 0 ? 0: (anchor.x >= bounds.w ? bounds.w-1: anchor.x);
  anchor.y = anchor.y < 0 ? 0: (anchor.y >= bounds.h ? bounds.h-1: anchor.y);
  mDrag->anchorPos = anchor;

  Rect dummy;
  mDrag->inArea = NULL;
  mDrag->dockIn = calcDestination(dummy);
  mDrag->inArea = mDockArea;
  // ...done.
  //////////////////////////////////////////////////////////////////////
}

// moves the DockBar to the place that indicates the current mDrag information
void DockBar::dragBar()
{
  // inside a dock bar
  if (mDrag->dockIn != NULL) {
    makeDock(mOwner->getDockArea(mDrag->dockIn->getSide()),
	     mDrag->dockIn);

    // now "mDockInfo" is "mDrag->dockIn", so avoid to delete it in endDrag()
    assert(mDockInfo == mDrag->dockIn);
    mDrag->dockIn = NULL;
  }
  // floating
  else {
    makeFloat(&mDrag->currentRect);
  }
}

// destroys the DragInfo (mDrag)
void DockBar::endDrag()
{
  assert(mDrag != NULL);
  
  // destroy the DragInfo...
  if (mDrag->dockIn != NULL)
    delete mDrag->dockIn;
  delete mDrag;
  mDrag = NULL;

  // we don't use anymore the keyboard hook
  if (dragHook != NULL) {
    UnhookWindowsHookEx(dragHook);
    dragHook = NULL;
  }

  assert(hwndDockWnd == getHWND());
  hwndDockWnd = NULL;

      // end dragging
//     if (mDockArea != NULL)
//       mDockArea->onEndDockBarDrag(this, mDrag->dockIn);

  releaseCapture();
}

void DockBar::cleanUp()
{
  // undock (dispose the DockArea)
  if (mDockArea != NULL) {
    if (mDockArea->getHWND() != NULL) {
      mDockArea->removeDockBar(this);
      mDockArea->getParent()->layout();
    }

    mDockArea = NULL;
  }

  // dispose the DockFrame
  if (mDockFrame != NULL) {
    mDockFrame->Close.disconnect_all_slots();
    mDockFrame->Destroy.disconnect_all_slots();

    if (mDockFrame->getHWND() != NULL)
      mDockFrame->removeChild(this, true);

    delete mDockFrame;
    mDockFrame = NULL;
  }

  // delete garbage
  if (mDockFrameGarbage != NULL) {
    delete mDockFrameGarbage;
    mDockFrameGarbage = NULL;
  }
}

/**
 * Focus the owner window.
 */
void DockBar::focusOwner()
{
  if (mOwner != NULL)
    mOwner->focus();
}

DockInfo *DockBar::calcDestination(Rect &rc)
{
  Point cursor = System::getCursorPos();
  Point pt = mDrag->startRect.getOrigin() + cursor - mDrag->begMousePos;
  Size sz = !mDrag->startDocked ? mDrag->startRect.getSize():
				  getFloatingSize();

  rc = Rect(pt, sz);

  if (!System::getKeyState(Keys::ControlKey) && (mOwner != NULL)) {
    // for each dock area
    std::vector<DockArea *> dockAreas = mOwner->getDockAreas();
    std::vector<DockArea *>::reverse_iterator it;

    // add the inArea, it has priority
    if (mDrag->inArea != NULL)
      dockAreas.push_back(mDrag->inArea);

    for (it=dockAreas.rbegin(); it!=dockAreas.rend(); ++it) {
      DockArea *dockArea = *it;

      // we can dock here?
      if (dockArea->hitTest(this,		      // this dock bar
			    cursor,		      // cursor position
			    mDrag->anchorPos,         // anchor position
			    mDrag->inArea == dockArea // from inside?
			    )) {
	// return a new DockInfo
	return dockArea->createDockInfo(this, cursor, mDrag->anchorPos);
      }
    }
  }

  return NULL;
}

void DockBar::drawFeedbackShape(Graphics &g)
{
  if (mDrag->dockIn != NULL)
    mDrag->inArea = xorFeedbackShape(g);
  else
    xorFeedbackShape(g);
}

void DockBar::cleanFeedbackShape(Graphics &g)
{
  xorFeedbackShape(g);
}

DockArea *DockBar::xorFeedbackShape(Graphics &g)
{
  assert(mDrag != NULL);

  // clean docked shape
  if (mDrag->dockIn != NULL) {
    DockArea *dockArea = mOwner->getDockArea(mDrag->dockIn->getSide());

    assert(dockArea != NULL);
    
    dockArea->drawXorDockInfoShape(g, mDrag->dockIn);

    return dockArea;
  }
  // clean floating rectangle
  else {
    g.drawXorFrame(mDrag->currentRect);
    return NULL;
  }
}
