// Vaca - Visual Application Components Abstraction
// Copyright (c) 2008, Jie Zhang, David Capello
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

#ifndef VACA_SPLITBAR_H
#define VACA_SPLITBAR_H

#include "Vaca/Point.h"
#include "Vaca/Widget.h"
#include "Vaca/Register.h"
#include "Vaca/WidgetClass.h"

namespace Vaca {

#define SplitBarStyle	(ChildStyle + ContainerStyle)

/**
 * Represents the Win32 class used by SplitBar.
 */
class SplitBarClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName("Vaca.SplitBar"); }
};

class VACA_DLL SplitBar : public Register<SplitBarClass>
			, public Widget
{
  Orientation m_orientation;
  Widget* m_first;
  Widget* m_second;
  Size m_size;
  Size m_oldSize;
  Point	m_oldPoint;
  bool m_fullDrag;
  bool m_trackerIsVisible;
  bool m_gripperVisible;
  int m_barWidth;

public:

  SplitBar(Orientation orientation, Widget* parent, Style style = SplitBarStyle);
  virtual ~SplitBar();

  virtual void layout();

  void setFirstWidget(Widget* widget);
  void setSecondWidget(Widget* widget);

  Widget* getFirstWidget() const;
  Widget* getSecondWidget() const;

  void setPaneSize(const Size& sz);

  Orientation getOrientation() const;
  void setOrientation(Orientation orientation);

  void setFullDrag(bool state);
  bool isFullDrag() const;

  void setGripperVisible(bool state);
  bool isGripperVisible() const;

  void setBarWidth(int width);
  int getBarWidth() const;

protected:
  virtual void onResize(const Size& sz);
  virtual void onPaint(Graphics& g);
  virtual void onMouseMove(MouseEvent& ev);
  virtual void onMouseDown(MouseEvent& ev);
  virtual void onMouseUp(MouseEvent& ev);
  virtual void onSetCursor(WidgetHitTest hitTest);
  virtual void onAddChild(Widget* child);

private:
  void updateChildrenVisibility();
  Rect getBarRect() const;
  void getRects(Rect& rcFirst, Rect& rcSecond) const;
  void drawTracker(Graphics& g);
  void cleanTracker(Graphics& g);
};

} // namespace Vaca

#endif // VACA_SPLITBAR_H
