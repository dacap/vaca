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

#ifndef EDITOR_H
#define EDITOR_H

#include <Vaca/Vaca.h>
#include "Document.h"

using namespace Vaca;

// Document's view
class Editor : public Panel
{
  Document* m_doc;
  Font m_font;
  DocPos m_carret;
  bool m_carretVisible;
  Timer m_carretFlicker;
  const DocPos max_length;
  
public:

  Editor(Widget* parent)
    : Panel(parent)
    , m_doc(NULL)
    , m_font("Courier New", 24)
    , m_carret(0)
    , m_carretVisible(true)
    , m_carretFlicker(750)
    , max_length(10)
  {
    setBgColor(Color::White);
    setDoubleBuffered(true);
    setFont(&m_font);

    m_carretFlicker.Action.connect(&Editor::toggleCarretVisibility, this);
    m_carretFlicker.start();

    setPreferredSize(Size(32*max_length, 32));
    requestFocus();
  }

  void setDocument(Document* doc) {
    m_doc = doc;
    m_carret = doc ? doc->size(): 0;
    makeCarretVisible();
  }

  void goBeginOfLine() {
    m_carret = 0;
    makeCarretVisible();
  }

  void goEndOfLine() {
    m_carret = m_doc->size();
    makeCarretVisible();
  }

  void goPreviousChar() {
    if (m_carret > 0)
      m_carret--;
    makeCarretVisible();
  }

  void goNextChar() {
    if (m_carret < m_doc->size())
      m_carret++;
    makeCarretVisible();
  }

  void removePreviousChar() {
    if (m_carret > 0)
      m_doc->remove(--m_carret, 1);
    makeCarretVisible();
  }

  void removeNextChar() {
    if (m_carret < m_doc->size())
      m_doc->remove(m_carret, 1);
    makeCarretVisible();
  }

  void writeChar(Character chr) {
    if (m_doc->size() < max_length)
      m_doc->add(m_carret++, chr);
    makeCarretVisible();
  }

protected:

  virtual void onPaint(Graphics& g)
  {
    Rect rc(getClientBounds());
    Pen blackPen(Color::Black);
    Pen bluePen(Color::Blue);
    Pen grayPen(Color(192, 192, 192));
    Size celBox(32, 32);
    Point origPt = rc.getCenter() - Point(celBox.w*max_length, celBox.h)/2;
    String tmp(1);

    grayPen.setStyle(PenStyle::Dot);

    g.drawRoundRect(grayPen,
		    Rect(origPt, Size(celBox.w*max_length, celBox.h)),
		    Size(6, 6));
    if (m_doc) {
      for (DocPos i=0; i<m_doc->size(); ++i) {
	int x = origPt.x+i*celBox.w;

	g.drawRoundRect(blackPen,
			Rect(Point(x, origPt.y), celBox),
			Size(6, 6));

	tmp[0] = m_doc->at(i);

	g.setColor(Color::Black);
	g.drawString(tmp, Point(x, origPt.y) + Point(celBox)/2 - Point(g.measureString(tmp))/2);
      }

      // draw the carret
      if (m_carretVisible) {
	int x = origPt.x + m_carret*celBox.w;
	g.drawLine(bluePen,
		   Point(x, origPt.y-8),
		   Point(x, origPt.y+celBox.h+8));
      }
    }
    else {
      g.setColor(Color::Black);
      g.drawString("No document", rc);
    }
  }

  virtual void onKeyDown(KeyEvent& ev)
  {
    Panel::onKeyDown(ev);

    if (m_doc) {
      switch (ev.getKeyCode()) {
	case Keys::Home: goBeginOfLine(); break;
	case Keys::End: goEndOfLine(); break;
	case Keys::Left: goPreviousChar(); break;
	case Keys::Right: goNextChar(); break;
	case Keys::Back: removePreviousChar(); break;
	case Keys::Delete: removeNextChar(); break;
	default:
	  if (ev.getCharCode() >= ' ') {
	    writeChar(ev.getCharCode());
	    break;
	  }
	  break;
      }
    }
  }

private:

  void toggleCarretVisibility()
  {
    m_carretVisible = !m_carretVisible;
    invalidate(true);
  }

  void makeCarretVisible()
  {
    m_carretVisible = true;
    m_carretFlicker.start();	// restart timer
    invalidate(true);
  }
  
};

#endif // EDITOR_H
