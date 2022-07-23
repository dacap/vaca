// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include "../resource.h"

using namespace vaca;

//////////////////////////////////////////////////////////////////////

struct ColorName
{
  const Char* name;
  int rgb;
} ColorNames[] = {		// The colors where taken from Emacs
  { L"alice blue", 0xf0f8ff },
  { L"antique white", 0xfaebd7 },
  { L"Antique White1", 0xffefdb },
  { L"Antique White2", 0xeedfcc },
  { L"Antique White3", 0xcdc0b0 },
  { L"Antique White4", 0x8b8378 },
  { L"aquamarine", 0x7fffd4 },
  { L"aquamarine1", 0x7fffd4 },
  { L"aquamarine2", 0x76eec6 },
  { L"aquamarine3", 0x66cdaa },
  { L"aquamarine4", 0x458b74 },
  { L"azure", 0xf0ffff },
  { L"azure1", 0xf0ffff },
  { L"azure2", 0xe0eeee },
  { L"azure3", 0xc1cdcd },
  { L"azure4", 0x838b8b },
  { L"beige", 0xf5f5dc },
  { L"bisque", 0xffe4c4 },
  { L"bisque1", 0xffe4c4 },
  { L"bisque2", 0xeed5b7 },
  { L"bisque3", 0xcdb79e },
  { L"bisque4", 0x8b7d6b },
  { L"black", 0x000000 },
  { L"blanched almond", 0xffebcd },
  { L"blue", 0x0000ff },
  { L"blue1", 0x0000ff },
  { L"blue2", 0x0000ee },
  { L"blue3", 0x0000cd },
  { L"blue4", 0x00008b },
  { L"blue violet", 0x8a2be2 },
  { L"brown", 0xa52a2a },
  { L"brown1", 0xff4040 },
  { L"brown2", 0xee3b3b },
  { L"brown3", 0xcd3333 },
  { L"brown4", 0x8b2323 },
  { L"burlywood", 0xdeb887 },
  { L"burlywood1", 0xffd39b },
  { L"burlywood2", 0xeec591 },
  { L"burlywood3", 0xcdaa7d },
  { L"burlywood4", 0x8b7355 },
  { L"cadet blue", 0x5f9ea0 },
  { L"Cadet Blue1", 0x98f5ff },
  { L"Cadet Blue2", 0x8ee5ee },
  { L"Cadet Blue3", 0x7ac5cd },
  { L"Cadet Blue4", 0x53868b },
  { L"chartreuse", 0x7fff00 },
  { L"chartreuse1", 0x7fff00 },
  { L"chartreuse2", 0x76ee00 },
  { L"chartreuse3", 0x66cd00 },
  { L"chartreuse4", 0x458b00 },
  { L"chocolate", 0xd2691e },
  { L"chocolate1", 0xff7f24 },
  { L"chocolate2", 0xee7621 },
  { L"chocolate3", 0xcd661d },
  { L"chocolate4", 0x8b4513 },
  { L"coral", 0xff7f50 },
  { L"coral1", 0xff7256 },
  { L"coral2", 0xee6a50 },
  { L"coral3", 0xcd5b45 },
  { L"coral4", 0x8b3e2f },
  { L"cornflower blue", 0x6495ed },
  { L"cornsilk", 0xfff8dc },
  { L"cornsilk1", 0xfff8dc },
  { L"cornsilk2", 0xeee8cd },
  { L"cornsilk3", 0xcdc8b1 },
  { L"cornsilk4", 0x8b8878 },
  { L"cyan", 0x00ffff },
  { L"cyan1", 0x00ffff },
  { L"cyan2", 0x00eeee },
  { L"cyan3", 0x00cdcd },
  { L"cyan4", 0x008b8b },
  { L"dark blue", 0x00008b },
  { L"dark cyan", 0x008b8b },
  { L"dark goldenrod", 0xb8860b },
  { L"Dark Goldenrod1", 0xffb90f },
  { L"Dark Goldenrod2", 0xeead0e },
  { L"Dark Goldenrod3", 0xcd950c },
  { L"Dark Goldenrod4", 0x8b6508 },
  { L"dark gray", 0xa9a9a9 },
  { L"dark green", 0x006400 },
  { L"dark khaki", 0xbdb76b },
  { L"dark magenta", 0x8b008b },
  { L"dark olive green", 0x556b2f },
  { L"Dark Olive Green1", 0xcaff70 },
  { L"Dark Olive Green2", 0xbcee68 },
  { L"Dark Olive Green3", 0xa2cd5a },
  { L"Dark Olive Green4", 0x6e8b3d },
  { L"dark orange", 0xff8c00 },
  { L"Dark Orange1", 0xff7f00 },
  { L"Dark Orange2", 0xee7600 },
  { L"Dark Orange3", 0xcd6600 },
  { L"Dark Orange4", 0x8b4500 },
  { L"dark orchid", 0x9932cc },
  { L"Dark Orchid1", 0xbf3eff },
  { L"Dark Orchid2", 0xb23aee },
  { L"Dark Orchid3", 0x9a32cd },
  { L"Dark Orchid4", 0x68228b },
  { L"dark red", 0x8b0000 },
  { L"dark salmon", 0xe9967a },
  { L"dark sea green", 0x8fbc8f },
  { L"Dark Sea Green1", 0xc1ffc1 },
  { L"Dark Sea Green2", 0xb4eeb4 },
  { L"Dark Sea Green3", 0x9bcd9b },
  { L"Dark Sea Green4", 0x698b69 },
  { L"dark slate blue", 0x483d8b },
  { L"dark slate gray", 0x2f4f4f },
  { L"Dark Slate Gray1", 0x97ffff },
  { L"Dark Slate Gray2", 0x8deeee },
  { L"Dark Slate Gray3", 0x79cdcd },
  { L"Dark Slate Gray4", 0x528b8b },
  { L"dark turquoise", 0x00ced1 },
  { L"dark violet", 0x9400d3 },
  { L"deep pink", 0xff1493 },
  { L"Deep Pink1", 0xff1493 },
  { L"Deep Pink2", 0xee1289 },
  { L"Deep Pink3", 0xcd1076 },
  { L"Deep Pink4", 0x8b0a50 },
  { L"deep sky blue", 0x00bfff },
  { L"Deep Sky Blue1", 0x00bfff },
  { L"Deep Sky Blue2", 0x00b2ee },
  { L"Deep Sky Blue3", 0x009acd },
  { L"Deep Sky Blue4", 0x00688b },
  { L"dim gray", 0x696969 },
  { L"dodger blue", 0x1e90ff },
  { L"Dodger Blue1", 0x1e90ff },
  { L"Dodger Blue2", 0x1c86ee },
  { L"Dodger Blue3", 0x1874cd },
  { L"Dodger Blue4", 0x104e8b },
  { L"firebrick", 0xb22222 },
  { L"firebrick1", 0xff3030 },
  { L"firebrick2", 0xee2c2c },
  { L"firebrick3", 0xcd2626 },
  { L"firebrick4", 0x8b1a1a },
  { L"floral white", 0xfffaf0 },
  { L"forest green", 0x228b22 },
  { L"gainsboro", 0xdcdcdc },
  { L"ghost white", 0xf8f8ff },
  { L"gold", 0xffd700 },
  { L"gold1", 0xffd700 },
  { L"gold2", 0xeec900 },
  { L"gold3", 0xcdad00 },
  { L"gold4", 0x8b7500 },
  { L"goldenrod", 0xdaa520 },
  { L"goldenrod1", 0xffc125 },
  { L"goldenrod2", 0xeeb422 },
  { L"goldenrod3", 0xcd9b1d },
  { L"goldenrod4", 0x8b6914 },
  { L"gray", 0xbebebe },
  { L"gray0", 0x000000 },
  { L"gray1", 0x030303 },
  { L"gray2", 0x050505 },
  { L"gray3", 0x080808 },
  { L"gray4", 0x0a0a0a },
  { L"gray5", 0x0d0d0d },
  { L"gray6", 0x0f0f0f },
  { L"gray7", 0x121212 },
  { L"gray8", 0x141414 },
  { L"gray9", 0x171717 },
  { L"gray10", 0x1a1a1a },
  { L"gray11", 0x1c1c1c },
  { L"gray12", 0x1f1f1f },
  { L"gray13", 0x212121 },
  { L"gray14", 0x242424 },
  { L"gray15", 0x262626 },
  { L"gray16", 0x292929 },
  { L"gray17", 0x2b2b2b },
  { L"gray18", 0x2e2e2e },
  { L"gray19", 0x303030 },
  { L"gray20", 0x333333 },
  { L"gray21", 0x363636 },
  { L"gray22", 0x383838 },
  { L"gray23", 0x3b3b3b },
  { L"gray24", 0x3d3d3d },
  { L"gray25", 0x404040 },
  { L"gray26", 0x424242 },
  { L"gray27", 0x454545 },
  { L"gray28", 0x474747 },
  { L"gray29", 0x4a4a4a },
  { L"gray30", 0x4d4d4d },
  { L"gray31", 0x4f4f4f },
  { L"gray32", 0x525252 },
  { L"gray33", 0x545454 },
  { L"gray34", 0x575757 },
  { L"gray35", 0x595959 },
  { L"gray36", 0x5c5c5c },
  { L"gray37", 0x5e5e5e },
  { L"gray38", 0x616161 },
  { L"gray39", 0x636363 },
  { L"gray40", 0x666666 },
  { L"gray41", 0x696969 },
  { L"gray42", 0x6b6b6b },
  { L"gray43", 0x6e6e6e },
  { L"gray44", 0x707070 },
  { L"gray45", 0x737373 },
  { L"gray46", 0x757575 },
  { L"gray47", 0x787878 },
  { L"gray48", 0x7a7a7a },
  { L"gray49", 0x7d7d7d },
  { L"gray50", 0x7f7f7f },
  { L"gray51", 0x828282 },
  { L"gray52", 0x858585 },
  { L"gray53", 0x878787 },
  { L"gray54", 0x8a8a8a },
  { L"gray55", 0x8c8c8c },
  { L"gray56", 0x8f8f8f },
  { L"gray57", 0x919191 },
  { L"gray58", 0x949494 },
  { L"gray59", 0x969696 },
  { L"gray60", 0x999999 },
  { L"gray61", 0x9c9c9c },
  { L"gray62", 0x9e9e9e },
  { L"gray63", 0xa1a1a1 },
  { L"gray64", 0xa3a3a3 },
  { L"gray65", 0xa6a6a6 },
  { L"gray66", 0xa8a8a8 },
  { L"gray67", 0xababab },
  { L"gray68", 0xadadad },
  { L"gray69", 0xb0b0b0 },
  { L"gray70", 0xb3b3b3 },
  { L"gray71", 0xb5b5b5 },
  { L"gray72", 0xb8b8b8 },
  { L"gray73", 0xbababa },
  { L"gray74", 0xbdbdbd },
  { L"gray75", 0xbfbfbf },
  { L"gray76", 0xc2c2c2 },
  { L"gray77", 0xc4c4c4 },
  { L"gray78", 0xc7c7c7 },
  { L"gray79", 0xc9c9c9 },
  { L"gray80", 0xcccccc },
  { L"gray81", 0xcfcfcf },
  { L"gray82", 0xd1d1d1 },
  { L"gray83", 0xd4d4d4 },
  { L"gray84", 0xd6d6d6 },
  { L"gray85", 0xd9d9d9 },
  { L"gray86", 0xdbdbdb },
  { L"gray87", 0xdedede },
  { L"gray88", 0xe0e0e0 },
  { L"gray89", 0xe3e3e3 },
  { L"gray90", 0xe5e5e5 },
  { L"gray91", 0xe8e8e8 },
  { L"gray92", 0xebebeb },
  { L"gray93", 0xededed },
  { L"gray94", 0xf0f0f0 },
  { L"gray95", 0xf2f2f2 },
  { L"gray96", 0xf5f5f5 },
  { L"gray97", 0xf7f7f7 },
  { L"gray98", 0xfafafa },
  { L"gray99", 0xfcfcfc },
  { L"gray100", 0xffffff },
  { L"green", 0x00ff00 },
  { L"green1", 0x00ff00 },
  { L"green2", 0x00ee00 },
  { L"green3", 0x00cd00 },
  { L"green4", 0x008b00 },
  { L"green yellow", 0xadff2f },
  { L"honeydew", 0xf0fff0 },
  { L"honeydew1", 0xf0fff0 },
  { L"honeydew2", 0xe0eee0 },
  { L"honeydew3", 0xc1cdc1 },
  { L"honeydew4", 0x838b83 },
  { L"hot pink", 0xff69b4 },
  { L"Hot Pink1", 0xff6eb4 },
  { L"Hot Pink2", 0xee6aa7 },
  { L"Hot Pink3", 0xcd6090 },
  { L"Hot Pink4", 0x8b3a62 },
  { L"indian red", 0xcd5c5c },
  { L"Indian Red1", 0xff6a6a },
  { L"Indian Red2", 0xee6363 },
  { L"Indian Red3", 0xcd5555 },
  { L"Indian Red4", 0x8b3a3a },
  { L"ivory", 0xfffff0 },
  { L"ivory1", 0xfffff0 },
  { L"ivory2", 0xeeeee0 },
  { L"ivory3", 0xcdcdc1 },
  { L"ivory4", 0x8b8b83 },
  { L"khaki", 0xf0e68c },
  { L"khaki1", 0xfff68f },
  { L"khaki2", 0xeee685 },
  { L"khaki3", 0xcdc673 },
  { L"khaki4", 0x8b864e },
  { L"lavender", 0xe6e6fa },
  { L"lavender blush", 0xfff0f5 },
  { L"Lavender Blush1", 0xfff0f5 },
  { L"Lavender Blush2", 0xeee0e5 },
  { L"Lavender Blush3", 0xcdc1c5 },
  { L"Lavender Blush4", 0x8b8386 },
  { L"lawn green", 0x7cfc00 },
  { L"lemon chiffon", 0xfffacd },
  { L"Lemon Chiffon1", 0xfffacd },
  { L"Lemon Chiffon2", 0xeee9bf },
  { L"Lemon Chiffon3", 0xcdc9a5 },
  { L"Lemon Chiffon4", 0x8b8970 },
  { L"light blue", 0xadd8e6 },
  { L"Light Blue1", 0xbfefff },
  { L"Light Blue2", 0xb2dfee },
  { L"Light Blue3", 0x9ac0cd },
  { L"Light Blue4", 0x68838b },
  { L"light coral", 0xf08080 },
  { L"light cyan", 0xe0ffff },
  { L"Light Cyan1", 0xe0ffff },
  { L"Light Cyan2", 0xd1eeee },
  { L"Light Cyan3", 0xb4cdcd },
  { L"Light Cyan4", 0x7a8b8b },
  { L"light goldenrod", 0xeedd82 },
  { L"Light Goldenrod1", 0xffec8b },
  { L"Light Goldenrod2", 0xeedc82 },
  { L"Light Goldenrod3", 0xcdbe70 },
  { L"Light Goldenrod4", 0x8b814c },
  { L"light goldenrod yellow", 0xfafad2 },
  { L"light gray", 0xd3d3d3 },
  { L"light green", 0x90ee90 },
  { L"light pink", 0xffb6c1 },
  { L"Light Pink1", 0xffaeb9 },
  { L"Light Pink2", 0xeea2ad },
  { L"Light Pink3", 0xcd8c95 },
  { L"Light Pink4", 0x8b5f65 },
  { L"light salmon", 0xffa07a },
  { L"Light Salmon1", 0xffa07a },
  { L"Light Salmon2", 0xee9572 },
  { L"Light Salmon3", 0xcd8162 },
  { L"Light Salmon4", 0x8b5742 },
  { L"light sea green", 0x20b2aa },
  { L"light sky blue", 0x87cefa },
  { L"Light Sky Blue1", 0xb0e2ff },
  { L"Light Sky Blue2", 0xa4d3ee },
  { L"Light Sky Blue3", 0x8db6cd },
  { L"Light Sky Blue4", 0x607b8b },
  { L"light slate blue", 0x8470ff },
  { L"light slate gray", 0x778899 },
  { L"light steel blue", 0xb0c4de },
  { L"Light Steel Blue1", 0xcae1ff },
  { L"Light Steel Blue2", 0xbcd2ee },
  { L"Light Steel Blue3", 0xa2b5cd },
  { L"Light Steel Blue4", 0x6e7b8b },
  { L"light yellow", 0xffffe0 },
  { L"Light Yellow1", 0xffffe0 },
  { L"Light Yellow2", 0xeeeed1 },
  { L"Light Yellow3", 0xcdcdb4 },
  { L"Light Yellow4", 0x8b8b7a },
  { L"lime green", 0x32cd32 },
  { L"linen", 0xfaf0e6 },
  { L"magenta", 0xff00ff },
  { L"magenta1", 0xff00ff },
  { L"magenta2", 0xee00ee },
  { L"magenta3", 0xcd00cd },
  { L"magenta4", 0x8b008b },
  { L"maroon", 0xb03060 },
  { L"maroon1", 0xff34b3 },
  { L"maroon2", 0xee30a7 },
  { L"maroon3", 0xcd2990 },
  { L"maroon4", 0x8b1c62 },
  { L"medium aquamarine", 0x66cdaa },
  { L"medium blue", 0x0000cd },
  { L"medium orchid", 0xba55d3 },
  { L"Medium Orchid1", 0xe066ff },
  { L"Medium Orchid2", 0xd15fee },
  { L"Medium Orchid3", 0xb452cd },
  { L"Medium Orchid4", 0x7a378b },
  { L"medium purple", 0x9370db },
  { L"Medium Purple1", 0xab82ff },
  { L"Medium Purple2", 0x9f79ee },
  { L"Medium Purple3", 0x8968cd },
  { L"Medium Purple4", 0x5d478b },
  { L"medium sea green", 0x3cb371 },
  { L"medium slate blue", 0x7b68ee },
  { L"medium spring green", 0x00fa9a },
  { L"medium turquoise", 0x48d1cc },
  { L"medium violet red", 0xc71585 },
  { L"midnight blue", 0x191970 },
  { L"mint cream", 0xf5fffa },
  { L"misty rose", 0xffe4e1 },
  { L"Misty Rose1", 0xffe4e1 },
  { L"Misty Rose2", 0xeed5d2 },
  { L"Misty Rose3", 0xcdb7b5 },
  { L"Misty Rose4", 0x8b7d7b },
  { L"moccasin", 0xffe4b5 },
  { L"navajo white", 0xffdead },
  { L"Navajo White1", 0xffdead },
  { L"Navajo White2", 0xeecfa1 },
  { L"Navajo White3", 0xcdb38b },
  { L"Navajo White4", 0x8b795e },
  { L"navy", 0x000080 },
  { L"old lace", 0xfdf5e6 },
  { L"olive drab", 0x6b8e23 },
  { L"Olive Drab1", 0xc0ff3e },
  { L"Olive Drab2", 0xb3ee3a },
  { L"Olive Drab3", 0x9acd32 },
  { L"Olive Drab4", 0x698b22 },
  { L"orange red", 0xff4500 },
  { L"Orange Red1", 0xff4500 },
  { L"Orange Red2", 0xee4000 },
  { L"Orange Red3", 0xcd3700 },
  { L"Orange Red4", 0x8b2500 },
  { L"orange", 0xffa500 },
  { L"orange1", 0xffa500 },
  { L"orange2", 0xee9a00 },
  { L"orange3", 0xcd8500 },
  { L"orange4", 0x8b5a00 },
  { L"orchid", 0xda70d6 },
  { L"orchid1", 0xff83fa },
  { L"orchid2", 0xee7ae9 },
  { L"orchid3", 0xcd69c9 },
  { L"orchid4", 0x8b4789 },
  { L"pale goldenrod", 0xeee8aa },
  { L"pale green", 0x98fb98 },
  { L"Pale Green1", 0x9aff9a },
  { L"Pale Green2", 0x90ee90 },
  { L"Pale Green3", 0x7ccd7c },
  { L"Pale Green4", 0x548b54 },
  { L"pale turquoise", 0xafeeee },
  { L"Pale Turquoise1", 0xbbffff },
  { L"Pale Turquoise2", 0xaeeeee },
  { L"Pale Turquoise3", 0x96cdcd },
  { L"Pale Turquoise4", 0x668b8b },
  { L"pale violet red", 0xdb7093 },
  { L"Pale Violet Red1", 0xff82ab },
  { L"Pale Violet Red2", 0xee799f },
  { L"Pale Violet Red3", 0xcd6889 },
  { L"Pale Violet Red4", 0x8b475d },
  { L"papaya whip", 0xffefd5 },
  { L"peach puff", 0xffdab9 },
  { L"Peach Puff1", 0xffdab9 },
  { L"Peach Puff2", 0xeecbad },
  { L"Peach Puff3", 0xcdaf95 },
  { L"Peach Puff4", 0x8b7765 },
  { L"peru", 0xcd853f },
  { L"pink", 0xffc0cb },
  { L"pink1", 0xffb5c5 },
  { L"pink2", 0xeea9b8 },
  { L"pink3", 0xcd919e },
  { L"pink4", 0x8b636c },
  { L"plum", 0xdda0dd },
  { L"plum1", 0xffbbff },
  { L"plum2", 0xeeaeee },
  { L"plum3", 0xcd96cd },
  { L"plum4", 0x8b668b },
  { L"powder blue", 0xb0e0e6 },
  { L"purple", 0xa020f0 },
  { L"purple1", 0x9b30ff },
  { L"purple2", 0x912cee },
  { L"purple3", 0x7d26cd },
  { L"purple4", 0x551a8b },
  { L"red", 0xff0000 },
  { L"red1", 0xff0000 },
  { L"red2", 0xee0000 },
  { L"red3", 0xcd0000 },
  { L"red4", 0x8b0000 },
  { L"rosy brown", 0xbc8f8f },
  { L"Rosy Brown1", 0xffc1c1 },
  { L"Rosy Brown2", 0xeeb4b4 },
  { L"Rosy Brown3", 0xcd9b9b },
  { L"Rosy Brown4", 0x8b6969 },
  { L"royal blue", 0x4169e1 },
  { L"Royal Blue1", 0x4876ff },
  { L"Royal Blue2", 0x436eee },
  { L"Royal Blue3", 0x3a5fcd },
  { L"Royal Blue4", 0x27408b },
  { L"saddle brown", 0x8b4513 },
  { L"salmon", 0xfa8072 },
  { L"salmon1", 0xff8c69 },
  { L"salmon2", 0xee8262 },
  { L"salmon3", 0xcd7054 },
  { L"salmon4", 0x8b4c39 },
  { L"sandy brown", 0xf4a460 },
  { L"sea green", 0x2e8b57 },
  { L"Sea Green1", 0x54ff9f },
  { L"Sea Green2", 0x4eee94 },
  { L"Sea Green3", 0x43cd80 },
  { L"Sea Green4", 0x2e8b57 },
  { L"seashell", 0xfff5ee },
  { L"seashell1", 0xfff5ee },
  { L"seashell2", 0xeee5de },
  { L"seashell3", 0xcdc5bf },
  { L"seashell4", 0x8b8682 },
  { L"sienna", 0xa0522d },
  { L"sienna1", 0xff8247 },
  { L"sienna2", 0xee7942 },
  { L"sienna3", 0xcd6839 },
  { L"sienna4", 0x8b4726 },
  { L"sky blue", 0x87ceeb },
  { L"Sky Blue1", 0x87ceff },
  { L"Sky Blue2", 0x7ec0ee },
  { L"Sky Blue3", 0x6ca6cd },
  { L"Sky Blue4", 0x4a708b },
  { L"slate blue", 0x6a5acd },
  { L"Slate Blue1", 0x836fff },
  { L"Slate Blue2", 0x7a67ee },
  { L"Slate Blue3", 0x6959cd },
  { L"Slate Blue4", 0x473c8b },
  { L"slate gray", 0x708090 },
  { L"Slate Gray1", 0xc6e2ff },
  { L"Slate Gray2", 0xb9d3ee },
  { L"Slate Gray3", 0x9fb6cd },
  { L"Slate Gray4", 0x6c7b8b },
  { L"snow", 0xfffafa },
  { L"snow1", 0xfffafa },
  { L"snow2", 0xeee9e9 },
  { L"snow3", 0xcdc9c9 },
  { L"snow4", 0x8b8989 },
  { L"spring green", 0x00ff7f },
  { L"Spring Green1", 0x00ff7f },
  { L"Spring Green2", 0x00ee76 },
  { L"Spring Green3", 0x00cd66 },
  { L"Spring Green4", 0x008b45 },
  { L"steel blue", 0x4682b4 },
  { L"Steel Blue1", 0x63b8ff },
  { L"Steel Blue2", 0x5cacee },
  { L"Steel Blue3", 0x4f94cd },
  { L"Steel Blue4", 0x36648b },
  { L"SystemActiveBorder", 0xd4d0c8 },
  { L"SystemActiveTitle", 0x0a246a },
  { L"SystemAppWorkSpace", 0x808080 },
  { L"SystemBackground", 0x3a6ea5 },
  { L"SystemButtonAlternateFace", 0xb5b5b5 },
  { L"SystemButtonDkShadow", 0x404040 },
  { L"SystemButtonFace", 0xd4d0c8 },
  { L"SystemButtonHilight", 0xffffff },
  { L"SystemButtonLight", 0xd4d0c8 },
  { L"SystemButtonShadow", 0x808080 },
  { L"SystemButtonText", 0x000000 },
  { L"SystemGradientActiveTitle", 0xa6caf0 },
  { L"SystemGradientInactiveTitle", 0xc0c0c0 },
  { L"SystemGrayText", 0x808080 },
  { L"SystemHilight", 0x0a246a },
  { L"SystemHilightText", 0xffffff },
  { L"SystemHotTrackingColor", 0x000080 },
  { L"SystemInactiveBorder", 0xd4d0c8 },
  { L"SystemInactiveTitle", 0x808080 },
  { L"SystemInactiveTitleText", 0xd4d0c8 },
  { L"SystemInfoText", 0x000000 },
  { L"SystemInfoWindow", 0xffffe1 },
  { L"SystemMenu", 0xd4d0c8 },
  { L"SystemMenuText", 0x000000 },
  { L"SystemScrollbar", 0xd4d0c8 },
  { L"SystemTitleText", 0xffffff },
  { L"SystemWindowFrame", 0x000000 },
  { L"tan", 0xd2b48c },
  { L"tan1", 0xffa54f },
  { L"tan2", 0xee9a49 },
  { L"tan3", 0xcd853f },
  { L"tan4", 0x8b5a2b },
  { L"thistle", 0xd8bfd8 },
  { L"thistle1", 0xffe1ff },
  { L"thistle2", 0xeed2ee },
  { L"thistle3", 0xcdb5cd },
  { L"thistle4", 0x8b7b8b },
  { L"tomato", 0xff6347 },
  { L"tomato1", 0xff6347 },
  { L"tomato2", 0xee5c42 },
  { L"tomato3", 0xcd4f39 },
  { L"tomato4", 0x8b3626 },
  { L"turquoise", 0x40e0d0 },
  { L"turquoise1", 0x00f5ff },
  { L"turquoise2", 0x00e5ee },
  { L"turquoise3", 0x00c5cd },
  { L"turquoise4", 0x00868b },
  { L"violet", 0xee82ee },
  { L"violet red", 0xd02090 },
  { L"Violet Red1", 0xff3e96 },
  { L"Violet Red2", 0xee3a8c },
  { L"Violet Red3", 0xcd3278 },
  { L"Violet Red4", 0x8b2252 },
  { L"wheat", 0xf5deb3 },
  { L"wheat1", 0xffe7ba },
  { L"wheat2", 0xeed8ae },
  { L"wheat3", 0xcdba96 },
  { L"wheat4", 0x8b7e66 },
  { L"white", 0xffffff },
  { L"white smoke", 0xf5f5f5 },
  { L"yellow", 0xffff00 },
  { L"yellow1", 0xffff00 },
  { L"yellow2", 0xeeee00 },
  { L"yellow3", 0xcdcd00 },
  { L"yellow4", 0x8b8b00 },
  { L"yellow green", 0x9acd32 },
};

int ColorNames_size = sizeof(ColorNames) / sizeof(struct ColorName);

//////////////////////////////////////////////////////////////////////

class AutoCompletionComboBox : public ComboBox
{
  Timer m_timer;

public:

  AutoCompletionComboBox(Widget* parent)
    : ComboBox(parent, ComboBox::Styles::Editable)
    , m_timer(250)
  {
    for (int c=0; c<ColorNames_size; ++c)
      addItem(ColorNames[c].name);

    m_timer.Tick.connect(&AutoCompletionComboBox::complete, this);
  }

  // signals
  Signal<void(const Color&)> SelColor;
  Signal<void()> NoColor;

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

    setSelectedItem(text);
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
    int i = getSelectedItem();
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

class ColorViewer : public Widget
{
  Color* m_color;

public:

  ColorViewer(Widget* parent)
    : Widget(parent)
  {
    m_color = NULL;
    setPreferredSize(Size(32, 8));
  }

  virtual ~ColorViewer()
  {
    delete m_color;
  }

  void setColor(const Color& color)
  {
    delete m_color;
    m_color = new Color(color);
    invalidate(true);
  }

  void setNoColor()
  {
    delete m_color;
    m_color = NULL;
    invalidate(true);
  }

protected:

  virtual void onPaint(PaintEvent& ev)
  {
    Graphics& g = ev.getGraphics();
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
  Widget m_bottomPanel;
  AutoCompletionComboBox m_comboBox;
  ColorViewer m_colorViewer;

public:

  MainFrame()
    : Frame(L"AutoCompletion")
    , m_label(L"Input the name of a color (it'll be autocompleted)", this)
    , m_bottomPanel(this)
    , m_comboBox(&m_bottomPanel)
    , m_colorViewer(&m_bottomPanel)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_bottomPanel.setLayout(new BoxLayout(Orientation::Horizontal, false));
    m_comboBox.setConstraint(new BoxConstraint(true));

    m_comboBox.requestFocus();

    m_comboBox.SelColor.connect(
      [this](const Color& color){
        m_colorViewer.setColor(color);
      });
    m_comboBox.NoColor.connect([this]{
      m_colorViewer.setNoColor();
    });

    setSize(getPreferredSize());
    center();
  }

};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
