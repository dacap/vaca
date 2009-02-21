// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, 2009, David Capello
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

#ifndef VACA_BACKWARD_H
#define VACA_BACKWARD_H

#include "Vaca/Vaca.h"

namespace Vaca
{

// ===================================================================
// Basic Operations
// ===================================================================

/**
 * @deprecated Use Vaca::min_value instead.
 */
#define VACA_MIN(x, y)			(Vaca::min_value(x, y))

/**
 * @deprecated Use Vaca::max_value instead.
 */
#define VACA_MAX(x, y)			(Vaca::max_value(x, y))

/**
 * @deprecated Use Vaca::clamp_value instead.
 */
#define VACA_MID(low, x, high)		(Vaca::clamp_value(x, low, high))

/**
 * @deprecated Use Vaca::clamp_value instead.
 */
#define VACA_CLAMP(x, low, high)	(Vaca::clamp_value(x, low, high))

// ===================================================================
// Old String
// ===================================================================

/**
 * @deprecated Use Char.
 */
typedef wchar_t Character;

/**
 * @deprecated This is the string class used in old versions of
 *   Vaca. Now you must to use the String type.
 */
class _String : public String
{
public:

  _String() : String(L"") { }
  explicit _String(int length) : String(length, L'\0') { }
  _String(const String& str) : String(str) { }
  _String(const _String& str) : String(str) { }
  _String(const std::string& str) : String(convert_to<String>(str)) { }
  _String(const char* str) : String(convert_to<String>(str)) { }
  virtual ~_String() { }

  _String& trim() { return *this = trim_string(*this); }
  static _String trim(const _String& str) { return trim_string(str); }

  // static _String format(const char* fmt, ...) { }
  // static _String format(const wchar_t* fmt, ...) { }

  std::string to_string() const { return convert_to<std::string, String>(*this); }
  std::wstring to_wstring() const { return *this; }

  void copyTo(wchar_t* dest, int size) const {
    copy_string_to(*this, dest, size);
  }

  // static String fromInt(int value, int base = 10, int precision = 0) { }
  // int parseInt(int base = 10) const { }
  static String fromInt(int value) { return convert_to<String>(value); }
  int parseInt() const { return convert_to<int, String>(*this); }

  // static String fromDouble(double value, int precision) { }
  // double parseDouble() const { }
  static String fromDouble(double value) { return convert_to<String>(value); }
  double parseDouble() const { return convert_to<double, String>(*this); }

  _String addPathComponent(const _String& component) const {
    return operator/(*this, component);
  }

  _String getFilePath() const { return file_path(*this); }
  _String getFileName() const { return file_name(*this); }
  _String getFileExtension() const { return file_extension(*this); }
  _String getFileTitle() const { return file_title(*this); }

  _String getUrlHost() const { return url_host(*this); }
  _String getUrlObject() const { return url_object(*this); }

  _String encodeUrl() const { return encode_url(*this); }
  _String decodeUrl() const { return decode_url(*this); }

};

inline String operator+(const String& _s1, const char* _s2)
{
  _String _res(_s1);
  _res.append(_String(_s2));
  return _res;
}

inline _String operator+(const _String& _s1, const wchar_t* _s2)
{
  _String _res(_s1);
  _res.append(_String(_s2));
  return _res;
}

inline _String operator+(const char* _s1, const _String& _s2)
{
  _String _res(_s1);
  _res.append(_s2);
  return _res;
}

inline _String operator+(const wchar_t* _s1, const _String& _s2)
{
  _String _res(_s1);
  _res.append(_s2);
  return _res;
}

// ===================================================================
// Styles
// ===================================================================

/**
 * @deprecated Use BandedDockArea#Styles#Default instead.
 */
#define BandedDockAreaStyle	ChildStyle

/**
 * @deprecated Use BasicDockArea::Styles::Default instead.
 */
#define BasicDockAreaStyle	ChildStyle

/**
 * @deprecated Use Button::Styles::Default instead.
 */
#define ButtonStyle		(ChildStyle + FocusableStyle + Vaca::Style(BS_PUSHBUTTON, 0))

/**
 * @deprecated Use CheckBox::Styles::Default instead.
 */
#define CheckBoxStyle		(ChildStyle + FocusableStyle + Vaca::Style(BS_AUTOCHECKBOX, 0))

#define CommonComboBoxStyle     (ChildStyle +				\
				 FocusableStyle +			\
				 ClientEdgeStyle +			\
				 VerticalScrollStyle +			\
				 Vaca::Style(CBS_HASSTRINGS |		\
					     CBS_AUTOHSCROLL, 0))

/**
 * @deprecated Use ComboBox::Styles::Default instead.
 */
#define ComboBoxStyle		(CommonComboBoxStyle + Vaca::Style(CBS_DROPDOWNLIST, 0))

/**
 * @deprecated Use ComboBox::Styles::Simple instead.
 */
#define SimpleComboBoxStyle	(CommonComboBoxStyle + Vaca::Style(CBS_SIMPLE, 0))

/**
 * @deprecated Use ComboBox::Styles::Editable instead.
 */
#define EditComboBoxStyle	(CommonComboBoxStyle + Vaca::Style(CBS_DROPDOWN, 0))

/**
 * @deprecated Use ComboBox::Styles::Default instead.
 */
#define ListComboBoxStyle	(ComboBoxStyle)

/**
 * @deprecated Use CustomButton::Styles::Default instead.
 */
#define CustomButtonStyle	(ButtonStyle + Vaca::Style(BS_OWNERDRAW, 0))

/**
 * @deprecated Use CustomLabel::Styles::Default instead.
 */
#define CustomLabelStyle	(LabelStyle + Vaca::Style(SS_OWNERDRAW, 0))

/**
 * @deprecated Use Dialog::Styles::Modal instead.
 */
#define ModalDialogStyle	Vaca::Style(0, WS_EX_DLGMODALFRAME)

/**
 * @deprecated Use Dialog::Styles::Default instead.
 */
#define DialogStyle		(WithCaptionFrameStyle +		\
				 WithSystemMenuFrameStyle +		\
				 ModalDialogStyle +			\
				 Vaca::Style(WS_POPUP | DS_CONTROL, 0))

/**
 * @deprecated Use DockArea::Styles::Default instead.
 */
#define DockAreaStyle		ChildStyle

/**
 * @deprecated Use DockBar::Styles::Default instead.
 */
#define DockBarStyle		ChildStyle

/**
 * @deprecated Use DockFrame::Styles::Default instead.
 */
#define DockFrameStyle		(WithCaptionFrameStyle +		\
				 WithSystemMenuFrameStyle +		\
				 ResizableFrameStyle +			\
				 Vaca::Style(WS_POPUP, WS_EX_TOOLWINDOW))

/**
 * @deprecated Use TextEdit::Styles::RightAligned instead.
 */
#define RightAlignedEditStyle		Vaca::Style(ES_RIGHT, 0)

/**
 * @deprecated Use TextEdit::Styles::ReadOnly instead.
 */
#define ReadOnlyEditStyle		Vaca::Style(ES_READONLY, 0)

/**
 * @deprecated Use TextEdit::Styles::AutoHorizontalScroll instead.
 */
#define AutoHorizontalScrollEditStyle	Vaca::Style(ES_AUTOHSCROLL, 0)

/**
 * @deprecated Use TextEdit::Styles::AutoVerticalScroll instead.
 */
#define AutoVerticalScrollEditStyle	Vaca::Style(ES_AUTOVSCROLL, 0)

/**
 * @deprecated Use TextEdit::Styles::Default with TextEdit widget instead.
 */
#define EditStyle		(ChildStyle +				\
				 FocusableStyle +			\
				 ClientEdgeStyle +			\
				 AutoHorizontalScrollEditStyle)

/**
 * @deprecated Use TextEdit::Styles::Password with TextEdit widget instead.
 */
#define PasswordEditStyle	(EditStyle + Vaca::Style(ES_PASSWORD, 0))

/**
 * @deprecated Use TextEdit::Styles::TextArea with TextEdit widget instead.
 */
#define MultilineEditStyle	(EditStyle +			\
				 Vaca::Style(ES_MULTILINE, 0))

/**
 * @deprecated Use Frame::Styles::WithCaption instead.
 */
#define WithCaptionFrameStyle		(Vaca::Style(WS_CAPTION, 0))

/**
 * @deprecated Use Frame::Styles::WithSystemMenu instead.
 */
#define WithSystemMenuFrameStyle	(Vaca::Style(WS_SYSMENU, 0))

/**
 * @deprecated Use Frame::Styles::Minimizable instead.
 */
#define MinimizableFrameStyle		(Vaca::Style(WS_MINIMIZEBOX, 0))

/**
 * @deprecated Use Frame::Styles:: instead.
 */
#define MaximizableFrameStyle		(Vaca::Style(WS_MAXIMIZEBOX, 0))

/**
 * @deprecated Use Frame::Styles::Resizable instead.
 */
#define ResizableFrameStyle		(Vaca::Style(WS_SIZEBOX, 0))

/**
 * @deprecated Use Frame::Styles::InitiallyMinimized instead.
 */
#define InitiallyMinimizedFrameStyle	(Vaca::Style(WS_MINIMIZE, 0))

/**
 * @deprecated Use Frame::Styles::InitiallyMaximized instead.
 */
#define InitiallyMaximizedFrameStyle	(Vaca::Style(WS_MAXIMIZE, 0))

/**
 * @deprecated Use Frame::Styles::Default instead.
 */
#define FrameStyle			(ContainerStyle +		\
					 WithCaptionFrameStyle +	\
					 WithSystemMenuFrameStyle +	\
					 MinimizableFrameStyle +	\
					 MaximizableFrameStyle +	\
					 ResizableFrameStyle)

/**
 * @deprecated Use GroupBox::Styles::Default instead.
 */
#define GroupBoxStyle		(ChildStyle +				\
				 ContainerStyle +			\
				 Vaca::Style(BS_GROUPBOX, 0))

/**
 * @deprecated Use Label::Styles::Default instead.
 */
#define LabelStyle		(ChildStyle + Vaca::Style(SS_NOTIFY, 0))

/**
 * @deprecated Use Label::Styles::Simple instead.
 */
#define SimpleLabelStyle	(Vaca::Style(SS_SIMPLE, 0))

/**
 * @deprecated Use Label::Styles::WordEllipsis instead.
 */
#define WordEllipsisLabelStyle	(Vaca::Style(SS_WORDELLIPSIS, 0))

/**
 * @deprecated Use Label::Styles::EndEllipsis instead.
 */
#define EndEllipsisLabelStyle	(Vaca::Style(SS_ENDELLIPSIS, 0))

/**
 * @deprecated Use Label::Styles::PathEllipsis instead.
 */
#define PathEllipsisLabelStyle	(Vaca::Style(SS_PATHELLIPSIS, 0))

/**
 * @deprecated Use LinkLabel::Styles::Default instead.
 */
#define LinkLabelStyle		(CustomLabelStyle + FocusableStyle)

/**
 * @deprecated Use ListBox::Styles::Default instead.
 */
#define ListBoxStyle		(ChildStyle +				\
				 FocusableStyle +			\
				 ClientEdgeStyle +			\
				 ScrollStyle +				\
				 Vaca::Style(LBS_NOINTEGRALHEIGHT | LBS_NOTIFY, 0))

/**
 * @deprecated Use DragListBox::Styles::Default instead.
 */
#define DragListBoxStyle	ListBoxStyle

/**
 * @deprecated Use ListView::Styles::Default instead.
 */
#define ListViewStyle			(ChildStyle + FocusableStyle + ClientEdgeStyle)

/**
 * @deprecated Use ListView::Styles::SingleSelection instead.
 */
#define SingleSelectionListViewStyle	(Vaca::Style(LVS_SINGLESEL, 0))

/**
 * @deprecated Use MdiChild::Styles::Default instead.
 */
#define MdiChildStyle		(FrameStyle +				\
				 Vaca::Style(WS_CHILD, WS_EX_MDICHILD))

/**
 * @deprecated Use MdiClient::Styles::Default instead.
 */
#define MdiClientStyle		(ChildStyle +				\
				 ScrollStyle +				\
				 ClientEdgeStyle +			\
				 ContainerStyle)

/**
 * @deprecated Use MdiFrame::Styles::Default instead.
 */
#define MdiFrameStyle		(FrameStyle)

/**
 * @deprecated Use Panel::Styles::Default instead.
 */
#define PanelStyle		(ChildStyle + ContainerStyle)

/**
 * @deprecated Use ProgressBar::Styles::Default instead.
 */
#define ProgressBarStyle		(ChildStyle)

/**
 * @deprecated Use ProgressBar::Styles::Smooth instead.
 */
#define SmoothProgressBarStyle		(Vaca::Style(PBS_SMOOTH, 0))

/**
 * @deprecated Use ProgressBar::Styles::Vertical instead.
 */
#define VerticalProgressBarStyle	(Vaca::Style(PBS_VERTICAL, 0))

/**
 * @deprecated Use RadioButton::Styles::Default instead.
 */
#define RadioButtonStyle	(ChildStyle +			\
				 FocusableStyle +		\
				 Vaca::Style(BS_RADIOBUTTON, 0))

/**
 * @deprecated Use SciEdit::Styles::Default instead.
 */
#define SciEditorStyle		(ChildStyle + ScrollStyle)

/**
 * @deprecated Use Separator::Styles::Default instead.
 */
#define SeparatorStyle		(ChildStyle + VisibleStyle +		\
				 Vaca::Style(SS_NOTIFY | SS_SUNKEN, 0))

/**
 * @deprecated Use Slider::Styles::Default instead.
 */
#define SliderStyle		(ChildStyle +				\
				 FocusableStyle +			\
				 Vaca::Style(TBS_HORZ | TBS_BOTH |	\
					     TBS_NOTICKS | TBS_AUTOTICKS, 0))

/**
 * @deprecated Use SpinButton::Styles::Default instead.
 */
#define SpinButtonStyle		(ChildStyle + Vaca::Style(UDS_SETBUDDYINT | \
							  UDS_ALIGNRIGHT | \
							  UDS_AUTOBUDDY | \
							  UDS_ARROWKEYS | \
							  UDS_NOTHOUSANDS, 0))

/**
 * @deprecated Use SpinButton::Styles::Horizontal instead.
 */
#define HorizontalSpinButtonStyle	(Vaca::Style(UDS_HORZ, 0))

/**
 * @deprecated Use SpinButton::Styles::HotTrack instead.
 */
#define HotTrackSpinButtonStyle		(Vaca::Style(UDS_HOTTRACK, 0))

/**
 * @deprecated Use Spinner::Styles::Default instead.
 */
#define SpinnerStyle		(ChildStyle + ContainerStyle)

/**
 * @deprecated Use StatusBar::Styles::Default instead.
 */
#define StatusBarStyle		(ChildStyle + Vaca::Style(SBARS_SIZEGRIP, 0))

/**
 * @deprecated Use TabBase::Styles::Default instead.
 */
#define TabBaseStyle		ChildStyle

/**
 * @deprecated Use Tab::Styles::Default instead.
 */
#define TabStyle		TabBaseStyle

/**
 * @deprecated Use TabPage::Styles::Default instead.
 */
#define TabPageStyle		PanelStyle

/**
 * @deprecated Use ToogleButton::Styles::Default instead.
 */
#define ToggleButtonStyle	(ChildStyle +				\
				 FocusableStyle +			\
				 Vaca::Style(BS_AUTOCHECKBOX | BS_PUSHLIKE, 0))

#if (_WIN32_IE >= 0x0300)

  /**
   * @deprecated Use ToolSet::Styles::Default instead.
   */
  #define ToolSetStyle	(ChildStyle + Vaca::Style(CCS_NODIVIDER | CCS_NOPARENTALIGN | \
						  CCS_NOMOVEY | CCS_NORESIZE | \
						  CCS_NOMOVEX, 0))

#else

  #define ToolSetStyle	(ChildStyle + Vaca::Style(CCS_NODIVIDER | CCS_NOPARENTALIGN | \
						  CCS_NOMOVEY | CCS_NORESIZE, 0))

#endif

/**
 * @deprecated Use ToolSet::Styles::Flat instead.
 */
#define FlatToolSetStyle (Vaca::Style(TBSTYLE_FLAT, 0))

/**
 * @deprecated Use ToolBar::Styles::Default instead.
 */
#define ToolBarStyle	DockBarStyle

/**
 * @deprecated Use TreeView::Styles::Default instead.
 */
#define TreeViewStyle					\
  (ChildStyle +						\
   FocusableStyle +					\
   ClientEdgeStyle +					\
   Vaca::Style(TVS_LINESATROOT | TVS_HASBUTTONS |	\
	       TVS_HASLINES | TVS_SHOWSELALWAYS |	\
	       TVS_DISABLEDRAGDROP, 0))

/**
 * @deprecated Use TreeView::Styles::EditLabel instead.
 */
#define EditLabelTreeViewStyle	(Vaca::Style(TVS_EDITLABELS, 0))

/**
 * @deprecated Use TreeView::Styles::ShowSelectionAlways instead.
 */
#define ShowSelectionAlwaysTreeViewStyle	\
  (Vaca::Style(TVS_SHOWSELALWAYS, 0))

/**
 * @deprecated Use Widget::Styles::None instead.
 */
#define NoStyle			(Vaca::Style(0, 0))

/**
 * @deprecated Use Widget::Styles::Visible instead.
 */
#define VisibleStyle            (Vaca::Style(WS_VISIBLE, 0))

/**
 * @deprecated Use Widget::Styles::Child instead.
 */
#define ChildStyle              (Vaca::Style(WS_CHILD | WS_VISIBLE, 0))

/**
 * @deprecated Use Widget::Styles::Focusable instead.
 */
#define FocusableStyle		(Vaca::Style(WS_TABSTOP, 0))

/**
 * @deprecated Use Widget::Styles::Scroll instead.
 */
#define ScrollStyle		(Vaca::Style(WS_HSCROLL | WS_VSCROLL, 0))

/**
 * @deprecated Use Widget::Styles::HorizontalScroll instead.
 */
#define HorizontalScrollStyle	(Vaca::Style(WS_HSCROLL, 0))

/**
 * @deprecated Use Widget::Styles::VerticalScroll instead.
 */
#define VerticalScrollStyle	(Vaca::Style(WS_VSCROLL, 0))

/**
 * @deprecated Use Widget::Styles::ClientEdge instead.
 */
#define ClientEdgeStyle         (Vaca::Style(0, WS_EX_CLIENTEDGE))

/**
 * @deprecated Use Widget::Styles::Container instead.
 */
#define ContainerStyle		(Vaca::Style(WS_CLIPCHILDREN, WS_EX_CONTROLPARENT))

/**
 * @deprecated Use Widget::Styles::AcceptFiles instead.
 */
#define AcceptFilesStyle	(Vaca::Style(0, WS_EX_ACCEPTFILES))

// ===================================================================
// Old Widgets
// ===================================================================

/**
 * @deprecated You can create user controls derivating Widget class
 * directly.
 */
typedef Widget Panel;

/**
 * @deprecated Replaced by a TextEdit with the TextEdit::Styles::Default style.
 *
 * @code
 * TextEdit text1("...", &frame);
 * // or ...
 * TextEdit text2("...", &frame, TextEdit::Styles::Default);
 * @endcode
 */
class Edit : public TextEdit
{
public:
  Edit(const String& text, Widget* parent, Style style = EditStyle)
    : TextEdit(text, parent, style) { }
  virtual ~Edit() { }
};

/**
 * @deprecated Replaced by a TextEdit with the TextEdit::Styles::Password style.
 *
 * @code
 * TextEdit text("...", &frame, TextEdit::Styles::Password);
 * @endcode
 */
class PasswordEdit : public TextEdit
{
public:
  PasswordEdit(const String& text, Widget* parent, Style style = PasswordEditStyle)
    : TextEdit(text, parent, style) { }
  virtual ~PasswordEdit() { }

  Character getPasswordCharacter() { return getPasswordChar(); }
  void setPasswordCharacter(Character passwordChar) {
    setPasswordChar(passwordChar);
  }
};

/**
 * @deprecated Replaced by a TextEdit with the TextEdit::Styles::TextArea style.
 *
 * @code
 * TextEdit text("...", &frame, TextEdit::Styles::TextArea);
 * @endcode
 */
class MultilineEdit : public TextEdit
{
public:
  MultilineEdit(const String& text, Widget* parent, Style style = MultilineEditStyle)
    : TextEdit(text, parent, style) { }
  virtual ~MultilineEdit() { }
};

} // namespace Vaca

#endif // VACA_BACKWARD_H
