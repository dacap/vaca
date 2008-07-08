// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

#include <Vaca/Vaca.hpp>

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

struct ColorName
{
  const char *name;
  int rgb;
} ColorNames[] = {		// The colors where taken from Emacs
  { "alice blue", 0xf0f8ff },
  { "antique white", 0xfaebd7 },
  { "Antique White1", 0xffefdb },
  { "Antique White2", 0xeedfcc },
  { "Antique White3", 0xcdc0b0 },
  { "Antique White4", 0x8b8378 },
  { "aquamarine", 0x7fffd4 },
  { "aquamarine1", 0x7fffd4 },
  { "aquamarine2", 0x76eec6 },
  { "aquamarine3", 0x66cdaa },
  { "aquamarine4", 0x458b74 },
  { "azure", 0xf0ffff },
  { "azure1", 0xf0ffff },
  { "azure2", 0xe0eeee },
  { "azure3", 0xc1cdcd },
  { "azure4", 0x838b8b },
  { "beige", 0xf5f5dc },
  { "bisque", 0xffe4c4 },
  { "bisque1", 0xffe4c4 },
  { "bisque2", 0xeed5b7 },
  { "bisque3", 0xcdb79e },
  { "bisque4", 0x8b7d6b },
  { "black", 0x000000 },
  { "blanched almond", 0xffebcd },
  { "blue", 0x0000ff },
  { "blue1", 0x0000ff },
  { "blue2", 0x0000ee },
  { "blue3", 0x0000cd },
  { "blue4", 0x00008b },
  { "blue violet", 0x8a2be2 },
  { "brown", 0xa52a2a },
  { "brown1", 0xff4040 },
  { "brown2", 0xee3b3b },
  { "brown3", 0xcd3333 },
  { "brown4", 0x8b2323 },
  { "burlywood", 0xdeb887 },
  { "burlywood1", 0xffd39b },
  { "burlywood2", 0xeec591 },
  { "burlywood3", 0xcdaa7d },
  { "burlywood4", 0x8b7355 },
  { "cadet blue", 0x5f9ea0 },
  { "Cadet Blue1", 0x98f5ff },
  { "Cadet Blue2", 0x8ee5ee },
  { "Cadet Blue3", 0x7ac5cd },
  { "Cadet Blue4", 0x53868b },
  { "chartreuse", 0x7fff00 },
  { "chartreuse1", 0x7fff00 },
  { "chartreuse2", 0x76ee00 },
  { "chartreuse3", 0x66cd00 },
  { "chartreuse4", 0x458b00 },
  { "chocolate", 0xd2691e },
  { "chocolate1", 0xff7f24 },
  { "chocolate2", 0xee7621 },
  { "chocolate3", 0xcd661d },
  { "chocolate4", 0x8b4513 },
  { "coral", 0xff7f50 },
  { "coral1", 0xff7256 },
  { "coral2", 0xee6a50 },
  { "coral3", 0xcd5b45 },
  { "coral4", 0x8b3e2f },
  { "cornflower blue", 0x6495ed },
  { "cornsilk", 0xfff8dc },
  { "cornsilk1", 0xfff8dc },
  { "cornsilk2", 0xeee8cd },
  { "cornsilk3", 0xcdc8b1 },
  { "cornsilk4", 0x8b8878 },
  { "cyan", 0x00ffff },
  { "cyan1", 0x00ffff },
  { "cyan2", 0x00eeee },
  { "cyan3", 0x00cdcd },
  { "cyan4", 0x008b8b },
  { "dark blue", 0x00008b },
  { "dark cyan", 0x008b8b },
  { "dark goldenrod", 0xb8860b },
  { "Dark Goldenrod1", 0xffb90f },
  { "Dark Goldenrod2", 0xeead0e },
  { "Dark Goldenrod3", 0xcd950c },
  { "Dark Goldenrod4", 0x8b6508 },
  { "dark gray", 0xa9a9a9 },
  { "dark green", 0x006400 },
  { "dark khaki", 0xbdb76b },
  { "dark magenta", 0x8b008b },
  { "dark olive green", 0x556b2f },
  { "Dark Olive Green1", 0xcaff70 },
  { "Dark Olive Green2", 0xbcee68 },
  { "Dark Olive Green3", 0xa2cd5a },
  { "Dark Olive Green4", 0x6e8b3d },
  { "dark orange", 0xff8c00 },
  { "Dark Orange1", 0xff7f00 },
  { "Dark Orange2", 0xee7600 },
  { "Dark Orange3", 0xcd6600 },
  { "Dark Orange4", 0x8b4500 },
  { "dark orchid", 0x9932cc },
  { "Dark Orchid1", 0xbf3eff },
  { "Dark Orchid2", 0xb23aee },
  { "Dark Orchid3", 0x9a32cd },
  { "Dark Orchid4", 0x68228b },
  { "dark red", 0x8b0000 },
  { "dark salmon", 0xe9967a },
  { "dark sea green", 0x8fbc8f },
  { "Dark Sea Green1", 0xc1ffc1 },
  { "Dark Sea Green2", 0xb4eeb4 },
  { "Dark Sea Green3", 0x9bcd9b },
  { "Dark Sea Green4", 0x698b69 },
  { "dark slate blue", 0x483d8b },
  { "dark slate gray", 0x2f4f4f },
  { "Dark Slate Gray1", 0x97ffff },
  { "Dark Slate Gray2", 0x8deeee },
  { "Dark Slate Gray3", 0x79cdcd },
  { "Dark Slate Gray4", 0x528b8b },
  { "dark turquoise", 0x00ced1 },
  { "dark violet", 0x9400d3 },
  { "deep pink", 0xff1493 },
  { "Deep Pink1", 0xff1493 },
  { "Deep Pink2", 0xee1289 },
  { "Deep Pink3", 0xcd1076 },
  { "Deep Pink4", 0x8b0a50 },
  { "deep sky blue", 0x00bfff },
  { "Deep Sky Blue1", 0x00bfff },
  { "Deep Sky Blue2", 0x00b2ee },
  { "Deep Sky Blue3", 0x009acd },
  { "Deep Sky Blue4", 0x00688b },
  { "dim gray", 0x696969 },
  { "dodger blue", 0x1e90ff },
  { "Dodger Blue1", 0x1e90ff },
  { "Dodger Blue2", 0x1c86ee },
  { "Dodger Blue3", 0x1874cd },
  { "Dodger Blue4", 0x104e8b },
  { "firebrick", 0xb22222 },
  { "firebrick1", 0xff3030 },
  { "firebrick2", 0xee2c2c },
  { "firebrick3", 0xcd2626 },
  { "firebrick4", 0x8b1a1a },
  { "floral white", 0xfffaf0 },
  { "forest green", 0x228b22 },
  { "gainsboro", 0xdcdcdc },
  { "ghost white", 0xf8f8ff },
  { "gold", 0xffd700 },
  { "gold1", 0xffd700 },
  { "gold2", 0xeec900 },
  { "gold3", 0xcdad00 },
  { "gold4", 0x8b7500 },
  { "goldenrod", 0xdaa520 },
  { "goldenrod1", 0xffc125 },
  { "goldenrod2", 0xeeb422 },
  { "goldenrod3", 0xcd9b1d },
  { "goldenrod4", 0x8b6914 },
  { "gray", 0xbebebe },
  { "gray0", 0x000000 },
  { "gray1", 0x030303 },
  { "gray2", 0x050505 },
  { "gray3", 0x080808 },
  { "gray4", 0x0a0a0a },
  { "gray5", 0x0d0d0d },
  { "gray6", 0x0f0f0f },
  { "gray7", 0x121212 },
  { "gray8", 0x141414 },
  { "gray9", 0x171717 },
  { "gray10", 0x1a1a1a },
  { "gray11", 0x1c1c1c },
  { "gray12", 0x1f1f1f },
  { "gray13", 0x212121 },
  { "gray14", 0x242424 },
  { "gray15", 0x262626 },
  { "gray16", 0x292929 },
  { "gray17", 0x2b2b2b },
  { "gray18", 0x2e2e2e },
  { "gray19", 0x303030 },
  { "gray20", 0x333333 },
  { "gray21", 0x363636 },
  { "gray22", 0x383838 },
  { "gray23", 0x3b3b3b },
  { "gray24", 0x3d3d3d },
  { "gray25", 0x404040 },
  { "gray26", 0x424242 },
  { "gray27", 0x454545 },
  { "gray28", 0x474747 },
  { "gray29", 0x4a4a4a },
  { "gray30", 0x4d4d4d },
  { "gray31", 0x4f4f4f },
  { "gray32", 0x525252 },
  { "gray33", 0x545454 },
  { "gray34", 0x575757 },
  { "gray35", 0x595959 },
  { "gray36", 0x5c5c5c },
  { "gray37", 0x5e5e5e },
  { "gray38", 0x616161 },
  { "gray39", 0x636363 },
  { "gray40", 0x666666 },
  { "gray41", 0x696969 },
  { "gray42", 0x6b6b6b },
  { "gray43", 0x6e6e6e },
  { "gray44", 0x707070 },
  { "gray45", 0x737373 },
  { "gray46", 0x757575 },
  { "gray47", 0x787878 },
  { "gray48", 0x7a7a7a },
  { "gray49", 0x7d7d7d },
  { "gray50", 0x7f7f7f },
  { "gray51", 0x828282 },
  { "gray52", 0x858585 },
  { "gray53", 0x878787 },
  { "gray54", 0x8a8a8a },
  { "gray55", 0x8c8c8c },
  { "gray56", 0x8f8f8f },
  { "gray57", 0x919191 },
  { "gray58", 0x949494 },
  { "gray59", 0x969696 },
  { "gray60", 0x999999 },
  { "gray61", 0x9c9c9c },
  { "gray62", 0x9e9e9e },
  { "gray63", 0xa1a1a1 },
  { "gray64", 0xa3a3a3 },
  { "gray65", 0xa6a6a6 },
  { "gray66", 0xa8a8a8 },
  { "gray67", 0xababab },
  { "gray68", 0xadadad },
  { "gray69", 0xb0b0b0 },
  { "gray70", 0xb3b3b3 },
  { "gray71", 0xb5b5b5 },
  { "gray72", 0xb8b8b8 },
  { "gray73", 0xbababa },
  { "gray74", 0xbdbdbd },
  { "gray75", 0xbfbfbf },
  { "gray76", 0xc2c2c2 },
  { "gray77", 0xc4c4c4 },
  { "gray78", 0xc7c7c7 },
  { "gray79", 0xc9c9c9 },
  { "gray80", 0xcccccc },
  { "gray81", 0xcfcfcf },
  { "gray82", 0xd1d1d1 },
  { "gray83", 0xd4d4d4 },
  { "gray84", 0xd6d6d6 },
  { "gray85", 0xd9d9d9 },
  { "gray86", 0xdbdbdb },
  { "gray87", 0xdedede },
  { "gray88", 0xe0e0e0 },
  { "gray89", 0xe3e3e3 },
  { "gray90", 0xe5e5e5 },
  { "gray91", 0xe8e8e8 },
  { "gray92", 0xebebeb },
  { "gray93", 0xededed },
  { "gray94", 0xf0f0f0 },
  { "gray95", 0xf2f2f2 },
  { "gray96", 0xf5f5f5 },
  { "gray97", 0xf7f7f7 },
  { "gray98", 0xfafafa },
  { "gray99", 0xfcfcfc },
  { "gray100", 0xffffff },
  { "green", 0x00ff00 },
  { "green1", 0x00ff00 },
  { "green2", 0x00ee00 },
  { "green3", 0x00cd00 },
  { "green4", 0x008b00 },
  { "green yellow", 0xadff2f },
  { "honeydew", 0xf0fff0 },
  { "honeydew1", 0xf0fff0 },
  { "honeydew2", 0xe0eee0 },
  { "honeydew3", 0xc1cdc1 },
  { "honeydew4", 0x838b83 },
  { "hot pink", 0xff69b4 },
  { "Hot Pink1", 0xff6eb4 },
  { "Hot Pink2", 0xee6aa7 },
  { "Hot Pink3", 0xcd6090 },
  { "Hot Pink4", 0x8b3a62 },
  { "indian red", 0xcd5c5c },
  { "Indian Red1", 0xff6a6a },
  { "Indian Red2", 0xee6363 },
  { "Indian Red3", 0xcd5555 },
  { "Indian Red4", 0x8b3a3a },
  { "ivory", 0xfffff0 },
  { "ivory1", 0xfffff0 },
  { "ivory2", 0xeeeee0 },
  { "ivory3", 0xcdcdc1 },
  { "ivory4", 0x8b8b83 },
  { "khaki", 0xf0e68c },
  { "khaki1", 0xfff68f },
  { "khaki2", 0xeee685 },
  { "khaki3", 0xcdc673 },
  { "khaki4", 0x8b864e },
  { "lavender", 0xe6e6fa },
  { "lavender blush", 0xfff0f5 },
  { "Lavender Blush1", 0xfff0f5 },
  { "Lavender Blush2", 0xeee0e5 },
  { "Lavender Blush3", 0xcdc1c5 },
  { "Lavender Blush4", 0x8b8386 },
  { "lawn green", 0x7cfc00 },
  { "lemon chiffon", 0xfffacd },
  { "Lemon Chiffon1", 0xfffacd },
  { "Lemon Chiffon2", 0xeee9bf },
  { "Lemon Chiffon3", 0xcdc9a5 },
  { "Lemon Chiffon4", 0x8b8970 },
  { "light blue", 0xadd8e6 },
  { "Light Blue1", 0xbfefff },
  { "Light Blue2", 0xb2dfee },
  { "Light Blue3", 0x9ac0cd },
  { "Light Blue4", 0x68838b },
  { "light coral", 0xf08080 },
  { "light cyan", 0xe0ffff },
  { "Light Cyan1", 0xe0ffff },
  { "Light Cyan2", 0xd1eeee },
  { "Light Cyan3", 0xb4cdcd },
  { "Light Cyan4", 0x7a8b8b },
  { "light goldenrod", 0xeedd82 },
  { "Light Goldenrod1", 0xffec8b },
  { "Light Goldenrod2", 0xeedc82 },
  { "Light Goldenrod3", 0xcdbe70 },
  { "Light Goldenrod4", 0x8b814c },
  { "light goldenrod yellow", 0xfafad2 },
  { "light gray", 0xd3d3d3 },
  { "light green", 0x90ee90 },
  { "light pink", 0xffb6c1 },
  { "Light Pink1", 0xffaeb9 },
  { "Light Pink2", 0xeea2ad },
  { "Light Pink3", 0xcd8c95 },
  { "Light Pink4", 0x8b5f65 },
  { "light salmon", 0xffa07a },
  { "Light Salmon1", 0xffa07a },
  { "Light Salmon2", 0xee9572 },
  { "Light Salmon3", 0xcd8162 },
  { "Light Salmon4", 0x8b5742 },
  { "light sea green", 0x20b2aa },
  { "light sky blue", 0x87cefa },
  { "Light Sky Blue1", 0xb0e2ff },
  { "Light Sky Blue2", 0xa4d3ee },
  { "Light Sky Blue3", 0x8db6cd },
  { "Light Sky Blue4", 0x607b8b },
  { "light slate blue", 0x8470ff },
  { "light slate gray", 0x778899 },
  { "light steel blue", 0xb0c4de },
  { "Light Steel Blue1", 0xcae1ff },
  { "Light Steel Blue2", 0xbcd2ee },
  { "Light Steel Blue3", 0xa2b5cd },
  { "Light Steel Blue4", 0x6e7b8b },
  { "light yellow", 0xffffe0 },
  { "Light Yellow1", 0xffffe0 },
  { "Light Yellow2", 0xeeeed1 },
  { "Light Yellow3", 0xcdcdb4 },
  { "Light Yellow4", 0x8b8b7a },
  { "lime green", 0x32cd32 },
  { "linen", 0xfaf0e6 },
  { "magenta", 0xff00ff },
  { "magenta1", 0xff00ff },
  { "magenta2", 0xee00ee },
  { "magenta3", 0xcd00cd },
  { "magenta4", 0x8b008b },
  { "maroon", 0xb03060 },
  { "maroon1", 0xff34b3 },
  { "maroon2", 0xee30a7 },
  { "maroon3", 0xcd2990 },
  { "maroon4", 0x8b1c62 },
  { "medium aquamarine", 0x66cdaa },
  { "medium blue", 0x0000cd },
  { "medium orchid", 0xba55d3 },
  { "Medium Orchid1", 0xe066ff },
  { "Medium Orchid2", 0xd15fee },
  { "Medium Orchid3", 0xb452cd },
  { "Medium Orchid4", 0x7a378b },
  { "medium purple", 0x9370db },
  { "Medium Purple1", 0xab82ff },
  { "Medium Purple2", 0x9f79ee },
  { "Medium Purple3", 0x8968cd },
  { "Medium Purple4", 0x5d478b },
  { "medium sea green", 0x3cb371 },
  { "medium slate blue", 0x7b68ee },
  { "medium spring green", 0x00fa9a },
  { "medium turquoise", 0x48d1cc },
  { "medium violet red", 0xc71585 },
  { "midnight blue", 0x191970 },
  { "mint cream", 0xf5fffa },
  { "misty rose", 0xffe4e1 },
  { "Misty Rose1", 0xffe4e1 },
  { "Misty Rose2", 0xeed5d2 },
  { "Misty Rose3", 0xcdb7b5 },
  { "Misty Rose4", 0x8b7d7b },
  { "moccasin", 0xffe4b5 },
  { "navajo white", 0xffdead },
  { "Navajo White1", 0xffdead },
  { "Navajo White2", 0xeecfa1 },
  { "Navajo White3", 0xcdb38b },
  { "Navajo White4", 0x8b795e },
  { "navy", 0x000080 },
  { "old lace", 0xfdf5e6 },
  { "olive drab", 0x6b8e23 },
  { "Olive Drab1", 0xc0ff3e },
  { "Olive Drab2", 0xb3ee3a },
  { "Olive Drab3", 0x9acd32 },
  { "Olive Drab4", 0x698b22 },
  { "orange red", 0xff4500 },
  { "Orange Red1", 0xff4500 },
  { "Orange Red2", 0xee4000 },
  { "Orange Red3", 0xcd3700 },
  { "Orange Red4", 0x8b2500 },
  { "orange", 0xffa500 },
  { "orange1", 0xffa500 },
  { "orange2", 0xee9a00 },
  { "orange3", 0xcd8500 },
  { "orange4", 0x8b5a00 },
  { "orchid", 0xda70d6 },
  { "orchid1", 0xff83fa },
  { "orchid2", 0xee7ae9 },
  { "orchid3", 0xcd69c9 },
  { "orchid4", 0x8b4789 },
  { "pale goldenrod", 0xeee8aa },
  { "pale green", 0x98fb98 },
  { "Pale Green1", 0x9aff9a },
  { "Pale Green2", 0x90ee90 },
  { "Pale Green3", 0x7ccd7c },
  { "Pale Green4", 0x548b54 },
  { "pale turquoise", 0xafeeee },
  { "Pale Turquoise1", 0xbbffff },
  { "Pale Turquoise2", 0xaeeeee },
  { "Pale Turquoise3", 0x96cdcd },
  { "Pale Turquoise4", 0x668b8b },
  { "pale violet red", 0xdb7093 },
  { "Pale Violet Red1", 0xff82ab },
  { "Pale Violet Red2", 0xee799f },
  { "Pale Violet Red3", 0xcd6889 },
  { "Pale Violet Red4", 0x8b475d },
  { "papaya whip", 0xffefd5 },
  { "peach puff", 0xffdab9 },
  { "Peach Puff1", 0xffdab9 },
  { "Peach Puff2", 0xeecbad },
  { "Peach Puff3", 0xcdaf95 },
  { "Peach Puff4", 0x8b7765 },
  { "peru", 0xcd853f },
  { "pink", 0xffc0cb },
  { "pink1", 0xffb5c5 },
  { "pink2", 0xeea9b8 },
  { "pink3", 0xcd919e },
  { "pink4", 0x8b636c },
  { "plum", 0xdda0dd },
  { "plum1", 0xffbbff },
  { "plum2", 0xeeaeee },
  { "plum3", 0xcd96cd },
  { "plum4", 0x8b668b },
  { "powder blue", 0xb0e0e6 },
  { "purple", 0xa020f0 },
  { "purple1", 0x9b30ff },
  { "purple2", 0x912cee },
  { "purple3", 0x7d26cd },
  { "purple4", 0x551a8b },
  { "red", 0xff0000 },
  { "red1", 0xff0000 },
  { "red2", 0xee0000 },
  { "red3", 0xcd0000 },
  { "red4", 0x8b0000 },
  { "rosy brown", 0xbc8f8f },
  { "Rosy Brown1", 0xffc1c1 },
  { "Rosy Brown2", 0xeeb4b4 },
  { "Rosy Brown3", 0xcd9b9b },
  { "Rosy Brown4", 0x8b6969 },
  { "royal blue", 0x4169e1 },
  { "Royal Blue1", 0x4876ff },
  { "Royal Blue2", 0x436eee },
  { "Royal Blue3", 0x3a5fcd },
  { "Royal Blue4", 0x27408b },
  { "saddle brown", 0x8b4513 },
  { "salmon", 0xfa8072 },
  { "salmon1", 0xff8c69 },
  { "salmon2", 0xee8262 },
  { "salmon3", 0xcd7054 },
  { "salmon4", 0x8b4c39 },
  { "sandy brown", 0xf4a460 },
  { "sea green", 0x2e8b57 },
  { "Sea Green1", 0x54ff9f },
  { "Sea Green2", 0x4eee94 },
  { "Sea Green3", 0x43cd80 },
  { "Sea Green4", 0x2e8b57 },
  { "seashell", 0xfff5ee },
  { "seashell1", 0xfff5ee },
  { "seashell2", 0xeee5de },
  { "seashell3", 0xcdc5bf },
  { "seashell4", 0x8b8682 },
  { "sienna", 0xa0522d },
  { "sienna1", 0xff8247 },
  { "sienna2", 0xee7942 },
  { "sienna3", 0xcd6839 },
  { "sienna4", 0x8b4726 },
  { "sky blue", 0x87ceeb },
  { "Sky Blue1", 0x87ceff },
  { "Sky Blue2", 0x7ec0ee },
  { "Sky Blue3", 0x6ca6cd },
  { "Sky Blue4", 0x4a708b },
  { "slate blue", 0x6a5acd },
  { "Slate Blue1", 0x836fff },
  { "Slate Blue2", 0x7a67ee },
  { "Slate Blue3", 0x6959cd },
  { "Slate Blue4", 0x473c8b },
  { "slate gray", 0x708090 },
  { "Slate Gray1", 0xc6e2ff },
  { "Slate Gray2", 0xb9d3ee },
  { "Slate Gray3", 0x9fb6cd },
  { "Slate Gray4", 0x6c7b8b },
  { "snow", 0xfffafa },
  { "snow1", 0xfffafa },
  { "snow2", 0xeee9e9 },
  { "snow3", 0xcdc9c9 },
  { "snow4", 0x8b8989 },
  { "spring green", 0x00ff7f },
  { "Spring Green1", 0x00ff7f },
  { "Spring Green2", 0x00ee76 },
  { "Spring Green3", 0x00cd66 },
  { "Spring Green4", 0x008b45 },
  { "steel blue", 0x4682b4 },
  { "Steel Blue1", 0x63b8ff },
  { "Steel Blue2", 0x5cacee },
  { "Steel Blue3", 0x4f94cd },
  { "Steel Blue4", 0x36648b },
  { "SystemActiveBorder", 0xd4d0c8 },
  { "SystemActiveTitle", 0x0a246a },
  { "SystemAppWorkSpace", 0x808080 },
  { "SystemBackground", 0x3a6ea5 },
  { "SystemButtonAlternateFace", 0xb5b5b5 },
  { "SystemButtonDkShadow", 0x404040 },
  { "SystemButtonFace", 0xd4d0c8 },
  { "SystemButtonHilight", 0xffffff },
  { "SystemButtonLight", 0xd4d0c8 },
  { "SystemButtonShadow", 0x808080 },
  { "SystemButtonText", 0x000000 },
  { "SystemGradientActiveTitle", 0xa6caf0 },
  { "SystemGradientInactiveTitle", 0xc0c0c0 },
  { "SystemGrayText", 0x808080 },
  { "SystemHilight", 0x0a246a },
  { "SystemHilightText", 0xffffff },
  { "SystemHotTrackingColor", 0x000080 },
  { "SystemInactiveBorder", 0xd4d0c8 },
  { "SystemInactiveTitle", 0x808080 },
  { "SystemInactiveTitleText", 0xd4d0c8 },
  { "SystemInfoText", 0x000000 },
  { "SystemInfoWindow", 0xffffe1 },
  { "SystemMenu", 0xd4d0c8 },
  { "SystemMenuText", 0x000000 },
  { "SystemScrollbar", 0xd4d0c8 },
  { "SystemTitleText", 0xffffff },
  { "SystemWindowFrame", 0x000000 },
  { "tan", 0xd2b48c },
  { "tan1", 0xffa54f },
  { "tan2", 0xee9a49 },
  { "tan3", 0xcd853f },
  { "tan4", 0x8b5a2b },
  { "thistle", 0xd8bfd8 },
  { "thistle1", 0xffe1ff },
  { "thistle2", 0xeed2ee },
  { "thistle3", 0xcdb5cd },
  { "thistle4", 0x8b7b8b },
  { "tomato", 0xff6347 },
  { "tomato1", 0xff6347 },
  { "tomato2", 0xee5c42 },
  { "tomato3", 0xcd4f39 },
  { "tomato4", 0x8b3626 },
  { "turquoise", 0x40e0d0 },
  { "turquoise1", 0x00f5ff },
  { "turquoise2", 0x00e5ee },
  { "turquoise3", 0x00c5cd },
  { "turquoise4", 0x00868b },
  { "violet", 0xee82ee },
  { "violet red", 0xd02090 },
  { "Violet Red1", 0xff3e96 },
  { "Violet Red2", 0xee3a8c },
  { "Violet Red3", 0xcd3278 },
  { "Violet Red4", 0x8b2252 },
  { "wheat", 0xf5deb3 },
  { "wheat1", 0xffe7ba },
  { "wheat2", 0xeed8ae },
  { "wheat3", 0xcdba96 },
  { "wheat4", 0x8b7e66 },
  { "white", 0xffffff },
  { "white smoke", 0xf5f5f5 },
  { "yellow", 0xffff00 },
  { "yellow1", 0xffff00 },
  { "yellow2", 0xeeee00 },
  { "yellow3", 0xcdcd00 },
  { "yellow4", 0x8b8b00 },
  { "yellow green", 0x9acd32 },
};

int ColorNames_size = sizeof(ColorNames) / sizeof(struct ColorName);

//////////////////////////////////////////////////////////////////////

class AutoCompletionComboBox : public ComboBox
{
  Timer m_timer;
  
public:

  AutoCompletionComboBox(Widget* parent)
    : ComboBox(parent, EditComboBoxStyle)
    , m_timer(250)
  {
    for (int c=0; c<ColorNames_size; ++c)
      addItem(ColorNames[c].name);

    m_timer.Action.connect(&AutoCompletionComboBox::complete, this);
  }

  // signals
  Signal1<void, const Color&> SelColor;
  Signal0<void> NoColor;

protected:

  // events
  virtual void onSelChange(Event& ev)
  {
    ComboBox::onSelChange(ev);
    fireSignal();
  }

  // events
  virtual void onEditChange(Event& ev)
  {
    ComboBox::onEditChange(ev);
    m_timer.start();
  }

private:

  void complete()
  {
    String text = getText();
    int len1 = text.size();

    setCurrentItem(text);
    setDropDownVisibile(true);

    String new_text = getText();
    int len2 = new_text.size();

    if (len2 < len1) {
      setText(text);
      NoColor();
    }
    else {
      setText(text + new_text.substr(len1));
      fireSignal();
    }

    sendMessage(CB_SETEDITSEL, 0, MAKELONG(len1, len2));

    m_timer.stop();
  }

  void fireSignal()
  {
    int i = getCurrentItem();
    if (i >= 0 && i < ColorNames_size) {
      int rgb = ColorNames[i].rgb;
      SelColor(Color((rgb & 0xff0000) >> 16,
		     (rgb & 0x00ff00) >> 8,
		     (rgb & 0x00000ff)));
    }
    else
      NoColor();
  }
  
};

//////////////////////////////////////////////////////////////////////

class ColorViewer : public Panel
{
  Color* m_color;

public:

  ColorViewer(Widget* parent)
    : Panel(parent)
  {
    m_color = NULL;
    setPreferredSize(Size(32, 8));
  }

  virtual ~ColorViewer()
  {
    if (m_color != NULL)
      delete m_color;
  }

  void setColor(const Color& color)
  {
    if (m_color != NULL)
      delete m_color;

    m_color = new Color(color);
    invalidate(true);
  }

  void setNoColor()
  {
    if (m_color != NULL) {
      delete m_color;
      m_color = NULL;
    }
    invalidate(true);
  }

protected:

  virtual void onPaint(Graphics& g)
  {
    Rect rc = getClientBounds(); 

    if (m_color != NULL) {
      Pen pen(m_color->toBlackAndWhite().negative());
      Brush brush(*m_color);

      g.drawRect(pen, rc);
      g.fillRect(brush, rc.shrink(1));
    }
    else {
      Pen pen(Color::Black);
      Brush brush(Color::White);

      g.fillRect(brush, Rect(rc).shrink(1));
      g.drawRect(pen, rc);
      g.drawLine(pen, rc.x, rc.y, rc.x+rc.w-1, rc.y+rc.h-1);
      g.drawLine(pen, rc.x, rc.y+rc.h-1, rc.x+rc.w-1, rc.y);
    }
  }
  
};

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  Label m_label;
  Panel m_bottomPanel;
  AutoCompletionComboBox m_comboBox;
  ColorViewer m_colorViewer;
  
public:

  MainFrame()
    : Frame("AutoCompletion")
    , m_label("Input the name of a color (it'll be autocompleted)", this)
    , m_bottomPanel(this)
    , m_comboBox(&m_bottomPanel)
    , m_colorViewer(&m_bottomPanel)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_bottomPanel.setLayout(new BoxLayout(Orientation::Horizontal, false));
    m_comboBox.setConstraint(new BoxConstraint(true));

    m_comboBox.requestFocus();

    m_comboBox.SelColor.connect(&ColorViewer::setColor, &m_colorViewer);
    m_comboBox.NoColor.connect(&ColorViewer::setNoColor, &m_colorViewer);

    setSize(getPreferredSize());
    center();
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main(std::vector<String> args) {
    m_mainFrame.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
