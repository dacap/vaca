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

#ifndef VACA_DOCKBAR_H
#define VACA_DOCKBAR_H

#include "Vaca/base.h"
#include "Vaca/Frame.h"
#include "Vaca/Event.h"
#include "Vaca/Rect.h"
#include "Vaca/Point.h"
#include "Vaca/Size.h"

namespace Vaca {

class DockArea;
class DockFrame;
class DockInfo;
class CloseEvent;

#define DockBarStyle		ChildStyle

/**
 * Win32 class used by the DockBar class.
 */
class DockBarClass : public WidgetClass
{
public:
  static LPCTSTR getClassName() { return _T("Vaca.DockBar"); }
};

/**
 * A dockable bar. Each dock bar as two possible states: docked or
 * floating. When it's floating, the DockBar is inside a DockFrame,
 * when the DockBar is docked, it's inside a DockArea.
 */
class VACA_DLL DockBar : public Register<DockBarClass>, public Widget
{
  // TODO less friendship
  friend class DockArea;
  friend class DockFrame;

  struct DragInfo;

  // In what DockArea is the DockBar docked? It's != NULL only if the
  // DockBar is docked.
  DockArea *mDockArea;

  // The DockFrame that contains the DockBar when it's floating. It's
  // != NULL only if the DockBar is floating.
  DockFrame *mDockFrame;

  // Used to hold on the pointer of the destroyed mDockFrame. This
  // pointer is != NULL only when the DockFrame is destroyed by its
  // parent (when the parent Frame or MdiFrame is closed).
  DockFrame *mDockFrameGarbage;

  // Pimpl idiom of the drag stuff. Not NULL means that the user is
  // dragging the DockBar.
  DragInfo *mDrag;

  // The Frame that own the DockBar. This owner generally should have
  // some DockArea to dock the DockBar, but isn't obligatory.
  Frame *mOwner;

  // Information of the DockBar when it's docked (or the information
  // about the last docked position). This can be a derived class of
  // DockInfo, used by your own DockArea (like BandedDockArea does).
  DockInfo *mDockInfo;

  bool mFullDrag;
  bool mFloatingGripper;

public:

  DockBar(const String &title, Frame *parent, Style style = DockBarStyle);
  virtual ~DockBar();

  virtual void setVisible(bool visible);
  virtual Size preferredSize();
//   virtual Size preferredSize(const Size &fitIn);
  virtual Rect getLayoutBounds();

  void setFullDrag(bool state);
  bool isFullDrag();

  void setFloatingGripper(bool state);
  bool isFloatingGripper();

  bool isDocked();
  bool isFloating();

  void dockIn(DockArea *dockArea);
  void floatOut();

  Frame *getOwnerFrame();
  DockArea *getDockArea();
  DockFrame *getDockFrame();
  DockInfo *getDockInfo();

  virtual Size getDockedSize(Side side);
  virtual Size getFloatingSize();

protected:

  // events
  virtual void onDestroy();
  virtual void onDockFrameClose(CloseEvent &ev);
  virtual void onDockFrameDestroy();
  virtual void onPaint(Graphics &g);
  virtual void onResize(const Size &sz);
  virtual void onMouseDown(MouseEvent &ev);
  virtual void onMouseMove(MouseEvent &ev);
  virtual void onMouseUp(MouseEvent &ev);
  virtual void onDoubleClick(MouseEvent &ev);
  virtual void onCancelMode();
  // new events
  virtual void onDocking();
  virtual void onFloating();
  virtual void onResizingFrame(DockFrame *frame, int edge, Rect &rc);

  virtual void paintGripper(Graphics &g);
  virtual Size measureGripper(bool docked, Side dockSide);
  virtual Side getGripperSide(bool docked, Side dockSide);
  virtual bool isGripperVisible(bool docked, Side dockSide);

private:

  void makeDock(DockArea *dockArea, DockInfo *dockInfo);
  void makeFloat(const Rect *rect = NULL);

  void beginDrag();
  void dragBar();
  void endDrag();
  
  void cleanUp();
  void focusOwner();
  DockInfo *calcDestination(Rect &rc);
  void drawFeedbackShape(Graphics &g);
  void cleanFeedbackShape(Graphics &g);
  DockArea *xorFeedbackShape(Graphics &g);

};

} // namespace Vaca

#endif
