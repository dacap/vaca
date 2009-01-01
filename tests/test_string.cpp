#include <cassert>
#include <climits>

#include "Vaca/String.h"

using namespace Vaca;

void test_basic()
{
  String a, b;
  assert(a == b);
  assert(String() == L"");
  assert(a == String());
}

void test_equal()
{
  assert(String(L"Hello") == L"Hello");
  assert(String(L"abc") == String(L"abc"));
  assert(String(L"abc") != String(L"abcd"));
  assert(String(L"abcd") != String(L"abc"));
}

void test_convert_to()
{
  assert(convert_to<String>(32039) == L"32039");
  assert(convert_to<String>(-32994) == L"-32994");
  assert(convert_to<String>(0.324) == L"0.324");
  assert(convert_to<String>(0xff00) == L"65280");

  String a = convert_to<String>(-32);
  String b = convert_to<String>(32);
  String c = convert_to<String>(0x10);
  assert(convert_to<int>(a) == -32);
  assert(convert_to<int>(b) == 32);
  assert(convert_to<int>(c) == 0x10);

  String a_int = convert_to<String, int>(INT_MIN);
  String b_int = convert_to<String, int>(INT_MAX);
  String a_uint = convert_to<String, unsigned int>(0U);
  String b_uint = convert_to<String, unsigned int>(UINT_MAX);
  String a_long = convert_to<String, long>(LONG_MIN);
  String b_long = convert_to<String, long>(LONG_MAX);
  String a_ulong = convert_to<String, unsigned long>(0UL);
  String b_ulong = convert_to<String, unsigned long>(ULONG_MAX);

  assert(convert_to<int>(a_int) == INT_MIN);
  assert(convert_to<int>(b_int) == INT_MAX);
  assert(convert_to<long>(a_long) == LONG_MIN);
  assert(convert_to<long>(b_long) == LONG_MAX);
  assert(convert_to<unsigned int>(a_uint) == 0U);
  assert(convert_to<unsigned int>(b_uint) == UINT_MAX);
  assert(convert_to<unsigned long>(a_ulong) == 0UL);
  assert(convert_to<unsigned long>(b_ulong) == ULONG_MAX);

}

void test_filename()
{
  assert(file_path(L"C:\\foo\\main.cpp") == L"C:\\foo");
  assert(file_path(L"C:/foo/pack.tar.gz") == L"C:/foo");
  assert(file_path(L"./main.cpp") == L".");
  assert(file_path(L".\\main.cpp") == L".");
  assert(file_path(L"\\main.cpp") == L"");
  assert(file_path(L"main.cpp") == L"");
  assert(file_path(L"main.") == L"");
  assert(file_path(L"main") == L"");
  assert(file_path(L"C:/foo/") == L"C:/foo");
  assert(file_path(L"C:\\") == L"C:");
  assert(file_path(L"C:\\.cpp") == L"C:");
  assert(file_path(L".cpp") == L"");
  assert(file_path(L"") == L"");

  assert(file_name(L"C:\\foo\\main.cpp") == L"main.cpp");
  assert(file_name(L"C:/foo/pack.tar.gz") == L"pack.tar.gz");
  assert(file_name(L"./main.cpp") == L"main.cpp");
  assert(file_name(L".\\main.cpp") == L"main.cpp");
  assert(file_name(L"\\main.cpp") == L"main.cpp");
  assert(file_name(L"main.cpp") == L"main.cpp");
  assert(file_name(L"main.") == L"main.");
  assert(file_name(L"main") == L"main");
  assert(file_name(L"C:/foo/") == L"");
  assert(file_name(L"C:\\") == L"");
  assert(file_name(L"C:\\.cpp") == L".cpp");
  assert(file_name(L".cpp") == L".cpp");
  assert(file_name(L"") == L"");

  assert(file_extension(L"C:\\foo\\main.cpp") == L"cpp");
  assert(file_extension(L"C:/foo/pack.tar.gz") == L"gz");
  assert(file_extension(L"./main.cpp") == L"cpp");
  assert(file_extension(L".\\main.cpp") == L"cpp");
  assert(file_extension(L"\\main.cpp") == L"cpp");
  assert(file_extension(L"main.cpp") == L"cpp");
  assert(file_extension(L"main.") == L"");
  assert(file_extension(L"main") == L"");
  assert(file_extension(L"C:/foo/") == L"");
  assert(file_extension(L"C:\\") == L"");
  assert(file_extension(L"C:\\.cpp") == L"cpp");
  assert(file_extension(L".cpp") == L"cpp");
  assert(file_extension(L"") == L"");

  assert(file_title(L"C:\\foo\\main.cpp") == L"main");
  assert(file_title(L"C:/foo/pack.tar.gz") == L"pack.tar");
  assert(file_title(L"./main.cpp") == L"main");
  assert(file_title(L".\\main.cpp") == L"main");
  assert(file_title(L"\\main.cpp") == L"main");
  assert(file_title(L"main.cpp") == L"main");
  assert(file_title(L"main.") == L"main");
  assert(file_title(L"main") == L"main");
  assert(file_title(L"C:/foo/") == L"");
  assert(file_title(L"C:\\") == L"");
  assert(file_title(L"C:\\.cpp") == L"");
  assert(file_title(L".cpp") == L"");
  assert(file_title(L"") == L"");

  String path(L"C:\\foo");
  assert(path / L"src" == L"C:\\foo\\src");

  path /= L"include";
  assert(path == L"C:\\foo\\include");

  path /= L"main.h";
  assert(path == L"C:\\foo\\include\\main.h");
}

void test_trim()
{
  assert(trim_string(String()) == L"");
  assert(trim_string(L"No trim") == L"No trim");
  assert(trim_string(L"Front    ") == L"Front");
  assert(trim_string(L"   End") == L"End");
  assert(trim_string(L"       ") == L"");
  assert(trim_string(L" \n \r  \t \r \n \r\n \t   ") == L"");
  assert(trim_string(L" \n \r Word \t   ") == L"Word");
  assert(trim_string(L"\tNo \n \r \ttrim \n ") == L"No \n \r \ttrim");
}

void test_format()
{
  assert(format_string(L"") == L"");
  assert(format_string(L"Hello") == L"Hello");
  assert(format_string(L"%d", 2) == L"2");
  assert(format_string(L"%.02f", 20.3248) == L"20.32");
  assert(format_string(L"%5d", 202) == L"  202");
  assert(format_string(L"%05d", 202) == L"00202");
  assert(format_string(L"%04x", 0xff00) == L"ff00");
  assert(format_string(L"%0.2g", 0.324) == L"0.32");
}

void test_format_overflow()
{
  for (int n=1024; n<1026; ++n) {
    wchar_t* buf(new wchar_t[n]);

    for (int d=10; d<1000; d*=10) {
      for (int c=0; c<n-3; ++c)
	buf[c] = L'x';
      buf[n-3] = L'%';
      buf[n-2] = L'd';
      buf[n-1] = 0;

      assert(format_string(buf, d) == String(n-3, L'x')+convert_to<String>(d));
    }

    delete buf;
  }
}

int main()
{
  test_basic();
  test_equal();
  test_convert_to();
  test_filename();
  test_trim();
  test_format();
  test_format_overflow();
  return 0;
}
