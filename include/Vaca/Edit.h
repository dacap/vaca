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

#ifndef VACA_EDIT_H
#define VACA_EDIT_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"

namespace Vaca {

/**
 * ES_RIGHT
 */
#define RightAlignedEditStyle		Style(ES_RIGHT, 0)

/**
 * ES_READONLY 
 */
#define ReadOnlyEditStyle		Style(ES_READONLY, 0)

/**
 * Default style for Edit: a visible child (ChildVisible), with tab
 * stop (TabStopStyle), edge in its client-are (ClientEdgeStyle), and
 * ES_AUTOHSCROLL.
 */
#define EditStyle		(ChildStyle +			\
				 TabStopStyle +				\
				 ClientEdgeStyle +			\
				 Style(ES_AUTOHSCROLL, 0))
/**
 * Default style for PasswordEdit: an EditStyle with ES_PASSWORD.
 */
#define PasswordEditStyle	(EditStyle + Style(ES_PASSWORD, 0))

/**
 * Default style for MultilineEdit: an EditStyle with ES_MULTILINE.
 */
#define MultilineEditStyle	(EditStyle + Style(ES_MULTILINE, 0))

/**
 * Control to edit a line of text.
 */
class VACA_DLL Edit : public Widget
{
public:

  Edit(const String &text, Widget *parent, Style style = EditStyle);
  virtual ~Edit();

  int getTextLength();

  int getTextLimit();
  void setTextLimit(int textLimit);

  bool isReadOnly();
  void setReadOnly(bool readOnly);

  bool canUndo();
  void undo();

  void selectAll();
  void selectRange(int startIndex, int endIndex);
  void deselect();

  virtual Size preferredSize();
  virtual Size preferredSize(const Size &fitIn);

  // signals
  boost::signal<void (Event &)> Change; ///< @see onChange

protected:
  // new events
  virtual void onChange(Event &ev);

  // reflection
  virtual bool onCommand(int id, int code, LRESULT &lResult);
};

/**
 * Control to edit a line of text.
 */
class VACA_DLL PasswordEdit : public Edit
{
public:

  PasswordEdit(const String &text, Widget *parent, Style style = PasswordEditStyle);
  virtual ~PasswordEdit();

  Character getPasswordCharacter();
  void setPasswordCharacter(Character passwordChar);
};

/**
 * Control to edit some lines of text.
 */
class VACA_DLL MultilineEdit : public Edit
{
public:

  MultilineEdit(const String &text, Widget *parent, Style style = MultilineEditStyle);
  virtual ~MultilineEdit();

  bool getWantReturnMode();
  void setWantReturnMode(bool wantReturn);

  int getLineCount();
  String getLine(int lineNo);
  int getLineLength(int lineNo);

  void scrollLines(int lines);

};

// class VACA_DLL RichEdit : public Edit {
//   bool canRedo();
//   void redo();
// }

} // namespace Vaca

#endif
