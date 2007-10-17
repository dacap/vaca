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
#include "Vaca/DockFrame.h"
#include "Vaca/DockBar.h"
#include "Vaca/ClientLayout.h"
#include "Vaca/WidgetEvent.h"
#include "Vaca/MouseEvent.h"
#include "Vaca/Debug.h"

using namespace Vaca;

DockFrame::DockFrame(DockBar *dockBar, Widget *parent, Style style)
  : Frame(DockFrameClass::getClassName(), dockBar->getText(), parent, style)
  , mDockBar(dockBar)
{
  VACA_ASSERT(mDockBar != NULL);

  setLayout(new ClientLayout);
  bringToTop();
  // SetWindowPos(getHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

DockFrame::~DockFrame()
{
  dispose();
}

/**
 * The DockFrame are enabled synchronised with its parents (generally
 * a Frame).
 */
bool DockFrame::keepEnabledSynchronised()
{
  return true;
}

void DockFrame::onDestroy()
{
  Destroy();

  Frame::onDestroy();
}

/**
 * Generates the DockBar::onResizingFrame() event for mDockBar member.
 */
void DockFrame::onResizing(int edge, Rect &rc)
{
  Frame::onResizing(edge, rc);

  mDockBar->onResizingFrame(this, edge, rc);
}

bool DockFrame::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
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
	VACA_ASSERT(mDockBar != NULL);

	lResult = mDockBar->sendMessage(WM_LBUTTONDOWN, wParam, lParam);
	return true;
      }
      break;

      // the double-click in the caption is handled by the DockBar
      // like a double-click in its client area (when the user does
      // double-click, he want to dock the DockBar)
    case WM_NCLBUTTONDBLCLK:
      if (wParam == HTCAPTION) {
	VACA_ASSERT(mDockBar != NULL);

	mDockBar->sendMessage(WM_LBUTTONDBLCLK, 0, 0); // TODO wParam and lParam
	lResult = 0;
	return true;
      }
      break;

    case WM_MOUSEACTIVATE:
      VACA_ASSERT(mDockBar != NULL);

      mDockBar->focusOwner();
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
      VACA_ASSERT(mDockBar != NULL);
      mDockBar->focusOwner();
      return false;

  }

  return false;
}
