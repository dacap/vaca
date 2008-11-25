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
#include <cmath>

using namespace Vaca;

class GridValue;
class GridColumn;
class GridRow;
class GridTable;
class GridSet;

//////////////////////////////////////////////////////////////////////
// GridValue

class GridValue
{
public:
  enum Type {
    Empty,
    Bool,
    Int,
    UInt,
    Long,
    ULong,
    Double,
    String,
  };
private:
  Type m_type;
  void* m_data;
public:
  GridValue();
  GridValue(const GridValue& value);
  explicit GridValue(bool value);
  explicit GridValue(int value);
  explicit GridValue(unsigned int value);
  explicit GridValue(long value);
  explicit GridValue(unsigned long value);
  explicit GridValue(double value);
  explicit GridValue(const Vaca::String& value);
  ~GridValue();

  void clear();
  void assign(const GridValue& value);
  void assign(bool value);
  void assign(int value);
  void assign(unsigned int value);
  void assign(long value);
  void assign(unsigned long value);
  void assign(double value);
  void assign(const Vaca::String& value);

  bool isEmpty() const;
  bool getBool() const;
  int getInt() const;
  unsigned int getUInt() const;
  long getLong() const;
  unsigned long getULong() const;
  double getDouble() const;
  Vaca::String getString() const;

private:
  template<typename T>
  void assign(Type type, const T& value)
  {
    clear();
    m_type = type;
    m_data = (void*)new T(value);
  }
};

GridValue::GridValue()
  : m_type(Empty)
  , m_data(NULL)
{
}

GridValue::GridValue(const GridValue& value)
  : m_type(Empty)
  , m_data(NULL)
{
  assign(value);
}

GridValue::GridValue(bool value)
  : m_type(Empty)
  , m_data(NULL)
{
  assign(value);
}

GridValue::GridValue(int value)
  : m_type(Empty)
  , m_data(NULL)
{
  assign(value);
}

GridValue::GridValue(unsigned int value)
  : m_type(Empty)
  , m_data(NULL)
{
  assign(value);
}

GridValue::GridValue(long value)
  : m_type(Empty)
  , m_data(NULL)
{
  assign(value);
}

GridValue::GridValue(unsigned long value)
  : m_type(Empty)
  , m_data(NULL)
{
  assign(value);
}

GridValue::GridValue(double value)
  : m_type(Empty)
  , m_data(NULL)
{
  assign(value);
}

GridValue::GridValue(const Vaca::String& value)
  : m_type(Empty)
  , m_data(NULL)
{
  assign(value);
}

GridValue::~GridValue()
{
  clear();
}

void GridValue::clear()
{
  switch (m_type) {
    case Empty:
      // do nothing
      break;
    case Bool:
      delete (bool*)m_data;
      break;
    case Int:
      delete (int*)m_data;
      break;
    case UInt:
      delete (unsigned int*)m_data;
      break;
    case Long:
      delete (long*)m_data;
      break;
    case ULong:
      delete (unsigned long*)m_data;
      break;
    case Double:
      delete (double*)m_data;
      break;
    case String:
      delete (Vaca::String*)m_data;
      break;
  }
  m_type = Empty;
  m_data = NULL;
}

void GridValue::assign(const GridValue& value)
{
  m_type = value.m_type;

  switch (m_type) {
    case Empty: m_data = NULL; break;
    case Bool: assign(*(bool*)value.m_data); break;
    case Int: assign(*(int*)value.m_data); break;
    case UInt: assign(*(unsigned int*)value.m_data); break;
    case Long: assign(*(long*)value.m_data); break;
    case ULong: assign(*(unsigned long*)value.m_data); break;
    case Double: assign(*(double*)value.m_data); break;
    case String: assign(*(Vaca::String*)value.m_data); break;
  }
}

void GridValue::assign(bool value)
{
  assign<bool>(Bool, value);
}

void GridValue::assign(int value)
{
  assign<int>(Int, value);
}

void GridValue::assign(unsigned int value)
{
  assign<unsigned int>(UInt, value);
}

void GridValue::assign(long value)
{
  assign<long>(Long, value);
}

void GridValue::assign(unsigned long value)
{
  assign<unsigned long>(ULong, value);
}

void GridValue::assign(double value)
{
  assign<double>(Double, value);
}

void GridValue::assign(const Vaca::String& value)
{
  assign<Vaca::String>(String, value);
}

bool GridValue::isEmpty() const
{
  return m_type == Empty;
}

bool GridValue::getBool() const
{
  switch (m_type) {
    case Empty:
      return false;
    case Bool: {
      bool value = *(bool*)m_data;
      return value;
    }
    case Int: {
      int value = *(int*)m_data;
      return value != 0;
    }
    case UInt: {
      unsigned int value = *(unsigned int*)m_data;
      return value != 0;
    }
    case Long: {
      long value = *(long*)m_data;
      return value != 0;
    }
    case ULong: {
      unsigned long value = *(unsigned long*)m_data;
      return value != 0;
    }
    case Double: {
      double value = *(double*)m_data;
      return std::floor(value) != 0.0;
    }
    case String: {
      const Vaca::String& value(*(Vaca::String*)m_data);
      return
	!value.empty() &&
	value != _T("0") &&
	value != _T("false");
    }
  }
  return false;
}

int GridValue::getInt() const
{
  switch (m_type) {
    case Empty:
      return 0;
    case Bool: {
      bool value = *(bool*)m_data;
      return value ? 1: 0;
    }
    case Int: {
      int value = *(int*)m_data;
      return value;
    }
    case UInt: {
      unsigned int value = *(unsigned int*)m_data;
      return (int)value;
    }
    case Long: {
      long value = *(long*)m_data;
      return (int)value;
    }
    case ULong: {
      unsigned long value = *(unsigned long*)m_data;
      return (int)value;
    }
    case Double: {
      double value = *(double*)m_data;
      return (int)value;
    }
    case String: {
      Vaca::String& value(*(Vaca::String*)m_data);
      return value.parseInt(10);
    }
  }
  return 0;
}

unsigned int GridValue::getUInt() const
{
  switch (m_type) {
    case Empty:
      return 0;
    case Bool: {
      bool value = *(bool*)m_data;
      return value ? 1: 0;
    }
    case Int: {
      int value = *(int*)m_data;
      return (unsigned int)value;
    }
    case UInt: {
      unsigned int value = *(unsigned int*)m_data;
      return value;
    }
    case Long: {
      long value = *(long*)m_data;
      return (unsigned int)value;
    }
    case ULong: {
      unsigned long value = *(unsigned long*)m_data;
      return (unsigned int)value;
    }
    case Double: {
      double value = *(double*)m_data;
      return (unsigned int)value;
    }
    case String: {
      Vaca::String& value(*(Vaca::String*)m_data);
      return (unsigned long)value.parseInt(10);
    }
  }
  return 0;
}

long GridValue::getLong() const
{
  switch (m_type) {
    case Empty:
      return 0;
    case Bool: {
      bool value = *(bool*)m_data;
      return value ? 1: 0;
    }
    case Int: {
      int value = *(int*)m_data;
      return (long)value;
    }
    case UInt: {
      unsigned int value = *(unsigned int*)m_data;
      return (long)value;
    }
    case Long: {
      long value = *(long*)m_data;
      return value;
    }
    case ULong: {
      unsigned long value = *(unsigned long*)m_data;
      return (long)value;
    }
    case Double: {
      double value = *(double*)m_data;
      return (long)value;
    }
    case String: {
      Vaca::String& value(*(Vaca::String*)m_data);
      return (long)value.parseInt(10);
    }
  }
  return 0;
}

unsigned long GridValue::getULong() const
{
  switch (m_type) {
    case Empty:
      return 0;
    case Bool: {
      bool value = *(bool*)m_data;
      return value ? 1: 0;
    }
    case Int: {
      int value = *(int*)m_data;
      return (unsigned long)value;
    }
    case UInt: {
      unsigned int value = *(unsigned int*)m_data;
      return (unsigned long)value;
    }
    case Long: {
      long value = *(long*)m_data;
      return (unsigned long)value;
    }
    case ULong: {
      unsigned long value = *(unsigned long*)m_data;
      return value;
    }
    case Double: {
      double value = *(double*)m_data;
      return (unsigned long)value;
    }
    case String: {
      Vaca::String& value(*(Vaca::String*)m_data);
      return (unsigned long)value.parseInt(10);
    }
  }
  return 0;
}

double GridValue::getDouble() const
{
  switch (m_type) {
    case Empty:
      return 0.0;
    case Bool: {
      bool value = *(bool*)m_data;
      return value ? 1.0: 0.0;
    }
    case Int: {
      int value = *(int*)m_data;
      return (double)value;
    }
    case UInt: {
      unsigned int value = *(unsigned int*)m_data;
      return (double)value;
    }
    case Long: {
      long value = *(long*)m_data;
      return (double)value;
    }
    case ULong: {
      unsigned long value = *(unsigned long*)m_data;
      return (double)value;
    }
    case Double: {
      double value = *(double*)m_data;
      return value;
    }
    case String: {
      Vaca::String& value(*(Vaca::String*)m_data);
      return value.parseDouble();
    }
  }
  return 0;
}

Vaca::String GridValue::getString() const
{
  switch (m_type) {
    case Empty:
      return Vaca::String();
    case Bool: {
      bool value = *(bool*)m_data;
      return value ? _T("1"): _T("0");
    }
    case Int: {
      int value = *(int*)m_data;
      return String::format("%d", value);
    }
    case UInt: {
      unsigned int value = *(unsigned int*)m_data;
      return String::format("%u", value);
    }
    case Long: {
      long value = *(long*)m_data;
      return String::format("%l", value);
    }
    case ULong: {
      unsigned long value = *(unsigned long*)m_data;
      return String::format("%ul", value);
    }
    case Double: {
      double value = *(double*)m_data;
      return String::format("%.16g", value);
    }
    case String: {
      Vaca::String& value(*(Vaca::String*)m_data);
      return value;
    }
  }
  return Vaca::String();
}

//////////////////////////////////////////////////////////////////////
// GridColumnStyle

// struct _GridColumnStyle
// {
//   enum enumeration {
//     Null,
//   };
//   static const enumeration default_value = Solid;
// };

// typedef Enum<_GridColumnStyle> GridColumnStyle;

//////////////////////////////////////////////////////////////////////
// GridColumn

class GridColumn
{
  friend class GridTable;
  friend class GridView;

  String m_name;
  String m_header;
  GridTable* m_table;
  GridValue* m_defaultValue;
  bool m_allowNull;

  TextAlign m_headerAlign;
  TextAlign m_dataAlign;
  // int m_headerHeight;
  int m_columnWidth;
  int m_preferredColumnWidth;
  
public:

  GridColumn(const String& name, GridValue* defaultValue);
  virtual ~GridColumn();

  String getName() const;
  void setName(const String& name);

  String getHeader() const;
  void setHeader(const String& header);

  bool getAllowNull() const;
  void setAllowNull(bool state);
  
  GridTable* getTable();
  GridValue* getDefaultValue();

  GridRow* operator[](int index);

private:
  
  void setTable(GridTable* table);

};

//////////////////////////////////////////////////////////////////////
// GridRow

class GridRow
{
  friend class GridTable;

  GridTable* m_table;
  std::vector<GridValue*> m_values;
//   int m_rowHeight;
  
private:

  GridRow(GridTable*table);

public:

  virtual ~GridRow();

  GridTable* getTable();

  GridValue* getValue(int columntIndex);
  void setValue(int columntIndex, GridValue *value);

//   GridColumn* operator[](int index);
//   GridColumn* operator[](const String &columnName);

};

//////////////////////////////////////////////////////////////////////
// GridTable

class GridTable
{
  String m_name;
  std::vector<GridColumn*> m_columns;
  std::vector<GridRow*> m_rows;
  
public:

  GridTable()
  {
    VACA_TRACE("GridTable()\n");
  }

  GridTable(const String &name)
  {
    VACA_TRACE("GridTable()\n");
    m_name = name;
  }

  virtual ~GridTable()
  {
    for (std::vector<GridColumn*>::iterator
	   it = m_columns.begin(); it != m_columns.end(); ++it) {
      delete *it;
    }
    VACA_TRACE("~GridTable()\n");
  }

  GridRow* createRow()
  {
    return new GridRow(this);
  }

  void addColumn(GridColumn* column)
  {
    // a column without owner
    assert(column->getTable() == NULL);

    column->setTable(this);
    m_columns.push_back(column);
  }

  void addRow(GridRow* row)
  {
    // a row created by this table
    assert(row->getTable() == this);

    m_rows.push_back(row);
  }

  String getName() const
  {
    return m_name;
  }

  void setName(const String &name)
  {
    m_name = name;
  }

  int getColumnCount() const
  {
    return m_columns.size();
  }

  int getRowCount() const
  {
    return m_rows.size();
  }

  GridColumn* getColumn(int index)
  {
    assert(index >= 0 && index < getColumnCount());
    return m_columns[index];
  }

  GridRow* getRow(int index)
  {
    assert(index >= 0 && index < getRowCount());
    return m_rows[index];
  }

  GridColumn* operator[](int index)
  {
    return getColumn(index);
  }

  GridColumn* operator[](const String &columnName)
  {
    for (std::vector<GridColumn*>::iterator
	   it = m_columns.begin(); it != m_columns.end(); ++it) {
      if ((*it)->getName() == columnName)
	return *it;
    }
    assert(false);
  }
  
};

//////////////////////////////////////////////////////////////////////
// GridSet

class GridSet
{
  std::vector<GridTable*> m_tables;

public:

  GridSet();
  virtual ~GridSet();

  void addTable(GridTable* table);
  std::vector<GridTable*> getTables();
  int getTableCount();
  GridTable* operator[](int index);
  GridTable* operator[](const String &tableName);
  
};

GridSet::GridSet()
{
  VACA_TRACE("GridSet()\n");
}

GridSet::~GridSet()
{
  for (std::vector<GridTable*>::iterator
	 it = m_tables.begin(); it != m_tables.end(); ++it) {
    delete *it;
  }
  VACA_TRACE("~GridSet()\n");
}

void GridSet::addTable(GridTable* table)
{
  m_tables.push_back(table);
}

std::vector<GridTable*> GridSet::getTables()
{
  return m_tables;
}

int GridSet::getTableCount()
{
  return m_tables.size();
}

GridTable* GridSet::operator[](int index)
{
  assert(index >= 0 && index < getTableCount());
  return m_tables[index];
}

GridTable* GridSet::operator[](const String &tableName)
{
  for (std::vector<GridTable*>::iterator
	 it = m_tables.begin(); it != m_tables.end(); ++it) {
    if ((*it)->getName() == tableName)
      return *it;
  }
  assert(false);
  // throw std::out_of_range(String("Invalid table name"));
  return NULL;
}

//////////////////////////////////////////////////////////////////////
// GridView

class GridView : public Panel
{
  struct Row {
    GridRow* rowData;
    int rowHeight;
  };
  std::vector<GridColumn*> m_columns;
  std::vector<Row*> m_rows;
  int m_currentRow;
  int m_hotRow;
  int m_hotCol;
  struct {
    int column;
    Rect startBounds;
    Point startPoint;
  } m_resizing;

  // customization...

  // bool m_allowNavigation;
  // bool m_allowSorting;
  bool m_flatMode;
  // bool m_columnHeadersVisible;
  int m_headerHeight;
  bool m_headerResizingEnabled;
  bool m_whiteFill;
  bool m_crudColumn;
  int m_crudColumnWidth;

  GridSet* m_dataSet;
  GridTable* m_dataTable;

public:

  GridView(Widget* parent);
  virtual ~GridView();

  void setGridSet(GridSet* set);

  bool getFlatMode() const;
  void setFlatMode(bool state);

  bool getCrudColumn() const;
  void setCrudColumn(bool state);

  int getCrudColumnWidth() const;
  void setCrudColumnWidth(int width);

protected:

  virtual void onScroll(ScrollEvent& ev);
  virtual void onDoubleClick(MouseEvent &ev);
  virtual void onMouseDown(MouseEvent &ev);
  virtual void onMouseUp(MouseEvent &ev);
  virtual void onMouseMove(MouseEvent &ev);
  virtual void onMouseWheel(MouseEvent &ev);
  virtual void onMouseLeave();
  virtual void onKeyDown(KeyEvent &ev);
  virtual void onSetCursor(WidgetHitTest hitTest);
  virtual void onResize(const Size &sz);
  virtual void onPaint(Graphics &g);

private:

  void clearGrid();
  void setGridTable(GridTable* table);
  void addColumn(GridColumn* column);
  void addRow(GridRow* row);
  int getColumnCount();
  int getRowCount();
  int getColumnByPoint(const Point &pt, bool useRows);
  int getRowByPoint(const Point &pt);
  Rect getColumnBounds(int columnIndex);
  Rect getCrudColumnBounds();
  Rect getRowBounds(int rowIndex);

  void drawCell(Graphics &g, const String &text, const Rect &rc,
		Color borderColor, Color backgroundColor, Color textColor, bool hot = false);
  void drawArrow(Graphics &g, const Rect &rc, const Color& color);

  int getHotResizingBorder(const Point &pt);
  void updateHorizontalScrollBarVisibility();
  void updateVerticalScrollBarVisibility();
  void updateHotTracking(const Point &pt);
  
};

#define NULL_ROW_INDEX    (-1)
#define NULL_COLUMN_INDEX (-2)
#define CRUD_COLUMN_INDEX (-1)

GridView::GridView(Widget* parent)
  : Panel(parent, PanelStyle +
		  ScrollStyle +
		  ClientEdgeStyle +
		  FocusableStyle)
{
  setBgColor(Color::White);
  setFgColor(Color::Black);

  m_currentRow = NULL_ROW_INDEX;
  m_hotRow = NULL_ROW_INDEX;
  m_hotCol = NULL_COLUMN_INDEX;
  m_resizing.column = NULL_COLUMN_INDEX;

  // default configuration
  m_headerHeight = 0;
  m_headerResizingEnabled = true;
  m_whiteFill = false;
  m_crudColumn = true;
  m_crudColumnWidth = 32;

  ::ShowScrollBar(getHandle(), SB_BOTH, FALSE);
}

GridView::~GridView()
{
  clearGrid();
}

void GridView::setGridSet(GridSet* set)
{
  m_dataSet = set;
  if (set->getTableCount() > 0)
    setGridTable(set->getTables().front());
}

bool GridView::getFlatMode() const
{
  return m_flatMode;
}

void GridView::setFlatMode(bool state)
{
  m_flatMode = state;
}

bool GridView::getCrudColumn() const
{
  return m_crudColumn;
}

void GridView::setCrudColumn(bool state)
{
  m_crudColumn = state;
}

int GridView::getCrudColumnWidth() const
{
  return m_crudColumnWidth;
}

void GridView::setCrudColumnWidth(int width)
{
  m_crudColumnWidth = width;
}

void GridView::onScroll(ScrollEvent& ev)
{
  Panel::onScroll(ev);

  Orientation orien = ev.getOrientation();
  int pos = getScrollPos(orien);
  int oldPos = pos;

  switch (ev.getRequest()) {
    case ScrollRequest::LineBackward:
      pos -= 32;
      break;
    case ScrollRequest::LineForward: 
      pos += 32;
      break;
    case ScrollRequest::PageBackward:
    case ScrollRequest::PageForward:
    case ScrollRequest::BoxTracking:
      pos = ev.getPosition();
      break;
    default:
      break;
  }

  setScrollPos(orien, pos);
  pos = getScrollPos(orien);

  if (pos != oldPos) {
    ScrollWindowEx(getHandle(),
		   (orien == Orientation::Horizontal) ? oldPos - pos: 0,
		   (orien == Orientation::Vertical  ) ? oldPos - pos: 0,
		   NULL, NULL, NULL, NULL,
		   SW_ERASE | SW_INVALIDATE);
  }
}

void GridView::onDoubleClick(MouseEvent &ev)
{
  Panel::onDoubleClick(ev);

  // double click to fit header width
  int column = getHotResizingBorder(ev.getPoint());
  if (column >= 0) {
    ScreenGraphics g;
    g.setFont(getFont());
    m_columns[column]->m_columnWidth =
      m_columns[column]->m_preferredColumnWidth;

    updateHorizontalScrollBarVisibility();
    invalidate(false);
  }
}

// when a mouse's button is pressed
void GridView::onMouseDown(MouseEvent &ev)
{
  Panel::onMouseDown(ev);

  if (!hasCapture()) {
    requestFocus();

    // resize a header?
    int column = getHotResizingBorder(ev.getPoint());
    if (column >= 0) {
      m_resizing.column = column;
      m_resizing.startBounds = getColumnBounds(column);
      m_resizing.startPoint = ev.getPoint() + getScrollPoint();

      captureMouse();
    }
    // does the user change current row selected?
    else if (m_hotRow != NULL_ROW_INDEX && m_currentRow != m_hotRow) {
      m_currentRow = m_hotRow;
      invalidate(false);
    }
  }
}

// when a mouse's button is unpressed
void GridView::onMouseUp(MouseEvent &ev)
{
  Panel::onMouseUp(ev);

  if (hasCapture()) {
    if (m_resizing.column >= 0) {
      m_resizing.column = -1;

      // 	updateHorizontalScrollBarVisibility();
      // 	invalidate(false);
    }
    
    releaseMouse();
  }
}

// when the mouse is moved
void GridView::onMouseMove(MouseEvent &ev)
{
  Panel::onMouseMove(ev);

  if (hasCapture()) {
    // resizing column's header
    if (m_resizing.column >= 0) {
      Rect client = getAbsoluteClientBounds();
      Point cursor = System::getCursorPos();
      Point clientCursor = (cursor - client.getOrigin());

      clientCursor.x = max_value(0, clientCursor.x);
      clientCursor.y = max_value(0, clientCursor.y);

      // amount of scroll
      Point delta =
	clientCursor
	+ getScrollPoint()
	- m_resizing.startPoint;

      // change column size
      m_columns[m_resizing.column]->m_columnWidth =
	max_value(m_columns[m_resizing.column]->m_preferredColumnWidth,
		  m_resizing.startBounds.w + delta.x);

      // update scroll bars
      updateHorizontalScrollBarVisibility();

      // the mouse is out the right side of the client bounds
      if (cursor.x > client.x+client.w) {
	int dx = cursor.x - (client.x+client.w);

	System::setCursorPos(Point(client.x+client.w-dx,
				   cursor.y));

	setScrollPos(Orientation::Horizontal, getScrollPos(Orientation::Horizontal)+dx);
      }

      invalidate(false);
    }
  }
  else {
    updateHotTracking(ev.getPoint());
  }
}

void GridView::onMouseWheel(MouseEvent &ev)
{
  Panel::onMouseWheel(ev);
  //     int columnIndex = getColumnByPoint(ev.getPoint());

  //     SCROLLINFO si;
  //     si.cbSize = sizeof(si);
  //     si.fMask  = SIF_ALL;
  //     GetScrollInfo(getHandle(), SB_HORZ, &si);

  //     int oldPos = si.nPos;
  //     int newPos = si.nPos - ev.getDelta() * 32;

  //     si.fMask = SIF_POS;
  //     si.nPos = VACA_CLAMP(static_cast<int>(newPos),
  //		       static_cast<int>(si.nMin),
  // 		       static_cast<int>(si.nMax - VACA_MAX(si.nPage, 0)));
  //     SetScrollInfo(getHandle(), SB_HORZ, &si, TRUE);
  //     GetScrollInfo(getHandle(), SB_HORZ, &si);

  // Vertical
  //     if (getScrollInfo(Vertical).maxPos >= 0) {
  //       int oldPos = getScrollPos(Vertical);
  //       setScrollPos(Horizontal, oldPos - ev.getDelta() * 32);
  //       int newPos = getScrollPos(Vertical);

  //       if (oldPos != newPos) {
  // 	ScrollWindowEx(getHandle(), 0, oldPos - newPos,
  // 		       NULL, NULL, NULL, NULL,
  // 		       SW_ERASE | SW_INVALIDATE);

  // 	updateHotTracking(ev.getPoint());
  //       }
  //     }
  //     // Horizontal
  //     else 
  {
    int oldPos = getScrollPos(Orientation::Horizontal);
    setScrollPos(Orientation::Horizontal, oldPos - ev.getDelta() * 32);
    int newPos = getScrollPos(Orientation::Horizontal);

    if (oldPos != newPos) {
      ScrollWindowEx(getHandle(), oldPos - newPos, 0,
		     NULL, NULL, NULL, NULL,
		     SW_ERASE | SW_INVALIDATE);

      updateHotTracking(ev.getPoint());
    }
  }
}

void GridView::onMouseLeave()
{
  Panel::onMouseLeave();

  if (m_hotCol != NULL_COLUMN_INDEX) {
    invalidate(getColumnBounds(m_hotCol), false);
    m_hotCol = NULL_COLUMN_INDEX;
  }
}

void GridView::onKeyDown(KeyEvent &ev)
{
  Panel::onKeyDown(ev);

  switch (ev.getKeyCode()) {

    case Keys::Down:
      if (m_currentRow == NULL_COLUMN_INDEX) {
	if (!m_rows.empty()) {
	  m_currentRow = 0;
	  invalidate(false);
	  update();
	}
      }
      else if (m_currentRow < static_cast<int>(m_rows.size())-1) {
	++m_currentRow;
      }
      break;

    case Keys::PageDown:
      if (m_currentRow == NULL_COLUMN_INDEX) {
	if (!m_rows.empty()) {
	  m_currentRow = 0;
	  invalidate(false);
	  update();
	}
      }
      else {
	m_currentRow = (m_currentRow+1) % m_rows.size();
      }
      break;

    case Keys::PageUp:
      break;

  }
}

void GridView::onSetCursor(WidgetHitTest hitTest)
{
  if (hitTest == WidgetHitTest::Client) {
    Point pt = System::getCursorPos() - getAbsoluteClientBounds().getOrigin();
    int colIndex = getHotResizingBorder(pt);
    if (colIndex != NULL_ROW_INDEX) {
      setCursor(Cursor(SysCursor::SizeE));
      return;
    }
  }
  setCursor(Cursor(SysCursor::Arrow));
}
  
void GridView::onResize(const Size &sz)
{
  Panel::onResize(sz);
  //       invalidate(true);
  updateHorizontalScrollBarVisibility();
  //       updateVerticalScrollBarVisibility();
}

void GridView::onPaint(Graphics &g)
{
  Color headerFaceColor = Color(212, 208, 200);//System::getColor(COLOR_3DFACE);
  Color headerBorderColor = Color(128, 128, 128);
  Color hotHeaderFaceColor = Color(182, 189, 210);
  Color hotHeaderBorderColor = Color(10, 36, 106);
  Rect clientBounds = getClientBounds();
  Point pt = clientBounds.getOrigin();
  int startColIndex = m_crudColumn ? CRUD_COLUMN_INDEX: 0;
  int colCount = getColumnCount();
  int rowCount = getRowCount();
  Brush whiteBrush(Color::White);

  if (colCount > 0) {
    // draw column headers
    for (int j=startColIndex; j<colCount; ++j) {
      if (j != m_hotCol)
	drawCell(g,
		 j != CRUD_COLUMN_INDEX ?
		    m_columns[j]->getHeader():
		    "",
		 getColumnBounds(j),
		 headerBorderColor,
		 headerFaceColor,
		 getFgColor());
    }

    // draw hot column header
    if (m_hotCol != NULL_COLUMN_INDEX) {
      drawCell(g,
	       m_hotCol != CRUD_COLUMN_INDEX ?
	          m_columns[m_hotCol]->getHeader():
		  "",
	       getColumnBounds(m_hotCol),
	       hotHeaderBorderColor,
	       hotHeaderFaceColor,
	       getFgColor(),
	       true);		// hot
    }

    // fill right side of the header
    Rect rightSide = getColumnBounds(colCount-1);

    int width = clientBounds.w - rightSide.x;
    rightSide.x += rightSide.w;
    rightSide.w = width;

    if (m_whiteFill) {
      Brush brush(getBgColor());
      g.fillRect(brush, rightSide);
    }
    else {
      // rightSide.x -= 1;
      drawCell(g, "", rightSide,
	       headerBorderColor,
	       headerFaceColor,
	       getFgColor());
    }

    // draw rows
    if (rowCount > 0) {
      for (int i=0; i<rowCount; ++i) {
        // draw each cell of each columns
	for (int j=startColIndex; j<colCount; ++j) {
	  Rect rc = getColumnBounds(j);
	  rc.y += (i+1)*rc.h;

	  String text;
	  Color borderColor;
	  Color backgroundColor;
	  Color textColor;

	  if (j != CRUD_COLUMN_INDEX) {
	    GridValue* value = m_rows[i]->rowData->getValue(j);
	    if (value != NULL)
	      text = value->getString();
	    else
	      text = "NULL";	// TODO customize "NULL" string

	    // current row
	    if (i == m_currentRow) {
	      borderColor = Color(192, 192, 192);
	      backgroundColor = Color(0, 0, 128);
	      textColor = Color::White;
	    }
	    // normal row
	    else {
	      borderColor = Color(192, 192, 192);
	      backgroundColor = (m_hotRow == i ? Color::Yellow:
						 Color::White);
	      textColor = getFgColor();
	    }
	  }
	  // crud column
	  else {
	    text = "";
	    borderColor = headerBorderColor;
	    backgroundColor = headerFaceColor;
	    textColor = getFgColor();
	  }
	    
	  drawCell(g, text, rc, borderColor, backgroundColor, textColor);

	  // CRUD icon
	  if (j == CRUD_COLUMN_INDEX) {
	    // current row
	    if (i == m_currentRow) {
	      drawArrow(g, rc, Color::Black);
	    }
	  }
	}
      }

      // fill bottom side of the rows
      Rect bottomSide = getRowBounds(rowCount-1);
      bottomSide.y += m_rows[rowCount-1]->rowHeight;
      bottomSide.h = clientBounds.h - bottomSide.y;
      bottomSide.w++;

      g.fillRect(whiteBrush, bottomSide);
    }

    // fill the right side of the rows
    int height = clientBounds.h - rightSide.h;
    rightSide.y += rightSide.h;
    rightSide.h = clientBounds.h - rightSide.y;

    g.fillRect(whiteBrush, rightSide);
  }
}

void GridView::clearGrid()
{
  int colCount = getColumnCount();
  int rowCount = getRowCount();

  for (int i=0; i<rowCount; ++i)
    delete m_rows[i];

  for (int i=0; i<colCount; ++i)
    delete m_columns[i];

  m_rows.clear();
  m_columns.clear();
}

void GridView::setGridTable(GridTable* table)
{
  m_dataTable = table;
  int cols = m_dataTable->getColumnCount();
  int rows = m_dataTable->getRowCount();

  for (int j=0; j<cols; ++j)
    addColumn(m_dataTable->getColumn(j));

  for (int i=0; i<rows; ++i)
    addRow(m_dataTable->getRow(i));
}
  
void GridView::addColumn(GridColumn* column)
{
  ScreenGraphics g;
  g.setFont(getFont());
  Size headerSize = g.measureString(column->getHeader()) + Size(16, 2);

  // column->m_headerHeight = headerSize.h;
  column->m_columnWidth = headerSize.w;
  column->m_preferredColumnWidth = headerSize.w;

  this->m_headerHeight = max_value(m_headerHeight, headerSize.h);

  m_columns.push_back(column);
}

void GridView::addRow(GridRow* row)
{
  Row* r = new Row;
  r->rowData = row;
//   r->rowHeight = m_columns[0]->headerHeight;
  r->rowHeight = m_headerHeight;
  m_rows.push_back(r);
}
  
int GridView::getColumnCount()
{
  return m_columns.size();
}

int GridView::getRowCount()
{
  return m_rows.size();
}

int GridView::getColumnByPoint(const Point &pt, bool useRows)
{
  Rect bounds = getClientBounds();
  int startColIndex = m_crudColumn ? CRUD_COLUMN_INDEX: 0;
  int colCount = getColumnCount();

  for (int i=startColIndex; i<colCount; ++i) {
    Rect colBounds = getColumnBounds(i);

    if (useRows) {
      colBounds.y = bounds.y;
      colBounds.h = bounds.h;
    }

    if (colBounds.contains(pt))
      return i;
  }

  return NULL_COLUMN_INDEX;
}

int GridView::getRowByPoint(const Point &pt)
{
  Rect bounds = getClientBounds();
  int rowCount = getRowCount();

  for (int i=0; i<rowCount; ++i)
    if (getRowBounds(i).contains(pt))
      return i;

  return NULL_ROW_INDEX;
}

Rect GridView::getColumnBounds(int columnIndex)
{
  if (columnIndex == CRUD_COLUMN_INDEX)
    return getCrudColumnBounds();
  
  assert(columnIndex >= 0 && columnIndex < getColumnCount());
  Point pt = getClientBounds().getOrigin() - getScrollPoint();
  int i;

  if (m_crudColumn)
    pt.x += m_crudColumnWidth;

  for (i=0; i<columnIndex; ++i)
    pt.x += m_columns[i]->m_columnWidth;

  return Rect(pt, Size(m_columns[i]->m_columnWidth,
		       m_headerHeight));
// 		       m_columns[i]->headerHeight));
}

Rect GridView::getCrudColumnBounds()
{
  assert(m_crudColumn);
  Point pt = getClientBounds().getOrigin() - getScrollPoint();

  return Rect(pt, Size(m_crudColumnWidth, m_headerHeight));
}

Rect GridView::getRowBounds(int rowIndex)
{
  assert(rowIndex >= 0 && rowIndex < getRowCount());
  Point pt = getClientBounds().getOrigin() - getScrollPoint();
  int i;

//   pt.y += m_columns[0]->headerHeight;
  pt.y += m_headerHeight;

  for (i=0; i<rowIndex; ++i)
    pt.y += m_rows[i]->rowHeight;

  Rect rc;
  int startColIndex = m_crudColumn ? CRUD_COLUMN_INDEX: 0;
  int colCount = getColumnCount();
  if (colCount > 0)
    rc = getColumnBounds(startColIndex).createUnion(getColumnBounds(colCount-1));

  return Rect(rc.x, pt.y, rc.w, m_rows[rowIndex]->rowHeight);
}

void GridView::drawCell(Graphics &g, const String &text, const Rect &rc,
			Color borderColor, Color backgroundColor, Color textColor, bool hot)
{
  Pen borderPen(borderColor);
  Brush backgroundBrush(backgroundColor);

  if (hot) {
    g.drawRect(borderPen, Rect(rc.x-1, rc.y-1, rc.w+1, rc.h+1));
  }
  else {
    g.drawLine(borderPen, rc.x, rc.y+rc.h-1, rc.x+rc.w-1, rc.y+rc.h-1);
    g.drawLine(borderPen, rc.x+rc.w-1, rc.y+rc.h-1, rc.x+rc.w-1, rc.y-1);
  }

  g.fillRect(backgroundBrush, Rect(rc).inflate(-1, -1));

  if (!text.empty())
    g.drawString(text, textColor, rc, DT_CENTER | DT_VCENTER);
}

void GridView::drawArrow(Graphics &g, const Rect &rc, const Color& color)
{
  Brush brush(color);

  GraphicsPath path;
  path.moveTo(-4, -4);
  path.lineTo(+4, 0);
  path.lineTo(-4, +4);
  path.closeFigure();

  g.fillPath(path, brush, rc.getCenter());
}

int GridView::getHotResizingBorder(const Point &pt)
{
  if (m_headerResizingEnabled) {
    int count = getColumnCount();

    for (int i=count-1; i>=0; --i) {
      Rect colBounds = getColumnBounds(i);
      Rect border = Rect(colBounds.x+colBounds.w-3, colBounds.y, 5, colBounds.h);
      if (border.contains(pt))
	return i;
    }
  }
  return NULL_ROW_INDEX;
}

void GridView::updateHorizontalScrollBarVisibility()
{
  int colCount = getColumnCount();
  if (colCount > 0) {
    Rect clientBounds = getClientBounds().offset(-getScrollPoint());
    Rect lastColumnBounds = getColumnBounds(colCount-1);

    ScrollInfo si(0, 0, clientBounds.w);

    if ((lastColumnBounds.x-clientBounds.x)+lastColumnBounds.w > clientBounds.w)
      si.setMaxPos((lastColumnBounds.x-clientBounds.x) + lastColumnBounds.w - 1);

    // si.pos = VACA_CLAMP(si.pos,
    // 			si.minPos,
    // 			si.maxPos - VACA_MAX(si.pageSize - 1, 0));

    setScrollInfo(Orientation::Horizontal, si);
  }
  else {
    hideScrollBar(Orientation::Horizontal);
  }

  //     updateVerticalScrollBarVisibility();
}

void GridView::updateVerticalScrollBarVisibility()
{
  int rowCount = getRowCount();
  if (rowCount > 0) {
    Rect clientBounds = getClientBounds().offset(-getScrollPoint());
    Rect lastRowBounds = getRowBounds(rowCount-1);

    ScrollInfo si(0, 0, clientBounds.h);

    if ((lastRowBounds.y-clientBounds.y)+lastRowBounds.h > clientBounds.h)
      si.setMaxPos((lastRowBounds.y-clientBounds.y) + lastRowBounds.h - 1);

    // si.pos = VACA_CLAMP(si.pos,
    // 			si.minPos,
    // 			si.maxPos - VACA_MAX(si.pageSize - 1, 0));

    setScrollInfo(Orientation::Vertical, si);
  }
  else {
    hideScrollBar(Orientation::Vertical);
  }

  //     ScrollInfo si = getScrollInfo(Vertical);
  //     si.pageSize = clientBounds.h;
  //     si.minPos = 0;
  //     si.maxPos = clientBounds.h*2;

  //     si.pos = VACA_MID(si.pos,
  // 		      si.minPos,
  // 		      si.maxPos - VACA_MAX(si.pageSize - 1, 0));

  //     setScrollInfo(Vertical, si);
}

void GridView::updateHotTracking(const Point &pt)
{
  int columnIndex = getColumnByPoint(pt, false);
  if (columnIndex != m_hotCol) {
    if (m_hotCol != NULL_COLUMN_INDEX)
      invalidate(getColumnBounds(m_hotCol).enlarge(1), false);

    m_hotCol = columnIndex;

    if (m_hotCol != NULL_COLUMN_INDEX)
      invalidate(getColumnBounds(m_hotCol).enlarge(1), false);
  }

  int rowIndex = getRowByPoint(pt);
  if (rowIndex != m_hotRow) {
    if (m_hotRow != NULL_ROW_INDEX)
      invalidate(getRowBounds(m_hotRow), false);

    m_hotRow = rowIndex;

    if (m_hotRow != NULL_ROW_INDEX)
      invalidate(getRowBounds(m_hotRow), false);
  }
}

//////////////////////////////////////////////////////////////////////
// GridColumn

GridColumn::GridColumn(const String& name, GridValue* defaultValue)
{
  VACA_TRACE("GridColumn()\n");
  m_name = name;
  m_header = name;
  m_table = NULL;
  m_defaultValue = defaultValue;
  m_headerAlign = TextAlign::Left;
  m_dataAlign = TextAlign::Left;
  // m_headerHeight = 0;
  m_columnWidth = 0;
  m_preferredColumnWidth = 0;
}

GridColumn::~GridColumn()
{
  VACA_TRACE("~GridColumn()\n");

  if (m_defaultValue != NULL)
    delete m_defaultValue;
}

String GridColumn::getName() const
{
  return m_name;
}

void GridColumn::setName(const String &name)
{
  m_name = name;
}

String GridColumn::getHeader() const
{
  return m_header;
}

void GridColumn::setHeader(const String &header)
{
  m_header = header;
}

bool GridColumn::getAllowNull() const
{
  return m_allowNull;
}

void GridColumn::setAllowNull(bool state)
{
  m_allowNull = state;
}

GridTable* GridColumn::getTable()
{
  return m_table;
}

GridValue* GridColumn::getDefaultValue()
{
  return m_defaultValue;
}

void GridColumn::setTable(GridTable* table)
{
  m_table = table;
}

GridRow* GridColumn::operator[](int index)
{
  assert(m_table != NULL);
  return m_table->getRow(index);
}

//////////////////////////////////////////////////////////////////////
// GridRow

GridRow::GridRow(GridTable* table)
{
  assert(table != NULL);

  VACA_TRACE("GridRow()\n");
  m_table = table;

  // default values for each column
  int j, cols = m_table->getColumnCount();

  for (j=0; j<cols; ++j) {
    GridValue* value = m_table->getColumn(j)->getDefaultValue();
    m_values.push_back(value != NULL ? new GridValue(*value): NULL);
  }
}

GridRow::~GridRow()
{
  VACA_TRACE("~GridRow()\n");

  // remove values for each column
  for (std::vector<GridValue*>::iterator
	 it = m_values.begin(); it != m_values.end(); ++it)
    delete *it;
}

GridTable* GridRow::getTable()
{
  return m_table;
}

GridValue* GridRow::getValue(int columntIndex)
{
  return m_values[columntIndex];
}

void GridRow::setValue(int columntIndex, GridValue* value)
{
  delete m_values[columntIndex];
  m_values[columntIndex] = value;
}

// GridColumn* GridRow::operator[](int index)
// {
//   assert(m_table != NULL);
//   return (*m_table)[index];
// }

// GridColumn* GridRow::operator[](const String& columnName)
// {
//   assert(m_table != NULL);
//   return (*m_table)[columnName];
// }

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  GridView m_gridView;

public:

  MainFrame()
    : Frame("GridViews (WIP)")
    , m_gridView(this)
  {
    setLayout(new ClientLayout);

    m_gridView.setGridSet(createGridSet());
  }

private:

  struct DummyData {
    int aint;
    const char* astring;
    double adouble;
    bool abool;
  };

  GridSet* createGridSet()
  {
    GridSet* set(new GridSet());
    GridTable* table(new GridTable());
    set->addTable(table);

    table->addColumn(new GridColumn("Integer", new GridValue(int(0))));
    table->addColumn(new GridColumn("String", new GridValue(String(""))));
    table->addColumn(new GridColumn("Double", new GridValue(double(0.0))));
    table->addColumn(new GridColumn("Boolean", new GridValue(bool(false))));

    DummyData data[4] = {
      { 1, "aaa", 0.2, true },
      { 2, "bbb", 2.3, true },
      { 3, "ccc", 42.32, false },
      { 4, "ddd", 392.329, false },
    };

    for (int i=0; i<4; ++i) {
      GridRow* row = table->createRow();
      row->setValue(0, new GridValue(data[i].aint));
      row->setValue(1, new GridValue(data[i].astring));
      row->setValue(2, new GridValue(data[i].adouble));
      row->setValue(3, new GridValue(data[i].abool));
      table->addRow(row);
    }

    return set;
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main() {
    m_mainFrame.setVisible(true);
  }
};

int VACA_MAIN()
{
  Example app;
  app.run();
  return 0;
}
