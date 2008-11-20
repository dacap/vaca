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

#ifndef VACA_BASE_H
#define VACA_BASE_H

#include <algorithm>
#include <stdarg.h>
#include <tchar.h>
#include <windows.h>
#include <commctrl.h>

#include "Vaca/Enum.h"

// memory leaks
#ifdef MEMORY_LEAK_DETECTOR
  #include "debug_new.h"
#endif

namespace Vaca {

#define VACA_VERSION     0
#define VACA_SUB_VERSION 0
#define VACA_WIP_VERSION 8

/**
 * Defines the name and arguments that the main routine
 * of the program should contain.
 *
 * @win32
 *   It is the signature of @msdn{WinMain}. In other
 *   operating systems this could be @c "main(int argc, char* argv[])".
 * @endwin32
 */
#define VACA_MAIN()				\
  PASCAL WinMain(HINSTANCE hInstance,		\
		 HINSTANCE hPrevInstance,	\
		 LPSTR lpCmdLine,		\
		 int nCmdShow)
  
#ifdef VACA_STATIC
  #define VACA_DLL
#else
  #ifdef VACA_SRC
    #define VACA_DLL __declspec(dllexport)
  #else
    #define VACA_DLL __declspec(dllimport)
  #endif
#endif

/**
 * Returns the minimum of @a x and @a y.
 *
 * @note It's just like @b std::min
 */
template<typename T>
inline T min_value(T x, T y)
{
  return x < y ? x: y;
}

/**
 * Returns the maximum of @a x and @a y.
 *
 * @note It's just like @b std::max
 */
template<typename T>
inline T max_value(T x, T y)
{
  return x > y ? x: y;
}

/**
 * Limits the posible values of @a x to the speficied range.
 *
 * If @a x is great than @a high, then @a high is returned,
 * if @a x is less than @a low, then @a low is returned.
 * In other case, @a x is in the range, and @a x is returned.
 */
template<typename T>
inline T clamp_value(T x, T low, T high)
{
  return x > high ? high: (x < low ? low: x);
}

/**
 * A character.
 *
 * It has 8-bits (char) if Unicode is disabled, or 16-bits (short) if
 * you compile with Unicode support.
 *
 * @win32
 *   It is a TCHAR.
 * @endwin32
 *
 * @see @ref page_tn_008
 */
typedef TCHAR Character;

/**
 * An identifier for an application's Command.
 *
 * @see Widget#onCommand, Command
 */
typedef unsigned int CommandId;

/**
 * An identifier for a Thread.
 */
typedef unsigned int ThreadId;

/**
 * A message that comes from the operating system.
 *
 * You should use a Message like a black-box, which means
 * that you can't see and ask for any property about it.
 *
 * @win32
 *   This is just a @msdn{MSG}.
 * @endwin32
 */
typedef MSG Message;

//////////////////////////////////////////////////////////////////////

/**
 * It's like a namespace for Orientation.
 * 
 * @see Orientation
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
 * Horizontal or vertical orientation.
 *
 * One of the following values:
 * @li Orientation::Horizontal (default)
 * @li Orientation::Vertical
 */
typedef Enum<OrientationEnum> Orientation;

//////////////////////////////////////////////////////////////////////

/**
 * It's like a namespace for TextAlign.
 * 
 * @see TextAlign
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
 * Horizontal alignment.
 *
 * One of the following values:
 * @li TextAlign::Left (default)
 * @li TextAlign::Center
 * @li TextAlign::Right
 */
typedef Enum<TextAlignEnum> TextAlign;

//////////////////////////////////////////////////////////////////////

/**
 * It's like a namespace for VerticalAlign.
 * 
 * @see VerticalAlign
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
 * Vertical alignment.
 *
 * One of the following values:
 * @li VerticalAlign::Top
 * @li VerticalAlign::Middle
 * @li VerticalAlign::Bottom
 */
typedef Enum<VerticalAlignEnum> VerticalAlign;

//////////////////////////////////////////////////////////////////////

/**
 * It's like a namespace for Side.
 * 
 * @see Side
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
 * A side.
 *
 * One of the following values:
 * @li Side::Left
 * @li Side::Top
 * @li Side::Right
 * @li Side::Bottom
 */
typedef Enum<SideEnum> Side;

//////////////////////////////////////////////////////////////////////

/**
 * It's like a namespace for Sides.
 * 
 * @see Sides
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
 * A set of sides.
 *
 * Zero or more of the following values:
 * @li Sides::Left
 * @li Sides::Top
 * @li Sides::Right
 * @li Sides::Bottom
 */
typedef EnumSet<SidesEnumSet> Sides;

//////////////////////////////////////////////////////////////////////

/**
 * It's like a namespace for CardinalDirection.
 * 
 * @see CardinalDirection
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
 * A cardinal direction.
 *
 * One of the following values:
 * @li CardinalDirection::North
 * @li CardinalDirection::Northeast
 * @li CardinalDirection::East
 * @li CardinalDirection::Southeast
 * @li CardinalDirection::South
 * @li CardinalDirection::Southwest
 * @li CardinalDirection::West
 * @li CardinalDirection::Northwest
 */
typedef Enum<CardinalDirectionEnum> CardinalDirection;

//////////////////////////////////////////////////////////////////////

/**
 * Removes an @a element from the specified STL @a container.
 *
 * This routine removes the first ocurrence of @a element in @a container. 
 * It is just a helper function to avoid cryptic STL code.
 *
 * @tparam ContainerType A STL container type.
 *
 * @param container The container to be modified.
 * @param element The element to be removed from the container.
 */
template<typename ContainerType>
void remove_from_container(ContainerType& container,
			   typename ContainerType::const_reference element)
{
  typename ContainerType::iterator
    it = std::find(container.begin(),
		   container.end(),
		   element);
  
  if (it != container.end())
    container.erase(it);
}

} // namespace Vaca

#endif // VACA_BASE_H
