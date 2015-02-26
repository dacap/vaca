// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_RICHEDIT_H
#define VACA_RICHEDIT_H

#include "vaca/base.h"
#include "vaca/Widget.h"

namespace vaca {

/**
   Loads the RichEdit DLL.
*/
class VACA_DLL RichEditRegister
{
  static HINSTANCE hmod;

public:
  RichEditRegister();
};

/**
   Widget to edit rich-text.
*/
class VACA_DLL RichEdit : public RichEditRegister, public Widget
{
public:

  struct VACA_DLL Styles {
    static const Style Default;
    static const Style RightAligned;
    static const Style ReadOnly;
    static const Style AutoHorizontalScroll;
    static const Style AutoVerticalScroll;
  };

  RichEdit(const String& text, Widget* parent, Style style = Styles::Default);
  virtual ~RichEdit();

  virtual void setBgColor(const Color& color);

  size_t getTextLength() const;
  size_t getTextLimit() const;
  void setTextLimit(size_t textLimit);

  bool canPaste() const;
  bool canUndo() const;
  bool canRedo() const;
  void undo();
  void redo();
  void setUndoLimit(int maximumActions);

  float getZoomFactor();
  void setZoomFactor(float f);

  void setDetectUrls(bool state);
  bool isDetectUrls();

  void cut();
  void copy();
  void paste();

  void selectAll();
  void selectRange(int start, int end);
  void deselect();

  void getSelection(int& start, int& end);
  String getSelectedText() const;

};

} // namespace vaca

#endif // VACA_TEXTEDIT_H
