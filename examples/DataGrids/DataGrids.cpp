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

class DataValue;
class DataColumn;
class DataRow;
class DataTable;
class DataSet;

//////////////////////////////////////////////////////////////////////
// DataValue

class DataValue
{
public:
  virtual ~DataValue() { }
  virtual String getString() const = 0;
  virtual void beginEdit(const Rect &cellBounds) = 0;
  virtual void endEdit() = 0;
  virtual DataValue* clone() = 0;
};

//////////////////////////////////////////////////////////////////////
// DataIntValue

class DataIntValue : public DataValue
{
  int m_value;
public:
  DataIntValue(int value);
  virtual ~DataIntValue();
  virtual String getString() const;
  virtual void beginEdit(const Rect &cellBounds);
  virtual void endEdit();
  virtual DataValue* clone();
};

//////////////////////////////////////////////////////////////////////
// DataStringValue

class DataStringValue : public DataValue
{
  String m_value;
public:
  DataStringValue(const String &value);
  virtual ~DataStringValue();
  virtual String getString() const;
  virtual void beginEdit(const Rect &cellBounds);
  virtual void endEdit();
  virtual DataValue* clone();
};

//////////////////////////////////////////////////////////////////////
// DataBoolValue

class DataBoolValue : public DataValue
{
  bool m_value;
public:
  DataBoolValue(bool value);
  virtual ~DataBoolValue();
  virtual String getString() const;
  virtual void beginEdit(const Rect &cellBounds);
  virtual void endEdit();
  virtual DataValue* clone();
};

//////////////////////////////////////////////////////////////////////
// DataDoubleValue

class DataDoubleValue : public DataValue
{
  double m_value;
public:
  DataDoubleValue(double value);
  virtual ~DataDoubleValue();
  virtual String getString() const;
  virtual void beginEdit(const Rect &cellBounds);
  virtual void endEdit();
  virtual DataValue* clone();
};

//////////////////////////////////////////////////////////////////////
// DataColumnStyle

// struct _DataColumnStyle
// {
//   enum enumeration {
//     Null,
//   };
//   static const enumeration default_value = Solid;
// };

// typedef Enum<_DataColumnStyle> DataColumnStyle;

//////////////////////////////////////////////////////////////////////
// DataColumn

class DataColumn
{
  friend class DataTable;
  
  String m_name;
  String m_header;
  DataTable* m_table;
  DataValue* m_defaultValue;
//   int m_headerHeight;
//   int m_columnWidth;
//   int m_preferredColumnWidth;
  bool m_allowNull;
  
public:

  DataColumn(const String& name, DataValue* defaultValue);
  virtual ~DataColumn();

  String getName() const;
  void setName(const String& name);

  String getHeader() const;
  void setHeader(const String& header);

  bool getAllowNull() const;
  void setAllowNull(bool state);
  
  DataTable* getTable();
  DataValue* getDefaultValue();

  DataRow* operator[](int index);

private:
  
  void setTable(DataTable* table);

};

//////////////////////////////////////////////////////////////////////
// DataRow

class DataRow
{
  friend class DataTable;

  DataTable* m_table;
  std::vector<DataValue*> m_values;
//   int m_rowHeight;
  
private:

  DataRow(DataTable*table);

public:

  virtual ~DataRow();

  DataTable* getTable();

  DataValue* getValue(int columntIndex);
  void setValue(int columntIndex, DataValue *value);

//   DataColumn* operator[](int index);
//   DataColumn* operator[](const String &columnName);

};

//////////////////////////////////////////////////////////////////////
// DataTable

class DataTable
{
  String m_name;
  std::vector<DataColumn*> m_columns;
  std::vector<DataRow*> m_rows;
  
public:

  DataTable()
  {
    VACA_TRACE("DataTable()\n");
  }

  DataTable(const String &name)
  {
    VACA_TRACE("DataTable()\n");
    m_name = name;
  }

  virtual ~DataTable()
  {
    for (std::vector<DataColumn*>::iterator
	   it = m_columns.begin(); it != m_columns.end(); ++it) {
      delete *it;
    }
    VACA_TRACE("~DataTable()\n");
  }

  DataRow* createRow()
  {
    return new DataRow(this);
  }

  void addColumn(DataColumn* column)
  {
    // a column without owner
    assert(column->getTable() == NULL);

    column->setTable(this);
    m_columns.push_back(column);
  }

  void addRow(DataRow* row)
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

  DataColumn* getColumn(int index)
  {
    assert(index >= 0 && index < getColumnCount());
    return m_columns[index];
  }

  DataRow* getRow(int index)
  {
    assert(index >= 0 && index < getRowCount());
    return m_rows[index];
  }

  DataColumn* operator[](int index)
  {
    return getColumn(index);
  }

  DataColumn* operator[](const String &columnName)
  {
    for (std::vector<DataColumn*>::iterator
	   it = m_columns.begin(); it != m_columns.end(); ++it) {
      if ((*it)->getName() == columnName)
	return *it;
    }
    assert(false);
  }
  
};

//////////////////////////////////////////////////////////////////////
// DataSet

class DataSet
{
  std::vector<DataTable*> m_tables;

public:

  DataSet();
  virtual ~DataSet();

  void addTable(DataTable* table);
  std::vector<DataTable*> getTables();
  int getTableCount();
  DataTable* operator[](int index);
  DataTable* operator[](const String &tableName);
  
};

DataSet::DataSet()
{
  VACA_TRACE("DataSet()\n");
}

DataSet::~DataSet()
{
  for (std::vector<DataTable*>::iterator
	 it = m_tables.begin(); it != m_tables.end(); ++it) {
    delete *it;
  }
  VACA_TRACE("~DataSet()\n");
}

void DataSet::addTable(DataTable* table)
{
  m_tables.push_back(table);
}

std::vector<DataTable*> DataSet::getTables()
{
  return m_tables;
}

int DataSet::getTableCount()
{
  return m_tables.size();
}

DataTable* DataSet::operator[](int index)
{
  assert(index >= 0 && index < getTableCount());
  return m_tables[index];
}

DataTable* DataSet::operator[](const String &tableName)
{
  for (std::vector<DataTable*>::iterator
	 it = m_tables.begin(); it != m_tables.end(); ++it) {
    if ((*it)->getName() == tableName)
      return *it;
  }
  assert(false);
  // throw std::out_of_range(String("Invalid table name"));
}

//////////////////////////////////////////////////////////////////////
// DataGrid

class DataGrid : public Panel
{
  struct Column {
    DataColumn* columnData;
    TextAlign headerAlign;
    TextAlign dataAlign;
    // int headerHeight;
    int columnWidth;
    int preferredColumnWidth;
  };
  struct Row {
    DataRow* rowData;
    int rowHeight;
  };
  std::vector<Column*> m_columns;
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

  DataSet* m_dataSet;
  DataTable* m_dataTable;

public:

  DataGrid(Widget* parent);
  virtual ~DataGrid();

  void setDataSet(DataSet* set);

  bool getFlatMode() const;
  void setFlatMode(bool state);

  bool getCrudColumn() const;
  void setCrudColumn(bool state);

  int getCrudColumnWidth() const;
  void setCrudColumnWidth(int width);

protected:

  virtual void onScroll(Orientation orientation, int code);
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
  void setDataTable(DataTable* table);
  void addColumn(DataColumn* column);
  void addRow(DataRow* row);
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

DataGrid::DataGrid(Widget* parent)
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

DataGrid::~DataGrid()
{
  clearGrid();
}

void DataGrid::setDataSet(DataSet* set)
{
  m_dataSet = set;
  if (set->getTableCount() > 0)
    setDataTable(set->getTables().front());
}

bool DataGrid::getFlatMode() const
{
  return m_flatMode;
}

void DataGrid::setFlatMode(bool state)
{
  m_flatMode = state;
}

bool DataGrid::getCrudColumn() const
{
  return m_crudColumn;
}

void DataGrid::setCrudColumn(bool state)
{
  m_crudColumn = state;
}

int DataGrid::getCrudColumnWidth() const
{
  return m_crudColumnWidth;
}

void DataGrid::setCrudColumnWidth(int width)
{
  m_crudColumnWidth = width;
}

void DataGrid::onScroll(Orientation orientation, int code)
{
  Panel::onScroll(orientation, code);

  ScrollInfo si = getScrollInfo(orientation);
  int oldPos = si.pos;

  switch (code) {
    case SB_LINELEFT: 
      si.pos -= 32;
      break;
    case SB_LINERIGHT: 
      si.pos += 32;
      break;
    case SB_PAGELEFT:
      si.pos -= si.pageSize;
      break;
    case SB_PAGERIGHT:
      si.pos += si.pageSize;
      break;
    case SB_THUMBTRACK: 
      si.pos = si.trackPos;
      break;
    default:
      break;
  }

  setScrollPos(orientation, si.pos);
  si.pos = getScrollPos(orientation);

  if (si.pos != oldPos) {
    ScrollWindowEx(getHandle(),
		   (orientation == Orientation::Horizontal) ? oldPos - si.pos: 0,
		   (orientation == Orientation::Vertical  ) ? oldPos - si.pos: 0,
		   NULL, NULL, NULL, NULL,
		   SW_ERASE | SW_INVALIDATE);
  }
}

void DataGrid::onDoubleClick(MouseEvent &ev)
{
  Panel::onDoubleClick(ev);

  // double click to fit header width
  int column = getHotResizingBorder(ev.getPoint());
  if (column >= 0) {
    ScreenGraphics g;
    g.setFont(getFont());
    m_columns[column]->columnWidth =
      m_columns[column]->preferredColumnWidth;

    updateHorizontalScrollBarVisibility();
    invalidate(false);
  }
}

// when a mouse's button is pressed
void DataGrid::onMouseDown(MouseEvent &ev)
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
void DataGrid::onMouseUp(MouseEvent &ev)
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
void DataGrid::onMouseMove(MouseEvent &ev)
{
  Panel::onMouseMove(ev);

  if (hasCapture()) {
    // resizing column's header
    if (m_resizing.column >= 0) {
      Rect client = getAbsoluteClientBounds();
      Point cursor = System::getCursorPos();
      Point clientCursor = (cursor - client.getOrigin());

      clientCursor.x = VACA_MAX(0, clientCursor.x);
      clientCursor.y = VACA_MAX(0, clientCursor.y);

      // amount of scroll
      Point delta =
	clientCursor
	+ getScrollPoint()
	- m_resizing.startPoint;

      // change column size
      m_columns[m_resizing.column]->columnWidth =
	VACA_MAX(m_columns[m_resizing.column]->preferredColumnWidth,
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

void DataGrid::onMouseWheel(MouseEvent &ev)
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

void DataGrid::onMouseLeave()
{
  Panel::onMouseLeave();

  if (m_hotCol != NULL_COLUMN_INDEX) {
    invalidate(getColumnBounds(m_hotCol), false);
    m_hotCol = NULL_COLUMN_INDEX;
  }
}

void DataGrid::onKeyDown(KeyEvent &ev)
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

void DataGrid::onSetCursor(WidgetHitTest hitTest)
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
  
void DataGrid::onResize(const Size &sz)
{
  Panel::onResize(sz);
  //       invalidate(true);
  updateHorizontalScrollBarVisibility();
  //       updateVerticalScrollBarVisibility();
}

void DataGrid::onPaint(Graphics &g)
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
		    m_columns[j]->columnData->getHeader():
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
	          m_columns[m_hotCol]->columnData->getHeader():
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
	    DataValue* value = m_rows[i]->rowData->getValue(j);
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

void DataGrid::clearGrid()
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

void DataGrid::setDataTable(DataTable* table)
{
  m_dataTable = table;
  int cols = m_dataTable->getColumnCount();
  int rows = m_dataTable->getRowCount();

  for (int j=0; j<cols; ++j)
    addColumn(m_dataTable->getColumn(j));

  for (int i=0; i<rows; ++i)
    addRow(m_dataTable->getRow(i));
}
  
void DataGrid::addColumn(DataColumn* column)
{
  Column* col = new Column;

  col->columnData = column;
  col->headerAlign = TextAlign::Left;
  col->dataAlign = TextAlign::Left;

  ScreenGraphics g;
  g.setFont(getFont());
  Size headerSize = g.measureString(column->getHeader()) + Size(16, 2);

//   col->headerHeight = headerSize.h;
  col->columnWidth = headerSize.w;
  col->preferredColumnWidth = headerSize.w;

  this->m_headerHeight = VACA_MAX(m_headerHeight, headerSize.h);

  m_columns.push_back(col);
}

void DataGrid::addRow(DataRow* row)
{
  Row* r = new Row;
  r->rowData = row;
//   r->rowHeight = m_columns[0]->headerHeight;
  r->rowHeight = m_headerHeight;
  m_rows.push_back(r);
}
  
int DataGrid::getColumnCount()
{
  return m_columns.size();
}

int DataGrid::getRowCount()
{
  return m_rows.size();
}

int DataGrid::getColumnByPoint(const Point &pt, bool useRows)
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

int DataGrid::getRowByPoint(const Point &pt)
{
  Rect bounds = getClientBounds();
  int rowCount = getRowCount();

  for (int i=0; i<rowCount; ++i)
    if (getRowBounds(i).contains(pt))
      return i;

  return NULL_ROW_INDEX;
}

Rect DataGrid::getColumnBounds(int columnIndex)
{
  if (columnIndex == CRUD_COLUMN_INDEX)
    return getCrudColumnBounds();
  
  assert(columnIndex >= 0 && columnIndex < getColumnCount());
  Point pt = getClientBounds().getOrigin() - getScrollPoint();
  int i;

  if (m_crudColumn)
    pt.x += m_crudColumnWidth;

  for (i=0; i<columnIndex; ++i)
    pt.x += m_columns[i]->columnWidth;

  return Rect(pt, Size(m_columns[i]->columnWidth,
		       m_headerHeight));
// 		       m_columns[i]->headerHeight));
}

Rect DataGrid::getCrudColumnBounds()
{
  assert(m_crudColumn);
  Point pt = getClientBounds().getOrigin() - getScrollPoint();

  return Rect(pt, Size(m_crudColumnWidth, m_headerHeight));
}

Rect DataGrid::getRowBounds(int rowIndex)
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

void DataGrid::drawCell(Graphics &g, const String &text, const Rect &rc,
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

void DataGrid::drawArrow(Graphics &g, const Rect &rc, const Color& color)
{
  Brush brush(color);

  GraphicsPath path;
  path.moveTo(-4, -4);
  path.lineTo(+4, 0);
  path.lineTo(-4, +4);
  path.closeFigure();

  g.fillPath(path, brush, rc.getCenter());
}

int DataGrid::getHotResizingBorder(const Point &pt)
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

void DataGrid::updateHorizontalScrollBarVisibility()
{
  int colCount = getColumnCount();
  if (colCount > 0) {
    Rect clientBounds = getClientBounds().offset(-getScrollPoint());
    Rect lastColumnBounds = getColumnBounds(colCount-1);

    ScrollInfo si = getScrollInfo(Orientation::Horizontal);

    si.pageSize = clientBounds.w;

    if ((lastColumnBounds.x-clientBounds.x)+lastColumnBounds.w > clientBounds.w) {
      si.minPos = 0;
      si.maxPos = (lastColumnBounds.x-clientBounds.x) + lastColumnBounds.w - 1;
    }
    else {
      si.minPos = si.maxPos = 0;
    }

    si.pos = VACA_CLAMP(si.pos,
			si.minPos,
			si.maxPos - VACA_MAX(si.pageSize - 1, 0));

    setScrollInfo(Orientation::Horizontal, si);
  }
  else {
    hideScrollBar(Orientation::Horizontal);
  }

  //     updateVerticalScrollBarVisibility();
}

void DataGrid::updateVerticalScrollBarVisibility()
{
  int rowCount = getRowCount();
  if (rowCount > 0) {
    Rect clientBounds = getClientBounds().offset(-getScrollPoint());
    Rect lastRowBounds = getRowBounds(rowCount-1);

    ScrollInfo si = getScrollInfo(Orientation::Vertical);

    si.pageSize = clientBounds.h;

    if ((lastRowBounds.y-clientBounds.y)+lastRowBounds.h > clientBounds.h) {
      si.minPos = 0;
      si.maxPos = (lastRowBounds.y-clientBounds.y) + lastRowBounds.h - 1;
    }
    else {
      si.minPos = si.maxPos = 0;
    }

    si.pos = VACA_CLAMP(si.pos,
			si.minPos,
			si.maxPos - VACA_MAX(si.pageSize - 1, 0));

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

void DataGrid::updateHotTracking(const Point &pt)
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
// DataIntValue

DataIntValue::DataIntValue(int value) : m_value(value) { }
DataIntValue::~DataIntValue() { }
String DataIntValue::getString() const { return String::fromInt(m_value); }
void DataIntValue::beginEdit(const Rect &cellBounds) { }
void DataIntValue::endEdit() { }
DataValue* DataIntValue::clone() { return new DataIntValue(m_value); }

//////////////////////////////////////////////////////////////////////
// DataStringValue

DataStringValue::DataStringValue(const String &value) : m_value(value) { }
DataStringValue::~DataStringValue() { }
String DataStringValue::getString() const { return m_value; }
void DataStringValue::beginEdit(const Rect &cellBounds) { }
void DataStringValue::endEdit() { }
DataValue* DataStringValue::clone() { return new DataStringValue(m_value); }

//////////////////////////////////////////////////////////////////////
// DataBoolValue

DataBoolValue::DataBoolValue(bool value) : m_value(value) { }
DataBoolValue::~DataBoolValue() { }
String DataBoolValue::getString() const { return m_value ? "1": "0"; }
void DataBoolValue::beginEdit(const Rect &cellBounds) { }
void DataBoolValue::endEdit() { }
DataValue* DataBoolValue::clone() { return new DataBoolValue(m_value); }

//////////////////////////////////////////////////////////////////////
// DataDoubleValue

DataDoubleValue::DataDoubleValue(double value) : m_value(value) { }
DataDoubleValue::~DataDoubleValue() { }
String DataDoubleValue::getString() const { return String::fromDouble(m_value, 16); }
void DataDoubleValue::beginEdit(const Rect &cellBounds) { }
void DataDoubleValue::endEdit() { }
DataValue* DataDoubleValue::clone() { return new DataDoubleValue(m_value); }

//////////////////////////////////////////////////////////////////////
// DataColumn

DataColumn::DataColumn(const String& name, DataValue* defaultValue)
{
  VACA_TRACE("DataColumn()\n");
  m_name = name;
  m_header = name;
  m_table = NULL;
  m_defaultValue = defaultValue;
}

DataColumn::~DataColumn()
{
  VACA_TRACE("~DataColumn()\n");

  if (m_defaultValue != NULL)
    delete m_defaultValue;
}

String DataColumn::getName() const
{
  return m_name;
}

void DataColumn::setName(const String &name)
{
  m_name = name;
}

String DataColumn::getHeader() const
{
  return m_header;
}

void DataColumn::setHeader(const String &header)
{
  m_header = header;
}

bool DataColumn::getAllowNull() const
{
  return m_allowNull;
}

void DataColumn::setAllowNull(bool state)
{
  m_allowNull = state;
}

DataTable* DataColumn::getTable()
{
  return m_table;
}

DataValue* DataColumn::getDefaultValue()
{
  return m_defaultValue;
}

void DataColumn::setTable(DataTable* table)
{
  m_table = table;
}

DataRow* DataColumn::operator[](int index)
{
  assert(m_table != NULL);
  return m_table->getRow(index);
}

//////////////////////////////////////////////////////////////////////
// DataRow

DataRow::DataRow(DataTable* table)
{
  assert(table != NULL);

  VACA_TRACE("DataRow()\n");
  m_table = table;

  // default values for each column
  int j, cols = m_table->getColumnCount();

  for (j=0; j<cols; ++j) {
    DataValue* value = m_table->getColumn(j)->getDefaultValue();
    m_values.push_back(value != NULL ? value->clone(): NULL);
  }
}

DataRow::~DataRow()
{
  VACA_TRACE("~DataRow()\n");

  // remove values for each column
  for (std::vector<DataValue*>::iterator
	 it = m_values.begin(); it != m_values.end(); ++it)
    delete *it;
}

DataTable* DataRow::getTable()
{
  return m_table;
}

DataValue* DataRow::getValue(int columntIndex)
{
  return m_values[columntIndex];
}

void DataRow::setValue(int columntIndex, DataValue* value)
{
  delete m_values[columntIndex];
  m_values[columntIndex] = value;
}

// DataColumn* DataRow::operator[](int index)
// {
//   assert(m_table != NULL);
//   return (*m_table)[index];
// }

// DataColumn* DataRow::operator[](const String& columnName)
// {
//   assert(m_table != NULL);
//   return (*m_table)[columnName];
// }

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  DataGrid m_dataGrid;

public:

  MainFrame()
    : Frame("DataGrids (WIP)")
    , m_dataGrid(this)
  {
    setLayout(new ClientLayout);

    m_dataGrid.setDataSet(createDataSet());
  }

private:

  struct Student {
    int id;
    const char* name;
    int age;
    String clas;
    bool subscribed;
    double average;
  };
  
  DataSet* createDataSet()
  {
    DataSet* set(new DataSet());
    DataTable* table(new DataTable());
    set->addTable(table);

    table->addColumn(new DataColumn("ID", new DataIntValue(0)));
    table->addColumn(new DataColumn("Name", new DataStringValue("")));
    table->addColumn(new DataColumn("Age", new DataIntValue(0)));
    table->addColumn(new DataColumn("Class", new DataIntValue(0)));
    table->addColumn(new DataColumn("Subscribed", new DataIntValue(0)));
    table->addColumn(new DataColumn("Average", new DataIntValue(0)));

    Student students[4] = {
      { 1, "David", 22, "Mathematics", true,  6.7 },
      { 2, "John",  21, "Physics",     true,  7.5 },
      { 3, "Peter", 23, "Chemistry",   true,  9.0 },
      { 4, "Mark",  22, "Philosophy",  false, 3.5 },
    };

    for (int i=0; i<4; ++i) {
      DataRow* row = table->createRow();
      row->setValue(0, new DataIntValue   (students[i].id));
      row->setValue(1, new DataStringValue(students[i].name));
      row->setValue(2, new DataIntValue   (students[i].age));
      row->setValue(3, new DataStringValue(students[i].clas));
      row->setValue(4, new DataBoolValue  (students[i].subscribed));
      row->setValue(5, new DataDoubleValue(students[i].average));
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
