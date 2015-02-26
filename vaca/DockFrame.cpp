// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#if 0

#include "vaca/DockFrame.h"
#include "vaca/DockBar.h"
#include "vaca/ClientLayout.h"
#include "vaca/Event.h"
#include "vaca/Debug.h"

using namespace vaca;

DockFrame::DockFrame(DockBar* dockBar, Widget* parent, Style style)
  : Frame(DockFrameClass::getClassName(), dockBar->getText(), parent, style)
  , m_dockBar(dockBar)
{
  assert(m_dockBar != NULL);

  setLayout(new ClientLayout);
  bringToTop();
  // SetWindowPos(getHandle(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

DockFrame::~DockFrame()
{
}

/**
   The DockFrame are synchronized with its parents (generally a
   Frame).
*/
bool DockFrame::keepSynchronized()
{
  return true;
}

// void DockFrame::onDestroy()
// {
//   Destroy();

//   Frame::onDestroy();
// }

/**
   Generates the DockBar#onResizingFrame event for m_dockBar member.
*/
void DockFrame::onResizing(CardinalDirection dir, Rect& rc)
{
  Frame::onResizing(dir, rc);

  m_dockBar->onResizingFrame(this, dir, rc);
}

bool DockFrame::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
  if (Frame::wndProc(message, wParam, lParam, lResult))
    return true;

  switch (message) {

    // DockFrame can be resized only from the sides, not from the corners
    // (TODO customizable from DockBar)
    case WM_NCHITTEST:
      lResult = defWndProc(message, wParam, lParam);
      if (lResult == HTBOTTOMLEFT || lResult == HTTOPLEFT)
	lResult = HTLEFT;
      else if (lResult == HTBOTTOMRIGHT || lResult == HTTOPRIGHT)
	lResult = HTRIGHT;
      return true;

    // drag the DockBar (but it's controlled by the DockBar, so we
    // must to send to it the same message)
    case WM_NCLBUTTONDOWN:
      if (wParam == HTCAPTION) {
	assert(m_dockBar != NULL);

	lResult = m_dockBar->sendMessage(WM_LBUTTONDOWN, wParam, lParam);
	return true;
      }
      break;

      // the double-click in the caption is handled by the DockBar
      // like a double-click in its client area (when the user does
      // double-click, he want to dock the DockBar)
    case WM_NCLBUTTONDBLCLK:
      if (wParam == HTCAPTION) {
	assert(m_dockBar != NULL);

	m_dockBar->sendMessage(WM_LBUTTONDBLCLK, 0, 0); // TODO wParam and lParam
	lResult = 0;
	return true;
      }
      break;

    case WM_MOUSEACTIVATE:
      assert(m_dockBar != NULL);

      m_dockBar->focusOwner();
      lResult = MA_NOACTIVATE;
      return true;

      // when the user start sizing or moving the DockFrame, bring it
      // to the top
    case WM_ENTERSIZEMOVE:
      bringToTop();
      return false;

      // when the user finish the sizing or moving action, focus the
      // owner of the DockBar
    case WM_EXITSIZEMOVE:
      assert(m_dockBar != NULL);
      m_dockBar->focusOwner();
      return false;

  }

  return false;
}

#endif
