// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

const wchar_t* convert_to_string(MouseButton but)
{
  switch (but) {
    case MouseButton::None: return L"None";
    case MouseButton::Left: return L"Left";
    case MouseButton::Right: return L"Right";
    case MouseButton::Middle: return L"Middle";
  }
  return L"Unknown";
}

const wchar_t* convert_to_string(Keys::Type keyCode)
{
  switch (keyCode) {
    case Keys::None: return L"None";
    case Keys::LButton: return L"LButton";
    case Keys::RButton: return L"RButton";
    case Keys::Cancel: return L"Cancel";
    case Keys::MButton: return L"MButton";
    case Keys::XButton1: return L"XButton1";
    case Keys::XButton2: return L"XButton2";
    case Keys::Back: return L"Back";
    case Keys::Tab: return L"Tab";
    case Keys::LineFeed: return L"LineFeed";
    case Keys::Clear: return L"Clear";
      //case Keys::Return: return L"Return";
    case Keys::Enter: return L"Enter";
    case Keys::ShiftKey: return L"ShiftKey";
    case Keys::ControlKey: return L"ControlKey";
    case Keys::Menu: return L"Menu";
    case Keys::Pause: return L"Pause";
      //case Keys::Capital: return L"Capital";
    case Keys::CapsLock: return L"CapsLock";
    case Keys::KanaMode: return L"KanaMode";
      //case Keys::HangeulMode: return L"HangeulMode";
      //case Keys::HangulMode: return L"HangulMode";
    case Keys::JunjaMode: return L"JunjaMode";
    case Keys::FinalMode: return L"FinalMode";
    case Keys::HanjaMode: return L"HanjaMode";
      //case Keys::KanjiMode: return L"KanjiMode";
    case Keys::Escape: return L"Escape";
    case Keys::IMEConvert: return L"IMEConvert";
    case Keys::IMENonconvert: return L"IMENonconvert";
    case Keys::IMEAccept: return L"IMEAccept";
    case Keys::IMEModeChange: return L"IMEModeChange";
    case Keys::Space: return L"Space";
      //case Keys::Prior: return L"Prior";
    case Keys::PageUp: return L"PageUp";
      //case Keys::Next: return L"Next";
    case Keys::PageDown: return L"PageDown";
    case Keys::End: return L"End";
    case Keys::Home: return L"Home";
    case Keys::Left: return L"Left";
    case Keys::Up: return L"Up";
    case Keys::Right: return L"Right";
    case Keys::Down: return L"Down";
    case Keys::Select: return L"Select";
    case Keys::Print: return L"Print";
    case Keys::Execute: return L"Execute";
      //case Keys::Snapshot: return L"Snapshot";
    case Keys::PrintScreen: return L"PrintScreen";
    case Keys::Insert: return L"Insert";
    case Keys::Delete: return L"Delete";
    case Keys::Help: return L"Help";
    case Keys::D0: return L"D0";
    case Keys::D1: return L"D1";
    case Keys::D2: return L"D2";
    case Keys::D3: return L"D3";
    case Keys::D4: return L"D4";
    case Keys::D5: return L"D5";
    case Keys::D6: return L"D6";
    case Keys::D7: return L"D7";
    case Keys::D8: return L"D8";
    case Keys::D9: return L"D9";
    case Keys::A: return L"A";
    case Keys::B: return L"B";
    case Keys::C: return L"C";
    case Keys::D: return L"D";
    case Keys::E: return L"E";
    case Keys::F: return L"F";
    case Keys::G: return L"G";
    case Keys::H: return L"H";
    case Keys::I: return L"I";
    case Keys::J: return L"J";
    case Keys::K: return L"K";
    case Keys::L: return L"L";
    case Keys::M: return L"M";
    case Keys::N: return L"N";
    case Keys::O: return L"O";
    case Keys::P: return L"P";
    case Keys::Q: return L"Q";
    case Keys::R: return L"R";
    case Keys::S: return L"S";
    case Keys::T: return L"T";
    case Keys::U: return L"U";
    case Keys::V: return L"V";
    case Keys::W: return L"W";
    case Keys::X: return L"X";
    case Keys::Y: return L"Y";
    case Keys::Z: return L"Z";
    case Keys::LWin: return L"LWin";
    case Keys::RWin: return L"RWin";
    case Keys::Apps: return L"Apps";
    case Keys::Sleep: return L"Sleep";
    case Keys::NumPad0: return L"NumPad0";
    case Keys::NumPad1: return L"NumPad1";
    case Keys::NumPad2: return L"NumPad2";
    case Keys::NumPad3: return L"NumPad3";
    case Keys::NumPad4: return L"NumPad4";
    case Keys::NumPad5: return L"NumPad5";
    case Keys::NumPad6: return L"NumPad6";
    case Keys::NumPad7: return L"NumPad7";
    case Keys::NumPad8: return L"NumPad8";
    case Keys::NumPad9: return L"NumPad9";
    case Keys::Multiply: return L"Multiply";
    case Keys::Add: return L"Add";
    case Keys::Separator: return L"Separator";
    case Keys::Subtract: return L"Subtract";
    case Keys::Decimal: return L"Decimal";
    case Keys::Divide: return L"Divide";
    case Keys::F1: return L"F1";
    case Keys::F2: return L"F2";
    case Keys::F3: return L"F3";
    case Keys::F4: return L"F4";
    case Keys::F5: return L"F5";
    case Keys::F6: return L"F6";
    case Keys::F7: return L"F7";
    case Keys::F8: return L"F8";
    case Keys::F9: return L"F9";
    case Keys::F10: return L"F10";
    case Keys::F11: return L"F11";
    case Keys::F12: return L"F12";
    case Keys::F13: return L"F13";
    case Keys::F14: return L"F14";
    case Keys::F15: return L"F15";
    case Keys::F16: return L"F16";
    case Keys::F17: return L"F17";
    case Keys::F18: return L"F18";
    case Keys::F19: return L"F19";
    case Keys::F20: return L"F20";
    case Keys::F21: return L"F21";
    case Keys::F22: return L"F22";
    case Keys::F23: return L"F23";
    case Keys::F24: return L"F24";
    case Keys::NumLock: return L"NumLock";
    case Keys::Scroll: return L"Scroll";
    case Keys::LShiftKey: return L"LShiftKey";
    case Keys::RShiftKey: return L"RShiftKey";
    case Keys::LControlKey: return L"LControlKey";
    case Keys::RControlKey: return L"RControlKey";
    case Keys::LMenu: return L"LMenu";
    case Keys::RMenu: return L"RMenu";
    case Keys::BrowserBack: return L"BrowserBack";
    case Keys::BrowserForward: return L"BrowserForward";
    case Keys::BrowserRefresh: return L"BrowserRefresh";
    case Keys::BrowserStop: return L"BrowserStop";
    case Keys::BrowserSearch: return L"BrowserSearch";
    case Keys::BrowserFavorites: return L"BrowserFavorites";
    case Keys::BrowserHome: return L"BrowserHome";
    case Keys::VolumeMute: return L"VolumeMute";
    case Keys::VolumeDown: return L"VolumeDown";
    case Keys::VolumeUp: return L"VolumeUp";
    case Keys::MediaNextTrack: return L"MediaNextTrack";
    case Keys::MediaPreviousTrack: return L"MediaPreviousTrack";
    case Keys::MediaStop: return L"MediaStop";
    case Keys::MediaPlayPause: return L"MediaPlayPause";
    case Keys::LaunchMail: return L"LaunchMail";
    case Keys::LaunchMediaSelect: return L"LaunchMediaSelect";
    case Keys::LaunchApplication1: return L"LaunchApplication1";
    case Keys::LaunchApplication2: return L"LaunchApplication2";
    case Keys::ProcessKey: return L"ProcessKey";
    case Keys::Attn: return L"Attn";
    case Keys::Crsel: return L"Crsel";
    case Keys::Exsel: return L"Exsel";
    case Keys::EraseEof: return L"EraseEof";
    case Keys::Play: return L"Play";
    case Keys::Zoom: return L"Zoom";
    case Keys::NoName: return L"NoName";
    case Keys::Pa1: return L"Pa1";
  }
  return L"Unknown";
}

const wchar_t* convert_to_string(WidgetHit hit)
{
  switch (hit) {
    case WidgetHit::Error: return L"WidgetHit::Error";
    case WidgetHit::Transparent: return L"WidgetHit::Transparent";
    case WidgetHit::Nowhere: return L"WidgetHit::Nowhere";
    case WidgetHit::Client: return L"WidgetHit::Client";
    case WidgetHit::Caption: return L"WidgetHit::Caption";
    case WidgetHit::SystemMenu: return L"WidgetHit::SystemMenu";
    case WidgetHit::Size: return L"WidgetHit::Size";
    case WidgetHit::Menu: return L"WidgetHit::Menu";
    case WidgetHit::HorizontalScroll: return L"WidgetHit::HorizontalScroll";
    case WidgetHit::VerticalScroll: return L"WidgetHit::VerticalScroll";
    case WidgetHit::MinimizeButton: return L"WidgetHit::MinimizeButton";
    case WidgetHit::MaximizeButton: return L"WidgetHit::MaximizeButton";
    case WidgetHit::Left: return L"WidgetHit::Left";
    case WidgetHit::Right: return L"WidgetHit::Right";
    case WidgetHit::Top: return L"WidgetHit::Top";
    case WidgetHit::TopLeft: return L"WidgetHit::TopLeft";
    case WidgetHit::TopRight: return L"WidgetHit::TopRight";
    case WidgetHit::Bottom: return L"WidgetHit::Bottom";
    case WidgetHit::BottomLeft: return L"WidgetHit::BottomLeft";
    case WidgetHit::BottomRight: return L"WidgetHit::BottomRight";
    case WidgetHit::Border: return L"WidgetHit::Border";
    case WidgetHit::Object: return L"WidgetHit::Object";
    case WidgetHit::Close: return L"WidgetHit::Close";
    case WidgetHit::Help: return L"WidgetHit::Help";
  }
  return L"Unknown";
}
