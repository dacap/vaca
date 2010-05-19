// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
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

#include "Vaca/System.h"
#include "Vaca/Rect.h"
#include "Vaca/Point.h"
#include "Vaca/Size.h"
#include "Vaca/Color.h"
#include "Vaca/Debug.h"
#include "Vaca/ImageList.h"
#include "Vaca/ScopedLock.h"
#include "Vaca/win32.h"

#include <cstdio>
#include <cwchar>
#include <lmcons.h>

using namespace std;
using namespace Vaca;

/**
   Prints a line in the console.
*/
void System::println(String line)
{
#ifdef _UNICODE
  _putws(line.c_str());
#else
  puts(line.c_str());
#endif
}

void System::printf(LPCTSTR fmt, ...)
{
  Char buf[1024];		// TODO: overflow
  va_list ap;

  va_start(ap, fmt);
#ifdef UNICODE
  vswprintf(buf, fmt, ap);
#else
  vsprintf(buf, fmt, ap);
#endif
  va_end(ap);

#ifdef UNICODE
  fputws(buf, stdout);
#else
  fputs(buf, stdout);
#endif
}

void System::print(String buf)
{
#ifdef UNICODE
  fputws(buf.c_str(), stdout);
#else
  fputs(buf.c_str(), stdout);
#endif
}

String System::getCurrentDirectory()
{
  Char* buf = new Char[MAX_PATH];

  if (GetCurrentDirectory(MAX_PATH, buf) > 0)
    return String(buf);
  else
    return L"";
}

String System::getWindowsDirectory()
{
  Char* buf = new Char[MAX_PATH];

  if (GetWindowsDirectory(buf, MAX_PATH) > 0)
    return String(buf);
  else
    return L"";
}

/**

   TODO folderCsidl can't be a virtual folder

   CSIDL_ADMINTOOLS
   CSIDL_ALTSTARTUP
   CSIDL_APPDATA
   CSIDL_BITBUCKET
   CSIDL_CDBURN_AREA
   CSIDL_COMMON_ADMINTOOLS
   CSIDL_COMMON_ALTSTARTUP
   CSIDL_COMMON_APPDATA
   CSIDL_COMMON_DESKTOPDIRECTORY
   CSIDL_COMMON_DOCUMENTS
   CSIDL_COMMON_FAVORITES
   CSIDL_COMMON_MUSIC
   CSIDL_COMMON_OEM_LINKS
   CSIDL_COMMON_PICTURES
   CSIDL_COMMON_PROGRAMS
   CSIDL_COMMON_STARTMENU
   CSIDL_COMMON_STARTUP
   CSIDL_COMMON_TEMPLATES
   CSIDL_COMMON_VIDEO
   CSIDL_COMPUTERSNEARME
   CSIDL_CONNECTIONS
   CSIDL_CONTROLS
   CSIDL_COOKIES
   CSIDL_DESKTOP
   CSIDL_DESKTOPDIRECTORY
   CSIDL_DRIVES
   CSIDL_FAVORITES
   CSIDL_FONTS
   CSIDL_HISTORY
   CSIDL_INTERNET
   CSIDL_INTERNET_CACHE
   CSIDL_LOCAL_APPDATA
   CSIDL_MYPICTURES
   CSIDL_NETHOOD
   CSIDL_NETWORK
   CSIDL_PERSONAL
   CSIDL_PRINTERS
   CSIDL_PRINTHOOD
   CSIDL_PROFILE
   CSIDL_PROGRAMS
   CSIDL_PROGRAM_FILES
   CSIDL_PROGRAM_FILESX86
   CSIDL_PROGRAM_FILES_COMMON
   CSIDL_PROGRAM_FILES_COMMONX86
   CSIDL_RECENT
   CSIDL_RESOURCES
   CSIDL_RESOURCES_LOCALIZED
   CSIDL_SENDTO
   CSIDL_STARTMENU
   CSIDL_STARTUP
   CSIDL_SYSTEM
   CSIDL_SYSTEMX86
   CSIDL_TEMPLATES
   CSIDL_WINDOWS
*/
String System::getShellFolderPath(int folderCsidl, bool create)
{
  Char* buf = new Char[MAX_PATH];

  if (SHGetSpecialFolderPath(NULL, buf, folderCsidl, create))
    return String(buf);
  else
    return L"";
}

/**
   Returns the system ImageList that contains icons for file-types.

   @see System#getSmallImageList
*/
ImageList System::getImageList()
{
  HIMAGELIST himl;
  SHFILEINFO shfi;

  himl = reinterpret_cast<HIMAGELIST>
    (SHGetFileInfo(L"",
		   0, &shfi, sizeof(shfi),
		   SHGFI_SYSICONINDEX | SHGFI_LARGEICON));

  return ImageList(himl);
}

/**
   Returns the system ImageList that contains small-icons for file-types.

   @see System#getImageList
*/
ImageList System::getSmallImageList()
{
  HIMAGELIST himl;
  SHFILEINFO shfi;

  himl = reinterpret_cast<HIMAGELIST>
    (SHGetFileInfo(L"",
		   0, &shfi, sizeof(shfi),
		   SHGFI_SYSICONINDEX | SHGFI_SMALLICON));

  return ImageList(himl);
}

/**
   Returns the image-index of the system ImageList that corresponds
   to the icon of the specified file.

   @see System#getImageList
*/
int System::getFileImageIndex(const String& fileName, bool smallImage)
{
  SHFILEINFO shfi;

  SHGetFileInfo(fileName.c_str(), 0, &shfi, sizeof(shfi),
		SHGFI_SYSICONINDEX |
		(smallImage ? SHGFI_SMALLICON:
			      SHGFI_LARGEICON));

  return shfi.iIcon;
}

/**
   Returns the screen size.
*/
Size System::getScreenSize()
{
  return Size(GetSystemMetrics(SM_CXFULLSCREEN),
	      GetSystemMetrics(SM_CYFULLSCREEN));
}

/**
   Returns the work-area of the screen, it is like the desktop area,
   that is not overlapped by the task-bar.

   @win32
     It is a call to @msdn{SystemParametersInfo} using @msdn{SPI_GETWORKAREA}.
   @endwin32
*/
Rect System::getWorkAreaBounds()
{
  RECT rc;

  // returns the work-area rectangle
  if (SystemParametersInfo(SPI_GETWORKAREA, 0, reinterpret_cast<PVOID>(&rc), 0))
    return Rect(&rc);
  // returns the full-screen rectangle
  else
    return Rect(Point(0, 0), System::getScreenSize());
}

/**
   Returns color from the system. It's a wrapper for the Win32's GetSysColor.

   @param index
       Can be one of the following values:
       @li COLOR_3DDKSHADOW
       @li COLOR_3DFACE
       @li COLOR_3DHILIGHT
       @li COLOR_3DHIGHLIGHT
       @li COLOR_3DLIGHT
       @li COLOR_BTNHILIGHT
       @li COLOR_3DSHADOW
       @li COLOR_ACTIVEBORDER
       @li COLOR_ACTIVECAPTION
       @li COLOR_APPWORKSPACE
       @li COLOR_BACKGROUND
       @li COLOR_DESKTOP
       @li COLOR_BTNFACE
       @li COLOR_BTNHIGHLIGHT
       @li COLOR_BTNSHADOW
       @li COLOR_BTNTEXT
       @li COLOR_CAPTIONTEXT
       @li COLOR_GRAYTEXT
       @li COLOR_HIGHLIGHT
       @li COLOR_HIGHLIGHTTEXT
       @li COLOR_INACTIVEBORDER
       @li COLOR_INACTIVECAPTION
       @li COLOR_INACTIVECAPTIONTEXT
       @li COLOR_INFOBK
       @li COLOR_INFOTEXT
       @li COLOR_MENU
       @li COLOR_MENUTEXT
       @li COLOR_SCROLLBAR
       @li COLOR_WINDOW
       @li COLOR_WINDOWFRAME
       @li COLOR_WINDOWTEXT
       @li COLOR_HOTLIGHT
       @li COLOR_GRADIENTACTIVECAPTION
       @li COLOR_GRADIENTINACTIVECAPTION

   @todo make the index a Enum
*/
Color System::getColor(int index)
{
  return convert_to<Color>(GetSysColor(index));
}

bool System::getKeyState(Keys::Type keyCode)
{
  // you can't use getKeyState for modifiers
  assert((keyCode & Keys::Modifiers) == 0);

  return (GetKeyState(keyCode) & 0x8000) != 0;
}

/**
   Returns the cursor position in the screen.
*/
Point System::getCursorPos()
{
  POINT pt;
  GetCursorPos(&pt);
  return Point(pt.x, pt.y);
}

void System::setCursorPos(const Point& pt)
{
  SetCursorPos(pt.x, pt.y);
}

String System::getUserName()
{
  Char buf[UNLEN+1];
  // char buf[UNLEN+1];
  DWORD len = UNLEN+1;

  if (GetUserName(buf, &len))
    return String(buf);
  else
    return String();
}

// Prototypes for Win2000/XP API
// typedef BOOLEAN (*GUNEProc)(EXTENDED_NAME_FORMAT NameFormat, LPTSTR lpNameBuffer, PULONG nSize);

String System::getFriendlyUserName()
{
#if 0				// TODO
#if (_WIN32_WINNT >= 0x0500)
  HMODULE hSecur32 = GetModuleHandle(L"SECUR32.DLL");
  if (!hSecur32)
    hSecur32 = LoadLibrary(L"SECUR32.DLL");
  GUNEProc pGUNE = (GUNEProc)GetProcAddress(hSecur32, "GetUserNameExW");

  if (pGUNE != NULL) {
    Char buf[UNLEN+1];
    ULONG len = UNLEN+1;

    if (pGUNE(NameDisplay, buf, &len))
      return String(buf);
  }
#endif
#endif
  return getUserName();
}

// int System::getWheelScrollLines()
// {
//   UINT pulScrollLines = 0;
//   return SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &pulScrollLines, 0);
// }
