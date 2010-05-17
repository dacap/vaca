#include <gtest/gtest.h>
#include <climits>

#include "Vaca/String.h"

using namespace Vaca;

TEST(String, Basic)
{
  String a, b;

  EXPECT_EQ(L"", a);
  EXPECT_EQ(0, a.size());

  EXPECT_TRUE(a == b);
  EXPECT_TRUE(a == String());

  EXPECT_TRUE(String() == L"");
}

TEST(String, Equal)
{
  EXPECT_TRUE(String(L"Hello") == L"Hello");
  EXPECT_TRUE(String(L"abc") == String(L"abc"));
  EXPECT_TRUE(String(L"abc") != String(L"abcd"));
  EXPECT_TRUE(String(L"abcd") != String(L"abc"));
}

TEST(String, Conversion)
{
  EXPECT_EQ(L"32039",  convert_to<String>(32039));
  EXPECT_EQ(L"-32994", convert_to<String>(-32994));
  EXPECT_EQ(L"0.324",  convert_to<String>(0.324));
  EXPECT_EQ(L"65280",  convert_to<String>(0xff00));

  String a = convert_to<String>(-32);
  String b = convert_to<String>(32);
  String c = convert_to<String>(0x10);
  EXPECT_EQ(-32,  convert_to<int>(a));
  EXPECT_EQ(32,   convert_to<int>(b));
  EXPECT_EQ(0x10, convert_to<int>(c));

  String a_int	 = convert_to<String, int>(INT_MIN);
  String b_int	 = convert_to<String, int>(INT_MAX);
  String a_uint	 = convert_to<String, unsigned int>(0U);
  String b_uint	 = convert_to<String, unsigned int>(UINT_MAX);
  String a_long	 = convert_to<String, long>(LONG_MIN);
  String b_long	 = convert_to<String, long>(LONG_MAX);
  String a_ulong = convert_to<String, unsigned long>(0UL);
  String b_ulong = convert_to<String, unsigned long>(ULONG_MAX);

  EXPECT_EQ(INT_MIN,   convert_to<int>(a_int));
  EXPECT_EQ(INT_MAX,   convert_to<int>(b_int));
  EXPECT_EQ(LONG_MIN,  convert_to<long>(a_long));
  EXPECT_EQ(LONG_MAX,  convert_to<long>(b_long));
  EXPECT_EQ(0U,        convert_to<unsigned int>(a_uint));
  EXPECT_EQ(UINT_MAX,  convert_to<unsigned int>(b_uint));
  EXPECT_EQ(0UL,       convert_to<unsigned long>(a_ulong));
  EXPECT_EQ(ULONG_MAX, convert_to<unsigned long>(b_ulong));
}

TEST(String, FilePath)
{
  EXPECT_EQ(L"C:\\foo", file_path(L"C:\\foo\\main.cpp"));
  EXPECT_EQ(L"C:/foo",	file_path(L"C:/foo/pack.tar.gz"));
  EXPECT_EQ(L".",	file_path(L"./main.cpp"));
  EXPECT_EQ(L".",	file_path(L".\\main.cpp"));
  EXPECT_EQ(L"",	file_path(L"\\main.cpp"));
  EXPECT_EQ(L"",	file_path(L"main.cpp"));
  EXPECT_EQ(L"",	file_path(L"main."));
  EXPECT_EQ(L"",	file_path(L"main"));
  EXPECT_EQ(L"C:/foo",	file_path(L"C:/foo/"));
  EXPECT_EQ(L"C:",	file_path(L"C:\\"));
  EXPECT_EQ(L"C:",	file_path(L"C:\\.cpp"));
  EXPECT_EQ(L"",	file_path(L".cpp"));
  EXPECT_EQ(L"",	file_path(L""));
}

TEST(String, FileName)
{
  EXPECT_EQ(L"main.cpp",	file_name(L"C:\\foo\\main.cpp"));
  EXPECT_EQ(L"pack.tar.gz",	file_name(L"C:/foo/pack.tar.gz"));
  EXPECT_EQ(L"main.cpp",	file_name(L"./main.cpp"));
  EXPECT_EQ(L"main.cpp",	file_name(L".\\main.cpp"));
  EXPECT_EQ(L"main.cpp",	file_name(L"\\main.cpp"));
  EXPECT_EQ(L"main.cpp",	file_name(L"main.cpp"));
  EXPECT_EQ(L"main.",		file_name(L"main."));
  EXPECT_EQ(L"main",		file_name(L"main"));
  EXPECT_EQ(L"",		file_name(L"C:/foo/"));
  EXPECT_EQ(L"",		file_name(L"C:\\"));
  EXPECT_EQ(L".cpp",		file_name(L"C:\\.cpp"));
  EXPECT_EQ(L".cpp",		file_name(L".cpp"));
  EXPECT_EQ(L"",		file_name(L""));
}

TEST(String, FileExtension)
{
  EXPECT_EQ(L"cpp",	file_extension(L"C:\\foo\\main.cpp"));
  EXPECT_EQ(L"gz",	file_extension(L"C:/foo/pack.tar.gz"));
  EXPECT_EQ(L"cpp",	file_extension(L"./main.cpp"));
  EXPECT_EQ(L"cpp",	file_extension(L".\\main.cpp"));
  EXPECT_EQ(L"cpp",	file_extension(L"\\main.cpp"));
  EXPECT_EQ(L"cpp",	file_extension(L"main.cpp"));
  EXPECT_EQ(L"",	file_extension(L"main."));
  EXPECT_EQ(L"",	file_extension(L"main"));
  EXPECT_EQ(L"",	file_extension(L"C:/foo/"));
  EXPECT_EQ(L"",	file_extension(L"C:\\"));
  EXPECT_EQ(L"cpp",	file_extension(L"C:\\.cpp"));
  EXPECT_EQ(L"cpp",	file_extension(L".cpp"));
  EXPECT_EQ(L"",	file_extension(L""));
}

TEST(String, FileTitle)
{
  EXPECT_EQ(L"main",		file_title(L"C:\\foo\\main.cpp"));
  EXPECT_EQ(L"pack.tar",	file_title(L"C:/foo/pack.tar.gz"));
  EXPECT_EQ(L"main",		file_title(L"./main.cpp"));
  EXPECT_EQ(L"main",		file_title(L".\\main.cpp"));
  EXPECT_EQ(L"main",		file_title(L"\\main.cpp"));
  EXPECT_EQ(L"main",		file_title(L"main.cpp"));
  EXPECT_EQ(L"main",		file_title(L"main."));
  EXPECT_EQ(L"main",		file_title(L"main"));
  EXPECT_EQ(L"",		file_title(L"C:/foo/"));
  EXPECT_EQ(L"",		file_title(L"C:\\"));
  EXPECT_EQ(L"",		file_title(L"C:\\.cpp"));
  EXPECT_EQ(L"",		file_title(L".cpp"));
  EXPECT_EQ(L"",		file_title(L""));
}

TEST(String, Slash)
{
  String path(L"C:\\foo");
  EXPECT_EQ(L"C:\\foo\\src", path / L"src");

  path /= L"include";
  EXPECT_EQ(L"C:\\foo\\include", path);

  path /= L"main.h";
  EXPECT_EQ(L"C:\\foo\\include\\main.h", path);
}

TEST(String, TrimAlgorithm)
{
  EXPECT_EQ(L"",		trim_string(String()));
  EXPECT_EQ(L"No trim",		trim_string(L"No trim"));
  EXPECT_EQ(L"Front",		trim_string(L"Front    "));
  EXPECT_EQ(L"End",		trim_string(L"   End"));
  EXPECT_EQ(L"",		trim_string(L"       "));
  EXPECT_EQ(L"",		trim_string(L" \n \r  \t \r \n \r\n \t   "));
  EXPECT_EQ(L"Word",		trim_string(L" \n \r Word \t   "));
  EXPECT_EQ(L"No \n \r \ttrim", trim_string(L"\tNo \n \r \ttrim \n "));
}

TEST(String, Format)
{
  EXPECT_EQ(L"",	format_string(L""));
  EXPECT_EQ(L"Hello",	format_string(L"Hello"));
  EXPECT_EQ(L"2",	format_string(L"%d", 2));
  EXPECT_EQ(L"20.32",	format_string(L"%.02f", 20.3248));
  EXPECT_EQ(L"  202",	format_string(L"%5d", 202));
  EXPECT_EQ(L"00202",	format_string(L"%05d", 202));
  EXPECT_EQ(L"ff00",	format_string(L"%04x", 0xff00));
  EXPECT_EQ(L"0.32",	format_string(L"%0.2g", 0.324));
}

TEST(String, FormatOverflow)
{
  for (int n=1024; n<1026; ++n) {
    wchar_t* buf(new wchar_t[n]);

    for (int d=10; d<1000; d*=10) {
      for (int c=0; c<n-3; ++c)
	buf[c] = L'x';
      buf[n-3] = L'%';
      buf[n-2] = L'd';
      buf[n-1] = 0;

      EXPECT_TRUE(format_string(buf, d) == String(n-3, L'x')+convert_to<String>(d));
    }

    delete buf;
  }
}
