// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

#include "Vaca/ToolBar.h"
#include "Vaca/AnchorLayout.h"
#include "Vaca/Anchor.h"
#include "Vaca/ClientLayout.h"
#include "Vaca/Event.h"
#include "Vaca/Debug.h"
#include "Vaca/ImageList.h"
#include "Vaca/DockFrame.h"
#include "Vaca/Command.h"
#include "Vaca/CommandEvent.h"
#include "Vaca/PreferredSizeEvent.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// ToolButton

ToolButton::ToolButton(CommandId id, int imageIndex, ToolButtonState state)
  : // m_set(NULL)
  // , 
    m_buttonIndex(-1)
  , m_imageIndex(imageIndex)
  , m_commandId(id)
  , m_state(state)
{
}

ToolButton::ToolButton(const ToolButton& button)
  : // m_set(NULL)
  // , 
    m_buttonIndex(button.m_buttonIndex)
  , m_imageIndex(button.m_imageIndex)
  , m_commandId(button.m_commandId)
  , m_state(button.m_state)
{
}

ToolButton::~ToolButton()
{
}

// void ToolButton::setSet(ToolSet* set)
// {
//   m_set = set;
// }

void ToolButton::setState(ToolButtonState state)
{
  m_state = state;
  // if (m_set)
  //   m_set->updateButton(this);
}

int ToolButton::getTBSTATE() const
{
  int state = 0;

  if (m_state & ToolButtonState::Enabled) state |= TBSTATE_ENABLED;
  if (m_state & ToolButtonState::Checked) state |= TBSTATE_CHECKED;
  if (m_state & ToolButtonState::Hidden) state |= TBSTATE_HIDDEN;

  return state;
}

// void ToolButton::setTBSTATE(int tbstate)
// {
//   newState = 0;
//   if (tbstate & TBSTATE_ENABLED) m_state = m_state | ToolButtonState::Enabled;
//   if (tbstate & TBSTATE_CHECKED) m_state = m_state | ToolButtonState::Checked;
//   if (tbstate & TBSTATE_HIDDEN) m_state = m_state | ToolButtonState::Hidden;
//   setState(newState);
// }

//////////////////////////////////////////////////////////////////////
// ToolSet

// WARNING! if we use TB_ADDBITMAP or TB_ADDBUTTONS, we MUST TO send
// the TB_BUTTONSTRUCTSIZE message, anyway, I'll try don't use those
// messages.

ToolSet::ToolSet(Widget* parent, Style style)
  : Widget(WidgetClassName(TOOLBARCLASSNAME), parent, style)
{
}

ToolSet::~ToolSet()
{
  int i, n = getButtonCount();
  for (i=0; i<n; i++) {
    if (ToolButton* button = getButtonByIndex(i))
      delete button;
  }
}

/// (TB_BUTTONCOUNT).
/// 
int ToolSet::getButtonCount() const
{
  return const_cast<ToolSet*>(this)->sendMessage(TB_BUTTONCOUNT, 0, 0);
}

/// (TB_GETROWS)
/// 
int ToolSet::getRows() const
{
  return const_cast<ToolSet*>(this)->sendMessage(TB_GETROWS, 0, 0);
}

/// (TB_SETROWS)
/// 
/// 
Rect ToolSet::setRows(int rows, bool expand)
{
  RECT rect;
  sendMessage(TB_SETROWS,
	      MAKEWPARAM(rows, expand),
	      reinterpret_cast<LPARAM>(&rect));

  return Rect(&rect);
}

/// (TB_SETIMAGELIST)
/// 
/// 
void ToolSet::setImageList(ImageList& imageList)
{
  sendMessage(TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(imageList.getHandle()));
  m_imageList = imageList;

  // TODO
//   sendMessage(TB_SETHOTIMAGELIST, 0,
// 	      reinterpret_cast<LPARAM>(imageList.getHandle()));
}

/// Adds a button to the ToolSet.
/// 
/// It uses Win32's TB_INSERTBUTTON with BTNS_BUTTON.
/// 
/// @param imageIndex The images to use for this button from the
///                   ImageList that you specified to setImageList().
/// 
/// @param id
///      Identifier of the command. This is the identifier that will be
///      sent to Widget#onCommand.
/// 
/// @param buttonState
///      One of the following values:
///      @li TBState::Checked
///      @li TBState::Enabled
///      @li TBState::Hidden
/// 
void ToolSet::addButton(ToolButton* button)
{
  TBBUTTON tbb;

  tbb.iBitmap = button->getImageIndex();
  tbb.idCommand = button->getCommandId();
  tbb.fsState = button->getTBSTATE();
  tbb.fsStyle = BTNS_BUTTON;	// TODO add ToolButtonStyles
  tbb.iString = 0;
  tbb.dwData = reinterpret_cast<DWORD_PTR>(button);

  // button->setSet(this);
  button->setButtonIndex(getButtonCount()-1);

  sendMessage(TB_INSERTBUTTON,
	      button->getButtonIndex()+1,
	      reinterpret_cast<LPARAM>(&tbb));

  updatePreferredSizes();
}

/// Adds a separator in the ToolSet.
/// 
/// It uses Win32's TB_INSERTBUTTON with BTNS_SEP.
/// 
void ToolSet::addSeparator(int width)
{
  TBBUTTON tbb;

  tbb.iBitmap = width;
  tbb.idCommand = 0;
  tbb.fsState = 0;
  tbb.fsStyle = BTNS_SEP;
  tbb.dwData = static_cast<DWORD_PTR>(NULL);

  sendMessage(TB_INSERTBUTTON, getButtonCount(),
	      reinterpret_cast<LPARAM>(&tbb));

  updatePreferredSizes();
}

void ToolSet::updateButton(ToolButton* button)
{
  TBBUTTONINFO tbbi;

  tbbi.cbSize = sizeof(TBBUTTONINFO);
  tbbi.dwMask = TBIF_COMMAND | TBIF_IMAGE | TBIF_STATE;
  tbbi.idCommand = button->getCommandId();
  tbbi.iImage = button->getImageIndex();
  tbbi.fsState = button->getTBSTATE();

  sendMessage(TB_SETBUTTONINFO,
	      button->getCommandId(),
	      reinterpret_cast<LPARAM>(&tbbi));

  updatePreferredSizes();
}

ToolButton* ToolSet::getButtonById(CommandId id) const
{
  TBBUTTONINFO tbbi;

  tbbi.cbSize = sizeof(TBBUTTONINFO);
  tbbi.dwMask = TBIF_LPARAM;

  if (const_cast<ToolSet*>(this)->
      sendMessage(TB_GETBUTTONINFO,
		  id,
		  reinterpret_cast<LPARAM>(&tbbi)) >= 0 &&
      tbbi.lParam) {
    return reinterpret_cast<ToolButton*>(tbbi.lParam);
  }
  else
    return NULL;
}

ToolButton* ToolSet::getButtonByIndex(int index) const
{
  TBBUTTONINFO tbbi;

  tbbi.cbSize = sizeof(TBBUTTONINFO);
  tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;

  if (const_cast<ToolSet*>(this)->
      sendMessage(TB_GETBUTTONINFO,
		  index,
		  reinterpret_cast<LPARAM>(&tbbi)) >= 0 &&
      tbbi.lParam) {
    return reinterpret_cast<ToolButton*>(tbbi.lParam);
  }
  else
    return NULL;
}

/// Returns the index of the button that is above the point @a pt
/// (relative to client area). Returns a negative index if the point is
/// inside a (or the nearest of) a separator button.
/// 
/// It uses Win32's TB_HITTEST.
/// 
int ToolSet::hitTest(const Point& pt) const
{
  POINT point = pt;
  return
    const_cast<ToolSet*>(this)->
    sendMessage(TB_HITTEST, 0, reinterpret_cast<LPARAM>(&point));
}

std::vector<Size> ToolSet::getPreferredSizes() const
{ 
  return m_preferredSizes;
}

void ToolSet::onPreferredSize(PreferredSizeEvent& ev)
{
  ev.setPreferredSize(m_preferredSizes[getRows()]);
}

void ToolSet::onUpdateIndicators()
{
  int i, n = getButtonCount();
  for (i=0; i<n; i++) {
    ToolButton* button = getButtonByIndex(i);
    if (button) {
      Command* cmd = findCommandById(button->getCommandId());
      if (cmd) {
	ToolButtonState state = button->getState();

	if (cmd->isEnabled())
	  state |= ToolButtonState::Enabled;
	else
	  state &= ~ToolButtonState::Enabled;

	if (state != button->getState()) {
	  button->setState(state);

	  TBBUTTONINFO tbbi;

	  tbbi.cbSize = sizeof(TBBUTTONINFO);
	  tbbi.dwMask = TBIF_STATE;
	  tbbi.fsState = button->getTBSTATE();

	  sendMessage(TB_SETBUTTONINFO,
		      button->getCommandId(),
		      reinterpret_cast<LPARAM>(&tbbi));
	}
      }
    }
  }
}

bool ToolSet::onReflectedCommand(int id, int code, LRESULT& lResult)
{
  if (Widget::onReflectedCommand(id, code, lResult))
    return true;

  assert(getParent() != NULL);
  return getParent()->sendMessage(WM_COMMAND, MAKEWPARAM(id, 0), 0) == 0;
}

bool ToolSet::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
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
      // if (getHandle() == ::WindowFromPoint(getAbsoluteCursorPos())) {

      // the mouse isn't above a button
      if (hitTest(Point(LOWORD(lParam), HIWORD(lParam))) < 0) {
	// don't use the WM_LBUTTONDOWN message, send it to the parent
	PostMessage(getParentHandle(), WM_LBUTTONDOWN, wParam, lParam);
	lResult = FALSE;
	// lResult = TRUE;
	return true;
      }
      break;

  }

  return Widget::wndProc(message, wParam, lParam, lResult);
}

/// Updates the preferred sizes of the tool-set when it has different
/// number of rows.
/// 
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

ToolBar::ToolBar(const String& title, Frame* parent, Style toolSetStyle, Style style)
  : DockBar(title, parent, style)
  , m_set(this, toolSetStyle)
  , m_rowsWhenFloating(1)
{
  setLayout(new ClientLayout);

  m_set.removeStyle(Style(CCS_LEFT | CCS_TOP | CCS_RIGHT | CCS_BOTTOM | CCS_VERT, 0));
  m_set.addStyle(Style(CCS_TOP, 0));

  layout();
}

ToolBar::~ToolBar()
{
}

// ToolSet& ToolBar::getSet()
// {
//   return m_set;
// }

/// Returns the size of the ToolBar when it should be docked in the
/// specified @a side.
/// 
/// 
Size ToolBar::getDockedSize(Side side) const
{
  Size size(0, 0);
  std::vector<Size> preferredSizes = m_set.getPreferredSizes();

  if (side == Side::Left || side == Side::Right)
    size = preferredSizes[m_set.getButtonCount()];
  else
    size = preferredSizes[1];

  return size + measureGripper(true, side);
}

Size ToolBar::getFloatingSize() const
{
  std::vector<Size> preferredSizes = m_set.getPreferredSizes();

  return
    preferredSizes[m_rowsWhenFloating] +
    measureGripper(false, Side::Left);
}

void ToolBar::onCommand(CommandEvent& ev)
{
  if (!ev.isConsumed()) {
    if (getOwnerFrame()->sendMessage(WM_COMMAND, MAKEWPARAM(ev.getCommandId(), 0), 0) == 0)
      ev.consume();
  }
  DockBar::onCommand(ev);
}

void ToolBar::onUpdateIndicators()
{
  m_set.updateIndicators();
}

/// When the ToolBar is docked in the top or bottom side, we must to
/// set the rows to 1, if it's docked in the left or right side we must
/// to make it vertical (set the rows to the maximum number).
/// 
/// @see ToolSet::setRows
/// 
void ToolBar::onDocking()
{
  DockBar::onDocking();

  if (getDockArea()->isHorizontal())
    m_set.setRows(1, true);
  else
    m_set.setRows(m_set.getButtonCount(), false);
}

void ToolBar::onFloating()
{
  DockBar::onFloating();

  m_set.setRows(m_rowsWhenFloating, false);
}

void ToolBar::onResizingFrame(DockFrame* frame, CardinalDirection dir, Rect& rc)
{
  DockBar::onResizingFrame(frame, dir, rc);

  Size prefSize = frame->getPreferredSize();
  Size extraSize = prefSize - getPreferredSize();
  std::vector<Size> preferredSizes = m_set.getPreferredSizes();
  int rows, maxRows = preferredSizes.size()-1;

  for (rows=1; rows<=maxRows; rows++) {
    //////////////////////////////////////////////////////////////////////
    // East
    if (dir == CardinalDirection::East &&
	preferredSizes[rows].w+extraSize.w <= rc.w) {
      m_set.setRows(m_rowsWhenFloating = rows, false);
      prefSize = frame->getPreferredSize();
      rc.setSize(prefSize);
      return;
    }
    //////////////////////////////////////////////////////////////////////
    // West
    else if (dir == CardinalDirection::West &&
	     preferredSizes[rows].w+extraSize.w <= rc.w) {
//       prefSize = preferredSizes[rows] + extraSize;

      m_set.setRows(m_rowsWhenFloating = rows, false);
      prefSize = frame->getPreferredSize();
      rc = Rect(Point(rc.x+rc.w-prefSize.w, rc.y), prefSize);
      return;
    }
    //////////////////////////////////////////////////////////////////////
    // South
    else if (dir == CardinalDirection::South &&
	     preferredSizes[rows].h+extraSize.h >= rc.h) {
//       prefSize = preferredSizes[rows] + extraSize;

      m_set.setRows(m_rowsWhenFloating = rows, false);
      prefSize = frame->getPreferredSize();
      rc.setSize(prefSize);
      return;
    }
    //////////////////////////////////////////////////////////////////////
    // North
    else if (dir == CardinalDirection::North &&
	     preferredSizes[rows].h+extraSize.h >= rc.h) {
//       prefSize = preferredSizes[rows] + extraSize;

      m_set.setRows(m_rowsWhenFloating = rows, false);
      prefSize = frame->getPreferredSize();
      rc = Rect(Point(rc.x, rc.y+rc.h-prefSize.h), prefSize);
      return;
    }
  }

  // no resize
  rc = frame->getAbsoluteBounds();
}
