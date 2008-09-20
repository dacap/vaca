namespace Vaca {

/**

@page page_classes Classes by Category

@li @ref page_classes_base
@li @ref page_classes_app
@li @ref page_classes_graphics
@li @ref page_classes_widgets
@li @ref page_classes_mess
@li @ref page_classes_cmds
@li @ref page_classes_menus
@li @ref page_classes_layout
@li @ref page_classes_thread
@li @ref page_classes_error
@li @ref page_classes_win32


@section page_classes_base Base

@beginOverviewTable
@titleRow{Classes}
  @itemRow{Component}
  @itemRow{NonCopyable}
  @itemRow{Referenceable}
  @itemRow{SmartPtr}
  @itemRow{String}
@endTable


@section page_classes_app Application and System

@beginOverviewTable
@titleRow{Classes}
  @itemRow{Application}
  @itemRow{Clipboard}
  @itemRow{HttpRequest}
  @itemRow{System}
  @itemRow{TimePoint}
  @itemRow{Timer}
@endTable


@section page_classes_graphics Graphics

@beginOverviewTable
@titleRow{Base Classes}
  @itemRow{Color}
  @itemRow{Point}
  @itemRow{Rect}
  @itemRow{Size}
@titleRow{Graphics Device}
  @itemRow{Graphics}
  @itemRow{ScreenGraphics}
@titleRow{Resources (Smart Pointers)}
  @itemRow{Brush}
  @itemRow{Cursor}
  @itemRow{Font}
  @itemRow{Icon}
  @itemRow{Image}
  @itemRow{ImageList}
  @itemRow{Pen}
  @itemRow{Region}
@titleRow{Helpers}
  @itemRow{FontMetrics}
  @itemRow{GraphicsPath}
@endTable


@section page_classes_widgets Widgets

@beginOverviewTable
@titleRow{Base}
  @itemRow{Widget}
@titleRow{Basic Widgets}
  @itemRow{Button}
  @itemRow{ButtonBase}
  @itemRow{CheckBox}
  @itemRow{ComboBox}
  @itemRow{DragListBox}
  @itemRow{Edit}
  @itemRow{Frame}
  @itemRow{GroupBox}
  @itemRow{Label}
  @itemRow{LinkLabel}
  @itemRow{ListBox}
  @itemRow{MultilineEdit}
  @itemRow{Panel}
  @itemRow{PasswordEdit}
  @itemRow{RadioButton}
  @itemRow{Separator}
  @itemRow{Slider}
  @itemRow{StatusBar}
  @itemRow{ToggleButton}
@titleRow{Basic Helpers}
  @itemRow{MakeWidgetRef}
  @itemRow{RadioGroup}
  @itemRow{ScrollInfo}
  @itemRow{Style}
@titleRow{Customizable Widgets}
  @itemRow{CustomButton}
  @itemRow{CustomLabel}
@titleRow{Dialogs}
  @itemRow{ColorDialog}
  @itemRow{CommonDialog}
  @itemRow{Dialog}
  @itemRow{FileDialog}
  @itemRow{FindTextDialog}
  @itemRow{FontDialog}
  @itemRow{MsgBox}
  @itemRow{OpenFileDialog}
  @itemRow{SaveFileDialog}
@titleRow{Toolbars}
  @itemRow{DockBar}
  @itemRow{DockFrame}
  @itemRow{ReBar}
  @itemRow{ToolBar}
  @itemRow{ToolSet}
@titleRow{Toolbar Helpers}
  @itemRow{BandedDockArea}
  @itemRow{BasicDockArea}
  @itemRow{DockArea}
  @itemRow{DockInfo}
  @itemRow{ToolButton}
@titleRow{Special Widgets}
  @itemRow{ListView}
  @itemRow{ProgressBar}
  @itemRow{SciEditor}
  @itemRow{SpinButton}
  @itemRow{Spinner}
  @itemRow{Tab}
  @itemRow{TabBase}
  @itemRow{TabPage}
  @itemRow{TreeView}
@titleRow{Special Helpers}
  @itemRow{ListItem}
  @itemRow{TreeNode}
@titleRow{MDI}
  @itemRow{MdiChild}
  @itemRow{MdiClient}
  @itemRow{MdiFrame}
  @itemRow{MdiListMenu}
@titleRow{DataGrids}
  @itemRow{DataGrid}
  @itemRow{DataGridColumn}
  @itemRow{DataGridSource}
  @itemRow{DataGridValue}
@endTable


@section page_classes_mess MESS

@beginOverviewTable
@titleRow{Messages}
  @itemRow{Message}
@titleRow{Events}
  @itemRow{CancelableEvent}
  @itemRow{CloseEvent}
  @itemRow{DropFilesEvent}
  @itemRow{Event}
  @itemRow{KeyEvent}
  @itemRow{ListViewEvent}
  @itemRow{MenuItemEvent}
  @itemRow{MouseEvent}
  @itemRow{SpinButtonEvent}
  @itemRow{TreeViewEvent}
@titleRow{Signals}
  @itemRow{Signal0}
  @itemRow{Signal1}
  @itemRow{Signal2}
@titleRow{Slots}
  @itemRow{Slot0}
  @itemRow{Slot1}
  @itemRow{Slot2}
@titleRow{Other}
  @itemRow{RefWrapper}
@endTable


@section page_classes_cmds Commands

@beginOverviewTable
@titleRow{Classes}
  @itemRow{Command}
  @itemRow{CommandsClient}
  @itemRow{SignalCommand}
@endTable


@section page_classes_menus Menus

@beginOverviewTable
@titleRow{Classes}
  @itemRow{Menu}
  @itemRow{MenuBar}
  @itemRow{MenuItem}
  @itemRow{MenuSeparator}
  @itemRow{PopupMenu}
@endTable


@section page_classes_layout Layout Managers

@beginOverviewTable
@titleRow{Base}
  @itemRow{Constraint}
  @itemRow{Layout}
@titleRow{Layouts}
  @itemRow{AnchorLayout}
  @itemRow{Bix}
  @itemRow{BoxLayout}
  @itemRow{ClientLayout}
@titleRow{Constraints}
  @itemRow{Anchor}
  @itemRow{BoxConstraint}
@endTable


@section page_classes_thread Multithreading Support

@beginOverviewTable
@titleRow{Classes}
  @itemRow{ConditionVariable}
  @itemRow{Mutex}
  @itemRow{ScopedLock}
  @itemRow{Thread}
@endTable


@section page_classes_error Error Handling

@beginOverviewTable
@titleRow{Base}
  @itemRow{Exception}
@titleRow{Specific}
  @itemRow{CreateConditionVariableException}
  @itemRow{CreateThreadException}
  @itemRow{CreateWidgetException}
  @itemRow{HttpRequestException}
  @itemRow{ParseException}
  @itemRow{RegisterException}
  @itemRow{ResourceException}
@endTable


@section page_classes_win32 Win32 Specific

These classes are internal to the Win32 implementation.

@beginOverviewTable
@titleRow{Base}
  @itemRow{GdiObject}
  @itemRow{ImageHandle}
  @itemRow{ResourceId}
@titleRow{Registration}
  @itemRow{Register}
  @itemRow{WidgetClassName}
@titleRow{Win32 Classes}
  @itemRow{DataGridClass}
  @itemRow{DialogClass}
  @itemRow{DockAreaClass}
  @itemRow{DockBarClass}
  @itemRow{DockFrameClass}
  @itemRow{FrameClass}
  @itemRow{MdiChildClass}
  @itemRow{PanelClass}
  @itemRow{SciRegister}
  @itemRow{SpinnerClass}
  @itemRow{TabPageClass}
  @itemRow{WidgetClass}
@endTable

*/

}
