// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
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

  bool isClosed() const
  {
    // the last node in the path indicates if the path is closed or not
    return (path.end()-1)->isCloseFigure();
  }

  void setClosed(bool state)
  {
    // the last node in the path indicates if the path is closed or not
    (path.end()-1)->setCloseFigure(state);
  }

};

//////////////////////////////////////////////////////////////////////
// Dialog to edit figure properties

class FigureProperties : public Dialog
{
  Figure& m_fig;
  Figure m_figBackup;
  // for Pen
  Separator m_penInfo;
  Label m_penLabel;
  Label m_penWidthLabel;
  Slider m_penWidthSlider;
  TextEdit m_penWidthEdit;
  Button m_penColor;
  GroupBox m_endCapInfo;
  RadioGroup m_endCapGroup;
  RadioButton m_roundEndCap;
  RadioButton m_squareEndCap;
  RadioButton m_flatEndCap;
  GroupBox m_joinInfo;
  RadioGroup m_joinGroup;
  RadioButton m_roundJoin;
  RadioButton m_bevelJoin;
  RadioButton m_miterJoin;
  // for Brush
  Separator m_brushInfo;
  Label m_brushLabel;
  Button m_brushColor;
  // bottom
  Separator m_separator;
  CheckBox m_closeFigure;
  Button m_ok;
  Button m_cancel;

public:

  // signal fired when the figure's properties are modified
  // in the dialog
  Signal0<void> Change;

  FigureProperties(Figure& fig, Widget* parent)
    : Dialog(L"Figure Properties", parent)
    , m_fig(fig)
    , m_figBackup(fig)
    , m_penInfo(this)
    , m_penLabel(L"Pen configuration:", this)
    , m_penWidthLabel(L"Width:", this)
    , m_penWidthSlider(1, 64, 32, this)
    , m_penWidthEdit(L"", this, TextEdit::Styles::Default +
				TextEdit::Styles::ReadOnly)
    , m_penColor(L"Color", this)
    , m_endCapInfo(L"End Cap", this)
    , m_roundEndCap(L"Round", m_endCapGroup, &m_endCapInfo)
    , m_squareEndCap(L"Square", m_endCapGroup, &m_endCapInfo)
    , m_flatEndCap(L"Flat", m_endCapGroup, &m_endCapInfo)
    , m_joinInfo(L"Join", this)
    , m_roundJoin(L"Round", m_joinGroup, &m_joinInfo)
    , m_bevelJoin(L"Bevel", m_joinGroup, &m_joinInfo)
    , m_miterJoin(L"Miter", m_joinGroup, &m_joinInfo)
    , m_brushInfo(this)
    , m_brushLabel(L"Brush configuration:", this)
    , m_brushColor(L"Color", this)
    , m_separator(this)
    , m_closeFigure(L"Close Figure", this)
    , m_ok(L"&OK", IDOK, this)
    , m_cancel(L"Cancel", IDCANCEL, this)
  {
    // preferred size for the TextEdit control of the pen-width
    m_penWidthEdit.setPreferredSize(Size(32, m_penWidthEdit.getPreferredSize().h));

    // this two labels will have boldface
    m_penLabel.setFont(Font(getFont(), FontStyle::Italic));
    m_brushLabel.setFont(Font(getFont(), FontStyle::Italic));

    // layout managers
    setLayout(Bix::parse(L"Y[%,%,Y[X[%,f%,%],X[f%,f%,Y[%]]],%,%,X[%],%,X[f%,eX[%,%]]]",
			 &m_penInfo,
			 &m_penLabel,
			 &m_penWidthLabel, &m_penWidthSlider, &m_penWidthEdit,
			 &m_endCapInfo, &m_joinInfo, &m_penColor,
			 &m_brushInfo,
			 &m_brushLabel,
			 &m_brushColor,
			 &m_separator,
			 &m_closeFigure, &m_ok, &m_cancel));
    m_endCapInfo.setLayout(new BoxLayout(Orientation::Vertical, false));
    m_joinInfo.setLayout(new BoxLayout(Orientation::Vertical, false));

    initValues();
    
    // signals/slots
    m_penWidthSlider.Change.connect(Bind(&FigureProperties::onPenChange, this));
    m_endCapGroup.Change.connect(Bind(&FigureProperties::onPenChange, this));
    m_joinGroup.Change.connect(Bind(&FigureProperties::onPenChange, this));

    m_penColor.Action.connect(Bind(&FigureProperties::onSelectPenColor, this));
    m_brushColor.Action.connect(Bind(&FigureProperties::onSelectBrushColor, this));
    m_closeFigure.Action.connect(Bind(&FigureProperties::onCloseFigure, this));
    
    // center the dialog
    setSize(getPreferredSize());
    center();
  }

private:

  // fill the widgets through the current figure values
  void initValues()
  {
    m_penWidthSlider.setValue(m_fig.pen.getWidth());
    m_penWidthEdit.setText(convert_to<String>(m_fig.pen.getWidth()));
      
    m_endCapGroup.setSelectedIndex(m_fig.pen.getEndCap());
    m_joinGroup.setSelectedIndex(m_fig.pen.getJoin());

    m_closeFigure.setSelected(m_fig.isClosed());
  }

  // If the user presses the Cancel button, then we have to restore to
  // the old figure properties
  virtual void onCancel()
  {
    // restore the figure configuration
    m_fig = m_figBackup;

    // fire the Change signal to indicate that the figure has changed
    Change();

    // close the dialog
    Dialog::onCancel();
  }

  void onPenChange()
  {
    m_fig.pen = createPen(m_fig.pen.getColor());
    m_penWidthEdit.setText(convert_to<String>(m_fig.pen.getWidth()));
    Change();
  }

  void onSelectPenColor()
  {
    ColorDialog dlg(m_fig.pen.getColor(), this);
    if (dlg.doModal()) {
      m_fig.pen = createPen(dlg.getColor());
      Change();
    }
  }

  void onSelectBrushColor()
  {
    ColorDialog dlg(m_fig.brush.getColor(), this);
    if (dlg.doModal()) {
      m_fig.brush = Brush(dlg.getColor());
      Change();
    }
  }

  void onCloseFigure()
  {
    m_fig.setClosed(m_closeFigure.isSelected());
    Change();
  }

  Pen createPen(const Color& color)
  {
    PenEndCap endCap;
    PenJoin join;
    switch (m_endCapGroup.getSelectedIndex()) {
      case 0: endCap = PenEndCap::Round; break;
      case 1: endCap = PenEndCap::Square; break;
      case 2: endCap = PenEndCap::Flat; break;
    }
    switch (m_joinGroup.getSelectedIndex()) {
      case 0: join = PenJoin::Round; break;
      case 1: join = PenJoin::Bevel; break;
      case 2: join = PenJoin::Miter; break;
    }
    return Pen(color,
  	       m_penWidthSlider.getValue(),
  	       PenStyle::Solid,
  	       endCap, join);
  }
  
};

//////////////////////////////////////////////////////////////////////
// A figures' editor

class FigsEditor : public Panel
{
  #define NULLFIGURE	m_figs.end()
  #define NULLNODE	m_newPath.end()

  typedef std::list<Figure> Figs;

  Figs m_figs;		            // list of figures
  Figs::iterator m_selFigure;       // selected figure
  GraphicsPath::iterator m_hotNode; // the path's node that has the mouse above
  GraphicsPath::iterator m_relNode; // related bezier control node to 'm_hotNode' node
  GraphicsPath m_newPath;	    // the temporary path that is being modified
  Figure m_clipboard;		    // the clipboard to save figures
  bool m_moved;			    // true if the mouse moved when after it is captured
  Point m_oldPoint;
  
public:

  FigsEditor(Widget* parent)
    : Panel(parent, Panel::Styles::Default +
		    Widget::Styles::ClientEdge)
  {
    setBgColor(Color::White);
    setDoubleBuffered(true);

    // create a figure by default
    createNewFigure(Point(80, 80));

    m_selFigure = NULLFIGURE;
    m_hotNode = NULLNODE;
  }

protected:

  // when the user presses a mouse button over the widget this event is generated
  virtual void onMouseDown(MouseEvent& ev)
  {
    Panel::onMouseDown(ev);

    // if we have the capture here is because the user pressed another
    // button but without releasing the first one
    if (hasCapture())
      return;

    m_moved = false;

    // first check if we are above a node of the current selected figure
    updateHotNode(ev.getPoint());
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
    else {
      // check if with this click we change the current selected figure
      updateSelectedFigure(ev.getPoint());
      if (m_selFigure != NULLFIGURE) {
	m_oldPoint = ev.getPoint();
	captureMouse();
      }
      // if the user select in the background we can show the menu to
      // create new figures
      else if (ev.getButton() == MouseButton::Right) {
	showPopupMenuForBackground(ev.getPoint());
      }
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
      updateHotNode(ev.getPoint());
    }
  }
  
  virtual void onMouseUp(MouseEvent& ev)
  {
    Panel::onMouseUp(ev);
    if (hasCapture()) {
      releaseMouse();

      // when finish the node movement
      if (m_hotNode != NULLNODE) {
	m_selFigure->path = m_newPath;
      }
      // when finish the path movement
      else {
	switch (ev.getButton()) {
	  case MouseButton::Left:
	  case MouseButton::Middle:
	    m_selFigure->path = m_newPath;
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
      // stroke & fill the closed paths
      if (it->isClosed())
	g.strokeAndFillPath(it->path, it->pen, it->brush, Point(0, 0));
      // only stroke opened paths
      else
	g.strokePath(it->path, it->pen, Point(0, 0));
    }

    // there are a selected figure?
    if (m_selFigure != NULLFIGURE)
      drawSelectedPath(g, hasCapture() ? m_newPath:
					 m_selFigure->path);

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
    Color color(120+(rand()%80),
		120+(rand()%80),
		120+(rand()%80));
    Figure fig;
    fig.pen = Pen(color, 3);
    fig.brush = Brush(color*1.3);

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

  void updateSelectedFigure(const Point& hotSpot)
  {
    Figs::iterator selFig = NULLFIGURE;

    for (Figs::iterator it=m_figs.begin(); it!=m_figs.end(); ++it) {
      if (it->path.toRegion().contains(hotSpot)) {
	selFig = it;
	break;
      }
    }

    if (m_selFigure != selFig) {
      m_selFigure = selFig;
      invalidate(true);

      if (m_selFigure != NULLFIGURE) {
	m_newPath = m_selFigure->path;
      }
      else {
	m_newPath = GraphicsPath();
      }
      m_hotNode = NULLNODE;
    }
  }

  void updateHotNode(const Point& hotSpot)
  {
    GraphicsPath::iterator it;

    for (it=m_newPath.begin(); it!=m_newPath.end(); ++it) {
      Point& pt = it->getPoint();
      if (Rect(pt-Point(4,4), pt+Point(4,4)).contains(hotSpot))
	break;
    }

    if (m_hotNode != it) {
      m_hotNode = it;
      invalidate(true);
    }
  }

  void drawSelectedPath(Graphics& g, const GraphicsPath& path)
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
    menu.add(L"&New", CommandId(ID_NEW));
    menu.add(L"&Paste", CommandId(ID_PASTE))->setEnabled(!m_clipboard.path.empty());
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
    menu.add(L"&Move Here", CommandId(ID_MOVE_HERE));
    menu.add(L"&Copy Here", CommandId(ID_COPY_HERE));

    if (CommandId id = menu.doModal(this, pt)) {
      switch (id) {
	case ID_MOVE_HERE:
	  m_selFigure->path = m_newPath;
	  break;
	case ID_COPY_HERE:
	  m_figs.push_front(Figure(m_newPath,
				   m_selFigure->pen,
				   m_selFigure->brush));
	  m_selFigure = m_figs.begin();
	  break;
      }
    }
  }

  void showPopupMenuForPath(const Point& pt)
  {
    PopupMenu menu;
    menu.add(L"Cu&t", CommandId(ID_CUT));
    menu.add(L"&Copy", CommandId(ID_COPY));
    menu.add(L"&Remove", CommandId(ID_REMOVE));
    menu.addSeparator();
    menu.add(L"Send to &Front", CommandId(ID_FRONT));
    menu.add(L"Send to &Back", CommandId(ID_BACK));
    menu.addSeparator();
    menu.add(L"&Properties", CommandId(ID_PROPERTIES));

    if (CommandId id = menu.doModal(this, pt)) {
      switch (id) {
	case ID_CUT:
	case ID_COPY: {
	  m_clipboard = *m_selFigure;

	  Rect bounds = m_clipboard.path.toRegion().getBounds();
	  m_clipboard.path.offset(-bounds.getOrigin() - Point(bounds.getSize()/2));

	  if (id == ID_CUT) {
	    m_figs.erase(m_selFigure);
	    m_selFigure = NULLFIGURE;
	  }
	  break;
	}
	case ID_REMOVE:
	  m_figs.erase(m_selFigure);
	  m_selFigure = NULLFIGURE;
	  break;
	case ID_FRONT: {
	  Figure copy = *m_selFigure;
	  m_figs.erase(m_selFigure);
	  m_figs.push_front(copy);
	  m_selFigure = m_figs.begin();
	  break;
	}
	case ID_BACK: {
	  Figure copy = *m_selFigure;
	  m_figs.erase(m_selFigure);
	  m_figs.push_back(copy);
	  m_selFigure = m_figs.end();
	  break;
	}
	case ID_PROPERTIES: {
	  FigureProperties dlg(*m_selFigure, getParent());

	  // if the properties in dialog are changed, then we have to
	  // invalidate the editor so we can see the changes immediately
	  dlg.Change.connect(Bind(&FigsEditor::invalidate, this, true));

	  // show the dialog
	  dlg.doModal();

	  m_newPath = m_selFigure->path;
	  break;
	}
      }
    }
  }

};

//////////////////////////////////////////////////////////////////////
// MainFrame

class MainFrame : public Frame
{
  FigsEditor m_editor;
  
public:

  MainFrame()
    : Frame(L"Paths")
    , m_editor(this)
  {
    setLayout(new ClientLayout);
  }

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
