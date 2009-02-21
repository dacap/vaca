// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

#ifndef VACA_SYSTEM_H
#define VACA_SYSTEM_H

#include <vector>
#include <shlobj.h>

#include "Vaca/base.h"
#include "Vaca/Rect.h"
#include "Vaca/Keys.h"

namespace Vaca {

/// Class to access to operating system information.
/// 
/// It is more like a namespace than a class, because all methods are
/// static.
/// 
class VACA_DLL System
{
public:

  static std::vector<String> getArgs();

  static void println(String line);
  static void printf(LPCTSTR fmt, ...);
  static void print(String buf);

  static String getCurrentDirectory();
  static String getWindowsDirectory();
  static String getShellFolderPath(int folderCsidl, bool create);

  static ImageList getImageList(bool smallImage);
  static int getFileImageIndex(const String& fileName, bool smallImage);

  static Size getScreenSize();
  static Rect getWorkAreaBounds();

  static Color getColor(int index);
  
  static bool getKeyState(Keys::Type);
  static Point getCursorPos();
  static void setCursorPos(const Point& pt);

  static String getUserName();
  static String getFriendlyUserName();

//   static int getWheelScrollLines();

  struct OS {
    typedef int Type;

    static const Type Unknown          = 0x0000;
    static const Type WinServer2003    = 0x0001;
    static const Type WinXP            = 0x0002;
    static const Type Win2K            = 0x0004;
    static const Type WinNT            = 0x0008;
    static const Type Win95            = 0x0010;
    static const Type Win95R2          = 0x1010;
    static const Type Win98            = 0x0020;
    static const Type Win98SE          = 0x1020;
    static const Type WinMe            = 0x0080;
    static const Type Win32s           = 0x0100;
  };

  static OS::Type getOS();

  static bool isWin95_98_Me();
  static bool isWinNT_2K_XP();
  static bool isWin2K_XP();
  static bool isWinXP();

};

} // namespace Vaca

#endif // VACA_SYSTEM_H
