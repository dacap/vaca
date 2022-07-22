// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_BASE_H
#define VACA_BASE_H

// If there are not a defined target (like VACA_WINDOWS)...
#if !defined(VACA_WINDOWS) &&			\
    !defined(VACA_ALLEGRO)
  // ...we define VACA_DEFAULT_PLATFORM to specify that the default
  // target will be used
  #define VACA_DEFAULT_TARGET
#endif

// If we are in Windows compiler
#if defined(_MSC_VER) || \
    defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || \
    defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
  #ifdef VACA_DEFAULT_TARGET
    #define VACA_WINDOWS
  #endif

  // We define a macro to know that we can use Windows API
  #define VACA_ON_WINDOWS
#else
  #ifdef VACA_DEFAULT_TARGET
    #define VACA_ALLEGRO
  #endif

  // We define a macro to know that we are in a Unix-like environment
  #define VACA_ON_UNIXLIKE
#endif

#ifdef _MSC_VER
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4275)
  #pragma warning(disable: 4355)
  #pragma warning(disable: 4996)
#endif

#include <algorithm>
#include <stdarg.h>
#include <string>

#ifdef VACA_WINDOWS
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <commctrl.h>

  #define VACA_MAIN()						\
      PASCAL WinMain(HINSTANCE hInstance,			\
		     HINSTANCE hPrevInstance,			\
		     LPSTR lpCmdLine,				\
		     int nCmdShow)
#else
  #define VACA_MAIN()						\
      main(int argc, char* argv[])
#endif

#include "vaca/Enum.h"

// memory leaks
#ifdef MEMORY_LEAK_DETECTOR
  #include "debug_new.h"
#endif

namespace vaca {

#define VACA_VERSION     0
#define VACA_SUB_VERSION 0
#define VACA_WIP_VERSION 8

// ============================================================
// DLL
// ============================================================

/**
   @def VACA_DLL
   @brief Used to export/import symbols to/from the dynamic library.
 */
#ifdef VACA_WINDOWS
  #ifdef VACA_STATIC
    #define VACA_DLL
  #else
    #ifdef VACA_SRC
      #define VACA_DLL __declspec(dllexport)
    #else
      #define VACA_DLL __declspec(dllimport)
    #endif
  #endif
#else
  #define VACA_DLL
#endif

// ============================================================
// CONVERSION
// ============================================================

// you have to use a specialization
template<typename To, typename From>
To convert_to(const From& from) {
  enum { not_supported = 1/(1 == 0) }; // static_assert(false)
}

// ============================================================
// ALGORITHMS
// ============================================================

/**
   Returns the minimum of @a x and @a y.

   @note It is just like @b std::min, but there are problems
	 in MSVC++ because a macro named @b min.

   @see max_value, clamp_value
*/
template<typename T>
inline T min_value(T x, T y)
{
  return x < y ? x: y;
}

/**
   Returns the maximum of @a x and @a y.

   @note It is just like @b std::max, but there are problems
	 in MSVC++ because a macro named @b max.

   @see min_value, clamp_value
*/
template<typename T>
inline T max_value(T x, T y)
{
  return x > y ? x: y;
}

/**
   Limits the posible values of @a x to the speficied range.

   If @a x is great than @a high, then @a high is returned,
   if @a x is less than @a low, then @a low is returned.
   In other case, @a x is in the range, and @a x is returned.

   @see min_value, max_value
*/
template<typename T>
inline T clamp_value(T x, T low, T high)
{
  return x > high ? high: (x < low ? low: x);
}

/**
   A wide character used in a String.

   @see String, @ref page_tn_008
*/
typedef wchar_t Char;

/**
   String type used through the Vaca API.

   It is a std::wstring.

   @see Char, @ref page_tn_008
*/
typedef std::wstring String;

/**
   An identifier for an application's Command.

   @see Widget#onCommand, Command
*/
typedef unsigned int CommandId;

/**
   An identifier for a Thread.
*/
typedef unsigned int ThreadId;

// ======================================================================

/**
   It's like a namespace for Orientation.

   @see Orientation
*/
struct OrientationEnum
{
  enum enumeration {
    Horizontal,
    Vertical
  };
  static const enumeration default_value = Horizontal;
};

/**
   Horizontal or vertical orientation.

   One of the following values:
   @li Orientation::Horizontal (default)
   @li Orientation::Vertical
*/
typedef Enum<OrientationEnum> Orientation;

// ======================================================================

/**
   It's like a namespace for TextAlign.

   @see TextAlign
*/
struct TextAlignEnum
{
  enum enumeration {
    Left,
    Center,
    Right
  };
  static const enumeration default_value = Left;
};

/**
   Horizontal alignment.

   One of the following values:
   @li TextAlign::Left (default)
   @li TextAlign::Center
   @li TextAlign::Right
*/
typedef Enum<TextAlignEnum> TextAlign;

// ======================================================================

/**
   It's like a namespace for VerticalAlign.

   @see VerticalAlign
*/
struct VerticalAlignEnum
{
  enum enumeration {
    Top,
    Middle,
    Bottom
  };
  static const enumeration default_value = Top;
};

/**
   Vertical alignment.

   One of the following values:
   @li VerticalAlign::Top
   @li VerticalAlign::Middle
   @li VerticalAlign::Bottom
*/
typedef Enum<VerticalAlignEnum> VerticalAlign;

// ======================================================================

/**
   It's like a namespace for Side.

   @see Side
*/
struct SideEnum
{
  enum enumeration {
    Left,
    Top,
    Right,
    Bottom
  };
  static const enumeration default_value = Left;
};

/**
   A side.

   One of the following values:
   @li Side::Left
   @li Side::Top
   @li Side::Right
   @li Side::Bottom
*/
typedef Enum<SideEnum> Side;

// ======================================================================

/**
   It's like a namespace for Sides.

   @see Sides
*/
struct SidesEnumSet
{
  enum {
    None   = 0,
    Left   = 1,
    Top    = 2,
    Right  = 4,
    Bottom = 8,
    All    = Left | Top | Right | Bottom
  };
};

/**
   A set of sides.

   Zero or more of the following values:
   @li Sides::Left
   @li Sides::Top
   @li Sides::Right
   @li Sides::Bottom
*/
typedef EnumSet<SidesEnumSet> Sides;

// ======================================================================

/**
   It's like a namespace for CardinalDirection.

   @see CardinalDirection
*/
struct CardinalDirectionEnum
{
  enum enumeration {
    North,
    Northeast,
    East,
    Southeast,
    South,
    Southwest,
    West,
    Northwest
  };
  static const enumeration default_value = North;
};

/**
   A cardinal direction.

   One of the following values:
   @li CardinalDirection::North
   @li CardinalDirection::Northeast
   @li CardinalDirection::East
   @li CardinalDirection::Southeast
   @li CardinalDirection::South
   @li CardinalDirection::Southwest
   @li CardinalDirection::West
   @li CardinalDirection::Northwest
*/
typedef Enum<CardinalDirectionEnum> CardinalDirection;

// ======================================================================

/**
   Removes an @a element from the specified STL @a container.

   This routine removes the first ocurrence of @a element in @a container.
   It is just a helper function to avoid cryptic STL code.

   @tparam ContainerType A STL container type.

   @param container The container to be modified.
   @param element The element to be removed from the container.
*/
template<typename ContainerType>
void remove_from_container(ContainerType& container,
			   typename ContainerType::const_reference element)
{
  for (typename ContainerType::iterator
	 it = container.begin(),
	 end = container.end(); it != end; ) {
    if (*it == element) {
      it = container.erase(it);
      end = container.end();
    }
    else
      ++it;
  }
}

// ======================================================================
// Forward declarations

class Anchor;
class AnchorLayout;
class Application;
class BandedDockArea;
class BasicDockArea;
class Bix;
class BoxConstraint;
class BoxLayout;
class Brush;
class Button;
class ButtonBase;
class CancelableEvent;
class CheckBox;
class ClientLayout;
class Clipboard;
class CloseEvent;
class Color;
class ColorDialog;
class ComboBox;
class Command;
class CommandEvent;
class CommandsClient;
class CommonDialog;
class Component;
class ConditionVariable;
class Constraint;
class Cursor;
class CustomButton;
class CustomLabel;
class Dialog;
class DockArea;
class DockBar;
class DockFrame;
class DockInfo;
class DragListBox;
class DropFilesEvent;
class Event;
class Exception;
class FileDialog;
class FindTextDialog;
class FocusEvent;
class Font;
class FontDialog;
class FontMetrics;
class Frame;
class Graphics;
class GraphicsPath;
class GroupBox;
class HttpRequest;
class HttpRequestException;
class Icon;
class Image;
class ImageHandle;
class ImageList;
class ImagePixels;
class KeyEvent;
class Label;
class Layout;
class LayoutEvent;
class LinkLabel;
class ListBox;
class ListColumn;
class ListItem;
class ListView;
class ListViewEvent;
class MdiChild;
class MdiClient;
class MdiFrame;
class MdiListMenu;
class Menu;
class MenuBar;
class MenuItem;
class MenuItemEvent;
class MenuSeparator;
class Message;
class MouseEvent;
class MsgBox;
class Mutex;
class Node;
class NonCopyable;
class OpenFileDialog;
class PaintEvent;
class Pen;
class Point;
class PopupMenu;
class PreferredSizeEvent;
class ProgressBar;
class Property;
class RadioButton;
class RadioGroup;
class ReBar;
class ReBarBand;
class Rect;
class Referenceable;
class Region;
class ResizeEvent;
class ResourceId;
class SaveFileDialog;
class SciEdit;
class SciRegister;
class ScopedLock;
class ScreenGraphics;
class ScrollEvent;
class ScrollInfo;
class Separator;
class SetCursorEvent;
class Size;
class Slider;
class SpinButton;
class Spinner;
class SplitBar;
class StatusBar;
class System;
class Tab;
class TabBase;
class TabPage;
class TextEdit;
class Thread;
class TimePoint;
class Timer;
class ToggleButton;
class ToolBar;
class ToolButton;
class ToolSet;
class TreeNode;
class TreeView;
class TreeViewEvent;
class TreeViewIterator;
class Widget;
class WidgetClassName;

template<class T>
class SharedPtr;

// ======================================================================
// Smart Pointers

/**
   @defgroup smart_pointers Smart Pointers
   @{
 */

typedef SharedPtr<Anchor> AnchorPtr;
typedef SharedPtr<AnchorLayout> AnchorLayoutPtr;
typedef SharedPtr<BandedDockArea> BandedDockAreaPtr;
typedef SharedPtr<BasicDockArea> BasicDockAreaPtr;
typedef SharedPtr<Bix> BixPtr;
typedef SharedPtr<BoxConstraint> BoxConstraintPtr;
typedef SharedPtr<BoxLayout> BoxLayoutPtr;
typedef SharedPtr<Button> ButtonPtr;
typedef SharedPtr<ButtonBase> ButtonBasePtr;
typedef SharedPtr<CheckBox> CheckBoxPtr;
typedef SharedPtr<ClientLayout> ClientLayoutPtr;
typedef SharedPtr<ColorDialog> ColorDialogPtr;
typedef SharedPtr<ComboBox> ComboBoxPtr;
typedef SharedPtr<Command> CommandPtr;
typedef SharedPtr<CommonDialog> CommonDialogPtr;
typedef SharedPtr<Component> ComponentPtr;
typedef SharedPtr<Constraint> ConstraintPtr;
typedef SharedPtr<CustomButton> CustomButtonPtr;
typedef SharedPtr<CustomLabel> CustomLabelPtr;
typedef SharedPtr<Dialog> DialogPtr;
typedef SharedPtr<DockArea> DockAreaPtr;
typedef SharedPtr<DockBar> DockBarPtr;
typedef SharedPtr<DockFrame> DockFramePtr;
typedef SharedPtr<DockInfo> DockInfoPtr;
typedef SharedPtr<DragListBox> DragListBoxPtr;
typedef SharedPtr<FileDialog> FileDialogPtr;
typedef SharedPtr<FindTextDialog> FindTextDialogPtr;
typedef SharedPtr<FontDialog> FontDialogPtr;
typedef SharedPtr<Frame> FramePtr;
typedef SharedPtr<GroupBox> GroupBoxPtr;
typedef SharedPtr<Label> LabelPtr;
typedef SharedPtr<Layout> LayoutPtr;
typedef SharedPtr<LinkLabel> LinkLabelPtr;
typedef SharedPtr<ListBox> ListBoxPtr;
typedef SharedPtr<ListItem> ListItemPtr;
typedef SharedPtr<ListView> ListViewPtr;
typedef SharedPtr<MdiChild> MdiChildPtr;
typedef SharedPtr<MdiClient> MdiClientPtr;
typedef SharedPtr<MdiFrame> MdiFramePtr;
typedef SharedPtr<MdiListMenu> MdiListMenuPtr;
typedef SharedPtr<Menu> MenuPtr;
typedef SharedPtr<MenuBar> MenuBarPtr;
typedef SharedPtr<MenuItem> MenuItemPtr;
typedef SharedPtr<MenuSeparator> MenuSeparatorPtr;
typedef SharedPtr<OpenFileDialog> OpenFileDialogPtr;
typedef SharedPtr<PopupMenu> PopupMenuPtr;
typedef SharedPtr<ProgressBar> ProgressBarPtr;
typedef SharedPtr<Property> PropertyPtr;
typedef SharedPtr<RadioButton> RadioButtonPtr;
typedef SharedPtr<ReBar> ReBarPtr;
typedef SharedPtr<SaveFileDialog> SaveFileDialogPtr;
typedef SharedPtr<SciEdit> SciEditPtr;
typedef SharedPtr<Separator> SeparatorPtr;
typedef SharedPtr<Slider> SliderPtr;
typedef SharedPtr<SpinButton> SpinButtonPtr;
typedef SharedPtr<Spinner> SpinnerPtr;
typedef SharedPtr<SplitBar> SplitBarPtr;
typedef SharedPtr<StatusBar> StatusBarPtr;
typedef SharedPtr<Tab> TabPtr;
typedef SharedPtr<TabBase> TabBasePtr;
typedef SharedPtr<TabPage> TabPagePtr;
typedef SharedPtr<TextEdit> TextEditPtr;
typedef SharedPtr<ToggleButton> ToggleButtonPtr;
typedef SharedPtr<ToolBar> ToolBarPtr;
typedef SharedPtr<ToolSet> ToolSetPtr;
typedef SharedPtr<TreeNode> TreeNodePtr;
typedef SharedPtr<TreeView> TreeViewPtr;
typedef SharedPtr<Widget> WidgetPtr;

/** @} */

} // namespace vaca

#endif // VACA_BASE_H
