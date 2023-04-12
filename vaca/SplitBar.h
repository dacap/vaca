// Vaca - Visual Application Components Abstraction
// Copyright (c) 2022 David Capello
// Copyright (c) 2008, 2009, Jie Zhang, David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SPLITBAR_H
#define VACA_SPLITBAR_H

#include "vaca/Point.h"
#include "vaca/Widget.h"
#include "vaca/Register.h"
#include "vaca/WidgetClass.h"

namespace vaca {

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

  struct Styles {
    static constexpr Style Default =
      Widget::Styles::Visible |
      Widget::Styles::Container;

    /**
       SplitBar position is expressed in percentage by default, but if you
       specify this style the position will be expressed in pixels.
    */
    static constexpr Style ByPixels = Style(1, 0);
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

} // namespace vaca

#endif // VACA_SPLITBAR_H
