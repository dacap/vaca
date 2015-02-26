// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/DockArea.h"
#include "vaca/DockBar.h"
#include "vaca/Frame.h"
#include "vaca/Point.h"
#include "vaca/Debug.h"

using namespace vaca;

DockArea::DockArea(Side side, Widget* parent, Style style)
  : Widget(DockAreaClass::getClassName(), parent, style)
  , m_side(side)
{
}

DockArea::~DockArea()
{
//   dispose();

  WidgetList children = getChildren();
  WidgetList::iterator it;

  for (it=children.begin(); it!=children.end(); ++it) {
    DockBar* dockBar = static_cast<DockBar*>(*it);

    removeDockBar(dockBar);
    dockBar->m_dockArea = NULL;
    dockBar->setVisible(false);
  }
}

bool DockArea::isHorizontal()
{
  return m_side == Side::Top || m_side == Side::Bottom;
}

bool DockArea::isVertical()
{
  return m_side == Side::Left || m_side == Side::Right;
}

Side DockArea::getSide()
{
  return m_side;
}

/**
   Returns true, because DockAreas are controlled by the Frame, not by
   the Layout manager.

   @see Frame::layout
*/
bool DockArea::isLayoutFree() const
{
  return true;
}

/**
   Adds the @a dockBar in this DockArea.

   @see onAddDockBar
*/
void DockArea::addDockBar(DockBar* dockBar)
{
  addChild(dockBar);

  onAddDockBar(dockBar);
}

/**
   Removes the @a dockBar from this DockArea.

   @see onRemoveDockBar
*/
void DockArea::removeDockBar(DockBar* dockBar)
{
  removeChild(dockBar);

  onRemoveDockBar(dockBar);
}

/**
   You can use onAddDockBar to hook the addDockBar action.

   @code
   void MyDockArea::onAddDockBar(DockBar* dockBar)
   {
     // do something, like add the dockBar in
     // some internal structure...
   }
   @endcode
*/
void DockArea::onAddDockBar(DockBar* dockBar)
{
  // do nothing
}

/**
   You can use onRemoveDockBar to hook the removeDockBar action.

   @code
   void MyDockArea::onRemoveDockBar(DockBar* dockBar)
   {
     // do something, like remove the dockBar from
     // some internal structure...
   }
   @endcode
*/
void DockArea::onRemoveDockBar(DockBar* dockBar)
{
  // do nothing
}

// Event generated when the user start dragging the @a dockBar from
// this DockArea.
//
// void DockArea::onBeginDockBarDrag(DockBar* dockBar)
// {
// }

/*
   Event generated when the user finish the dragging of the @a dockBar
   that start from this DockArea.

   @param newDockInfo The new dockInfo where the dockBar will be
                      docked. NULL means that dockBar will be
                      floating, or the drag operation was canceled.
 */
// void DockArea::onEndDockBarDrag(DockBar* dockBar, DockInfo* newDockInfo)
// {
// }

void DockArea::onRedock(DockBar* dockBar, DockInfo* newDockInfo)
{
}
