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

#ifndef VACA_BANDEDDOCKAREA_HPP
#define VACA_BANDEDDOCKAREA_HPP

#include <vector>

#include "Vaca/base.hpp"
#include "Vaca/DockArea.hpp"

namespace Vaca {

#define BandedDockAreaStyle		ChildStyle

/**
 * An area where you can put tool-bars separated by bands.
 */
class VACA_DLL BandedDockArea : public DockArea
{
  struct BandInfo
  {
    std::vector<DockBar *> bars; // bars in the band
    int size;		// band's height (or width for vertical bands)
    BandInfo() : bars()
	       , size(0) { }
  };

  std::vector<BandInfo> m_bandInfo;

public:

  BandedDockArea(Side side, Widget *parent, Style style = BandedDockAreaStyle);
  virtual ~BandedDockArea();

  virtual bool hitTest(DockBar *bar, const Point &cursor, const Point &anchor, bool fromInside);
  virtual DockInfo *createDefaultDockInfo(DockBar *bar);
  virtual DockInfo *createDockInfo(DockBar *bar, const Point &cursor, const Point &anchor);
  virtual void drawXorDockInfoShape(Graphics &g, DockInfo *dockInfo);

  virtual void layout();

protected:
  // events
  virtual void onPreferredSize(Size &sz);
  virtual void onPaint(Graphics &g);
  virtual void onAddDockBar(DockBar *dockBar);
  virtual void onRemoveDockBar(DockBar *dockBar);
  virtual void onRedock(DockBar *dockBar, DockInfo *newDockInfo);

private:
  void updateBandSize(int bandIndex);
  Rect getBandBounds(int bandIndex);
  void fitBounds(int bandIndex, int barIndex1, std::vector<Rect> &bounds);

};

} // namespace Vaca

#endif