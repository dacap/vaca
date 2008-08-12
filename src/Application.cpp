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

#include "Vaca/Application.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/Frame.hpp"
#include "Vaca/Timer.hpp"
#include "Vaca/Graphics.hpp"
#include "Vaca/Font.hpp"

using namespace Vaca;

HINSTANCE Application::m_HINSTANCE = NULL;
Application* Application::m_instance = NULL;

/**
 * Initializes the variables of the Application class. It calls
 * CoInitialize and InitCommonControls.
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
}

/**
 * Finishes the application. It calls CoUninitialize.
 */
Application::~Application()
{
  assert(Application::m_HINSTANCE != NULL);
  assert(Application::m_instance == this);

  Timer::stop_timer_thread();

  Application::m_HINSTANCE = NULL;
  Application::m_instance = NULL;

  // delete brushes, pens and fonts
  Graphics::deleteHandles();
  Font::deleteHandles();

  // close the log file
  __vaca_remove_all_thread_data();
  __vaca_close_log_file();

  CoUninitialize();

#ifdef MEMORY_LEAK_DETECTOR
  atexit(check_leaks);
#endif
}

Application* Application::getInstance()
{
  return m_instance;
}

HINSTANCE Application::getHINSTANCE()
{
  return Application::m_HINSTANCE;
}

/**
 * The common way to start the application. You should call this
 * method from @e main or @e WinMain, using an instance of
 * a class derived of Application.
 */
void Application::run()
{
  main();
  doMessageLoop();
}

/**
 * The application entry point.
 *
 * After calling run(), main() is executed, and when it finishes, the
 * doMessageLoop() is automatically executed.
 */
void Application::main()
{
  // do nothing
}
