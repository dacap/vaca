// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_WIDGETHIT_H
#define VACA_WIDGETHIT_H

#include "vaca/base.h"

namespace vaca {

/**
   @see WidgetHit
*/
struct WidgetHitEnum
{
  enum enumeration {
    Error,
    Transparent,

    /**
       When the mouse is in the background of a Widget.
    */
    Nowhere,

    /**
       When the mouse is in the client area of a Widget.
    */
    Client,

    /**
       When the mouse is in the title bar of a Frame.
    */
    Caption,

    /**
       @todo
    */
    SystemMenu,

    /**
       @todo
    */
    Size,

    /**
       When the mouse is in a menu of a Frame.
    */
    Menu,

    /**
       When the mouse is in a horizontal scroll bar.
    */
    HorizontalScroll,

    /**
       When the mouse is in a vertical scroll bar.
    */
    VerticalScroll,

    /**
       When the mouse is in the minimize button.
    */
    MinimizeButton,

    /**
       When the mouse is in the maximize button.
    */
    MaximizeButton,

    /**
       When the mouse is in the left side to resize a Frame.
    */
    Left,

    /**
       When the mouse is in the right side to resize a Frame.
    */
    Right,

    /**
       When the mouse is in the top side to resize a Frame.
    */
    Top,

    /**
       When the mouse is in the top-left corner to resize a Frame.
    */
    TopLeft,

    /**
       When the mouse is in the top-right corner to resize a Frame.
    */
    TopRight,

    /**
       When the mouse is in the bottom side to resize a Frame.
    */
    Bottom,

    /**
       When the mouse is in the bottom-left corner to resize a Frame.
    */
    BottomLeft,

    /**
       When the mouse is in the bottom-right corner to resize a Frame.
    */
    BottomRight,

    /**
       The cursor is in a border of the widget that doesn't support.
    */
    Border,

    Object,
    Close,
    Help
  };
  static const enumeration default_value = Nowhere;
};

/**
   It indicates a place inside a Widget which the mouse cursor
   could be.

   One of the following values:
   @li WidgetHit::Error
   @li WidgetHit::Transparent
   @li WidgetHit::Nowhere
   @li WidgetHit::Client
   @li WidgetHit::Caption
   @li WidgetHit::SystemMenu
   @li WidgetHit::Size
   @li WidgetHit::Menu
   @li WidgetHit::HorizontalScroll
   @li WidgetHit::VerticalScroll
   @li WidgetHit::MinimizeButton
   @li WidgetHit::MaximizeButton
   @li WidgetHit::Left
   @li WidgetHit::Right
   @li WidgetHit::Top
   @li WidgetHit::TopLeft
   @li WidgetHit::TopRight
   @li WidgetHit::Bottom
   @li WidgetHit::BottomLeft
   @li WidgetHit::BottomRight
   @li WidgetHit::Border
   @li WidgetHit::Object
   @li WidgetHit::Close
   @li WidgetHit::Help
*/
typedef Enum<WidgetHitEnum> WidgetHit;

} // namespace vaca

#endif // VACA_WIDGETHIT_H
