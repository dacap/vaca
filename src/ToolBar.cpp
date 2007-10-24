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

#include "stdvaca.h"
#include "Vaca/ToolBar.hpp"
#include "Vaca/AnchorLayout.hpp"
#include "Vaca/Anchor.hpp"
#include "Vaca/ClientLayout.hpp"
#include "Vaca/Event.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/ImageList.hpp"
#include "Vaca/DockFrame.hpp"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// ToolSet


// WARNING! if we use TB_ADDBITMAP or TB_ADDBUTTONS, we MUST TO send
// the TB_BUTTONSTRUCTSIZE message, anyway, I'll try don't use those
// messages.

ToolSet::ToolSet(Widget *parent, Style style)
  : Widget(TOOLBARCLASSNAME, parent, style)
{
  m_loadedImageList = NULL;
}

ToolSet::~ToolSet()
{
  if (m_loadedImageList != NULL) {
    ImageList_Destroy(m_loadedImageList);
    m_loadedImageList = NULL;
  }
}

/**
 * (TB_BUTTONCOUNT).
 * 
 */
int ToolSet::getButtonCount()
{
  return sendMessage(TB_BUTTONCOUNT, 0, 0);
}

/**
 * (TB_GETROWS)
 * 
 */
int ToolSet::getRows()
{
  return sendMessage(TB_GETROWS, 0, 0);
}

/**
 * (TB_SETROWS)
 * 
 */
Rect ToolSet::setRows(int rows, bool expand)
{
  RECT rect;
  sendMessage(TB_SETROWS,
	      MAKEWPARAM(rows, expand),
	      reinterpret_cast<LPARAM>(&rect));

  return Rect(&rect);
}

/**
 * (TB_SETIMAGELIST)
 * 
 */
void ToolSet::setImageList(ImageList &imageList)
{
  assert(imageList.isValid());
  
  sendMessage(TB_SETIMAGELIST, 0,
	      reinterpret_cast<LPARAM>(imageList.getHIMAGELIST()));

  // TODO
//   sendMessage(TB_SETHOTIMAGELIST, 0,
// 	      reinterpret_cast<LPARAM>(imageList.getHIMAGELIST()));
}

/**
 * @param imageListId
 * @li @c IDB_HIST_LARGE_COLOR
 * @li @c IDB_HIST_SMALL_COLOR
 * @li @c IDB_STD_LARGE_COLOR
 * @li @c IDB_STD_SMALL_COLOR (default value).
 * @li @c IDB_VIEW_LARGE_COLOR
 * @li @c IDB_VIEW_SMALL_COLOR
 */
void ToolSet::loadStandardImageList(int imageListId)
{
  if (m_loadedImageList != NULL)
    ImageList_Destroy(m_loadedImageList);

  m_loadedImageList =
    ImageList_Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 1);

  if (m_loadedImageList != NULL) {
    sendMessage(TB_SETIMAGELIST, 0,
		reinterpret_cast<LPARAM>(m_loadedImageList));
  
    sendMessage(TB_LOADIMAGES,
		IDB_STD_SMALL_COLOR,
		reinterpret_cast<LPARAM>(HINST_COMMCTRL));
  }
}

/**
 * (TB_INSERTBUTTON with BTNS_BUTTON)
 * 
 * @param imageIndex The images to use for this button from the
 *                   ImageList that you specified to setImageList().
 *
 * @param commandId
 *      Identifier of the command. This is the identifier that will be
 *      sent to Widget#onActionById(int).
 *
 * @param buttonState
 *     @li TBSTATE_CHECKED
 *     @li TBSTATE_ENABLED
 *     @li TBSTATE_HIDDEN
 *     @li TBSTATE_INDETERMINATE
 *     @li TBSTATE_ELLIPSES
 *     @li TBSTATE_MARKED
 */
void ToolSet::addButton(int imageIndex, int commandId, int buttonState)
{
  TBBUTTON tbb;

  tbb.iBitmap = imageIndex; //I_IMAGENONE
  tbb.idCommand = commandId;
  tbb.fsState = buttonState;
  tbb.fsStyle = BTNS_BUTTON;
  tbb.dwData = 0;
  tbb.iString = 0;

  sendMessage(TB_INSERTBUTTON, getButtonCount(),
	      reinterpret_cast<LPARAM>(&tbb));

  updatePreferredSizes();
}

void ToolSet::addToggleButton(int imageIndex, int commandId, int buttonState)
{
  TBBUTTON tbb;

  tbb.iBitmap = imageIndex; //I_IMAGENONE
  tbb.idCommand = commandId;
  tbb.fsState = buttonState;
  tbb.fsStyle = BTNS_CHECK;
  tbb.dwData = 0;
  tbb.iString = 0;

  sendMessage(TB_INSERTBUTTON, getButtonCount(),
	      reinterpret_cast<LPARAM>(&tbb));

  updatePreferredSizes();
}

void ToolSet::addDropDownButton(int imageIndex, int commandId, int buttonState)
{
  TBBUTTON tbb;

  tbb.iBitmap = imageIndex; //I_IMAGENONE
  tbb.idCommand = commandId;
  tbb.fsState = buttonState;
  tbb.fsStyle = BTNS_DROPDOWN;
  tbb.dwData = 0;
  tbb.iString = 0;

  sendMessage(TB_INSERTBUTTON, getButtonCount(),
	      reinterpret_cast<LPARAM>(&tbb));

  updatePreferredSizes();
}

void ToolSet::addWholeDropDownButton(int imageIndex, int commandId, int buttonState)
{
  TBBUTTON tbb;

  tbb.iBitmap = imageIndex; //I_IMAGENONE
  tbb.idCommand = commandId;
  tbb.fsState = buttonState;
  tbb.fsStyle = BTNS_WHOLEDROPDOWN;
  tbb.dwData = 0;
  tbb.iString = 0;

  sendMessage(TB_INSERTBUTTON, getButtonCount(),
	      reinterpret_cast<LPARAM>(&tbb));

  updatePreferredSizes();
}

/**
 * (TB_INSERTBUTTON with BTNS_SEP)
 * 
 */
void ToolSet::addSeparator(int width)
{
  TBBUTTON tbb;

  tbb.iBitmap = width;
  tbb.idCommand = 0;
  tbb.fsState = 0;
  tbb.fsStyle = BTNS_SEP;

  sendMessage(TB_INSERTBUTTON, getButtonCount(),
	      reinterpret_cast<LPARAM>(&tbb));

  updatePreferredSizes();
}

/**
 * (TB_SETCMDID)
 * 
 */
void ToolSet::setButtonCommandId(int buttonIndex, int newCommandId)
{
  BOOL res = sendMessage(TB_SETCMDID, buttonIndex, newCommandId);
  assert(res != FALSE);
}

/**
 * Returns the index of the button that is above the point @a pt
 * (relative to client area). Returns a negative index if the point is
 * inside a (or the nearest of) a separator button.
 *
 * (TB_HITTEST)
 */
int ToolSet::hitTest(const Point &pt)
{
  POINT point = pt;
  return sendMessage(TB_HITTEST, 0,
		     reinterpret_cast<LPARAM>(&point));
}

std::vector<Size> ToolSet::getPreferredSizes()
{ 
  return m_preferredSizes;
}

void ToolSet::onPreferredSize(Size &sz)
{
  sz = m_preferredSizes[getRows()];
}

bool ToolSet::onReflectedCommand(int id, int code, LRESULT &lResult)
{
  if (Widget::onReflectedCommand(id, code, lResult))
    return true;

  assert(getParent() != NULL);
  return getParent()->sendMessage(WM_COMMAND, MAKEWPARAM(id, 0), 0) == 0;
}

bool ToolSet::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
  if (Widget::wndProc(message, wParam, lParam, lResult))
    return true;

  switch (message) {

    // avoid passing this message to ToolSet::defWndProc
    case WM_SIZE:
      // the Layout manager take care of this message
      lResult = FALSE;
      return true;

    case WM_LBUTTONDOWN:
      // we are above the ToolSet?
      // if (getHWND() == ::WindowFromPoint(getAbsoluteCursorPos())) {

      // the mouse isn't above a button
      if (hitTest(Point(LOWORD(lParam), HIWORD(lParam))) < 0) {
	// don't use the WM_LBUTTONDOWN message, send it to the parent
	PostMessage(getParentHWND(), WM_LBUTTONDOWN, wParam, lParam);
	lResult = FALSE;
	// lResult = TRUE;
	return true;
      }
      break;

  }

  return Widget::wndProc(message, wParam, lParam, lResult);
}

/**
 * Updates the preferred sizes of the tool-set when it has different
 * number of rows.
 */
void ToolSet::updatePreferredSizes()
{
  m_preferredSizes.clear();

  int maxRows = getButtonCount();
  int origRows = getRows();

  if (maxRows > 0) {
    //////////////////////////////////////////////////////////////////////
    setRows(maxRows, false); // unbelievably true!!! we need to do
			     // this before to get a correct result
			     // with "setRows(1, false).getSize()"
    //////////////////////////////////////////////////////////////////////

    // add a null preferred size (for rows=0)
    m_preferredSizes.push_back(Size(0, 0));

    // add a each preferred size for specified rows
    for (int rows=1; rows<=maxRows; ++rows)
      m_preferredSizes.push_back(setRows(rows, false).getSize());

    setRows(origRows, false);
  }
}

//////////////////////////////////////////////////////////////////////
// ToolBar


ToolBar::ToolBar(const String &title, Frame *parent, Style toolSetStyle, Style style)
  : DockBar(title, parent, style)
  , mSet(this, toolSetStyle)
  , mRowsWhenFloating(1)
{
  setLayout(new ClientLayout);

  mSet.removeStyle(Style(CCS_LEFT | CCS_TOP | CCS_RIGHT | CCS_BOTTOM | CCS_VERT, 0));
  mSet.addStyle(Style(CCS_TOP, 0));

  layout();
}

ToolBar::~ToolBar()
{
}

ToolSet &ToolBar::getSet()
{
  return mSet;
}

/**
 * Returns the size of the ToolBar when it should be docked in the
 * specified @a side.
 * 
 */
Size ToolBar::getDockedSize(Side side)
{
  Size size(0, 0);
  std::vector<Size> preferredSizes = mSet.getPreferredSizes();

  if (side == Side::Left || side == Side::Right)
    size = preferredSizes[mSet.getButtonCount()];
  else
    size = preferredSizes[1];

  return size + measureGripper(true, side);
}

Size ToolBar::getFloatingSize()
{
  std::vector<Size> preferredSizes = mSet.getPreferredSizes();

  return
    preferredSizes[mRowsWhenFloating] +
    measureGripper(false, Side::Left);
}

bool ToolBar::onActionById(int actionId)
{
  return getOwnerFrame()->sendMessage(WM_COMMAND,
				      MAKEWPARAM(actionId, 0), 0) == 0;
}

/**
 * When the ToolBar is docked in the top or bottom side, we must to
 * set the rows to 1, if it's docked in the left or right side we must
 * to make it vertical (set the rows to the maximum number).
 *
 * @see ToolSet::setRows
 */
void ToolBar::onDocking()
{
  DockBar::onDocking();

  if (getDockArea()->isHorizontal())
    mSet.setRows(1, true);
  else
    mSet.setRows(mSet.getButtonCount(), false);
}

void ToolBar::onFloating()
{
  DockBar::onFloating();

  mSet.setRows(mRowsWhenFloating, false);
}

void ToolBar::onResizingFrame(DockFrame *frame, int edge, Rect &rc)
{
  DockBar::onResizingFrame(frame, edge, rc);

  Size prefSize = frame->getPreferredSize();
  Size extraSize = prefSize - getPreferredSize();
  std::vector<Size> preferredSizes = mSet.getPreferredSizes();
  int rows, maxRows = preferredSizes.size()-1;

  for (rows=1; rows<=maxRows; rows++) {
    //////////////////////////////////////////////////////////////////////
    // WMSZ_RIGHT
    if (edge == WMSZ_RIGHT && preferredSizes[rows].w+extraSize.w <= rc.w) {
      mSet.setRows(mRowsWhenFloating = rows, false);
      prefSize = frame->getPreferredSize();
      rc.setSize(prefSize);
      return;
    }
    //////////////////////////////////////////////////////////////////////
    // WMSZ_LEFT
    else if (edge == WMSZ_LEFT && preferredSizes[rows].w+extraSize.w <= rc.w) {
//       prefSize = preferredSizes[rows] + extraSize;

      mSet.setRows(mRowsWhenFloating = rows, false);
      prefSize = frame->getPreferredSize();
      rc = Rect(Point(rc.x+rc.w-prefSize.w, rc.y), prefSize);
      return;
    }
    //////////////////////////////////////////////////////////////////////
    // WMSZ_BOTTOM
    else if (edge == WMSZ_BOTTOM && preferredSizes[rows].h+extraSize.h >= rc.h) {
//       prefSize = preferredSizes[rows] + extraSize;

      mSet.setRows(mRowsWhenFloating = rows, false);
      prefSize = frame->getPreferredSize();
      rc.setSize(prefSize);
      return;
    }
    //////////////////////////////////////////////////////////////////////
    // WMSZ_TOP
    else if (edge == WMSZ_TOP && preferredSizes[rows].h+extraSize.h >= rc.h) {
//       prefSize = preferredSizes[rows] + extraSize;

      mSet.setRows(mRowsWhenFloating = rows, false);
      prefSize = frame->getPreferredSize();
      rc = Rect(Point(rc.x, rc.y+rc.h-prefSize.h), prefSize);
      return;
    }
  }

  // no resize
  rc = frame->getAbsoluteBounds();
}
