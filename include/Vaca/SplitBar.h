// Vaca - Visual Application Components Abstraction
// Copyright (c) 2008, 2009, Jie Zhang, David Capello
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

#ifndef VACA_SPLITBAR_H
#define VACA_SPLITBAR_H

#include "Vaca/Point.h"
#include "Vaca/Widget.h"
#include "Vaca/Register.h"
#include "Vaca/WidgetClass.h"

namespace Vaca {

/**
   Represents the Win32 class used by SplitBar.
*/
class SplitBarClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName(L"Vaca.SplitBar"); }
};

class VACA_DLL SplitBar : public Register<SplitBarClass>
			, public Widget
{
  Orientation m_orientation;
  Widget* m_pane1;
  Widget* m_pane2;
  int m_barSize;
  double m_barPos;
  double m_oldBarPos;
  Point	m_oldPoint;
  bool m_fullDrag;
  bool m_trackerIsVisible;
  bool m_gripperVisible;

public:

  struct VACA_DLL Styles {
    static const Style Default;
    static const Style ByPixels;
  };

  SplitBar(Orientation orientation, Widget* parent, Style style = Styles::Default);
  virtual ~SplitBar();

  void setPane1(Widget* widget);
  void setPane2(Widget* widget);

  Widget* getPane1() const;
  Widget* getPane2() const;

  int getBarSize() const;
  void setBarSize(int size);

  double getBarPosition() const;
  void setBarPosition(double pos);

  Orientation getOrientation() const;
  void setOrientation(Orientation orientation);

  void setFullDrag(bool state);
  bool isFullDrag() const;

  void setGripperVisible(bool state);
  bool isGripperVisible() const;

protected:
  // Events
  virtual void onLayout(LayoutEvent& ev);
  virtual void onResize(ResizeEvent& ev);
  virtual void onPaint(PaintEvent& ev);
  virtual void onMouseMove(MouseEvent& ev);
  virtual void onMouseDown(MouseEvent& ev);
  virtual void onMouseUp(MouseEvent& ev);
  virtual void onSetCursor(SetCursorEvent& ev);

private:
  void updateChildrenVisibility();
  Rect getBarRect() const;
  void getRects(Rect& rcFirst, Rect& rcSecond) const;
  void drawTracker(Graphics& g);
  void cleanTracker(Graphics& g);
};

} // namespace Vaca

#endif // VACA_SPLITBAR_H
