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

#include "Vaca/Vaca.h"

#include <cmath>

using namespace Vaca;

class Regions : public Panel
{
  Region *mRegion;
  Point mStartPoint;
  Point mOldPoint;
  bool mReadOnly;
  bool mErasing;
  bool mEllipses;

public:

  Regions(Widget *parent)
    : Panel(parent, PanelStyle + ClientEdgeStyle)
    , mRegion(NULL)
    , mReadOnly(true)
  {
    setBgColor(Color::White);
  }

  void setRegion(Region *region, bool readOnly, bool ellipses = false)
  {
    mRegion = region;
    mReadOnly = readOnly;
    mEllipses = ellipses;

    invalidate(true);
  }

protected:
    
  virtual void onPaint(Graphics &g)
  {
    if (mRegion != NULL) {
      g.setColor(mReadOnly ? Color::Gray: Color::Red);
      g.fillRegion(*mRegion);
    }
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    if (mRegion != NULL &&
	!mReadOnly &&
	!hasCapture()) {
      acquireCapture();

      mStartPoint =
	mOldPoint = System::getCursorPos();

      ScreenGraphics g;
      xorRect(g, mOldPoint);

      // with right mouse button we erase 
      mErasing = ev.getButton() == MouseButtons::Right;
    }
  }

  virtual void onMouseMove(MouseEvent &ev)
  {
    if (hasCapture()) {
      Point newPoint = System::getCursorPos();

      // clean the feedback rectangle
      ScreenGraphics g;
      xorRect(g, mOldPoint);

      // draw the feedback rectangle in new position
      xorRect(g, newPoint);
      
      // the old point is now the new point
      mOldPoint = newPoint;
    }
  }

  virtual void onMouseUp(MouseEvent &ev)
  {
    if (hasCapture()) {
      // we can release the capture (remember to check hasCapture()
      // before to release the mouse capture)
      releaseCapture();

      // clean the feedback rectangle
      ScreenGraphics g;
      xorRect(g, mOldPoint);

      // new rectangle (absolute coordinates)
      Rect newRect(VACA_MIN(mStartPoint.x, mOldPoint.x),
		   VACA_MIN(mStartPoint.y, mOldPoint.y),
		   abs(mStartPoint.x-mOldPoint.x)+1,
		   abs(mStartPoint.y-mOldPoint.y)+1);

      // translate to client area (relative)
      newRect.offset(-getAbsoluteClientBounds().getOrigin());

      Region newRegion =
	mEllipses ? Region::fromEllipse(newRect):
		    Region::fromRect(newRect);

      // update the region
      if (mErasing)
	*mRegion = (*mRegion) - newRegion; // substract
      else
	*mRegion = (*mRegion) | newRegion; // union

      invalidate(true);
    }
  }

private:

  void xorRect(Graphics &g, const Point &pt)
  {
    Rect rc(VACA_MIN(mStartPoint.x, pt.x),
	    VACA_MIN(mStartPoint.y, pt.y),
	    abs(mStartPoint.x-pt.x)+1,
	    abs(mStartPoint.y-pt.y)+1);
    
    g.setRop2(R2_NOTXORPEN);
    g.intersectClip(getAbsoluteClientBounds());

    if (mEllipses)
      g.drawEllipse(rc);
    else
      g.drawRect(rc);

    g.setRop2(R2_COPYPEN);
  }

};

class MainFrame : public Frame
{
  TabBase mTab;
  Regions mRegions;
  Region mRegionA;
  Region mRegionB;
  Region mRegion;

public:
  
  MainFrame()
    : Frame("Regions")
    , mTab(this)
    , mRegions(&mTab)
  {
    setLayout(new ClientLayout);
    mTab.setLayout(new BoxLayout(Vertical, true)); // as ClientLayout,
						   // but with borders

    mTab.addPage("Region A");
    mTab.addPage("Region B");
    mTab.addPage("Union: A | B");
    mTab.addPage("Intersect: A && B");
    mTab.addPage("Substract: A - B");
    mTab.addPage("Substract: B - A");
    mTab.addPage("Xor: A ^ B");

    mTab.PageChange.connect(Bind(&MainFrame::onPageChange, this));

    mRegionA = Region::fromRect(Rect(32, 32, 64, 64));
    mRegionB = Region::fromEllipse(Rect(48, 48, 64, 64));

    onPageChange();
  }

private:

  void onPageChange()
  {
    int pageIndex = mTab.getActivePage();

    switch (pageIndex) {
      // Region A
      case 0:
	mRegions.setRegion(&mRegionA, false);
	break;
      // Region B
      case 1:
	mRegions.setRegion(&mRegionB, false, true);
	break;
      // A | B
      case 2:
	mRegion = mRegionA | mRegionB;
	mRegions.setRegion(&mRegion, true);
	break;
      // A & B
      case 3: 
	mRegion = mRegionA & mRegionB;
	mRegions.setRegion(&mRegion, true);
	break;
      // A - B
      case 4: 
	mRegion = mRegionA - mRegionB;
	mRegions.setRegion(&mRegion, true);
	break;
      // B - A
      case 5: 
	mRegion = mRegionB - mRegionA;
	mRegions.setRegion(&mRegion, true);
	break;
      // A ^ B
      case 6:
	mRegion = mRegionA ^ mRegionB;
	mRegions.setRegion(&mRegion, true);
	break;
    }
  }
  
};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame mMainWnd;
public:
  virtual void main(std::vector<String> args) {
    mMainWnd.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
