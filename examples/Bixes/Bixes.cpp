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

#include <map>
#include <Vaca/Vaca.h>
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

  typedef std::vector<Element*> Elements;

private:

  String m_name;
  Type m_type;
  int m_columns;
  Element* m_parent;
  Elements m_children;

public:

  Element(const String& name, Type type, int columns = 0) {
    m_name = name;
    m_type = type;
    m_parent = NULL;
    m_columns = columns;
  }

  virtual ~Element() {
    Elements::iterator it;
    for (it = m_children.begin(); it != m_children.end(); ++it)
      delete *it;
  }

  String getName() const { return m_name; }
  Type getType() const { return m_type; }
  int getColumns() const { return m_columns; }
  Element* getParent() const { return m_parent; }
  Elements getChildren() const { return m_children; }

  void setName(const String& name) { m_name = name; }
  void setType(Type type) { m_type = type; }
  void setColumns(int columns) { m_columns = columns; }

  bool acceptChildren() const {
    return m_type != Element::Widget;
  }

  bool isAncestor(Element* parent) const {
    Element* p = m_parent;
    
    while (p != NULL) {
      if (p == parent)
	return true;
      p = p->m_parent;
    }

    return false;
  }

  void add(Element* e) {
    m_children.push_back(e);
    e->m_parent = this; 
  }

  void remove(Element* e) {
    remove_from_container(m_children, e);
    e->m_parent = NULL; 
  }

};

//////////////////////////////////////////////////////////////////////
// the model

class Model
{
  Element* m_root;

public:

  Signal2<void, Element*, Element*> BeforeAddElement;
  Signal1<void, Element*>           AfterAddElement;
  Signal1<void, Element*>           BeforeRemoveElement;
  Signal2<void, Element*, Element*> AfterRemoveElement;

  Model() {
    m_root = NULL;
  }

  virtual ~Model() {
    delete m_root;
  }

  Element* getRoot() const {
    return m_root;
  }

  void addElement(Element* element, Element* parent = NULL)
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

  void removeElement(Element* element)
  {
    BeforeRemoveElement(element);

    Element* parent = element->getParent();
    if (parent != NULL)
      parent->remove(element);
    else if (element == m_root)
      m_root = NULL;

    AfterRemoveElement(element, parent);
  }

};

//////////////////////////////////////////////////////////////////////
// a representation of an element of the model in a TreeNode

class TreeNode_Element : public TreeNode
{
  Element* m_element;

public:
  TreeNode_Element(Element* element)
    : TreeNode(element->getName())
  {
    m_element = element;
  }

  Element* getElement() {
    return m_element;
  }
};

//////////////////////////////////////////////////////////////////////
// a representation of the model in a TreeView

class TreeView_Model : public TreeView
{
  Model* m_model;
    
public:

  Signal1<void, Element*> ElementSelected;

  TreeView_Model(Model* model, Widget* parent)
    : TreeView(parent)
    , m_model(model)
  {
    m_model->BeforeAddElement.connect(&TreeView_Model::onBeforeAddElement, this);
    m_model->AfterAddElement.connect(&TreeView_Model::onAfterAddElement, this);
    m_model->BeforeRemoveElement.connect(&TreeView_Model::onBeforeRemoveElement, this);
    m_model->AfterRemoveElement.connect(&TreeView_Model::onAfterRemoveElement, this);
  }

  void selectElement(Element* element)
  {
    TreeView::iterator it;

    for (it = begin(); it != end(); ++it) {
      TreeNode_Element* node = dynamic_cast<TreeNode_Element*>(*it);
      assert(node != NULL);

      if (node->getElement() == element) {
	setSelectedNode(node);
	break;
      }
    }
  }

protected:

  virtual void onAfterSelect(TreeViewEvent& ev)
  {
    TreeView::onAfterSelect(ev);
    if (!ev.isCanceled()) {
      // generate the ElementSelected signal...

      TreeNode* node = ev.getTreeNode();
      if (node != NULL) {
	TreeNode_Element* elemNode = dynamic_cast<TreeNode_Element*>(node);
	assert(elemNode != NULL);

	ElementSelected(elemNode->getElement());
      }
      else
	ElementSelected(NULL);
    }
  }

private:

  void onBeforeAddElement(Element* element, Element* parent)
  {
  }

  void onAfterAddElement(Element* element)
  {
    TreeNode_Element* elementNode = new TreeNode_Element(element);
    TreeNode_Element* parentNode = findNodeByElement(element->getParent());

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

  void onBeforeRemoveElement(Element* element)
  {
    TreeNode_Element* elementNode = findNodeByElement(element);
    removeNode(elementNode);
    delete elementNode;

    invalidate(false);
  }

  void onAfterRemoveElement(Element* element, Element* parent)
  {
  }

  TreeNode_Element* findNodeByElement(Element* element)
  {
    TreeView::iterator it;

    // iterate all the TreeView to find the TreeNode that has the specified "element"
    for (it = begin(); it != end(); ++it) {
      TreeNode_Element* node = dynamic_cast<TreeNode_Element*>(*it);
      assert(node != NULL);

      if (node->getElement() == element)
	return node;
    }

    return NULL;
  }
  
};

//////////////////////////////////////////////////////////////////////
// a representation of an element as widgets

class TextEdit_Element : public TextEdit
{
  Element* m_element;
  bool m_selected;

public:

  Signal1<void, Element*> ElementSelected;

  TextEdit_Element(Element* element, Widget* parent)
    : TextEdit(element->getName(), parent,
	       TextEdit::Styles::TextArea +
	       TextEdit::Styles::AutoVerticalScroll)
  {
    m_element = element;
    m_selected = false;

    Change.connect(&TextEdit_Element::onChange, this);
  }

  Element* getElement() {
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

  virtual void onGotFocus(Event& ev)
  {
    TextEdit::onGotFocus(ev);
    ElementSelected(m_element);
  }

  virtual void onLostFocus(Event& ev)
  {
    TextEdit::onLostFocus(ev);
  }

  void onChange(Event& ev)
  {
    // when the user writes text we have to relayout the parent
    getParent()->layout();
  }

};

//////////////////////////////////////////////////////////////////////

template<class Map>
typename Map::mapped_type find_element(const Map& map,
				       const typename Map::key_type& key)
{
  typename Map::const_iterator it = map.find(key);
  if (it != map.end())
    return it->second;
  else
    return typename Map::mapped_type(0); // NULL
}

class Panel_Model : public Panel
{
  Model* m_model;
  std::map<Element*, Bix*> m_mapElemBix;

public:

  Signal1<void, Element*> ElementSelected;

  Panel_Model(Model* model, Widget* parent)
    : Panel(parent, Panel::Styles::Default +
		    Widget::Styles::ClientEdge)
    , m_model(model)
  {
    m_model->BeforeAddElement.connect(&Panel_Model::onBeforeAddElement, this);
    m_model->AfterAddElement.connect(&Panel_Model::onAfterAddElement, this);
    m_model->BeforeRemoveElement.connect(&Panel_Model::onBeforeRemoveElement, this);
    m_model->AfterRemoveElement.connect(&Panel_Model::onAfterRemoveElement, this);
  }

  virtual ~Panel_Model()
  {
    Container children = getChildren();
    Container::iterator it;
    for (it = children.begin(); it != children.end(); ++it)
      delete *it;
  }

  void selectElement(Element* element)
  {
    Container children = getChildren();
    Container::iterator it;
    for (it = children.begin(); it != children.end(); ++it) {
      TextEdit_Element* w = dynamic_cast<TextEdit_Element*>(*it);
      assert(w != NULL);

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

  int convertElementTypeToBixFlags(Element* element)
  {
    switch (element->getType()) {
      case Element::Row:    return BixRow; break;
      case Element::Column: return BixCol; break;
      case Element::Matrix: return BixMat; break;
      default:
	return 0;
    }
  }

  void onBeforeAddElement(Element* element, Element* parent)
  {
    int flags = convertElementTypeToBixFlags(element);

    // put "element" as the root
    if (parent == NULL) {
      assert(getLayout() == NULL);

      if (flags == 0) {
	// avoid
      }
      else {
	Bix* bix = new Bix(flags, element->getColumns());
	bix->setChildSpacing(4);

	m_mapElemBix[element] = bix;
	setLayout(bix);
      }
    }
    // insert a child in "parent" element
    else {
      Bix* parentBix = find_element(m_mapElemBix, parent);
      assert(parentBix);
      
      if (flags == 0) {
	TextEdit_Element* elementWidget = new TextEdit_Element(element, this);
	elementWidget->ElementSelected.connect(&Panel_Model::onElementSelected, this);
	parentBix->add(elementWidget);
      }
      else {
	m_mapElemBix[element] = parentBix->add(flags, element->getColumns());
      }

      layout();
    }
  }

  void onAfterAddElement(Element* element)
  {
  }

  void onBeforeRemoveElement(Element* element)
  {
    if (element->getType() == Element::Widget) {
      Container children = getChildren();
      Container::iterator it;
      for (it = children.begin(); it != children.end(); ++it) {
	TextEdit_Element* w = dynamic_cast<TextEdit_Element*>(*it);
	assert(w != NULL);

	if (w->getElement() == element) {
	  Element* parentElement = element->getParent();
	  Bix* parentBix = find_element(m_mapElemBix, parentElement);
	  assert(parentBix);

	  parentBix->remove(w);
	  w->getParent()->removeChild(w);
	  delete w;
	  break;
	}
      }
    }
    else {
      Container children = getChildren();
      Container::iterator it;
      for (it = children.begin(); it != children.end(); ++it) {
	TextEdit_Element* w = dynamic_cast<TextEdit_Element*>(*it);
	assert(w != NULL);

	if (w->getElement()->isAncestor(element)) {
	  w->getParent()->removeChild(w);
	  delete w;
	}
      }

      if (element->getParent() != NULL) {
	Bix* elementBix = find_element(m_mapElemBix, element);
	Bix* parentBix = find_element(m_mapElemBix, element->getParent());
	if (parentBix)
	  parentBix->remove(find_element(m_mapElemBix, element));

	m_mapElemBix.erase(element);
	delete elementBix;
      }
      else {
	setLayout(NULL);

	m_mapElemBix.clear();
      }
    }

    layout();
  }
  
  void onAfterRemoveElement(Element* element, Element* parent)
  {
  }

  void onElementSelected(Element* element)
  {
    selectElement(element);
    ElementSelected(element);
  }

};

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame,
		  public CommandsClient
{
  Model m_model;
  Element* m_selectedElement;
  TreeView_Model m_treeView;
  Panel_Model m_widgetsView;
  ToolBar m_toolBar;
  ImageList m_imageList;
  int m_widgetCounter;
  bool m_disableReselect;

public:

  MainFrame()
    : Frame(L"Bixes")
    , m_model()
    , m_selectedElement(NULL)
    , m_treeView(&m_model, this)
    , m_widgetsView(&m_model, this)
    , m_toolBar(L"Standard", this, ToolSet::Styles::Default +
				   ToolSet::Styles::Flat)
    , m_imageList(ResourceId(IDB_TOOLBAR), 16, Color(192, 192, 192))
    , m_widgetCounter(0)
    , m_disableReselect(false)
  {
    // commands
    addCommand(new SignalCommand(IDM_ADD_COLUMN, Bind(&MainFrame::onAddColumn, this)));
    addCommand(new SignalCommand(IDM_ADD_ROW, Bind(&MainFrame::onAddRow, this)));
    addCommand(new SignalCommand(IDM_ADD_MATRIX, Bind(&MainFrame::onAddMatrix, this)));
    addCommand(new SignalCommand(IDM_ADD_WIDGET, Bind(&MainFrame::onAddWidget, this)));
    addCommand(new SignalCommand(IDM_REMOVE, Bind(&MainFrame::onRemove, this)));
    addCommand(new SignalCommand(IDM_PROPERTIES));

    // layout
    setLayout(Bix::parse(L"X[%,f%]", &m_treeView, &m_widgetsView));
    m_treeView.setPreferredSize(Size(256, 256));

    // signals
    m_treeView.ElementSelected.connect(&MainFrame::onElementSelectedFromTreeView, this);
    m_widgetsView.ElementSelected.connect(&MainFrame::onElementSelectedFromWidgets, this);

    // setup the tool bar
    m_toolBar.setImageList(m_imageList);
    m_toolBar.addButton(new ToolButton(IDM_ADD_COLUMN, 0));
    m_toolBar.addButton(new ToolButton(IDM_ADD_ROW, 1));
    m_toolBar.addButton(new ToolButton(IDM_ADD_MATRIX, 2));
    m_toolBar.addButton(new ToolButton(IDM_ADD_WIDGET, 3));
    m_toolBar.addSeparator();
    m_toolBar.addButton(new ToolButton(IDM_REMOVE, 4));
    m_toolBar.addSeparator();
    m_toolBar.addButton(new ToolButton(IDM_PROPERTIES, 5));

    // setup the defaults dock areas
    defaultDockAreas();

    // put the tool bar in the top dock area
    m_toolBar.dockIn(getDockArea(Side::Top));
  }

protected:

  void onAddColumn()
  {
    addElement(Element::Column);
    updateToolBarButtons();
  }

  void onAddRow()
  {
    addElement(Element::Row);
    updateToolBarButtons();
  }

  void onAddMatrix()
  {
    int columns;
    if (askIntValue(columns))
      addElement(Element::Matrix, columns);
    updateToolBarButtons();
  }

  void onAddWidget()
  {
    addElement(Element::Widget);
    updateToolBarButtons();
  }

  void onRemove()
  {
    removeElement();
    updateToolBarButtons();
  }

  void onElementSelectedFromTreeView(Element* element)
  {
    if (!m_disableReselect) {
      m_selectedElement = element;

      m_disableReselect = true;
      m_widgetsView.selectElement(element);
      m_disableReselect = false;

      updateToolBarButtons();
    }
  }

  void onElementSelectedFromWidgets(Element* element)
  {
    if (!m_disableReselect) {
      m_selectedElement = element;

      m_disableReselect = true;
      m_treeView.selectElement(element);
      m_disableReselect = false;

      updateToolBarButtons();
    }
  }

private:

  // adds the specified type of element has a child of the selected element
  void addElement(Element::Type elementType, int columns = 0)
  {
    if ((m_selectedElement == NULL && m_model.getRoot() == NULL) ||
	(m_selectedElement != NULL && m_selectedElement->acceptChildren())) {
      String name = L"Unknown";

      switch (elementType) {
	case Element::Widget: name = L"Widget " + convert_to<String>(++m_widgetCounter); break;
	case Element::Row:    name = L"BixRow (a row vector)"; break;
	case Element::Column: name = L"BixCol (a column vector)"; break;
	case Element::Matrix: name = L"BixMat (a matrix)"; break;
      }

      m_model.addElement(new Element(name, elementType, columns),
			 m_selectedElement);

      if (m_selectedElement == NULL)
	m_treeView.selectElement(m_model.getRoot());
    }
  }

  // remove the selected element
  void removeElement()
  {
    if (m_selectedElement != NULL) {
      Element* e = m_selectedElement;
      m_selectedElement = NULL;

      m_model.removeElement(e);
      delete e;
    }
  }

  bool askIntValue(int& retValue)
  {
    Dialog dlg(L"Insert a value", this);
    Label label(L"Number of columns of the matrix:", &dlg);
    TextEdit value(L"3", &dlg);
    Button ok(L"&OK", IDOK, &dlg);
    Button cancel(L"&Cancel", IDCANCEL, &dlg);

    ok.setDefault(true);

    dlg.setLayout(Bix::parse(L"Y[X[%,f%],X[fX[],eX[%,%]]]",
			     &label, &value, &ok, &cancel));

    value.setPreferredSize(Size(32, value.getPreferredSize().h));
    dlg.setSize(dlg.getPreferredSize());
    dlg.center();

    while (true) {
      value.requestFocus();
      value.selectAll();
      if (!dlg.doModal())
	return false;

      retValue = convert_to<int>(value.getText());
      if (retValue < 1)
	MsgBox::show(this, L"Error",
		     L"You must to insert a value greater than 1",
		     MsgBox::Type::Ok,
		     MsgBox::Icon::Error);
      else
	return true;
    }
  }

  void updateToolBarButtons()
  {
    ToolButtonState canAdd =
      ((m_selectedElement == NULL && m_model.getRoot() == NULL) ||
       (m_selectedElement != NULL && m_selectedElement->acceptChildren())) ?
      ToolButtonState::Enabled:
      ToolButtonState::None;

    ToolButtonState canRemove =
      (m_selectedElement != NULL) ?
      ToolButtonState::Enabled:
      ToolButtonState::None;

    ToolButtonState canShowProperties =
      (m_selectedElement != NULL && m_selectedElement->getType() == Element::Matrix) ?
      ToolButtonState::Enabled:
      ToolButtonState::None;

    m_toolBar.getButtonById(IDM_ADD_COLUMN)->setState(canAdd);
    m_toolBar.getButtonById(IDM_ADD_ROW)->setState(canAdd);
    m_toolBar.getButtonById(IDM_ADD_MATRIX)->setState(canAdd);
    m_toolBar.getButtonById(IDM_ADD_WIDGET)->setState(canAdd);
    m_toolBar.getButtonById(IDM_REMOVE)->setState(canRemove);
    m_toolBar.getButtonById(IDM_PROPERTIES)->setState(canShowProperties);
  }

};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  try {
    Application app;
    MainFrame frame;
    frame.setVisible(true);
    app.run();
  }
  catch (Exception& e) {
    e.show();
  }
  return 0;
}
