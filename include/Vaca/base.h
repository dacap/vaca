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

#ifndef VACA_BASE_HPP
#define VACA_BASE_HPP

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
#define VACA_WIP_VERSION 6

#ifdef VACA_STATIC
  #define VACA_DLL
#else
  #ifdef VACA_SRC
    #define VACA_DLL __declspec(dllexport)
  #else
    #define VACA_DLL __declspec(dllimport)
  #endif
#endif

#if 1
  // Visual C++ has "max" and "min" as macros... so there is some
  // problems using "std::max" and "std::min"
  #define VACA_MIN(x,y)		((x) < (y) ? (x): (y))
  #define VACA_MAX(x,y)		((x) < (y) ? (y): (x))
#else
  #define VACA_MIN(x,y)		(std::min((x), (y)))
  #define VACA_MAX(x,y)		(std::max((x), (y)))
#endif
#define VACA_MID(x,y,z)		(VACA_MAX(x, VACA_MIN(y, z)))

/**
 * A character (TCHAR). It has 8-bits (char) if Unicode is disabled,
 * or 16-bits (short) if you compile with Unicode support.
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
 * Horizontal or Vertical orientation.
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
 * Horizontal alignment. One of the following values:
 * @li TextAlign::Left
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
 * Vertical alignment. One of the following values:
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
 * A side. One of the following values:
 * 
 * @li Side::Left
 * @li Side::Top
 * @li Side::Right
 * @li Side::Bottom
 */
typedef Enum<SideEnum> Side;

//////////////////////////////////////////////////////////////////////

/**
 * It's like a namespace for Borders.
 * 
 * @see Borders
 */
struct BordersEnumSet
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
 * A set of borders. Zero or more of the following values:
 * @li Borders::Left
 * @li Borders::Top
 * @li Borders::Right
 * @li Borders::Bottom
 */
typedef EnumSet<BordersEnumSet> Borders;

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
 * A cardinal direction. One of the following values:
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
 * Removes an element from the container.
 *
 * This is just a helper function to avoid cryptic code.
 */
template<typename _ContainerType,
	 typename _ElementType>
void remove_from_container(_ContainerType& container,
			   const _ElementType& element)
{
  typename _ContainerType::iterator
    it = std::find(container.begin(),
		   container.end(),
		   element);
  
  if (it != container.end())
    container.erase(it);
}

} // namespace Vaca

#endif
