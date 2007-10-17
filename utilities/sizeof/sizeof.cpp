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

#include <Vaca/Vaca.h>
#include <cstdio>

using namespace Vaca;
using namespace std;

#define PRINTF_SIZEOF(type) \
  printf("  sizeof(" #type ") = %d bytes\n", sizeof(type))

int main()
{
  printf("Vaca overhead:\n");
  printf("\n");
  PRINTF_SIZEOF(Point);
  PRINTF_SIZEOF(Size);
  PRINTF_SIZEOF(Rect);
  printf("\n");
  PRINTF_SIZEOF(Widget);
  PRINTF_SIZEOF(Layout);
  PRINTF_SIZEOF(Constraint);
  printf("\n");
  PRINTF_SIZEOF(Frame);
  PRINTF_SIZEOF(Dialog);
  printf("\n");
  printf("Examples:\n");
  printf("\n");
  printf("      10 Widgets = %.2f Kb\n", sizeof(Widget)*10.0f/1024.0f);
  printf("     100 Widgets = %.2f Kb\n", sizeof(Widget)*100.0f/1024.0f);
  printf("   1,000 Widgets = %.2f Kb\n", sizeof(Widget)*1000.0f/1024.0f);
  printf("  10,000 Widgets = %.2f Mb\n", sizeof(Widget)*10000.0f/1024.0f/1024.0f);
  printf("\n");
  return 0;
}
