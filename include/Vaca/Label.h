// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#ifndef VACA_LABEL_H
#define VACA_LABEL_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"

namespace Vaca {

/**
 * Default style for Label: a ChildStyle with SS_NOTIFY (to receive
 * mouse notifications).
 */
#define LabelStyle		(ChildStyle + Style(SS_NOTIFY, 0))

#define SimpleLabelStyle	(Style(SS_SIMPLE, 0))
#define WordEllipsisLabelStyle	(Style(SS_WORDELLIPSIS, 0))
#define EndEllipsisLabelStyle	(Style(SS_ENDELLIPSIS, 0))
#define PathEllipsisLabelStyle	(Style(SS_PATHELLIPSIS, 0))

/**
 * Static label control. It's just an informative label. Remember to
 * use the SS_NOTIFY if you derived from this class and want to use
 * mouse events (see the source code of LinkLabel for more
 * information). Also, you should be aware to use @c SS_ styles, some
 * can overlap the SS_CENTER or SS_RIGHT, so you should override
 * [gs]etTextAlign methods (like CustomLabel does).
 */
class VACA_DLL Label : public Widget
{
public:

  Label(const String& text, Widget* parent, Style style = LabelStyle);
  virtual ~Label();

  virtual TextAlign getTextAlign();
  virtual void setTextAlign(TextAlign align);

protected:
  
  bool useWordWrap();
  int getFlagsForDrawString();

  // events
  virtual void onPreferredSize(Size& sz);
  virtual void onResize(const Size& sz);

};

} // namespace Vaca

#endif // VACA_LABEL_H
