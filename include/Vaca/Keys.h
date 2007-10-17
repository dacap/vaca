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

#ifndef VACA_KEYS_H
#define VACA_KEYS_H

#include "Vaca/base.h"

namespace Vaca {

struct Keys {
  typedef int Type;

  // codes
  static const int None = 0;
  static const int LButton = VK_LBUTTON;
  static const int RButton = VK_RBUTTON;
  static const int Cancel = VK_CANCEL;
  static const int MButton = VK_MBUTTON;
#if (_WIN32_WINNT >= 0x0500)
  static const int XButton1 = VK_XBUTTON1;
  static const int XButton2 = VK_XBUTTON2;
#endif
  static const int Back = VK_BACK;
  static const int Tab = VK_TAB;
  static const int LineFeed = 10;
  static const int Clear = VK_CLEAR;
  static const int Return = VK_RETURN;
  static const int Enter = VK_RETURN;
  static const int ShiftKey = VK_SHIFT;
  static const int ControlKey = VK_CONTROL;
  static const int Menu = VK_MENU;
  static const int Pause = VK_PAUSE;
  static const int Capital = VK_CAPITAL;
  static const int CapsLock = VK_CAPITAL;
  static const int KanaMode = VK_KANA;
  static const int HangeulMode = VK_HANGEUL;
  static const int HangulMode = VK_HANGUL;
  static const int JunjaMode = VK_JUNJA;
  static const int FinalMode = VK_FINAL;
  static const int HanjaMode = VK_HANJA;
  static const int KanjiMode = VK_KANJI;
  static const int Escape = VK_ESCAPE;
  static const int IMEConvert = VK_CONVERT;
  static const int IMENonconvert = VK_NONCONVERT;
  static const int IMEAccept = VK_ACCEPT;
  static const int IMEModeChange = VK_MODECHANGE;
  static const int Space = VK_SPACE;
  static const int Prior = VK_PRIOR;
  static const int PageUp = VK_PRIOR;
  static const int Next = VK_NEXT;
  static const int PageDown = VK_NEXT;
  static const int End = VK_END;
  static const int Home = VK_HOME;
  static const int Left = VK_LEFT;
  static const int Up = VK_UP;
  static const int Right = VK_RIGHT;
  static const int Down = VK_DOWN;
  static const int Select = VK_SELECT;
  static const int Print = VK_PRINT;
  static const int Execute = VK_EXECUTE;
  static const int Snapshot = VK_SNAPSHOT;
  static const int PrintScreen = VK_SNAPSHOT;
  static const int Insert = VK_INSERT;
  static const int Delete = VK_DELETE;
  static const int Help = VK_HELP;
  static const int D0 = 0x30;
  static const int D1 = 0x31;
  static const int D2 = 0x32;
  static const int D3 = 0x33;
  static const int D4 = 0x34;
  static const int D5 = 0x35;
  static const int D6 = 0x36;
  static const int D7 = 0x37;
  static const int D8 = 0x38;
  static const int D9 = 0x39;
  static const int A = 0x41;
  static const int B = 0x42;
  static const int C = 0x43;
  static const int D = 0x44;
  static const int E = 0x45;
  static const int F = 0x46;
  static const int G = 0x47;
  static const int H = 0x48;
  static const int I = 0x49;
  static const int J = 0x4A;
  static const int K = 0x4B;
  static const int L = 0x4C;
  static const int M = 0x4D;
  static const int N = 0x4E;
  static const int O = 0x4F;
  static const int P = 0x50;
  static const int Q = 0x51;
  static const int R = 0x52;
  static const int S = 0x53;
  static const int T = 0x54;
  static const int U = 0x55;
  static const int V = 0x56;
  static const int W = 0x57;
  static const int X = 0x58;
  static const int Y = 0x59;
  static const int Z = 0x5A;
  static const int LWin = VK_LWIN;
  static const int RWin = VK_RWIN;
  static const int Apps = VK_APPS;
  static const int Sleep = VK_SLEEP;
  static const int NumPad0 = VK_NUMPAD0;
  static const int NumPad1 = VK_NUMPAD1;
  static const int NumPad2 = VK_NUMPAD2;
  static const int NumPad3 = VK_NUMPAD3;
  static const int NumPad4 = VK_NUMPAD4;
  static const int NumPad5 = VK_NUMPAD5;
  static const int NumPad6 = VK_NUMPAD6;
  static const int NumPad7 = VK_NUMPAD7;
  static const int NumPad8 = VK_NUMPAD8;
  static const int NumPad9 = VK_NUMPAD9;
  static const int Multiply = VK_MULTIPLY;
  static const int Add = VK_ADD;
  static const int Separator = VK_SEPARATOR;
  static const int Subtract = VK_SUBTRACT;
  static const int Decimal = VK_DECIMAL;
  static const int Divide = VK_DIVIDE;
  static const int F1 = VK_F1;
  static const int F2 = VK_F2;
  static const int F3 = VK_F3;
  static const int F4 = VK_F4;
  static const int F5 = VK_F5;
  static const int F6 = VK_F6;
  static const int F7 = VK_F7;
  static const int F8 = VK_F8;
  static const int F9 = VK_F9;
  static const int F10 = VK_F10;
  static const int F11 = VK_F11;
  static const int F12 = VK_F12;
  static const int F13 = VK_F13;
  static const int F14 = VK_F14;
  static const int F15 = VK_F15;
  static const int F16 = VK_F16;
  static const int F17 = VK_F17;
  static const int F18 = VK_F18;
  static const int F19 = VK_F19;
  static const int F20 = VK_F20;
  static const int F21 = VK_F21;
  static const int F22 = VK_F22;
  static const int F23 = VK_F23;
  static const int F24 = VK_F24;
  static const int NumLock = VK_NUMLOCK;
  static const int Scroll = VK_SCROLL;
  static const int LShiftKey = VK_LSHIFT;
  static const int RShiftKey = VK_RSHIFT;
  static const int LControlKey = VK_LCONTROL;
  static const int RControlKey = VK_RCONTROL;
  static const int LMenu = VK_LMENU;
  static const int RMenu = VK_RMENU;
#if (_WIN32_WINNT >= 0x0500)
  static const int BrowserBack = VK_BROWSER_BACK;
  static const int BrowserForward = VK_BROWSER_FORWARD;
  static const int BrowserRefresh = VK_BROWSER_REFRESH;
  static const int BrowserStop = VK_BROWSER_STOP;
  static const int BrowserSearch = VK_BROWSER_SEARCH;
  static const int BrowserFavorites = VK_BROWSER_FAVORITES;
  static const int BrowserHome = VK_BROWSER_HOME;
  static const int VolumeMute = VK_VOLUME_MUTE;
  static const int VolumeDown = VK_VOLUME_DOWN;
  static const int VolumeUp = VK_VOLUME_UP;
  static const int MediaNextTrack = VK_MEDIA_NEXT_TRACK;
  static const int MediaPreviousTrack = VK_MEDIA_PREV_TRACK;
  static const int MediaStop = VK_MEDIA_STOP;
  static const int MediaPlayPause = VK_MEDIA_PLAY_PAUSE;
  static const int LaunchMail = VK_LAUNCH_MAIL;
  static const int LaunchMediaSelect = VK_LAUNCH_MEDIA_SELECT;
  static const int LaunchApplication1 = VK_LAUNCH_APP1;
  static const int LaunchApplication2 = VK_LAUNCH_APP2;
#endif
//   const int Oem_1 = VK_OEM_1;
// #if (_WIN32_WINNT >= 0x0500)
//   const int OemPlus = VK_OEM_PLUS;
//   const int OemComma = VK_OEM_COMMA;
//   const int OemMinus = VK_OEM_MINUS;
//   const int OemPeriod = VK_OEM_PERIOD;
// #endif
//   const int Oem_2 = VK_OEM_2;
//   const int Oem_3 = VK_OEM_3;
//   const int Oem_4 = VK_OEM_4;
//   const int Oem_5 = VK_OEM_5;
//   const int Oem_6 = VK_OEM_6;
//   const int Oem_7 = VK_OEM_7;
//   const int Oem_8 = VK_OEM_8;
// #if (_WIN32_WINNT >= 0x0500)
//   const int Oem_102 = VK_OEM_102;
// #endif
  static const int ProcessKey = VK_PROCESSKEY;
// #if (_WIN32_WINNT >= 0x0500)
//   const int Packet = VK_PACKET;
// #endif
  static const int Attn = VK_ATTN;
  static const int Crsel = VK_CRSEL;
  static const int Exsel = VK_EXSEL;
  static const int EraseEof = VK_EREOF;
  static const int Play = VK_PLAY;
  static const int Zoom = VK_ZOOM;
  static const int NoName = VK_NONAME;
  static const int Pa1 = VK_PA1;
//   static const int OemClear = VK_OEM_CLEAR;

  // masks
  static const int KeyCode   = 0x0000ffff;
  static const int Modifiers = 0xffff0000;

  // modifiers
  static const int Shift     = 0x00010000;
  static const int Control   = 0x00020000;
  static const int Alt       = 0x00040000;

  static Keys::Type VACA_DLL fromMessageParams(WPARAM wParam, LPARAM lParam);
  
};

} // namespace Vaca

#endif
