// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_BANDEDDOCKAREA_H
#define VACA_BANDEDDOCKAREA_H

#include "vaca/base.h"
#include "vaca/DockArea.h"

#include <vector>

namespace vaca {

/**
   An area where you can put @link vaca::DockBar DockBars@endlink
   separated by bands.
*/
class VACA_DLL BandedDockArea : public DockArea
{
  struct BandInfo
  {
    std::vector<DockBar*> bars; // bars in the band
    int size;			// band's height (or width for vertical bands)
    BandInfo() : bars()
	       , size(0) { }
  };

  std::vector<BandInfo> m_bandInfo;

public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  BandedDockArea(Side side, Widget* parent, Style style = Styles::Default);
  virtual ~BandedDockArea();

  virtual bool hitTest(DockBar* bar, const Point& cursor, const Point& anchor, bool fromInside);
  virtual DockInfo* createDefaultDockInfo(DockBar* bar);
  virtual DockInfo* createDockInfo(DockBar* bar, const Point& cursor, const Point& anchor);
  virtual void drawXorTracker(Graphics& g, DockInfo* dockInfo);

  virtual void layout();

protected:
  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);
  virtual void onPaint(PaintEvent& ev);
  virtual void onAddDockBar(DockBar* dockBar);
  virtual void onRemoveDockBar(DockBar* dockBar);
  virtual void onRedock(DockBar* dockBar, DockInfo* newDockInfo);

private:
  void updateBandSize(int bandIndex);
  Rect getBandBounds(int bandIndex);
  void fitBounds(int bandIndex, int barIndex1, std::vector<Rect>& bounds);

};

} // namespace vaca

#endif // VACA_BANDEDDOCKAREA_H
