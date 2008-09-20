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

#include <Vaca/Vaca.h>

using namespace Vaca;

// commands of the application
enum {
  // commands for popup-menu in the background
  ID_NEW = 1,
  ID_PASTE,
  // commands for popup-menu in a path
  ID_CUT,
  ID_COPY,
  ID_REMOVE,
  ID_FRONT,
  ID_BACK,
  ID_PROPERTIES,
  // commands for popup-menu in path movement
  ID_MOVE_HERE,
  ID_COPY_HERE,
};

//////////////////////////////////////////////////////////////////////
// A figure

class Figure
{
public:
  GraphicsPath path;
  Pen pen;
  Brush brush;

  Figure() { }
  Figure(const GraphicsPath& path,
	 const Pen& pen,
	 const Brush& brush)
    : path(path), pen(pen), brush(brush) { }
};

//////////////////////////////////////////////////////////////////////
// A figures' editor

class FigsEditor : public Panel
{
  #define NULLFIGURE	m_figs.end()
  #define NULLNODE	m_newPath.end()

  typedef std::list<Figure> Figs;

  Figs m_figs;		            // list of figures
  Figs::iterator m_hotFigure;       // the figure that has the mouse above
  GraphicsPath::iterator m_hotNode; // the path's node that has the mouse above
  GraphicsPath::iterator m_relNode; // related bezier control node to 'm_hotNode' node
  GraphicsPath m_newPath;	    // the temporary path that is being modified
  Figure m_clipboard;		    // the clipboard to save figures
  bool m_moved;			    // true if the mouse moved when after it is captured
  Point m_oldPoint;
  
public:

  FigsEditor(Widget* parent)
    : Panel(parent, PanelStyle + ClientEdgeStyle)
  {
    setBgColor(Color::White);
    setDoubleBuffered(true);

    // create a figure by default
    createNewFigure(Point(80, 80));

    m_hotFigure = NULLFIGURE;
    m_hotNode = NULLNODE;
  }

protected:

  // when the user presses a mouse button over the widget this event is generated
  virtual void onMouseDown(MouseEvent& ev)
  {
    Panel::onMouseDown(ev);

    // update which object (node/figure) is hot
    updateHot(ev.getPoint());
    m_moved = false;

    if (m_hotNode != NULLNODE) {
      m_oldPoint = ev.getPoint();

      if (m_hotNode->getType() == GraphicsPath::BezierTo) {
	m_relNode = m_hotNode-1;
      }
      else if ((m_hotNode+1) != m_newPath.end() &&
	       (m_hotNode+1)->getType() == GraphicsPath::BezierControl1) {
	m_relNode = m_hotNode+1;
      }

      captureMouse();
    }
    else if (m_hotFigure != NULLFIGURE) {
      m_newPath = m_hotFigure->path;
      m_hotNode = NULLNODE;
      m_oldPoint = ev.getPoint();
      captureMouse();
    }
    else if (ev.getButton() == MouseButton::Right) {
      showPopupMenuForBackground(ev.getPoint());
    }
  }

  virtual void onMouseMove(MouseEvent& ev)
  {
    Panel::onMouseMove(ev);

    // is the user dragging a node or figure?
    if (hasCapture()) {
      if (m_oldPoint != ev.getPoint())
	m_moved = true;

      // moving a node?
      if (m_hotNode != NULLNODE) {
	m_hotNode->getPoint() += ev.getPoint() - m_oldPoint;
	if (m_relNode != NULLNODE)
	  m_relNode->getPoint() += ev.getPoint() - m_oldPoint;
	invalidate(true);
      }
      // moving a whole figure/path...
      else {
	m_newPath.offset(ev.getPoint() - m_oldPoint);
	invalidate(true);
      }

      m_oldPoint = ev.getPoint();
    }
    else {
      updateHot(ev.getPoint());
    }
  }
  
  virtual void onMouseUp(MouseEvent& ev)
  {
    Panel::onMouseUp(ev);
    if (hasCapture()) {
      releaseMouse();

      // when finish the node movement
      if (m_hotNode != NULLNODE) {
	m_hotFigure->path = m_newPath;
      }
      // when finish the path movement
      else {
	switch (ev.getButton()) {
	  case MouseButton::Left:
	  case MouseButton::Middle:
	    m_hotFigure->path = m_newPath;
	    break;
	  case MouseButton::Right: {
	    if (m_moved)
	      showPopupMenuForMovePath(ev.getPoint());
	    else
	      showPopupMenuForPath(ev.getPoint());
	    break;
	  }
	  default:
	    // do nothing
	    break;
	}
      }

      m_hotFigure = NULLFIGURE;
      m_hotNode = NULLNODE;
      m_relNode = NULLNODE;
      invalidate(true);
    }
  }

  virtual void onPaint(Graphics &g)
  {
    g.setMiterLimit(10000.0f);

    // stroke and fill all figures
    for (Figs::reverse_iterator it=m_figs.rbegin(); it!=m_figs.rend(); ++it) {
      // stroke & fill the path of this figure
      g.strokeAndFillPath(it->path, it->pen, it->brush, Point(0, 0));
    }

    // there are a hot figure?
    if (m_hotFigure != NULLFIGURE)
      drawHotPath(g, hasCapture() ? m_newPath:
				    m_hotFigure->path);

    // there are a hot node?
    if (m_hotNode != NULLNODE) {
      Point& pt = m_hotNode->getPoint();
      g.drawRect(Pen(Color::Blue, 2), Rect(pt-Point(4,4), pt+Point(4,4)));
    }
  }

  virtual void onResize(const Size &sz)
  {
    Panel::onResize(sz);
    invalidate(true);
  }

private:

  void createNewFigure(const Point& putHere)
  {
    Color color(rand()%200, rand()%200, rand()%200);
    Figure fig;
    fig.pen = Pen(color, 3);
    fig.brush = Brush(color*1.5);

    // fig.path is a GraphicsPath
    fig.path
      .moveTo(-64, -64)
      .lineTo(64, -64)
      .curveTo(128, -32, 128, 32, 64, 64)
      .lineTo(-64, 64)
      .closeFigure()
      .offset(putHere);

    m_figs.push_front(fig);
  }

  void updateHot(const Point& hotSpot)
  {
    // calculate the hot figure
    Figs::iterator hotFig = NULLFIGURE;
    for (Figs::iterator it=m_figs.begin(); it!=m_figs.end(); ++it) {
      if (it->path.toRegion().contains(hotSpot)) {
	hotFig = it;
	break;
      }
    }

    // calculate the hot node
    GraphicsPath::iterator hotNode = NULLNODE;
    for (Figs::iterator it=m_figs.begin(); it!=m_figs.end(); ++it) {
      for (GraphicsPath::iterator it2=it->path.begin(); it2!=it->path.end(); ++it2) {
	Point& pt = it2->getPoint();
	if (Rect(pt-Point(4,4), pt+Point(4,4)).contains(hotSpot)) {
	  m_newPath = it->path;
	  hotNode = m_newPath.begin() + (it2 - it->path.begin());
	  hotFig = it;
	  break;
	}
      }
    }

    if ((m_hotFigure != hotFig) ||
	(m_hotNode != hotNode)) {
      m_hotFigure = hotFig;
      m_hotNode = hotNode;
      invalidate(true);
    }
  }
  
  void drawHotPath(Graphics& g, const GraphicsPath& path)
  {
    GraphicsPath::const_iterator it, end = path.end();
    Pen pen(Color::Blue);
    Pen dot(Color::Blue, 1, PenStyle::Dot);

    // stroke the path with a thick blue pen
    g.strokePath(path, dot, Point(0, 0));

    // draw nodes of the path
    for (it=path.begin(); it!=end; ++it) {
      const Point& pt = it->getPoint();
      g.drawRect(pen, Rect(pt-Point(4,4), pt+Point(4,4)));
    }

    // draw control lines of bezier curves
    for (it=path.begin(); it!=end; ++it) {
      if (it->getType() == GraphicsPath::BezierControl1) {
	Point p1 = (it-1)->getPoint();
	Point p2 = (it  )->getPoint();
	Point p3 = (it+1)->getPoint();
	Point p4 = (it+2)->getPoint();
	g.drawLine(dot, p1, p2);
	g.drawLine(dot, p3, p4);
	it += 2;
      }
    }
  }

  void showPopupMenuForBackground(const Point& pt)
  {
    PopupMenu menu;
    menu.add("&New", CommandId(ID_NEW));
    menu.add("&Paste", CommandId(ID_PASTE))->setEnabled(!m_clipboard.path.empty());
    if (CommandId id = menu.doModal(this, pt)) {
      switch (id) {
	case ID_NEW:
	  createNewFigure(pt);
	  invalidate(true);
	  break;
	case ID_PASTE:
	  m_figs.push_front(m_clipboard);
	  m_figs.front().path.offset(pt);
	  invalidate(true);
	  break;
      }
    }
  }

  void showPopupMenuForMovePath(const Point& pt)
  {
    PopupMenu menu;
    menu.add("&Move Here", CommandId(ID_MOVE_HERE));
    menu.add("&Copy Here", CommandId(ID_COPY_HERE));

    if (CommandId id = menu.doModal(this, pt)) {
      switch (id) {
	case ID_MOVE_HERE:
	  m_hotFigure->path = m_newPath;
	  break;
	case ID_COPY_HERE:
	  m_figs.push_front(Figure(m_newPath,
				   m_hotFigure->pen,
				   m_hotFigure->brush));
	  break;
      }
    }
  }

  void showPopupMenuForPath(const Point& pt)
  {
    PopupMenu menu;
    menu.add("Cu&t", CommandId(ID_CUT));
    menu.add("&Copy", CommandId(ID_COPY));
    menu.add("&Remove", CommandId(ID_REMOVE));
    menu.addSeparator();
    menu.add("Send to &Front", CommandId(ID_FRONT));
    menu.add("Send to &Back", CommandId(ID_BACK));
    menu.addSeparator();
    menu.add("&Properties", CommandId(ID_PROPERTIES));

    if (CommandId id = menu.doModal(this, pt)) {
      switch (id) {
	case ID_CUT:
	case ID_COPY: {
	  m_clipboard = *m_hotFigure;

	  Rect bounds = m_clipboard.path.toRegion().getBounds();
	  m_clipboard.path.offset(-bounds.getOrigin() - Point(bounds.getSize()/2));

	  if (id == ID_CUT)
	    m_figs.erase(m_hotFigure);
	  break;
	}
	case ID_REMOVE:
	  m_figs.erase(m_hotFigure);
	  break;
	case ID_FRONT: {
	  Figure copy = *m_hotFigure;
	  m_figs.erase(m_hotFigure);
	  m_figs.push_front(copy);
	  break;
	}
	case ID_BACK: {
	  Figure copy = *m_hotFigure;
	  m_figs.erase(m_hotFigure);
	  m_figs.push_back(copy);
	  break;
	}
	case ID_PROPERTIES:
	  // TODO
	  break;
      }
    }
  }

};

//////////////////////////////////////////////////////////////////////
// MainFrame

class MainFrame : public Frame
{
  FigsEditor m_editor;
  // CheckBox m_closeFigure;
  // GroupBox m_penPanel;
  // GroupBox m_brushPanel;
  // for Pen
  // Panel m_penLeftPanel;
  // Label m_penWidthLabel;
  // Slider m_penWidth;
  // Button m_penColor;
  // GroupBox m_endCapPanel;
  // RadioGroup m_endCapGroup;
  // RadioButton m_roundEndCap;
  // RadioButton m_squareEndCap;
  // RadioButton m_flatEndCap;
  // GroupBox m_joinPanel;
  // RadioGroup m_joinGroup;
  // RadioButton m_roundJoin;
  // RadioButton m_bevelJoin;
  // RadioButton m_miterJoin;
  // // for Brush
  // Button m_brushColor;
  
public:

  MainFrame()
    : Frame("Paths")
    , m_editor(this)
    // , m_closeFigure("Close Figure", this)
    // , m_penPanel("Pen", this)
    // , m_brushPanel("Brush", this)
    // , m_penLeftPanel(&m_penPanel)
    // , m_penWidthLabel("Width:", &m_penLeftPanel)
    // , m_penWidth(1, 64, 32, &m_penLeftPanel)
    // , m_penColor("Color", &m_penLeftPanel)
    // , m_endCapPanel("End Cap", &m_penPanel)
    // , m_roundEndCap("Round", m_endCapGroup, &m_endCapPanel)
    // , m_squareEndCap("Square", m_endCapGroup, &m_endCapPanel)
    // , m_flatEndCap("Flat", m_endCapGroup, &m_endCapPanel)
    // , m_joinPanel("Join", &m_penPanel)
    // , m_roundJoin("Round", m_joinGroup, &m_joinPanel)
    // , m_bevelJoin("Bevel", m_joinGroup, &m_joinPanel)
    // , m_miterJoin("Miter", m_joinGroup, &m_joinPanel)
    // , m_brushColor("Color", &m_brushPanel)
  {
    setLayout(new ClientLayout);
    // setLayout(new BoxLayout(Orientation::Vertical, false));
    // m_editor.setConstraint(new BoxConstraint(true));
    // m_penPanel.setLayout(new BoxLayout(Orientation::Horizontal, true, 0));
    // m_penLeftPanel.setLayout(new BoxLayout(Orientation::Vertical, false, 0));
    // m_endCapPanel.setLayout(new BoxLayout(Orientation::Vertical, true, 0));
    // m_joinPanel.setLayout(new BoxLayout(Orientation::Vertical, true, 0));
    // m_brushPanel.setLayout(new BoxLayout(Orientation::Horizontal, false, 0));

    // m_roundEndCap.setSelected(true);
    // m_roundJoin.setSelected(true);

    // m_penWidth.Change.connect(Bind(&MainFrame::regenerate, this));
    // m_endCapGroup.Change.connect(Bind(&MainFrame::regenerate, this));
    // m_joinGroup.Change.connect(Bind(&MainFrame::regenerate, this));

    // m_penColor.Action.connect(Bind(&MainFrame::onSelectPenColor, this));
    // m_brushColor.Action.connect(Bind(&MainFrame::onSelectBrushColor, this));
    // m_closeFigure.Action.connect(Bind(&MainFrame::onCloseFigure, this));
  }

private:

  // void onSelectPenColor()
  // {
  //   ColorDialog dlg(m_preview.getPen().getColor(), this);
  //   if (dlg.doModal())
  //     m_preview.setPen(createPen(dlg.getColor()));
  // }

  // void onSelectBrushColor()
  // {
  //   ColorDialog dlg(m_preview.getBrush().getColor(), this);
  //   if (dlg.doModal())
  //     m_preview.setBrush(Brush(dlg.getColor()));
  // }

  // void onCloseFigure()
  // {
  //   m_preview.setCloseFigure(m_closeFigure.isSelected());
  // }

  // void regenerate()
  // {
  //   m_preview.setPen(createPen(m_preview.getPen().getColor()));
  // }

  // Pen createPen(const Color& color)
  // {
  //   PenEndCap endCap;
  //   PenJoin join;
  //   switch (m_endCapGroup.getSelectedIndex()) {
  //     case 0: endCap = PenEndCap::Round; break;
  //     case 1: endCap = PenEndCap::Square; break;
  //     case 2: endCap = PenEndCap::Flat; break;
  //   }
  //   switch (m_joinGroup.getSelectedIndex()) {
  //     case 0: join = PenJoin::Round; break;
  //     case 1: join = PenJoin::Bevel; break;
  //     case 2: join = PenJoin::Miter; break;
  //   }
  //   return Pen(color,
  // 	       m_penWidth.getValue(),
  // 	       PenStyle::Solid,
  // 	       endCap, join);
  // }

};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setVisible(true);
  app.run();
  return 0;
}
