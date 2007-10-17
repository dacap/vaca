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

#ifndef ADDRESSBOOK_SECTIONHEADER_H
#define ADDRESSBOOK_SECTIONHEADER_H

#include "Vaca/Vaca.h"
#include "LightingPanel.h"

using namespace Vaca;

class SectionHeader : public LightingPanel
{
  bool mSelected;
  bool mStartSelected;
  
public:

  SectionHeader(const String &text, Color color, Widget *parent);

  void back();

  // action signal
  boost::signal<void (SectionHeader *)> Action;

protected:

  // events
  virtual void onPreferredSize(Size &sz);
  virtual void onResize(const Size &sz);
  virtual void onMouseDown(MouseEvent &ev);
  virtual void onMouseMove(MouseEvent &ev);
  virtual void onMouseUp(MouseEvent &ev);
  virtual void onSetCursor(int hitTest);
  virtual void onPaint(Graphics &g);
  virtual void onKeyDown(KeyEvent &ev);
  
};

#endif
