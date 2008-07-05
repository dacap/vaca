// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

#include <map>
#include <Vaca/Vaca.hpp>
#include "resource.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// an element of the model

class Element
{
public:

  enum Type {
    Widget,
    Row,
    Column,
    Matrix
  };

private:

  String m_name;
  Type m_type;
  int m_columns;
  Element *m_parent;
  std::vector<Element *> m_children;

public:

  Element(const String &name, Type type, int columns = 0) {
    m_name = name;
    m_type = type;
    m_parent = NULL;
    m_columns = columns;
  }

  virtual ~Element() {
    std::vector<Element *>::iterator it;
    for (it = m_children.begin(); it != m_children.end(); ++it)
      delete *it;
  }

  String getName() {
    return m_name;
  }

  void setName(const String &name) {
    m_name = name;
  }

  Type getType() {
    return m_type;
  }

  void setType(Type type) {
    m_type = type;
  }

  int getColumns() {
    return m_columns;
  }

  void setColumns(int columns) {
    m_columns = columns;
  }

  bool acceptChildren() {
    return m_type != Element::Widget;
  }

  Element *getParent() {
    return m_parent;
  }

  bool isAncestor(Element *parent) {
    Element *p = m_parent;
    
    while (p != NULL) {
      if (p == parent)
	return true;
      p = p->m_parent;
    }

    return false;
  }

  std::vector<Element *> getChildren() {
    return m_children;
  }

  void add(Element *e) {
    m_children.push_back(e);
    e->m_parent = this; 
  }

  void remove(Element *e) {
    remove_element_from_container(m_children, e);
    e->m_parent = NULL; 
  }

};

//////////////////////////////////////////////////////////////////////
// the model

class Model
{
  Element *m_root;

public:

  boost::signal<void (Element *element, Element *parent)> BeforeAddElement;
  boost::signal<void (Element *element)> AfterAddElement;
  boost::signal<void (Element *element)> BeforeRemoveElement;
  boost::signal<void (Element *element, Element *parent)> AfterRemoveElement;

  Model() {
    m_root = NULL;
  }

  virtual ~Model() {
    if (m_root != NULL)
      delete m_root;
  }

  Element *getRoot() {
    return m_root;
  }

  void addElement(Element *element, Element *parent = NULL)
  {
    if (parent == NULL)
      parent = m_root;

    BeforeAddElement(element, parent);

    if (parent != NULL)
      parent->add(element);
    else
      m_root = element;

    AfterAddElement(element);
  }

  void removeElement(Element *element)
  {
    BeforeRemoveElement(element);

    Element *parent = element->getParent();
    if (parent != NULL)
      parent->remove(element);
    else if (element == m_root)
      m_root = NULL;

    AfterRemoveElement(element, parent);
  }

};

//////////////////////////////////////////////////////////////////////
// a representation of an element of the model in a TreeNode

class ElementViewAsTreeNode : public TreeNode
{
  Element *m_element;

public:
  ElementViewAsTreeNode(Element *element)
    : TreeNode(element->getName())
  {
    m_element = element;
  }

  Element *getElement() {
    return m_element;
  }
};

//////////////////////////////////////////////////////////////////////
// a representation of the model in a TreeView

class ModelViewAsTreeView : public TreeView
{
  Model *m_model;
    
public:

  boost::signal<void (Element *)> ElementSelected;

  ModelViewAsTreeView(Model *model, Widget *parent)
    : TreeView(parent)
    , m_model(model)
  {
    m_model->BeforeAddElement.connect(Bind(&ModelViewAsTreeView::onBeforeAddElement, this));
    m_model->AfterAddElement.connect(Bind(&ModelViewAsTreeView::onAfterAddElement, this));
    m_model->BeforeRemoveElement.connect(Bind(&ModelViewAsTreeView::onBeforeRemoveElement, this));
    m_model->AfterRemoveElement.connect(Bind(&ModelViewAsTreeView::onAfterRemoveElement, this));
  }

  void selectElement(Element *element)
  {
    TreeView::iterator it;

    for (it = begin(); it != end(); ++it) {
      ElementViewAsTreeNode *node = dynamic_cast<ElementViewAsTreeNode *>(*it);
      if (node->getElement() == element) {
	setSelectedNode(node);
	break;
      }
    }
  }

protected:

  virtual void onAfterSelect(TreeViewEvent &ev)
  {
    TreeView::onAfterSelect(ev);
    if (!ev.isCanceled()) {
      // generate the ElementSelected signal...

      TreeNode *node = ev.getTreeNode();
      if (node != NULL) {
	ElementViewAsTreeNode *elemNode = dynamic_cast<ElementViewAsTreeNode *>(node);
	ElementSelected(elemNode->getElement());
      }
      else
	ElementSelected(NULL);
    }
  }

private:

  void onBeforeAddElement(Element *element, Element *parent)
  {
  }

  void onAfterAddElement(Element *element)
  {
    ElementViewAsTreeNode *elementNode = new ElementViewAsTreeNode(element);
    ElementViewAsTreeNode *parentNode = findNodeByElement(element->getParent());

    if (parentNode != NULL) {
      parentNode->addNode(elementNode);	// add the new node in the parent

      // ...expand the parent
      if (!parentNode->isExpanded())
	parentNode->setExpanded(true);

      setSelectedNode(parentNode);	// ...selected the parent node
      elementNode->ensureVisibility();	// ...but ensure visibility of the new node
    }
    else
      addNode(elementNode);

    invalidate(false);
  }

  void onBeforeRemoveElement(Element *element)
  {
    ElementViewAsTreeNode *elementNode = findNodeByElement(element);
    removeNode(elementNode);
    delete elementNode;

    invalidate(false);
  }

  void onAfterRemoveElement(Element *element, Element *parent)
  {
  }

  ElementViewAsTreeNode *findNodeByElement(Element *element)
  {
    TreeView::iterator it;

    for (it = begin(); it != end(); ++it) {
      ElementViewAsTreeNode *node = dynamic_cast<ElementViewAsTreeNode *>(*it);
      if (node->getElement() == element)
	return node;
    }

    return NULL;
  }
  
};

//////////////////////////////////////////////////////////////////////
// a representation of an element as widgets

class ElementViewAsWidgets : public MultilineEdit
{
  Element *m_element;
  bool m_selected;

public:

  boost::signal<void (Element *)> ElementSelected;

  ElementViewAsWidgets(Element *element, Widget *parent)
    : MultilineEdit(element->getName(), parent, MultilineEditStyle + AutoVerticalScrollEditStyle)
  {
    m_element = element;
    m_selected = false;

    Change.connect(Bind(&Widget::layout, parent));
  }

  Element *getElement() {
    return m_element;
  }

  bool isSelected() {
    return m_selected;
  }

  void setSelected(bool state) {
    m_selected = state;

    setBgColor(state ? Color(255, 255, 220):
		       Color::White);

    invalidate(true);
  }

protected:

  virtual void onGotFocus(Event &ev)
  {
    MultilineEdit::onGotFocus(ev);
    ElementSelected(m_element);
  }

  virtual void onLostFocus(Event &ev)
  {
    MultilineEdit::onLostFocus(ev);
  }

};

//////////////////////////////////////////////////////////////////////

class ModelViewAsWidgets : public Panel
{
  Model *m_model;
  Bix *m_bix;
  std::map<Element *, Bix *> m_mapElemBix;

public:

  boost::signal<void (Element *)> ElementSelected;

  ModelViewAsWidgets(Model *model, Widget *parent)
    : Panel(parent, PanelStyle + ClientEdgeStyle)
    , m_model(model)
    , m_bix(NULL)
  {
    m_model->BeforeAddElement.connect(Bind(&ModelViewAsWidgets::onBeforeAddElement, this));
    m_model->AfterAddElement.connect(Bind(&ModelViewAsWidgets::onAfterAddElement, this));
    m_model->BeforeRemoveElement.connect(Bind(&ModelViewAsWidgets::onBeforeRemoveElement, this));
    m_model->AfterRemoveElement.connect(Bind(&ModelViewAsWidgets::onAfterRemoveElement, this));
  }

  virtual ~ModelViewAsWidgets()
  {
    Container children = getChildren();
    Container::iterator it;
    for (it = children.begin(); it != children.end(); ++it)
      delete *it;
  }

  void selectElement(Element *element)
  {
    Container children = getChildren();
    Container::iterator it;
    for (it = children.begin(); it != children.end(); ++it) {
      ElementViewAsWidgets *w = dynamic_cast<ElementViewAsWidgets *>(*it);

      if (w->getElement() == element ||
	  w->getElement()->isAncestor(element)) {
	w->setSelected(true);
      }
      else if (w->isSelected()) {
	w->setSelected(false);
      }
    }
  }

private:

  int convertElementTypeToBixFlags(Element *element)
  {
    switch (element->getType()) {
      case Element::Row:    return BixRow; break;
      case Element::Column: return BixCol; break;
      case Element::Matrix: return BixMat; break;
      default:
	return 0;
    }
  }

  void onBeforeAddElement(Element *element, Element *parent)
  {
    int flags = convertElementTypeToBixFlags(element);

    if (parent == NULL) {
      assert(m_bix == NULL);

      if (flags == 0) {
	// avoid
      }
      else {
	m_bix = new Bix(this, flags, element->getColumns());
	m_mapElemBix[element] = m_bix;
	setLayout(m_bix);
      }
    }
    else {
      Bix *parentBix = m_mapElemBix[parent];
      
      if (flags == 0) {
	ElementViewAsWidgets *elementWidget = new ElementViewAsWidgets(element, this);
	elementWidget->ElementSelected.connect(Bind(&ModelViewAsWidgets::onElementSelected, this));
	parentBix->add(elementWidget);
      }
      else {
	m_mapElemBix[element] = parentBix->add(flags, element->getColumns());
      }

      layout();
    }

//     if (parentNode != NULL) {
//       parentNode->addNode(elementNode);	// add the new node in the parent

//       // ...expand the parent
//       if (!parentNode->isExpanded())
// 	parentNode->setExpanded(true);

//       setSelectedNode(parentNode);	// ...selected the parent node
//       elementNode->ensureVisibility();	// ...but ensure visibility of the new node
//     }
//     else
//       addNode(elementNode);

//     invalidate(false);
  }

  void onAfterAddElement(Element *element)
  {
  }

  void onBeforeRemoveElement(Element *element)
  {
    if (element->getType() == Element::Widget) {
      Container children = getChildren();
      Container::iterator it;
      for (it = children.begin(); it != children.end(); ++it) {
	ElementViewAsWidgets *w = dynamic_cast<ElementViewAsWidgets *>(*it);

	if (w->getElement() == element) {
	  Element *parentElement = element->getParent();
	  Bix *parentBix = m_mapElemBix[parentElement];

	  parentBix->remove(w);
	  w->getParent()->removeChild(w, true);
	  delete w;
	  break;
	}
      }
    }
    else {
      Container children = getChildren();
      Container::iterator it;
      for (it = children.begin(); it != children.end(); ++it) {
	ElementViewAsWidgets *w = dynamic_cast<ElementViewAsWidgets *>(*it);

	if (w->getElement()->isAncestor(element)) {
	  w->getParent()->removeChild(w, true);
	  delete w;
	}
      }

      if (element->getParent() != NULL) {
	Bix *parentBix = m_mapElemBix[element->getParent()];
	if (parentBix != NULL)
	  parentBix->remove(m_mapElemBix[element]);
      }
      else {
	m_bix = NULL;
	setLayout(NULL);
      }
    }

    layout();
  }
  
  void onAfterRemoveElement(Element *element, Element *parent)
  {
  }

  void onElementSelected(Element *element)
  {
    selectElement(element);
    ElementSelected(element);
  }

};

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  Model m_model;
  Element *m_selectedElement;
  ModelViewAsTreeView m_treeView;
  ModelViewAsWidgets m_widgetsView;
  ToolBar m_toolBar;
  ImageList m_imageList;
  int m_widgetCounter;
  bool m_disableReselect;

public:

  MainFrame()
    : Frame("Bixes")
    , m_model()
    , m_selectedElement(NULL)
    , m_treeView(&m_model, this)
    , m_widgetsView(&m_model, this)
    , m_toolBar("Standard", this, ToolSetStyle + FlatToolSetStyle)
    , m_imageList(ResourceId(IDB_TOOLBAR), 16, Color(192, 192, 192))
    , m_widgetCounter(0)
    , m_disableReselect(false)
  {
    // layout
    setLayout(Bix::parse("X[%,f%]", &m_treeView, &m_widgetsView));
    m_treeView.setPreferredSize(Size(256, 256));

    // signals
    m_treeView.ElementSelected.connect(Bind(&MainFrame::onElementSelectedFromTreeView, this));
    m_widgetsView.ElementSelected.connect(Bind(&MainFrame::onElementSelectedFromWidgets, this));

    // setup the tool bar
    m_toolBar.getSet().setImageList(m_imageList);
    m_toolBar.getSet().addButton(0, IDM_ADD_COLUMN, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(1, IDM_ADD_ROW, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(2, IDM_ADD_MATRIX, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(3, IDM_ADD_WIDGET, TBSTATE_ENABLED);
    m_toolBar.getSet().addSeparator();
    m_toolBar.getSet().addButton(4, IDM_REMOVE, TBSTATE_ENABLED);
    m_toolBar.getSet().addSeparator();
    m_toolBar.getSet().addButton(5, IDM_PROPERTIES, TBSTATE_ENABLED);

    // setup the defaults dock areas
    defaultDockAreas();

    // put the tool bar in the top dock area
    m_toolBar.dockIn(getDockArea(Side::Top));
  }

protected:

  
  virtual bool onActionById(int actionId)
  {
    switch (actionId) {

      case IDM_ADD_COLUMN:
	onAddElement(Element::Column);
	break;

      case IDM_ADD_ROW:
	onAddElement(Element::Row);
	break;

      case IDM_ADD_MATRIX: {
	int columns;
	if (askIntValue(columns))
	  onAddElement(Element::Matrix, columns);
	break;
      }

      case IDM_ADD_WIDGET:
	onAddElement(Element::Widget);
	break;

      case IDM_REMOVE:
	onRemoveElement();
	break;

      case IDM_PROPERTIES:
	break;

      default:
	return false;		// "id" not used
    }
    return true;		// "id" was used
  }

private:

  // adds the specified type of element has a child of the selected element
  void onAddElement(Element::Type elementType, int columns = 0)
  {
    if (m_selectedElement == NULL ||
	m_selectedElement->acceptChildren()) {
      String name = "Unknown";

      switch (elementType) {
	case Element::Widget: name = "Widget " + String::fromInt(++m_widgetCounter); break;
	case Element::Row:    name = "BixRow (a row vector)"; break;
	case Element::Column: name = "BixCol (a column vector)"; break;
	case Element::Matrix: name = "BixMat (a matrix)"; break;
      }

      m_model.addElement(new Element(name, elementType, columns),
			 m_selectedElement);
    }
  }

  // remove the selected element
  void onRemoveElement()
  {
    if (m_selectedElement != NULL) {
      Element *e = m_selectedElement;
      m_selectedElement = NULL;

      m_model.removeElement(e);
      delete e;
    }
  }

  void onElementSelectedFromTreeView(Element *element)
  {
    if (!m_disableReselect) {
      m_selectedElement = element;

      m_disableReselect = true;
      m_widgetsView.selectElement(element);
      m_disableReselect = false;
    }
  }

  void onElementSelectedFromWidgets(Element *element)
  {
    if (!m_disableReselect) {
      m_selectedElement = element;

      m_disableReselect = true;
      m_treeView.selectElement(element);
      m_disableReselect = false;
    }
  }

  bool askIntValue(int &retValue)
  {
    Dialog dlg("Insert a value", this);
    Label label("Number of columns of the matrix:", &dlg);
    Edit value("3", &dlg);
    Button ok("&OK", &dlg);
    Button cancel("&Cancel", &dlg);

    dlg.setLayout(Bix::parse("Y[X[%,f%],X[fX[],eX[%,%]]]",
			     &label, &value, &ok, &cancel));

    value.setPreferredSize(Size(32, value.getPreferredSize().h));
    dlg.setSize(dlg.getPreferredSize());
    dlg.center();

    ok.setDefault(true);
    ok.Action.connect(Bind(&Dialog::defaultOkAction, &dlg));
    cancel.Action.connect(Bind(&Dialog::defaultCancelAction, &dlg));

    while (true) {
      value.requestFocus();
      value.selectAll();
      if (!dlg.doModal())
	return false;

      retValue = value.getText().parseInt();
      if (retValue < 1)
	msgBox("You must to insert a value greater than 1", "Error", MB_OK);
      else
	return true;
    }
  }

};

//////////////////////////////////////////////////////////////////////

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Application app;
  try {
    MainFrame frame;
    frame.setVisible(true);
    app.run();
  }
  catch (Exception &e) {
    e.show();
  }
  return 0;
}
