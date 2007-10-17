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

#include "stdvaca.h"
#include "Vaca/FileDialog.h"
#include "Vaca/Widget.h"
#include "Vaca/Application.h"
#include "Vaca/System.h"

// 32k is the limit for Win95/98/Me/NT4/2000/XP with ANSI version
#define FILENAME_BUFSIZE (1024*32)

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// FileDialog

FileDialog::FileDialog(const String &title, Widget *parent)
  : CommonDialog(parent)
  , mTitle(title)
  , mDefaultExtension("")
  , mShowReadOnly(false)
  , mShowHelp(false)
  , mDefaultFilter(0)
{
  mFileName = new TCHAR[FILENAME_BUFSIZE];
  ZeroMemory(mFileName, sizeof(TCHAR[FILENAME_BUFSIZE]));
}

FileDialog::~FileDialog()
{
  delete mFileName;
}

/**
 * Sets the title text.
 */
void FileDialog::setTitle(const String &str)
{
  mTitle = str;
}

/**
 * Sets the default extension to add to the entered file name when an
 * extension isn't specified by the user. By default it's an empty
 * string.
 */
void FileDialog::setDefaultExtension(const String &str)
{
  mDefaultExtension = str;
}

/**
 * Sets the property that indicates if the dialog should show the read
 * only check box. By default it's false: the button is hidden.
 */
void FileDialog::setShowReadOnly(bool state)
{
  mShowReadOnly = state;
}

/**
 * Sets the property that indicates if the dialog should show the help
 * button. By default it's false: the button is hidden.
 */
void FileDialog::setShowHelp(bool state)
{
  mShowHelp = state;
}

void FileDialog::addFilter(const String &extensions, const String &description, bool defaultFilter)
{
  mFilters.push_back(std::make_pair(extensions, description));

  if (defaultFilter)
    mDefaultFilter = mFilters.size();
}

String FileDialog::getFileName()
{
  return String(mFileName);
}

void FileDialog::setFileName(const String &fileName)
{
  fileName.copyTo(mFileName, FILENAME_BUFSIZE);
}

LPTSTR FileDialog::getOriginalFileName()
{
  return mFileName;
}

bool FileDialog::doModal()
{
  // make the mFiltersString
  mFiltersString.clear();
  for (std::vector<std::pair<String, String> >::iterator it=mFilters.begin();
       it!=mFilters.end(); ++it) {
    mFiltersString.append(it->first);
    mFiltersString.push_back('\0');
    mFiltersString.append(it->second);
    mFiltersString.push_back('\0');
  }
  mFiltersString.push_back('\0');

  // fill the OPENFILENAME structure
  OPENFILENAME ofn;

#if (_WIN32_WINNT >= 0x0500)
  if (System::isWin95_98_Me())
    ofn.lStructSize = sizeof(OPENFILENAME)
      - sizeof(void *)
      - sizeof(DWORD)
      - sizeof(DWORD);
  else
    ofn.lStructSize = sizeof(OPENFILENAME);
#else
  ofn.lStructSize = sizeof(OPENFILENAME);
#endif
  ofn.hwndOwner = getParentHWND();
  ofn.hInstance = Application::getHINSTANCE();
  ofn.lpstrFilter = mFiltersString.c_str();
  ofn.lpstrCustomFilter = NULL;
  ofn.nMaxCustFilter = 0;
  ofn.nFilterIndex = mDefaultFilter;
  ofn.lpstrFile = mFileName;
  ofn.nMaxFile = FILENAME_BUFSIZE;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.lpstrTitle = mTitle.c_str();
  ofn.Flags = 0
// #define OFN_CREATEPROMPT 0x2000
// #define OFN_ENABLEHOOK 32
    | OFN_ENABLESIZING
// #define OFN_ENABLETEMPLATE 64
// #define OFN_ENABLETEMPLATEHANDLE 128
    | OFN_EXPLORER
// #define OFN_EXTENSIONDIFFERENT 0x400
    | (mShowReadOnly ? 0: OFN_HIDEREADONLY)
    | OFN_LONGNAMES
    | OFN_NOCHANGEDIR
// | OFN_NODEREFERENCELINKS
// | OFN_NOLONGNAMES
// | OFN_NONETWORKBUTTON
// | OFN_NOREADONLYRETURN
// | OFN_NOTESTFILECREATE
// | OFN_NOVALIDATE
// #define OFN_READONLY 1
// #define OFN_SHAREAWARE 0x4000
    | (mShowHelp ? OFN_SHOWHELP: 0)
    ;
// | OFN_SHAREFALLTHROUGH
// | OFN_SHARENOWARN
// | OFN_SHAREWARN
// | OFN_NODEREFERENCELINKS
// #if (_WIN32_WINNT >= 0x0500)
// | OFN_DONTADDTORECENT
// #endif

  ofn.nFileOffset = 0;
  ofn.nFileExtension = 0;
  ofn.lpstrDefExt = mDefaultExtension.c_str();
  ofn.lCustData = 0;
  ofn.lpfnHook = NULL;
  ofn.lpTemplateName = NULL;
#if (_WIN32_WINNT >= 0x0500)
  ofn.pvReserved = NULL;
  ofn.dwReserved = 0;
  ofn.FlagsEx = 0;
#endif

  return showDialog(&ofn);
}

//////////////////////////////////////////////////////////////////////
// OpenFileDialog

OpenFileDialog::OpenFileDialog(const String &title, Widget *parent)
  : FileDialog(title, parent)
  , mMultiselect(false)
{
}

OpenFileDialog::~OpenFileDialog()
{
}

/**
 * By default it's false.
 */
void OpenFileDialog::setMultiselect(bool state)
{
  mMultiselect = state;
}

std::vector<String> OpenFileDialog::getFileNames()
{
  std::vector<String> result;

  if (mMultiselect) {
    LPTSTR ptr, start;
    String path;

    for (ptr=start=getOriginalFileName(); ; ++ptr) {
      if (*ptr == '\0') {
	if (path.empty())
	  path = start;
	else
	  result.push_back(path + "\\" + start);

	if (*(++ptr) == '\0')
	  break;
	start = ptr;
      }
    }
  }

  // empty results? one file-name selected
  if (result.empty())
    result.push_back(getFileName());

  return result;
}

bool OpenFileDialog::showDialog(LPOPENFILENAME lpofn)
{
  lpofn->Flags |= 0
    | (mMultiselect ? OFN_ALLOWMULTISELECT: 0)
    | OFN_FILEMUSTEXIST
    | OFN_PATHMUSTEXIST
    ;

  return GetOpenFileName(lpofn) != FALSE;
}

//////////////////////////////////////////////////////////////////////
// SaveFileDialog

SaveFileDialog::SaveFileDialog(const String &title, Widget *parent)
  : FileDialog(title, parent)
{
}

SaveFileDialog::~SaveFileDialog()
{
}

void SaveFileDialog::setFileName(const String &string)
{
  FileDialog::setFileName(string);
}

bool SaveFileDialog::showDialog(LPOPENFILENAME lpofn)
{
  lpofn->Flags |= 0
    | OFN_PATHMUSTEXIST
    | OFN_OVERWRITEPROMPT
    ;

  return GetSaveFileName(lpofn) != FALSE;
}
