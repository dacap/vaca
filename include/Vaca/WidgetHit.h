// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

#ifndef VACA_WIDGETHIT_H
#define VACA_WIDGETHIT_H

#include "Vaca/base.h"

namespace Vaca {

/// @see WidgetHitTest
/// 
struct WidgetHitTestEnum
{
  enum enumeration {
    Error,
    Transparent,

    /// When the mouse is in the background of a Widget.
    ///
    Nowhere,

    /// When the mouse is in the client area of a Widget.
    /// 
    
    Client,

    ///
    /// When the mouse is in the title bar of a Frame.
    ///
    Caption,

    /// @todo
    /// 
    SystemMenu,

    /// @todo
    /// 
    Size,

    /// When the mouse is in a menu of a Frame.
    /// 
    Menu,

    /// When the mouse is in a horizontal scroll bar.
    /// 
    HorizontalScroll,

    /// When the mouse is in a vertical scroll bar.
    /// 
    VerticalScroll,

    /// When the mouse is in the minimize button.
    /// 
    MinimizeButton,

    /// When the mouse is in the maximize button.
    /// 
    MaximizeButton,

    /// When the mouse is in the left side to resize a Frame.
    /// 
    Left,

    /// When the mouse is in the right side to resize a Frame.
    /// 
    Right,

    /// When the mouse is in the top side to resize a Frame.
    /// 
    Top,

    /// When the mouse is in the top-left corner to resize a Frame.
    /// 
    TopLeft,

    /// When the mouse is in the top-right corner to resize a Frame.
    /// 
    TopRight,

    /// When the mouse is in the bottom side to resize a Frame.
    /// 
    Bottom,

    /// When the mouse is in the bottom-left corner to resize a Frame.
    /// 
    BottomLeft,

    /// When the mouse is in the bottom-right corner to resize a Frame.
    /// 
    BottomRight,

    /// The cursor is in a border of the widget that doesn't support.
    /// 
    Border,
    
    Object,
    Close,
    Help
  };
  static const enumeration default_value = Nowhere;
};

/// It indicates a place inside a Widget which the mouse cursor
/// could be.
/// 
/// One of the following values:
/// @li WidgetHitTest::Error
/// @li WidgetHitTest::Transparent
/// @li WidgetHitTest::Nowhere
/// @li WidgetHitTest::Client
/// @li WidgetHitTest::Caption
/// @li WidgetHitTest::SystemMenu
/// @li WidgetHitTest::Size
/// @li WidgetHitTest::Menu
/// @li WidgetHitTest::HorizontalScroll
/// @li WidgetHitTest::VerticalScroll
/// @li WidgetHitTest::MinimizeButton
/// @li WidgetHitTest::MaximizeButton
/// @li WidgetHitTest::Left
/// @li WidgetHitTest::Right
/// @li WidgetHitTest::Top
/// @li WidgetHitTest::TopLeft
/// @li WidgetHitTest::TopRight
/// @li WidgetHitTest::Bottom
/// @li WidgetHitTest::BottomLeft
/// @li WidgetHitTest::BottomRight
/// @li WidgetHitTest::Border
/// @li WidgetHitTest::Object
/// @li WidgetHitTest::Close
/// @li WidgetHitTest::Help
/// 
typedef Enum<WidgetHitTestEnum> WidgetHitTest;

} // namespace Vaca

#endif // VACA_WIDGETHIT_H
