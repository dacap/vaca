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

#include "FormView.h"
#include "FormDocument.h"

#include "Vaca/Vaca.h"

FormView::FormView(FormDocument *doc, MdiFrame *parent)
  : View(doc, parent)
{
  setBgColor(Color::White);
}

void FormView::onPaint(Graphics &g)
{
  FormDocument *doc = dynamic_cast<FormDocument *>(getDocument());
  Rect rc = doc->getBounds().offset(getClientBounds().getOrigin()).offset(4, 4);

  //////////////////////////////////////////////////////////////////////
  // form border
  {
    Pen pen(Color(104, 131, 139));

    for (int c=1; c<=3; ++c)
      g.drawRect(pen, Rect(rc).enlarge(c));
  }

  //////////////////////////////////////////////////////////////////////
  // form background
  {
    Brush brush(Color(212, 208, 200));
    g.fillRect(brush, rc);
  }
}

void FormView::onSetCursor(int hitTest)
{
  FormDocument *doc = dynamic_cast<FormDocument *>(getDocument());
  Point pt = System::getCursorPos() - getAbsoluteClientBounds().getOrigin();
  Rect rc = doc->getBounds().offset(getClientBounds().getOrigin()).offset(4, 4);

  if (Rect(rc.x-3, rc.y-3, rc.w+6, 3).contains(pt)) {
    setCursor(Cursor(SizeNCursor));
  }
  else if (Rect(rc.x-3, rc.y+rc.h, rc.w+6, 3).contains(pt)) {
    setCursor(Cursor(SizeSCursor));
  }
  else {
    View::onSetCursor(hitTest);
  }
}

void FormView::onMouseDown(MouseEvent &ev)
{
  View::onMouseDown(ev);
}

void FormView::onMouseUp(MouseEvent &ev)
{
  View::onMouseUp(ev);
}

void FormView::onDoubleClick(MouseEvent &ev)
{
  View::onDoubleClick(ev);
}

void FormView::onMouseMove(MouseEvent &ev)
{
  View::onMouseMove(ev);
}
