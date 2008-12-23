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

#include "Vaca/Vaca.h"

using namespace Vaca;

// ===================================================================
// Base Styles
// ===================================================================

/**
 * Without style.
 */
const Style Widget::Styles::None(0, 0);

/**
 * A widget that should be shown to the user.
 *
 * @win32
 *   It is WS_VISIBLE.
 * @endwin32
 */
const Style Widget::Styles::Visible(WS_VISIBLE, 0);

/**
 * Basic style for every child that will be inside a container.
 * 
 * @win32
 *   It is WS_CHILD | WS_VISIBLE.
 * @endwin32
 */
const Style Widget::Styles::Child(WS_CHILD | WS_VISIBLE, 0);

/**
 * Indicates that a Widget can receive the focus in Dialogs.
 *
 * @win32
 *   It is WS_TABSTOP.
 * @endwin32
 */
const Style Widget::Styles::Focusable(WS_TABSTOP, 0);

/**
 * When the widget needs both scroll bars (horizontal and vertical).
 *
 * @win32
 *   It is WS_HSCROLL | WS_VSCROLL.
 * @endwin32
 */
const Style Widget::Styles::Scroll(WS_HSCROLL | WS_VSCROLL, 0);

/**
 * When the widget needs the horizontal scroll bar.
 *
 * @win32
 *   It is WS_HSCROLL.
 * @endwin32
 */
const Style Widget::Styles::HorizontalScroll(WS_HSCROLL, 0);

/**
 * When the widget needs the veritcal scroll bar.
 *
 * @win32
 *   It is WS_VSCROLL.
 * @endwin32
 */
const Style Widget::Styles::VerticalScroll(WS_VSCROLL, 0);

/**
 * This style makes the widget to have an edge in its client area.
 * E.g.: This style is used by text-fields (TextEdit) to enclose the
 * typed text in a box for example.
 *
 * @win32
 *   It is WS_EX_CLIENTEDGE.
 * @endwin32
 */
const Style Widget::Styles::ClientEdge(0, WS_EX_CLIENTEDGE);

/**
 * Style used for container widgets, that is, widget that contains
 * children.
 *
 * @win32
 *   It is WS_CLIPCHILDREN | WS_EX_CONTROLPARENT.
 * @endwin32
 */
const Style Widget::Styles::Container(WS_CLIPCHILDREN, WS_EX_CONTROLPARENT);

/**
 * Use this style if the user should be able to drop files on your widget.
 *
 * @warning Using this style is the only way to receive the Widget#onDropFiles event.
 *
 * @win32
 *   It is WS_EX_ACCEPTFILES.
 * @endwin32
 */
const Style Widget::Styles::AcceptFiles(0, WS_EX_ACCEPTFILES);

/**
 * Default style for Panel widget.
 */
const Style Panel::Styles::Default
  (Widget::Styles::Child |
   Widget::Styles::Container);

// ===================================================================
// Labels
// ===================================================================

/**
 * Default style for Label.
 *
 * @win32
 *   It is WS_CHILD | WS_VISIBLE | SS_NOTIFY
 * @endwin32
 */
const Style Label::Styles::Default =
  // (WS_CHILD | WS_VISIBLE | SS_NOTIFY, 0);
  Widget::Styles::Child |
  Style(SS_NOTIFY, 0);

/**
 * A simple label has left-alignment and is not wrapped in multiple lines.
 *
 * @win32
 *   @msdn{SS_SIMPLE}
 * @endwin32
 */
const Style Label::Styles::Simple(SS_SIMPLE, 0);

/**
 * If the label is too long, ellipsis ("...") is used at the end of
 * each word instead of wrap them.
 *
 * @win32
 *   @msdn{SS_WORDELLIPSIS}
 * @endwin32
 */
const Style Label::Styles::WordEllipsis(SS_WORDELLIPSIS, 0);

/**
 * If the label is too long, ellipsis ("...") is used at the end of
 * the text. The text is not wrapped.
 *
 * @win32
 *   @msdn{SS_ENDELLIPSIS}
 * @endwin32
 */
const Style Label::Styles::EndEllipsis(SS_ENDELLIPSIS, 0);

/**
 * If the label is too long, ellipsis ("...") is used in the middle of
 * the text. It is useful to show file name paths, because the
 * ellipsis is used trying to show the file name part
 * (String#getFileName). The text is not wrapped.
 *
 * @win32
 *   @msdn{SS_PATHELLIPSIS}
 * @endwin32
 */
const Style Label::Styles::PathEllipsis(SS_PATHELLIPSIS, 0);

/**
 * Default style for CustomLabel.
 *
 * It contains the following styles:
 * @li Widget::Styles::Child
 *
 * @win32
 *   It is WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_OWNERDRAW
 * @endwin32
 */
const Style CustomLabel::Styles::Default =
  // (WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_OWNERDRAW, 0);
  Label::Styles::Default |
  Style(SS_OWNERDRAW, 0);

/**
 * Default style for LinkLabel widget.
 * 
 * It contains the following styles:
 * @li CustomLabel::Styles::Default
 * @li Widget::Styles::Focusable
 *
 * @win32
 *   It is 
 * @endwin32
 */
const Style LinkLabel::Styles::Default =
  CustomLabel::Styles::Default |
  Widget::Styles::Focusable;

// ===================================================================
// Buttons
// ===================================================================

/**
 * Default style for Button widget.
 * 
 * It contains the following styles:
 * @li Widget::Styles::Child
 * @li Widget::Styles::Focusable
 *
 * @win32
 *   It is WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON
 * @endwin32
 */
// const Style Button::Styles::Default
//   (WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 0);
const Style Button::Styles::Default =
  Widget::Styles::Child |
  Widget::Styles::Focusable |
  Style(BS_PUSHBUTTON, 0);

/**
 * Default style for CheckBox widget.
 * 
 * It contains the following styles:
 * @li Widget::Styles::Child
 * @li Widget::Styles::Focusable
 *
 * @win32
 *   It is WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX
 * @endwin32
 */
// const Style CheckBox::Styles::Default
//   (WS_CHILD | WS_VISIBLE |
//    WS_TABSTOP |
//    BS_AUTOCHECKBOX, 0);
const Style CheckBox::Styles::Default =
  Widget::Styles::Child |
  Widget::Styles::Focusable |
  Style(BS_AUTOCHECKBOX, 0);

const Style RadioButton::Styles::Default =
  Widget::Styles::Child |
  Widget::Styles::Focusable |
  Style(BS_RADIOBUTTON, 0);

const Style ToggleButton::Styles::Default =
  Widget::Styles::Child |
  Widget::Styles::Focusable |
  Style(BS_AUTOCHECKBOX |
	BS_PUSHLIKE, 0);

/**
 * Default style for CustomButton.
 *
 * @win32
 *   WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_OWNERDRAW
 * @endwin32
 */
const Style CustomButton::Styles::Default
  // (WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_OWNERDRAW, 0);
  (Button::Styles::Default |
   Style(BS_OWNERDRAW, 0));

// ===================================================================
// TextEdit
// ===================================================================

const Style TextEdit::Styles::RightAligned(ES_RIGHT, 0);

const Style TextEdit::Styles::ReadOnly(ES_READONLY, 0);

const Style TextEdit::Styles::AutoHorizontalScroll(ES_AUTOHSCROLL, 0);

const Style TextEdit::Styles::AutoVerticalScroll(ES_AUTOVSCROLL, 0);

/**
 * Default style for TextEdit widget.
 * 
 * It contains the following styles:
 * @li Widget::Styles::Child
 * @li Widget::Styles::Focusable
 * @li Widget::Styles::ClientEdge
 * @li TextEdit::Styles::AutoHorizontalScroll
 *
 * @win32
 *   It is WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | WS_EX_CLIENTEDGE
 * @endwin32
 */
const Style TextEdit::Styles::Default =
  // (WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE);
  Widget::Styles::Child |
  Widget::Styles::Focusable |
  Widget::Styles::ClientEdge |
  TextEdit::Styles::AutoHorizontalScroll;

const Style TextEdit::Styles::Password =
  TextEdit::Styles::Default |
  Style(ES_PASSWORD, 0);

const Style TextEdit::Styles::TextArea =
  TextEdit::Styles::Default |
  Style(ES_MULTILINE, 0);

const Style SciEdit::Styles::Default =
  Widget::Styles::Child +
  Widget::Styles::Scroll;

// ===================================================================
// ListBox
// ===================================================================

/**
 * Default style for ListBox: a visible child (ChildVisible), with tab
 * stop (FocusableStyle), with client-edges (ClientEdgeStyle), and
 * scroll bars (ScrollStyle), and LBS_NOINTEGRALHEIGHT | LBS_NOTIFY.
 */
const Style ListBox::Styles::Default =
  Widget::Styles::Child |
  Widget::Styles::Focusable |
  Widget::Styles::ClientEdge |
  Widget::Styles::Scroll |
  Style(LBS_NOINTEGRALHEIGHT | LBS_NOTIFY, 0);

const Style DragListBox::Styles::Default = ListBox::Styles::Default;

// ===================================================================
// ComboBox
// ===================================================================

/**
 * Default style for ComboBox widget. It is the classic ComboBox in
 * which the user can select only items in the list (but can't edit
 * text nor items).
 * 
 * It contains the following styles:
 * @li Widget::Styles::Child
 * @li Widget::Styles::Focusable
 * @li Widget::Styles::VerticalScroll
 * @li Widget::Styles::ClientEdge
 *
 * Example: 
 * @code
 * Frame frame(...);
 * // Use:
 * ComboBox combo1(&frame);
 * // or:
 * ComboBox combo2(&frame, ComboBox::Styles::Default);
 * @endcode
 *
 * @win32
 *   It is WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL,
 *   CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST |
 *   WS_EX_CLIENTEDGE
 * @endwin32
 */
const Style ComboBox::Styles::Default =
  // (WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
  //  CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST,
  //  WS_EX_CLIENTEDGE);
  Widget::Styles::Child |
  Widget::Styles::Focusable |
  Widget::Styles::ClientEdge |
  Widget::Styles::VerticalScroll |
  Style(CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST, 0);

/**
 * A simple ComboBox (shows the list-box below the combo-box).
 *
 * @warning A ListBox is recommended instead of this kind of ComboBox.
 *
 * Example: 
 * @code
 * Frame frame(...);
 * ComboBox combo(&frame, ComboBox::Styles::Simple);
 * @endcode
 * 
 * @win32
 *   It is WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
 *   CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_SIMPLE | WS_EX_CLIENTEDGE
 * @endwin32
 */
const Style ComboBox::Styles::Simple =
  // (WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
  //  CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_SIMPLE,
  //  WS_EX_CLIENTEDGE);
  Widget::Styles::Child |
  Widget::Styles::Focusable |
  Widget::Styles::ClientEdge |
  Widget::Styles::VerticalScroll |
  Style(CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_SIMPLE, 0);

/**
 * Special style for ComboBox so the user can enter any text (not only
 * the items in the list).
 *
 * Example: 
 * @code
 * Frame frame(...);
 * ComboBox combo(&frame, ComboBox::Styles::Editable);
 * @endcode
 * 
 * @win32
 *   It is WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
 *   CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_DROPDOWN | WS_EX_CLIENTEDGE
 * @endwin32
 */
const Style ComboBox::Styles::Editable =
  Widget::Styles::Child |
  Widget::Styles::Focusable |
  Widget::Styles::ClientEdge |
  Widget::Styles::VerticalScroll |
  Style(CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_DROPDOWN, 0);

// ===================================================================
// Frames and Dialogs
// ===================================================================

/**
 * This Frame is a window that as a title bar.
 * 
 * @win32
 *   It is WS_CAPTION
 * @endwin32
 */
const Style Frame::Styles::WithCaption(WS_CAPTION, 0);

/**
 * @win32
 *   It is WS_SYSMENU
 * @endwin32
 */
const Style Frame::Styles::WithSystemMenu(WS_SYSMENU, 0);

/**
 * @win32
 *   It is WS_MINIMIZEBOX
 * @endwin32
 */
const Style Frame::Styles::Minimizable(WS_MINIMIZEBOX, 0);

/**
 * @win32
 *   It is WS_MAXIMIZEBOX
 * @endwin32
 */
const Style Frame::Styles::Maximizable(WS_MAXIMIZEBOX, 0);

/**
 * The user will be able to change the window's size.
 * 
 * @win32
 *   It is WS_SIZEBOX
 * @endwin32
 */
const Style Frame::Styles::Resizable(WS_SIZEBOX, 0);

/**
 * The window can be minimized.
 *
 * @win32
 *   It is WS_MINIMIZE
 * @endwin32
 */
const Style Frame::Styles::InitiallyMinimized(WS_MINIMIZE, 0);

/**
 * The window can be maximized.
 *
 * @win32
 *   It is WS_MAXIMIZE
 * @endwin32
 */
const Style Frame::Styles::InitiallyMaximized(WS_MAXIMIZE, 0);

/**
 * Default style for Frame widget.
 * 
 * It contains the following styles:
 * @li Widget::Styles::Container
 * @li Frame::Styles::WithCaption
 * @li Frame::Styles::WithSystemMenu
 * @li Frame::Styles::Minimizable
 * @li Frame::Styles::Maximizable
 * @li Frame::Styles::Resizable
 * 
 * @win32
 *   It is WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU |
 *   WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX |
 *   WS_EX_CONTROLPARENT
 * @endwin32
 */
const Style Frame::Styles::Default =
  // (WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU |
  //  WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX,
  //  WS_EX_CONTROLPARENT);
  Widget::Styles::Container |
  Frame::Styles::WithCaption |
  Frame::Styles::WithSystemMenu |
  Frame::Styles::Minimizable |
  Frame::Styles::Maximizable |
  Frame::Styles::Resizable;

/**
 * This style is activated by default in Dialogs.
 *
 * @win32
 *   It is WS_EX_DLGMODALFRAME
 * @endwin32
 */
const Style Dialog::Styles::Modal(0, WS_EX_DLGMODALFRAME);

/**
 * Default style for Dialogs.
 * 
 * It contains the following styles:
 * @li Frame::Styles::WithCaption
 * @li Frame::Styles::WithSystemMenu
 * @li Dialog::Styles::Modal
 *
 * @win32
 *   It is WS_CAPTION | WS_SYSMENU | WS_POPUP | DS_CONTROL | WS_EX_DLGMODALFRAME
 * @endwin32
 */
const Style Dialog::Styles::Default =
  // (WS_CAPTION | WS_SYSMENU | WS_POPUP | DS_CONTROL,
  //  WS_EX_DLGMODALFRAME);
  Frame::Styles::WithCaption |
  Frame::Styles::WithSystemMenu |
  Dialog::Styles::Modal |
  Style(WS_POPUP | DS_CONTROL, 0);

// ===================================================================
// Separators
// ===================================================================

/**
 * Default style for GroupBox widget.
 * 
 * It contains the following styles:
 * @li Widget::Styles::Child
 * @li Widget::Styles::Container
 *
 * @win32
 *   It is WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN |
 *   BS_GROUPBOX | WS_EX_CONTROLPARENT
 * @endwin32
 */
const Style GroupBox::Styles::Default =
  // (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | BS_GROUPBOX,
  //  WS_EX_CONTROLPARENT);
  Widget::Styles::Child |
  Widget::Styles::Container |
  Style(BS_GROUPBOX, 0);

const Style Separator::Styles::Default =
  Widget::Styles::Child |
  Widget::Styles::Visible |
  Style(SS_NOTIFY | SS_SUNKEN, 0);

const Style SplitBar::Styles::Default =
  Widget::Styles::Child |
  Widget::Styles::Container;

// ===================================================================
// Sliders
// ===================================================================

const Style Slider::Styles::Default =
  Widget::Styles::Child +
  Widget::Styles::Focusable +
  Style(TBS_HORZ | TBS_BOTH |
	TBS_NOTICKS | TBS_AUTOTICKS, 0);

const Style SpinButton::Styles::Default =
  Widget::Styles::Child |
  Style(UDS_SETBUDDYINT |
	UDS_ALIGNRIGHT |
	UDS_AUTOBUDDY |
	UDS_ARROWKEYS |
	UDS_NOTHOUSANDS, 0);

const Style SpinButton::Styles::Horizontal(UDS_HORZ, 0);

const Style SpinButton::Styles::HotTrack(UDS_HOTTRACK, 0);

const Style Spinner::Styles::Default =
  Widget::Styles::Child +
  Widget::Styles::Container;

// ===================================================================
// Tabs
// ===================================================================

const Style TabBase::Styles::Default(Widget::Styles::Child);

const Style Tab::Styles::Default(TabBase::Styles::Default);

const Style TabPage::Styles::Default(Panel::Styles::Default);

// ===================================================================
// MDI
// ===================================================================

const Style MdiChild::Styles::Default =
  Frame::Styles::Default |
  Style(WS_CHILD, WS_EX_MDICHILD);

const Style MdiClient::Styles::Default =
  Widget::Styles::Child |
  Widget::Styles::Scroll |
  Widget::Styles::ClientEdge |
  Widget::Styles::Container;

const Style MdiFrame::Styles::Default = Frame::Styles::Default;

// ===================================================================
// Status Bars
// ===================================================================

const Style StatusBar::Styles::Default =
  Widget::Styles::Child |
  Style(SBARS_SIZEGRIP, 0);

const Style ProgressBar::Styles::Default = Widget::Styles::Child;

const Style ProgressBar::Styles::Smooth(PBS_SMOOTH, 0);

const Style ProgressBar::Styles::Vertical(PBS_VERTICAL, 0);

// ===================================================================
// Tool Bars
// ===================================================================

/**
 * Default style for DockBar widget.
 * 
 * It contains the following styles:
 * @li Widget::Styles::Child
 * 
 * @win32
 *   It is WS_CHILD | WS_VISIBLE
 * @endwin32
 */
const Style DockBar::Styles::Default = Widget::Styles::Child;
 // (WS_CHILD | WS_VISIBLE, 0);

/**
 * Default style for DockArea widget.
 * 
 * It contains the following styles:
 * @li Widget::Styles::Child
 * 
 * @win32
 *   It is WS_CHILD | WS_VISIBLE
 * @endwin32
 */
const Style DockArea::Styles::Default = Widget::Styles::Child;

const Style ToolSet::Styles::Default =
  Widget::Styles::Child |
  Style(CCS_NODIVIDER | CCS_NOPARENTALIGN |
	CCS_NOMOVEY | CCS_NORESIZE
#if (_WIN32_IE >= 0x0300)
	| CCS_NOMOVEX
#endif
	, 0);

const Style ToolSet::Styles::Flat(TBSTYLE_FLAT, 0);

const Style ToolBar::Styles::Default = DockBar::Styles::Default;

/**
 * Default style for BandedDockArea widget.
 * 
 * It contains the following styles:
 * @li Widget::Styles::Child
 *
 * @win32
 *   It is WS_CHILD | WS_VISIBLE
 * @endwin32
 */
// const Style BandedDockArea::Styles::Default(WS_CHILD | WS_VISIBLE, 0);
const Style BandedDockArea::Styles::Default =
  Widget::Styles::Child;

/**
 * Default style for BasicDockArea widget.
 * 
 * It contains the following styles:
 * @li Widget::Styles::Child
 *
 * @win32
 *   It is WS_CHILD | WS_VISIBLE
 * @endwin32
 */
// const Style BasicDockArea::Styles::Default(WS_CHILD | WS_VISIBLE, 0);
const Style BasicDockArea::Styles::Default =
  Widget::Styles::Child;

const Style ReBar::Styles::Default =
  Widget::Styles::Child |
  Widget::Styles::Container |
  Style(WS_CLIPSIBLINGS |
	RBS_BANDBORDERS
#if (_WIN32_IE >= 0x0400)
	| RBS_DBLCLKTOGGLE
#endif
	, 0);

/**
 * Default style for DockFrame widget.
 *
 * It contains the following styles:
 * @li Frame::Styles::WithCaption
 * @li Frame::Styles::WithSystemMenu
 * @li Frame::Styles::Resizable
 *
 * @win32
 *   It is WS_CAPTION | WS_SYSMENU | WS_SIZEBOX | WS_POPUP | WS_EX_TOOLWINDOW
 * @endwin32
 */
const Style DockFrame::Styles::Default =
  // (WS_CAPTION | WS_SYSMENU | WS_SIZEBOX | WS_POPUP, WS_EX_TOOLWINDOW);
  Frame::Styles::WithCaption |
  Frame::Styles::WithSystemMenu |
  Frame::Styles::Resizable |
  Style(WS_POPUP, WS_EX_TOOLWINDOW);

// ===================================================================
// ListView
// ===================================================================

const Style ListView::Styles::Default =
  Widget::Styles::Child |
  Widget::Styles::Focusable |
  Widget::Styles::ClientEdge;

const Style ListView::Styles::SingleSelection(LVS_SINGLESEL, 0);

// ===================================================================
// TreeView
// ===================================================================

const Style TreeView::Styles::Default =
  Widget::Styles::Child |
  Widget::Styles::Focusable |
  Widget::Styles::ClientEdge |
  Style(TVS_LINESATROOT | TVS_HASBUTTONS |
	TVS_HASLINES | TVS_SHOWSELALWAYS |
	TVS_DISABLEDRAGDROP, 0);

/**
 * With this style in a TreeeView the label of each TreeNode can be edited.
 */
const Style TreeView::Styles::EditLabel(TVS_EDITLABELS, 0);

const Style TreeView::Styles::ShowSelectionAlways(TVS_SHOWSELALWAYS, 0);
