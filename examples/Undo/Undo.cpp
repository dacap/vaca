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

#include <Vaca/Vaca.hpp>

using namespace Vaca;

#define ID_MOVE_LEFT 1001

//////////////////////////////////////////////////////////////////////

class Document
{
  String m_string;
public:
  Document() { }
  ~Document() { }

  Character at(String::size_type pos) const { return m_string.at(pos); }
  String::size_type size() const { return m_string.size(); }

  void add(String::size_type pos, Character chr) {
    m_string.insert(pos, 1, chr);
  }

  void add(String::size_type pos, const String& s) {
    m_string.insert(pos, s);
  }

  void remove(String::size_type pos,
	      String::size_type n) {
    m_string.erase(pos, n);
  }
  
};

//////////////////////////////////////////////////////////////////////

class Editor : public Panel
{
  Document* m_doc;
  Font m_font;
  String::size_type m_carret;
  bool m_carretVisible;
  Timer m_carretFlicker;
  const String::size_type max_length;
  
public:

  Editor(Document* d, Widget* parent)
    : Panel(parent)
    , m_doc(d)
    , m_font("Courier New", 24)
    , m_carret(0)
    , m_carretVisible(true)
    , m_carretFlicker(750)
    , max_length(10)
  {
    setBgColor(Color::White);
    setDoubleBuffered(true);
    setFont(&m_font);

    m_doc->add(0, "Hello");
    m_carret = m_doc->size();

    m_carretFlicker.Action.connect(&Editor::toggleCarretVisibility, this);
    m_carretFlicker.start();

    setPreferredSize(Size(32*max_length, 32));
    requestFocus();
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
    for (String::size_type i=0; i<m_doc->size(); ++i) {
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

  virtual void onKeyDown(KeyEvent& ev)
  {
    Panel::onKeyDown(ev);

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

//////////////////////////////////////////////////////////////////////

// class ModelCommand : public Command
// {
//   Model* m_doc;
// public:
//   ModelCommand(CommandId id, Model* m) : Command(id), m_doc(m) { }
//   Model* getModel() { return m_doc; }
// };

//////////////////////////////////////////////////////////////////////

// class MoveLeftCommand : public ModelCommand
// {
// public:
//   MoveLeftCommand(Model* m) : ModelCommand(ID_MOVE_LEFT, m) { }

//   virtual void execute() {
//   }

//   virtual bool isEnabled() {
//     return true;
//   }
// };

//////////////////////////////////////////////////////////////////////

class CmdStack : public Panel
{
public:
  CmdStack(Widget* parent)
    : Panel(parent)
  {
  }
};

//////////////////////////////////////////////////////////////////////

// class DocumentContainer
// {
//   typedef std::vector<Command*> Commands;
//   Commands m_cmds;

// public:

//   DocumentContainer() {
//   }

//   ~DocumentContainer() {
//     for (Commands::iterator
// 	   it=m_cmds.begin(); it!=m_cmds.end(); ++it) {
//       delete *it;
//     }
//   }

//   void add(Command* cmd) {
//     m_cmds.push_back(cmd);
//   }

//   Document* get(CommandId id) {
//     for (Commands::iterator
// 	   it=m_cmds.begin(); it!=m_cmds.end(); ++it) {
//       if ((*it)->getId() == id) {
// 	return *it,
//       }
//     }
//     return NULL;
//   }
// };

class MainFrame : public Frame
{
  Document* m_doc;
  Editor m_editor;
  CmdStack m_cmds;

public:
  MainFrame(Document* doc)
    : Frame("Undo")
    , m_doc(doc)
    , m_editor(doc, this)
    , m_cmds(this)
  {
    setLayout(new BoxLayout(Orientation::Horizontal, false));
    m_editor.setConstraint(new BoxConstraint(true));
    m_cmds.setConstraint(new BoxConstraint(false));

    // Example* app = dynamic_cast<Example*>(Application::getInstance());
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  typedef std::vector<Document*> Documents;
  Documents m_docs;
  Document m_doc;
  MainFrame m_mainFrame;

public:

  Example() : m_mainFrame(&m_doc) { }

  virtual ~Example() {
    for (Documents::iterator
	   it=m_docs.begin(); it!=m_docs.end(); ++it) {
      delete *it;
    }
  }

  virtual void main() {
    m_mainFrame.setVisible(true);
  }

};

//////////////////////////////////////////////////////////////////////

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  std::auto_ptr<Example> app(new Example);
  app->run();
  return 0;
}
