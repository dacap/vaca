// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_APPLICATION_H
#define VACA_APPLICATION_H

#include "vaca/base.h"
#include "vaca/Thread.h"
#include <vector>

namespace vaca {

  // Forward declarations
  namespace details { class MainArgs; }

// ======================================================================

/**
   Process priority.

   One of the following values:
   @li ProcessPriority::Idle
   @li ProcessPriority::Normal
   @li ProcessPriority::High
   @li ProcessPriority::RealTime
*/
enum class ProcessPriority
{
  Idle,
  Normal,
  High,
  RealTime,
};

// ======================================================================

/**
   The main class of Vaca: initializes and destroys the GUI library
   resources.

   A program that uses Vaca library must to create one instance of
   this class, or an instance of a derived class.
*/
class VACA_DLL Application : public Thread
{
public:

  Application();
  virtual ~Application();

  static size_t getArgc();
  static const String& getArgv(size_t i);
  static const std::vector<String>& getArgs();

  static Application* getInstance();
  static HINSTANCE getHandle();
  static void setProcessPriority(ProcessPriority priority);

  virtual void run();

protected:

  virtual void main();

private:

  friend class details::MainArgs;

  /**
     @internal

     @win32
       Program @msdn{HINSTANCE}.
     @endwin32
   */
  static HINSTANCE m_HINSTANCE;

  /**
     The singleton, the only instance of Application (or a class
     derived from Application) that a program can contain.
   */
  static Application* m_instance;

  static std::vector<String> m_args;

  static void setArgs(const std::vector<String>& args);

};

} // namespace vaca

#endif // VACA_APPLICATION_H
