// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SYSTEM_H
#define VACA_SYSTEM_H

#include "vaca/base.h"
#include "vaca/Rect.h"
#include "vaca/Keys.h"

#include <vector>
#include <shlobj.h>

namespace vaca {

/**
   Class to access to operating system information.

   It is more like a namespace than a class, because all member
   functions are static.
*/
class VACA_DLL System
{
public:

  static void println(String line);
  static void printf(LPCTSTR fmt, ...);
  static void print(String buf);

  static String getCurrentDirectory();
  static String getWindowsDirectory();
  static String getShellFolderPath(int folderCsidl, bool create);

  static ImageList getImageList();
  static ImageList getSmallImageList();
  static int getFileImageIndex(const String& fileName, bool smallImage);

  static Size getScreenSize();
  static Rect getWorkAreaBounds();

  static Color getColor(int index);

  static bool getKeyState(Keys::Type);
  static Point getCursorPos();
  static void setCursorPos(const Point& pt);

  static String getUserName();
  static String getFriendlyUserName();

  //static int getWheelScrollLines();

};

} // namespace vaca

#endif // VACA_SYSTEM_H
