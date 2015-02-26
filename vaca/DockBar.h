// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_DOCKBAR_H
#define VACA_DOCKBAR_H

#include "vaca/base.h"
#include "vaca/Frame.h"
#include "vaca/Event.h"
#include "vaca/Rect.h"
#include "vaca/Point.h"
#include "vaca/Size.h"

namespace vaca {

/**
   Represents the Win32 class used by DockBar.
*/
class DockBarClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName(L"Vaca.DockBar"); }
};

/**
   A dockable bar. Each dock bar as two possible states: docked or
   floating. When it's floating, the DockBar is inside a DockFrame,
   when the DockBar is docked, it's inside a DockArea.

   @see ToolBar
*/
class VACA_DLL DockBar : public Register<DockBarClass>, public Widget
{
  // TODO less friendship here
  friend class DockArea;
  friend class DockFrame;

  struct DragInfo;

  /**
     In what DockArea is the DockBar docked? It's != NULL only if the
     DockBar is docked.
  */
  DockArea* m_dockArea;

  /**
     The DockFrame that contains the DockBar when it's floating. It's
     != NULL only if the DockBar is floating.
  */
  DockFrame* m_dockFrame;

  /**
     Drag stuff. Not NULL means that the user is dragging the DockBar.
  */
  DragInfo* m_drag;

  /**
     The Frame that own the DockBar. This owner generally should have
     some DockArea to dock the DockBar, but isn't obligatory.
  */
  Frame* m_owner;

  /**
     Information of the DockBar when it's docked (or the information
     about the last docked position). This can be a derived class of
     DockInfo, used by your own DockArea (like BandedDockArea does).
  */
  DockInfo* m_dockInfo;

  bool m_fullDrag;
  bool m_floatingGripper;

public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  DockBar(const String& title, Frame* parent, Style style = Styles::Default);
  virtual ~DockBar();

  virtual void setVisible(bool visible);

  void setFullDrag(bool state);
  bool isFullDrag() const;

  void setFloatingGripper(bool state);
  bool isFloatingGripper() const;

  bool isDocked() const;
  bool isFloating() const;

  void dockIn(DockArea* dockArea);
  void floatOut();

  Frame* getOwnerFrame();
  DockArea* getDockArea();
  DockFrame* getDockFrame();
  DockInfo* getDockInfo();

  virtual Size getDockedSize(Side side) const;
  virtual Size getFloatingSize() const;

protected:

  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);
  virtual void onDockFrameClose(CloseEvent& ev);
  virtual void onPaint(PaintEvent& ev);
  virtual void onLayout(LayoutEvent& ev);
  virtual void onResize(ResizeEvent& ev);
  virtual void onMouseDown(MouseEvent& ev);
  virtual void onMouseMove(MouseEvent& ev);
  virtual void onMouseUp(MouseEvent& ev);
  virtual void onDoubleClick(MouseEvent& ev);
  // New events
  virtual void onDocking();
  virtual void onFloating();
  virtual void onResizingFrame(DockFrame* frame, CardinalDirection dir, Rect& rc);

  virtual void paintGripper(Graphics& g);
  virtual Size measureGripper(bool docked, Side dockSide) const;
  virtual Side getGripperSide(bool docked, Side dockSide) const;
  virtual bool isGripperVisible(bool docked, Side dockSide) const;

  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

private:

  void makeDock(DockArea* dockArea, DockInfo* dockInfo);
  void makeFloat(const Rect* rect = NULL);

  void beginDrag();
  void dragBar();
  void endDrag();

  void cleanUp();
  void cleanFrame();
  void focusOwner();
  DockInfo* calcDestination(Rect& rc);
  void drawTracker(Graphics& g);
  void cleanTracker(Graphics& g);
  DockArea* xorTracker(Graphics& g);
  static Size getNonClientSizeForADockFrame();

};

} // namespace vaca

#endif // VACA_DOCKBAR_H
