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

#ifndef VACA_FILEDIALOG_H
#define VACA_FILEDIALOG_H

#include <vector>
#include <utility>

#include "Vaca/base.h"
#include "Vaca/CommonDialog.h"
#include "Vaca/String.h"

namespace Vaca {

class Widget;

/**
 * A common dialog box to select files. You shouldn't use directly
 * this class, you must to use the OpenFileDialog and SaveFileDialog
 * classes.
 */
class VACA_DLL FileDialog : public CommonDialog
{
  String m_title;
  String m_defaultExtension;
  bool m_showReadOnly : 1;
  bool m_showHelp : 1;
  LPTSTR m_fileName;
  String m_filtersString;
  std::vector<std::pair<String, String> > m_filters;
  int m_defaultFilter;

public:

  FileDialog(const String& title, Widget* parent);
  virtual ~FileDialog();

  void setTitle(const String& str);
  void setDefaultExtension(const String& str);
  void setShowReadOnly(bool state);
  void setShowHelp(bool state);

  void addFilter(const String& extensions, const String& description, bool defaultFilter = false);

  String getFileName();
  void setFileName(const String& string);

  virtual bool doModal();

protected:
  LPTSTR getOriginalFileName();

private:
  virtual bool showDialog(LPOPENFILENAME lpofn) = 0;
};

/**
 * The common dialog box to open file(s).
 */
class VACA_DLL OpenFileDialog : public FileDialog
{
  bool m_multiselect : 1;

public:
  OpenFileDialog(const String& title, Widget* parent);
  virtual ~OpenFileDialog();

  void setMultiselect(bool state);

  std::vector<String> getFileNames();

private:
  virtual bool showDialog(LPOPENFILENAME lpofn);
};

/**
 * The common dialog box to save a file.
 */
class VACA_DLL SaveFileDialog : public FileDialog
{
public:
  SaveFileDialog(const String& title, Widget* parent);
  virtual ~SaveFileDialog();

private:
  virtual bool showDialog(LPOPENFILENAME lpofn);
};

} // namespace Vaca

#endif // VACA_FILEDIALOG_H
