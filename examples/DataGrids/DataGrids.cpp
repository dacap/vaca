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

#include "Vaca/Vaca.h"

using namespace Vaca;

// struct DataType {
//   enum Type {
//     Bool,
//     Byte,
//     Char,
//     DateTime,
//     Date,
//     Time,
//     Decimal,
//     Int16,
//     Int32,
//     Int64,
//     UInt16,
//     UInt32,
//     UInt64,
//     Float,
//     Double,
//     String
//   };
// };

class DataValue;
class DataColumn;
class DataRow;
class DataTable;
class DataSet;

// class DataValue
// {
//   DataType mType;
//   int mSize;
//   union {
//     void *		ptr;

//     bool		bool_value;
//     byte		byte_value;
//     char		char_value;
//     short		int16_value;
//     long		int32_value;
//     LONGLONG		int64_value;
//     unsigned short	uint16_value;
//     unsigned long	uint32_value;
//     DWORDLONG		uint64_value;
//     float		float_value;
//     double		double_value;
//     TCHAR *		string_value;
//   } mValue;

// //   DataValue(DataType type, int size, const char *src)
// //     : mType(type)
// //     : mSize(type)
// //   {
// //     memcpy(&mValue.ptr, src, size);
// //   }

// public:

// //   DataValue(DataType type, String value)
// //   {
// //     assert(DataType::String == type);
// //     initialize(type, sizeof(TCHAR), &value);
// //   }

// };

//////////////////////////////////////////////////////////////////////

class DataValue
{
public:
  virtual ~DataValue() { }
  virtual String getString() = 0;
  virtual void beginEdit(const Rect &cellBounds) = 0;
  virtual void endEdit() = 0;
  virtual DataValue *clone() = 0;
};

class DataIntValue : public DataValue
{
  int mValue;
public:
  DataIntValue(int value);
  virtual ~DataIntValue();
  virtual String getString();
  virtual void beginEdit(const Rect &cellBounds);
  virtual void endEdit();
  virtual DataValue *clone();
};

class DataColumn
{
  friend class DataTable;
  
  String mName;
  String mCaption;
  DataTable *mTable;
  DataValue *mDefaultValue;
  int mHeaderHeight;
  int mColumnWidth;
  int mPreferredColumnWidth;
  
public:

  DataColumn(const String &name, DataValue *defaultValue);
  virtual ~DataColumn();

  String getName();
  void setName(const String &name);

  String getCaption();
  void setCaption(const String &caption);
  
  DataTable *getTable();

private:
  
  void setTable(DataTable *table);

};

class DataRow
{
  friend class DataTable;

  DataTable *mTable;
  std::vector<DataValue *> mValue;
  int mRowHeight;
  
private:

  DataRow(DataTable *table);

public:

  virtual ~DataRow();

  DataTable *getTable();

  DataColumn *operator[](int index);
  DataColumn *operator[](const String &columnName);

};

class DataTable
{
  String mName;
  std::vector<DataColumn *> mColumns;
  std::vector<DataRow *> mRows;
  
public:

  DataTable()
  {
    VACA_TRACE("DataTable()\n");
  }

  DataTable(const String &name)
  {
    VACA_TRACE("DataTable()\n");
    mName = name;
  }

  virtual ~DataTable()
  {
    for (std::vector<DataColumn *>::iterator it = mColumns.begin();
	 it != mColumns.end();
	 ++it) {
      delete *it;
    }
    VACA_TRACE("~DataTable()\n");
  }

  DataRow *createRow()
  {
    return new DataRow(this);
  }

  void addColumn(DataColumn *column)
  {
    // a column without owner
    assert(column->getTable() == NULL);

    column->setTable(this);
    mColumns.push_back(column);
  }

  void addRow(DataRow *row)
  {
    // a row created by this table
    assert(row->getTable() == this);

    mRows.push_back(row);
  }

  String getName()
  {
    return mName;
  }

  void setName(const String &name)
  {
    mName = name;
  }

  int getColumnCount()
  {
    return mColumns.size();
  }

  int getRowCount()
  {
    return mRows.size();
  }

  DataColumn *operator[](int index)
  {
    assert(index >= 0 && index < getColumnCount());
    return mColumns[index];
  }

  DataColumn *operator[](const String &columnName)
  {
    for (std::vector<DataColumn *>::iterator it = mColumns.begin();
	 it != mColumns.end();
	 ++it) {
      if ((*it)->getName() == columnName)
	return *it;
    }
    assert(false);
  }
  
};

class DataSet
{
  std::vector<DataTable *> mTables;

public:

  DataSet()
  {
    VACA_TRACE("DataSet()\n");
  }

  virtual ~DataSet()
  {
    for (std::vector<DataTable *>::iterator it = mTables.begin();
	 it != mTables.end();
	 ++it) {
      delete *it;
    }
    VACA_TRACE("~DataSet()\n");
  }

  void addTable(DataTable *table)
  {
    mTables.push_back(table);
  }

  std::vector<DataTable *> getTables()
  {
    return mTables;
  }

  int getTableCount()
  {
    return mTables.size();
  }

  DataTable *operator[](int index)
  {
    assert(index >= 0 && index < getTableCount());
    return mTables[index];
  }

  DataTable *operator[](const String &tableName)
  {
    for (std::vector<DataTable *>::iterator it = mTables.begin();
	 it != mTables.end();
	 ++it) {
      if ((*it)->getName() == tableName)
	return *it;
    }
    assert(false);
    // throw std::out_of_range(String("Invalid table name"));
  }
  
};

class DataGrid : public Panel
{
  struct Column {
//     DataGridType type;
    String header;
    TextAlign headerAlign;
    TextAlign dataAlign;
    int headerHeight;
    int columnWidth;
    int preferredColumnWidth;
  };
  struct Row {
    int rowHeight;
//     std::vector<DataGridValue *> data;
  };
  std::vector<Column *> mColumns;
  std::vector<Row *> mRows;
  int mHotCol;
  int mHotRow;
  struct {
    int column;
    Rect startBounds;
    Point startPoint;
  } mResizing;
  // customization
  bool mHeaderResizingEnabled;
  bool mWhiteFill;

public:

  DataGrid(Widget *parent)
    : Panel(parent, PanelStyle +
		    ScrollStyle +
		    ClientEdgeStyle +
		    TabStopStyle)
  {
    setBgColor(Color::White);
    setFgColor(Color::Black);

    mHotCol = -1;
    mHotRow = -1;
    mResizing.column = -1;

    // default configuration
    mHeaderResizingEnabled = true;
    mWhiteFill = false;

    ::ShowScrollBar(getHWND(), SB_BOTH, FALSE);
  }

  virtual ~DataGrid()
  {
    int colCount = getColumnCount();
    int rowCount = getRowCount();

    for (int i=0; i<rowCount; ++i) {
//       for (int j=0; j<colCount; ++j)
// 	delete mRows[i]->data[j];

      delete mRows[i];
    }

    for (int i=0; i<colCount; ++i)
      delete mColumns[i];
  }

  //////////////////////////////////////////////////////////////////////
  void getScrollInfo(Orientation orientation, int &minPos, int &maxPos, int &pageSize)
  {
    assert(getHWND() != NULL);
    
    int fnBar = orientation == Horizontal ? SB_HORZ:
					    SB_VERT;

    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE;

    GetScrollInfo(getHWND(), fnBar, &si);

    minPos = si.nMin;
    maxPos = si.nMax;
    pageSize = si.nPage;
  }

  void setScrollInfo(Orientation orientation, int minPos, int maxPos, int pageSize)
  {
    assert(getHWND() != NULL);
    
    int fnBar = orientation == Horizontal ? SB_HORZ:
					    SB_VERT;

    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE;
    si.nMin = minPos;
    si.nMax = maxPos;
    si.nPage = pageSize;

    SetScrollInfo(getHWND(), fnBar, &si, TRUE);
  }
  
  int getScrollPos(Orientation orientation)
  {
    assert(getHWND() != NULL);
    
    int fnBar = orientation == Horizontal ? SB_HORZ:
					    SB_VERT;

    SCROLLINFO si;
    si.cbSize = sizeof (si);
    si.fMask  = SIF_POS;
    GetScrollInfo(getHWND(), fnBar, &si);
    return si.nPos;
  }

  void setScrollPos(Orientation orientation, int pos)
  {
    assert(getHWND() != NULL);

    int fnBar = orientation == Horizontal ? SB_HORZ:
					    SB_VERT;
    
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask  = SIF_ALL;
    GetScrollInfo(getHWND(), fnBar, &si);

    si.fMask = SIF_POS;
    si.nPos = VACA_MID(si.nMin,
		       pos,
		       si.nMax - VACA_MAX(static_cast<int>(si.nPage) - 1, 0));
    SetScrollInfo(getHWND(), fnBar, &si, TRUE);
  }
  
  Point getScrollPoint()
  {
    return Point(getScrollPos(Horizontal),
		 getScrollPos(Vertical));
  }

  void setScrollPoint(const Point &pt)
  {
    setScrollPos(Horizontal, pt.x);
    setScrollPos(Vertical, pt.y);
  }
  //////////////////////////////////////////////////////////////////////

  void addColumn(const String &header,
		 TextAlign headerAlign = LeftAlign,
		 TextAlign dataAlign = LeftAlign)
  {
    Column *column = new Column;

//     column->type = type;
    column->header = header;
    column->headerAlign = headerAlign;
    column->dataAlign = dataAlign;

    ScreenGraphics g;
    g.setFont(getFont());
    Size headerSize = g.measureString(header) + Size(16, 2);

    column->headerHeight = headerSize.h;
    column->columnWidth = headerSize.w;
    column->preferredColumnWidth = headerSize.w;

    mColumns.push_back(column);
  }
#if 0
  void addRow(std::vector<DataGridValue *> &rowData)
  {
#ifndef NDEBUG
    int count = getColumnCount();
    assert(count > 0);
    assert(rowData.size() == count);
//     for (int i=0; i<count; ++i)
//       assert(mColumns[i]->type == rowData[i]->getType());
#endif

    Row *row = new Row;
    row->rowHeight = mColumns[0]->headerHeight;
    row->data = rowData;
    mRows.push_back(row);
  }
#endif
  
  int getColumnCount()
  {
    return mColumns.size();
  }

  int getRowCount()
  {
    return mRows.size();
  }

  int getColumnByPoint(const Point &pt, bool useRows)
  {
    Rect bounds = getClientBounds();
    int count = getColumnCount();

    for (int i=0; i<count; ++i) {
      Rect colBounds = getColumnBounds(i);

      if (useRows) {
	colBounds.y = bounds.y;
	colBounds.h = bounds.h;
      }

      if (colBounds.contains(pt))
	return i;
    }

    return -1;
  }
 
  Rect getColumnBounds(int columnIndex)
  {
    assert(columnIndex >= 0 && columnIndex < getColumnCount());
    Point pt = getClientBounds().getOrigin() - getScrollPoint();
    int i;

    for (i=0; i<columnIndex; ++i)
      pt.x += mColumns[i]->columnWidth;

    return Rect(pt, Size(mColumns[i]->columnWidth,
			 mColumns[i]->headerHeight));
  }

protected:

  virtual void onScroll(Orientation orientation, int code)
  {
    if (orientation == Horizontal) {
      SCROLLINFO si;
      si.cbSize = sizeof (si);
      si.fMask  = SIF_ALL;
      GetScrollInfo(getHWND(), SB_HORZ, &si);
      int xPos = si.nPos;
      switch (code) {
	case SB_LINELEFT: 
	  si.nPos -= 32;
	  break;
	case SB_LINERIGHT: 
	  si.nPos += 32;
	  break;
	case SB_PAGELEFT:
	  si.nPos -= si.nPage;
	  break;
	case SB_PAGERIGHT:
	  si.nPos += si.nPage;
	  break;
	case SB_THUMBTRACK: 
	  si.nPos = si.nTrackPos;
	  break;
	default:
	  break;
      }
      si.fMask = SIF_POS;
      ::SetScrollInfo(getHWND(), SB_HORZ, &si, TRUE);
      ::GetScrollInfo(getHWND(), SB_HORZ, &si);

      if (si.nPos != xPos) {
	ScrollWindowEx(getHWND(), xPos - si.nPos, 0,
		       NULL, NULL, NULL, NULL,
		       SW_INVALIDATE);
      }
    }
  }

  virtual void onDoubleClick(MouseEvent &ev)
  {
    // double click to fit header width
    int column = getHotResizingBorder(ev.getPoint());
    if (column >= 0) {
      ScreenGraphics g;
      g.setFont(getFont());
      mColumns[column]->columnWidth =
	mColumns[column]->preferredColumnWidth;

      updateHorizontalScrollBarVisibility();
      invalidate(false);
    }
  }

  // when a mouse's button is pressed
  virtual void onMouseDown(MouseEvent &ev)
  {
    if (!hasCapture()) {
      focus();
      
      // resize a header?
      int column = getHotResizingBorder(ev.getPoint());
      if (column >= 0) {
	mResizing.column = column;
	mResizing.startBounds = getColumnBounds(column);
	mResizing.startPoint = ev.getPoint() + getScrollPoint();

	acquireCapture();
      }
    }
  }

  // when a mouse's button is unpressed
  virtual void onMouseUp(MouseEvent &ev)
  {
    if (hasCapture()) {
      if (mResizing.column >= 0) {
	mResizing.column = -1;

// 	updateHorizontalScrollBarVisibility();
// 	invalidate(false);
      }
      
      releaseCapture();
    }
  }

  // when the mouse is moved
  virtual void onMouseMove(MouseEvent &ev)
  {
    if (hasCapture()) {
      // resizing column's header
      if (mResizing.column >= 0) {
	Rect client = getAbsoluteClientBounds();
	Point cursor = System::getCursorPos();
	Point clientCursor = (cursor - client.getOrigin());

	clientCursor.x = VACA_MAX(0, clientCursor.x);
	clientCursor.y = VACA_MAX(0, clientCursor.y);

	// amount of scroll
	Point delta =
	  clientCursor
	  + getScrollPoint()
	  - mResizing.startPoint;

	// change column size
	mColumns[mResizing.column]->columnWidth =
	  VACA_MAX(mColumns[mResizing.column]->preferredColumnWidth,
		   mResizing.startBounds.w + delta.x);

	// update scroll bars
	updateHorizontalScrollBarVisibility();

	// the mouse is out the right side of the client bounds
	if (cursor.x > client.x+client.w) {
	  int dx = cursor.x - (client.x+client.w);

	  System::setCursorPos(Point(client.x+client.w-dx,
				     cursor.y));

	  setScrollPos(Horizontal,
		       getScrollPos(Horizontal)+dx);
	}

	invalidate(false);
      }
    }
    else {
      updateHotTracking(ev.getPoint());
    }
  }

  // TODO why DataGrid::onMouseWheel isn't called?
  virtual void onMouseWheel(MouseEvent &ev)
  {
//     int columnIndex = getColumnByPoint(ev.getPoint());

//     SCROLLINFO si;
//     si.cbSize = sizeof(si);
//     si.fMask  = SIF_ALL;
//     GetScrollInfo(getHWND(), SB_HORZ, &si);

//     int oldPos = si.nPos;
//     int newPos = si.nPos - ev.getDelta() * 32;

//     si.fMask = SIF_POS;
//     si.nPos = VACA_MID(static_cast<int>(si.nMin),
// 		       static_cast<int>(newPos),
// 		       static_cast<int>(si.nMax - VACA_MAX(si.nPage, 0)));
//     SetScrollInfo(getHWND(), SB_HORZ, &si, TRUE);
//     GetScrollInfo(getHWND(), SB_HORZ, &si);

    int oldPos = getScrollPos(Horizontal);
    setScrollPos(Horizontal, oldPos - ev.getDelta() * 32);
    int newPos = getScrollPos(Horizontal);

    if (oldPos != newPos) {
      ScrollWindowEx(getHWND(), oldPos - newPos, 0,
		     NULL, NULL, NULL, NULL,
		     SW_INVALIDATE);

      updateHotTracking(ev.getPoint());
    }
  }

  virtual void onMouseLeave()
  {
    if (mHotCol >= 0) {
      invalidate(getColumnBounds(mHotCol), false);
      mHotCol = -1;
    }
  }

  virtual void onSetCursor(int hitTest)
  {
    if (hitTest == HTCLIENT) {
      Point pt = System::getCursorPos() - getAbsoluteClientBounds().getOrigin();
      int colIndex = getHotResizingBorder(pt);
      if (colIndex >= 0) {
	setCursor(Cursor(SizeECursor));
	return;
      }
    }
    setCursor(Cursor(ArrowCursor));
  }
  
    virtual void onResize(const Size &sz)
    {
      Panel::onResize(sz);
//       invalidate(true);
      updateHorizontalScrollBarVisibility();
    }

  virtual void onPaint(Graphics &g)
  {
    Color headerFaceColor = Color(212, 208, 200);//System::getColor(COLOR_3DFACE);
    Color headerBorderColor = Color(128, 128, 128);
    Color hotHeaderFaceColor = Color(182, 189, 210);
    Color hotHeaderBorderColor = Color(10, 36, 106);
    Point pt = getClientBounds().getOrigin();
    int colCount = getColumnCount();
    int rowCount = getRowCount();
    
    if (colCount > 0) {
      // draw normal column headers
      for (int i=0; i<colCount; ++i)
	if (i != mHotCol)
	  drawCell(g, mColumns[i]->header,
		   getColumnBounds(i),
		   headerBorderColor,
		   headerFaceColor);

      // draw hot column header
      if (mHotCol >= 0)
	drawCell(g, mColumns[mHotCol]->header,
		 getColumnBounds(mHotCol),
		 hotHeaderBorderColor,
		 hotHeaderFaceColor);

      // fill right side of the header
      Rect rightSide = getColumnBounds(colCount-1);

      int width = getClientBounds().getSize().w - rightSide.x;
      rightSide.x += rightSide.w;
      rightSide.w = width;

      if (mWhiteFill) {
	g.setColor(getBgColor());
	g.fillRect(rightSide);
      }
      else {
// 	rightSide.x -= 1;
	drawCell(g, "", rightSide,
		 headerBorderColor,
		 headerFaceColor);
      }

      // draw rows
      for (int j=0; j<rowCount; ++j) {
	for (int i=0; i<colCount; ++i) {
	  Rect rc = getColumnBounds(i);
	  rc.y += (j+1)*rc.h;

	  String text = "Unknown";

// 	  switch (mRows[j]->data[i]->getType()) {
// 	    case DataGrid_Bool:   text="Bool";   break;
// 	    case DataGrid_Int:    text="Int";    break;
// 	    case DataGrid_Float:  text="Float";  break;
// 	    case DataGrid_String: text="String"; break;
// 	  }

	  drawCell(g, text,
		   rc,
		   Color(192, 192, 192),
		   Color::White);
	}
      }

      // fill right side of the rows
      int height = getClientBounds().getSize().h - rightSide.h;
      rightSide.y += rightSide.h;
      rightSide.h = height;
      rightSide.x++;
      rightSide.w--;

      g.setColor(getBgColor());
      g.fillRect(rightSide);
    }
  }

private:

  void drawCell(Graphics &g, const String &text, const Rect &rc, Color border, Color face)
  {
    g.setColor(border);
//     g.drawRect(rc);
    g.moveTo(rc.x, rc.y+rc.h-1);
    g.lineTo(rc.x+rc.w-1, rc.y+rc.h-1);
    g.lineTo(rc.x+rc.w-1, rc.y-1);

    g.setColor(face);
    g.fillRect(Rect(rc).inflate(-1, -1));

    if (!text.empty()) {
      g.setColor(getFgColor());
      g.drawString(text, rc, DT_CENTER | DT_VCENTER);
    }
  }

  int getHotResizingBorder(const Point &pt)
  {
    if (mHeaderResizingEnabled) {
      int count = getColumnCount();

      for (int i=count-1; i>=0; --i) {
	Rect colBounds = getColumnBounds(i);
	Rect border = Rect(colBounds.x+colBounds.w-3, colBounds.y, 5, colBounds.h);
	if (border.contains(pt))
	  return i;
      }
    }
    return -1;
  }

  void updateHorizontalScrollBarVisibility()
  {
    int colCount = getColumnCount();
    if (colCount > 0) {
      Rect clientBounds = getClientBounds().offset(-getScrollPoint());
      Rect lastColumnBounds = getColumnBounds(colCount-1);

      SCROLLINFO si;
      si.cbSize = sizeof(si);
      si.fMask  = SIF_ALL;
      GetScrollInfo(getHWND(), SB_HORZ, &si);

      si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
      si.nPage = clientBounds.w;

      if ((lastColumnBounds.x-clientBounds.x)+lastColumnBounds.w > clientBounds.w) {
	si.nMin = 0;
	si.nMax = (lastColumnBounds.x-clientBounds.x) + lastColumnBounds.w - 1;
      }
      else {
	si.nMin = si.nMax = 0;
      }

      si.nPos = VACA_MID(si.nMin,
			 si.nPos,
			 si.nMax - VACA_MAX(static_cast<int>(si.nPage) - 1, 0));

      ::SetScrollInfo(getHWND(), SB_HORZ, &si, TRUE);
    }
    else {
      SCROLLINFO si;
      si.cbSize = sizeof(si);
      si.fMask  = SIF_RANGE;
      si.nMin = si.nMax = 0;
      ::SetScrollInfo(getHWND(), SB_HORZ, &si, TRUE);
    }
  }

  void updateHotTracking(const Point &pt)
  {
    int columnIndex = getColumnByPoint(pt, true);
    if (columnIndex != mHotCol) {
      if (mHotCol >= 0)
	invalidate(getColumnBounds(mHotCol), false);

      mHotCol = columnIndex;

      if (mHotCol >= 0)
	invalidate(getColumnBounds(mHotCol), false);
    }
  }
  
};

//////////////////////////////////////////////////////////////////////
// DataIntValue

DataIntValue::DataIntValue(int value) : mValue(value) { }
DataIntValue::~DataIntValue() { }
String DataIntValue::getString() { return String::fromInt(mValue); }
void DataIntValue::beginEdit(const Rect &cellBounds) { }
void DataIntValue::endEdit() { }
DataValue *DataIntValue::clone() { return new DataIntValue(mValue); }

//////////////////////////////////////////////////////////////////////
// DataColumn

DataColumn::DataColumn(const String &name, DataValue *defaultValue)
{
  VACA_TRACE("DataColumn()\n");
  mName = name;
  mCaption = name;
  mTable = NULL;
  mDefaultValue = defaultValue;
}

DataColumn::~DataColumn()
{
  VACA_TRACE("~DataColumn()\n");

  if (mDefaultValue != NULL)
    delete mDefaultValue;
}

String DataColumn::getName()
{
  return mName;
}

void DataColumn::setName(const String &name)
{
  mName = name;
}

String DataColumn::getCaption()
{
  return mCaption;
}

void DataColumn::setCaption(const String &caption)
{
  mCaption = caption;
}
  
DataTable *DataColumn::getTable()
{
  return mTable;
}
  
void DataColumn::setTable(DataTable *table)
{
    mTable = table;
}

//////////////////////////////////////////////////////////////////////
// DataRow

DataRow::DataRow(DataTable *table)
{
  assert(table != NULL);

  VACA_TRACE("DataRow()\n");
  mTable = table;
}

DataRow::~DataRow()
{
  VACA_TRACE("~DataRow()\n");
}

DataTable *DataRow::getTable()
{
  return mTable;
}

DataColumn *DataRow::operator[](int index)
{
  assert(mTable != NULL);
  return (*mTable)[index];
}

DataColumn *DataRow::operator[](const String &columnName)
{
  assert(mTable != NULL);
  return (*mTable)[columnName];
}

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  DataGrid mDataGrid;
  
public:

  MainFrame()
    : Frame("DataGrids (WIP)")
    , mDataGrid(this)
  {
    setLayout(new ClientLayout);

    DataSet *set(new DataSet());
    DataTable *table(new DataTable());
    set->addTable(table);
    table->addColumn(new DataColumn("ID", new DataIntValue(0)));
    table->addColumn(new DataColumn("Name", new DataIntValue(0)));
    table->addColumn(new DataColumn("Age", new DataIntValue(0)));
    table->addColumn(new DataColumn("Class", new DataIntValue(0)));
    table->addColumn(new DataColumn("Subscribed", new DataIntValue(0)));
    table->addColumn(new DataColumn("Average", new DataIntValue(0)));
//     delete set;

    mDataGrid.addColumn("ID");
    mDataGrid.addColumn("Name");
    mDataGrid.addColumn("Age");
    mDataGrid.addColumn("Class");
    mDataGrid.addColumn("Subscribed");
    mDataGrid.addColumn("Average");

//     std::vector<DataGridValue *> data;
//     data.push_back(new DataGridValue(1));
//     data.push_back(new DataGridValue("Pedrito"));
//     data.push_back(new DataGridValue("10"));
//     data.push_back(new DataGridValue("A"));
//     data.push_back(new DataGridValue(true));
//     data.push_back(new DataGridValue(8.8));
//     mDataGrid.addRow(data);

//     data.clear();
//     data.push_back(new DataGridValue(2));
//     data.push_back(new DataGridValue("Juancito"));
//     data.push_back(new DataGridValue("12"));
//     data.push_back(new DataGridValue("B"));
//     data.push_back(new DataGridValue(true));
//     data.push_back(new DataGridValue(6.7));
//     mDataGrid.addRow(data);

//     data.clear();
//     data.push_back(new DataGridValue(3));
//     data.push_back(new DataGridValue("Josecito"));
//     data.push_back(new DataGridValue("12"));
//     data.push_back(new DataGridValue("C"));
//     data.push_back(new DataGridValue(false));
//     data.push_back(new DataGridValue(0.0));
//     mDataGrid.addRow(data);

//     mDataGrid.addField(DataGridValue(0), );
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame mMainWnd;
public:
  virtual void main(std::vector<String> args) {
    mMainWnd.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
