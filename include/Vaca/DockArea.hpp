// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#ifndef VACA_DOCKAREA_HPP
#define VACA_DOCKAREA_HPP

#include "Vaca/base.hpp"
#include "Vaca/Widget.hpp"
#include "Vaca/WidgetClass.hpp"
#include "Vaca/Register.hpp"

namespace Vaca {

class Widget;
class Frame;
class DockBar;

#define DockAreaStyle		ChildStyle

class DockInfo
{
public:
  virtual Size getSize() = 0;
  virtual Side getSide() = 0;
};

/**
 * Win32 class used by the DockArea class.
 */
class DockAreaClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName("Vaca.DockArea"); }
};

/**
 * The abstract class to create DockAreas. A DockArea is the place
 * where you can dock toolbars (any DockBar really).
 */
class VACA_DLL DockArea : public Register<DockAreaClass>, public Widget
{
  friend class DockBar;

  Side m_side;

public:

  DockArea(Side side, Widget* parent, Style style = DockAreaStyle);
  virtual ~DockArea();

  bool isHorizontal();
  bool isVertical();

  Side getSide();

  virtual bool isLayoutFree();

  /**
   * Returns true if the point @a pt is inside a valid rectangle to dock
   * in this DockArea.
   *
   * @param bar
   *     The DockBar to dock.
   *	
   * @param cursor
   *     Absolute position.
   *	
   * @param fromInside
   *     True means that we are already inside the
   *	 DockArea, so try to use the more biggest are to
   *     return a true hit-test.
   */
  virtual bool hitTest(DockBar* bar, const Point& cursor, const Point& anchor, bool fromInside) = 0;

  /**
   * Returns a default DockInfo for the floating DockBar that want to be
   * docked by double-click by first-time.
   */
  virtual DockInfo* createDefaultDockInfo(DockBar* bar) = 0;

  /**
   * @return A new created dockInfo (you must to delete this pointer).
   */
  virtual DockInfo* createDockInfo(DockBar* bar, const Point& cursor, const Point& anchor) = 0;

  /**
   * Draws the shape (generally a rectangle) in Xor mode that represents
   * the specified @a dockInfo.
   */
  virtual void drawXorDockInfoShape(Graphics& g, DockInfo* dockInfo) = 0;

  /**
   * Lays out all dock bars inside the dock area using the DockInfo of
   * each DockBar.
   *
   * @see DockBar, DockInfo
   */
  virtual void layout() = 0;

private:

  void addDockBar(DockBar* dockBar);
  void removeDockBar(DockBar* dockBar);

protected:
  // events
  virtual void onPreferredSize(Size& sz) = 0;
//   virtual void onDestroy();

  // new events
  virtual void onAddDockBar(DockBar* dockBar);
  virtual void onRemoveDockBar(DockBar* dockBar);
  virtual void onRedock(DockBar* dockBar, DockInfo* newDockInfo);

};

} // namespace Vaca

#endif
