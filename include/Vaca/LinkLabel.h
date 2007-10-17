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

#ifndef VACA_LINKLABEL_H
#define VACA_LINKLABEL_H

#include <boost/signal.hpp>

#include "Vaca/base.h"
#include "Vaca/CustomLabel.h"
// #include "Vaca/Panel.h"
#include "Vaca/Font.h"

namespace Vaca {

#define LinkLabelStyle		CustomLabelStyle

/**
 * A link to internet.
 */
class VACA_DLL LinkLabel : public CustomLabel
// class VACA_DLL LinkLabel : public Panel
{
  enum {
    Outside,
    Inside,
    Hover
  };

  int mTriState;
  String mUrl;
  Font mUnderlineFont;

public:

  LinkLabel(const String &url, const String &text, Widget *parent, Style style = LinkLabelStyle);
  LinkLabel(const String &url, Widget *parent, Style style = LinkLabelStyle);

  virtual void setFont(Font &font);

  virtual Color getLinkColor();
  virtual Color getHoverColor();

  boost::signal<void (WidgetEvent &)> Action; ///< @see onAction

protected:

  // events
  virtual void onPaint(Graphics &g);
  virtual void onMouseEnter(MouseEvent &ev);
  virtual void onMouseMove(MouseEvent &ev);
  virtual void onMouseLeave();
  virtual void onMouseDown(MouseEvent &ev);
  virtual void onSetCursor(int hitTest);
//   virtual void onResize(const Size &sz);

  // new events
  virtual void onAction(WidgetEvent &ev);

private:

  void updateFont(Font &font);
  Rect getLinkRect(Graphics &g);

};

} // namespace Vaca

#endif
