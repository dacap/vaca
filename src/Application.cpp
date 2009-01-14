// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, 2009, David Capello
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

#include "Vaca/Application.h"
#include "Vaca/Debug.h"
#include "Vaca/Frame.h"
#include "Vaca/Timer.h"
#include "Vaca/Graphics.h"
#include "Vaca/Font.h"

#ifndef NDEBUG
#include "Vaca/System.h"
#endif

using namespace Vaca;

HINSTANCE Application::m_HINSTANCE = NULL;
Application* Application::m_instance = NULL;

/**
 * Initializes the variables of the Application class.
 *
 * It calls Win32's @c CoInitialize and @c InitCommonControls.
 *
 * @see run
 */
Application::Application()
  : Thread()
{
  assert(Application::m_HINSTANCE == NULL);
  assert(Application::m_instance == NULL);

  CoInitialize(NULL);

  Application::m_HINSTANCE = ::GetModuleHandle(NULL);
  Application::m_instance = this;

  INITCOMMONCONTROLSEX icce;
  icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icce.dwICC = 0
    | ICC_WIN95_CLASSES
    | ICC_BAR_CLASSES
    | ICC_COOL_CLASSES
    ;

  InitCommonControlsEx(&icce);

  VACA_TRACE("Program %s started\n", convert_to<std::string>(System::getArgs()[0]).c_str());
}

/**
 * Finishes the application.
 *
 * It calls Win32's @c CoUninitialize.
 */
Application::~Application()
{
  assert(Application::m_HINSTANCE != NULL);
  assert(Application::m_instance == this);

  Timer::stop_timer_thread();

  Application::m_HINSTANCE = NULL;
  Application::m_instance = NULL;

#ifndef NDEBUG
  Referenceable::showLeaks();
#endif

  VACA_TRACE("Program %s finished\n", convert_to<std::string>(System::getArgs()[0]).c_str());

  // close the log file
  details::remove_all_thread_data();
  details::close_log_file();

  CoUninitialize();

#ifdef MEMORY_LEAK_DETECTOR
  atexit(check_leaks);
#endif
}

/**
 * Returns the Application's singleton.
 *
 * A program using Vaca must have one instance of Applicaton or a
 * class derived from it.
 */
Application* Application::getInstance()
{
  return m_instance;
}

/**
 * Returns the Win32's @c HINSTANCE.
 *
 * @internal
 */
HINSTANCE Application::getHandle()
{
  return Application::m_HINSTANCE;
}

/**
 * Changes the priority of the process respecting to all other
 * processes currently running on the operating system.
 *
 * @see Thread#setThreadPriority
 */
void Application::setProcessPriority(ProcessPriority priority)
{
  int nPriority;
  switch (priority) {
    case ProcessPriority::Idle:     nPriority = IDLE_PRIORITY_CLASS; break;
    case ProcessPriority::Normal:   nPriority = NORMAL_PRIORITY_CLASS; break;
    case ProcessPriority::High:     nPriority = HIGH_PRIORITY_CLASS; break;
    case ProcessPriority::RealTime: nPriority = REALTIME_PRIORITY_CLASS; break;
    default:
      assert(false);	      // TODO throw invalid argument exception
      return;
  }
  ::SetPriorityClass(::GetCurrentProcess(), nPriority);
}

/**
 * The common way to start the application. You should call this
 * method from @e main or @e WinMain, using the instance of
 * Application.
 *
 * The work of this routine is really simple: it calls #main and then
 * #doMessageLoop. You can make your own class derived from
 * Application and override #main so you can customized the
 * initialization (e.g. show a Frame).
 *
 * Example:
 * @code
 * int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
 *                    LPSTR lpCmdLine, int nCmdShow)
 * {
 *   Application app;
 *   Frame frm("My frame");
 *   frm.setVisible(true);
 *   app.run();
 *   return 0;
 * }
 * @endcode
 *
 * The same example with a variation:
 * @code
 * class MyApp : public Application
 * {
 *   Frame frm;
 * public:
 *   MyApp() : frm("My frame") { }
 *   
 *   virtual void main() {
 *     frm.setVisible(true);
 *   }
 * };
 * 
 * int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
 *                    LPSTR lpCmdLine, int nCmdShow)
 * {
 *   MyApp app;
 *   app.run();
 *   return 0;
 * }
 * @endcode
 *
 * @see Thread#doMessageLoop
 */
void Application::run()
{
  main();
  doMessageLoop();
}

/**
 * The application entry point.
 *
 * After calling #run, #main is executed and when it finishes, the
 * #doMessageLoop is automatically executed (to process messages
 * for visible @link Frame frames@endlink).
 */
void Application::main()
{
  // do nothing
}
