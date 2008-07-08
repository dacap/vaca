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

#ifndef VACA_DATAGRID_HPP
#define VACA_DATAGRID_HPP

#include "Vaca/base.hpp"
#include "Vaca/Widget.hpp"
#include "Vaca/WidgetClass.hpp"
#include "Vaca/Register.hpp"

namespace Vaca {

// TODO

//////////////////////////////////////////////////////////////////////
// DataGridType

enum DataGridType
{
  DataGrid_Bool,
  DataGrid_Int,
  DataGrid_Float,
  DataGrid_String,
};

//////////////////////////////////////////////////////////////////////
// DataGridColumn

class DataGridColumn
{
  DataGridType mType;
  void* mValue;

public:

  DataGridColumn(bool);

};

//////////////////////////////////////////////////////////////////////
// DataGridValue

class DataGridValue
{
public:

public:

  DataGridType();
  
};

//////////////////////////////////////////////////////////////////////
// DataGridSource

class DataGridSource
{
public:

  virtual int getColumnCount() = 0;
  virtual DataGridColumn* getColumn(int columnIndex) = 0;

  virtual int getRowCount() = 0;
  virtual String getRowText(int rowIndex) = 0;

  virtual DataGridValue* getCellValue(int col, int row) = 0;
  virtual void setCellValue(int col, int row, DataGridValue* value) = 0;

};

//////////////////////////////////////////////////////////////////////
// DataGridClass

class DataGridClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName("Vaca.DataGrid"); }
};

//////////////////////////////////////////////////////////////////////
// DataGrid

#define DataGridStyle		(ChildStyle)

class VACA_DLL DataGrid : public Register<DataGridClass>, public Widget
{
public:

  DataGrid(DataGridSource* dataSource, Widget* parent, Style style = DataGridStyle);
  virtual ~DataGrid();

};

} // namespace Vaca

#endif
