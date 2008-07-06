#include <cassert>

#include "Vaca/String.hpp"

using namespace Vaca;

void test_equal()
{
  assert(String("Hello") == _T("Hello"));

  assert(String("abc") == String("abc"));
  assert(String("abc") != String("abcd"));
  assert(String("abcd") != String("abc"));
}

void test_from()
{
  String a = String::fromInt(-32);
  String b = "-32";
  assert(a == b);

  assert(String::fromInt(32039) == _T("32039"));
  assert(String::fromInt(-32994) == _T("-32994"));
  assert(String::fromDouble(0.324, 10) == _T("0.324"));
  assert(String::fromDouble(0.324, 1) == _T("0.3"));
  assert(String::fromInt(0xff00, 16) == _T("ff00"));
}

void test_parse()
{
  String a = String::fromInt(-32);
  String b = String::fromInt(32);
  String c = String::fromInt(0x10, 16);
  assert(a.parseInt() == -32);
  assert(b.parseInt() == 32);
  assert(c.parseInt(16) == 0x10);
}

void test_filename()
{
  assert(String("C:\\foo\\main.cpp").getFilePath() == _T("C:\\foo"));
  assert(String("C:\\foo\\main.cpp").getFileName() == _T("main.cpp"));
  assert(String("C:\\foo\\main.cpp").getFileExtension() == _T("cpp"));
  assert(String("C:\\foo\\main.cpp").getFileTitle() == _T("main"));

  String path("C:\\foo");
  path = path.addPathComponent("src");
  assert(path == _T("C:\\foo\\src"));
  path = path.addPathComponent("headers\\");
  assert(path == _T("C:\\foo\\src\\headers\\"));
  path = path.addPathComponent("main.h");
  assert(path == _T("C:\\foo\\src\\headers\\main.h"));
}

int main()
{
  test_equal();
  test_from();
  test_parse();
  test_filename();
  return 0;
}
