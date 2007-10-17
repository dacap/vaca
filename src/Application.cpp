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
#include "Vaca/Debug.h"
#include "Vaca/Application.h"
#include "Vaca/Frame.h"

using namespace Vaca;

HINSTANCE Application::mHinstance = NULL;
Application *Application::mInstance = NULL;

/**
 * Initializes the variables of the Application class. Also calls the
 * InitCommonControls.
 */
Application::Application()
  : Thread(true)
{
  VACA_ASSERT(Application::mHinstance == NULL);
  VACA_ASSERT(Application::mInstance == NULL);

  Application::mHinstance = ::GetModuleHandle(NULL);
  Application::mInstance = this;

  INITCOMMONCONTROLSEX icce;
  icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icce.dwICC = 0
    | ICC_WIN95_CLASSES
    | ICC_BAR_CLASSES
    | ICC_COOL_CLASSES
    ;

  InitCommonControlsEx(&icce);

}

Application::~Application()
{
  VACA_ASSERT(Application::mHinstance != NULL);
  VACA_ASSERT(Application::mInstance == this);

  Application::mHinstance = NULL;
  Application::mInstance = NULL;

  // close log the file
  __vaca_trace(NULL, 0, NULL);
}

Application &Application::instance()
{
  return *mInstance;
}

HINSTANCE Application::getHinstance()
{
  return Application::mHinstance;
}

/**
 * The common way to start the application. You should call this
 * method from <em>main</em> or <em>WinMain</em>, using an instance of
 * a class derived of Application.
 */
void Application::run()
{
  std::vector<String> args;

  LPTSTR cmdline = GetCommandLine();
  // int length = strlen(cmdline) + 1;
  // int length = wcslen(cmdline) + 1;
  // char *argbuf = new char[length];
  // memcpy(argbuf, cmdline, length);

#ifdef UNICODE
  LPTSTR argbuf = _wcsdup(cmdline);
#else
  LPTSTR argbuf = strdup(cmdline);
#endif
  int i = 0;
  _TCHAR q;
  

  while (argbuf[i]) {
    while ((argbuf[i]) && (iswspace(argbuf[i])))
      i++;

    if (argbuf[i]) {
      if ((argbuf[i] == '\'') || (argbuf[i] == '"')) {
	q = argbuf[i++];
	if (!argbuf[i])
	  break;
      }
      else
	q = 0;

      args.push_back(String(&argbuf[i]));

      while ((argbuf[i]) && ((q) ? (argbuf[i] != q) : (!iswspace(argbuf[i]))))
	i++;

      if (argbuf[i]) {
	argbuf[i] = 0;
	i++;
      }
    }
  }

  free(argbuf);

  main(args);
  doMessageLoop();
}
