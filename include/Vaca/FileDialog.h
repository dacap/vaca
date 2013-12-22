// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_FILEDIALOG_H
#define VACA_FILEDIALOG_H

#include "Vaca/base.h"
#include "Vaca/CommonDialog.h"

#include <vector>
#include <utility>

namespace Vaca {

/**
   A common dialog box to select files. You shouldn't use directly
   this class, you must to use the OpenFileDialog and SaveFileDialog
   classes.
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
   The common dialog box to open file(s).
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
   The common dialog box to save a file.
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
