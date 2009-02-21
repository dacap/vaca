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

#ifndef VACA_LAYOUT_H
#define VACA_LAYOUT_H

#include <vector>

#include "Vaca/Size.h"
#include "Vaca/Widget.h"

namespace Vaca {

/**
 * A Layout setups the position of a collection of widgets. It's used
 * to arrange the widgets quickly without worrying about the exact
 * position of them.
 *
 * Each widget can have a layout manager, but it's only useful when
 * the widget has children.
 * 
 * @warning If the parent widget doesn't have a layout manager
 *          specified, the children bounds aren't modified (see the
 *          @c FreeOfLayout example).
 *
 * @see @ref page_tn_011
 */
class VACA_DLL Layout : public Referenceable
{
  HDWP m_HDWP;
  Widget::Container m_relayoutWidgets;

public:

  Layout();
  virtual ~Layout();

  void beginMovement(const Widget::Container& widgets);
  void moveWidget(Widget* widget, const Rect& rc);
  void endMovement();

  virtual Size getPreferredSize(Widget* parent, Widget::Container& widgets, const Size& fitIn);
  virtual void layout(Widget* parent, Widget::Container& widgets, const Rect& rc) = 0;

};

} // namespace Vaca

#endif // VACA_LAYOUT_H

