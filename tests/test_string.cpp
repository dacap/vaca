#include <cassert>

#include "Vaca/String.hpp"

using namespace Vaca;

void test_equal()
{
  assert(String("Hello") == "Hello");

  assert(String("abc") == String("abc"));
  assert(String("abc") != String("abcd"));
  assert(String("abcd") != String("abc"));
}

void test_from()
{
  String a = String::fromInt(-32);
  String b = "-32";
  assert(a == b);
  assert(strtol(a.c_str(), NULL, 10) == -32);

  assert(String::fromInt(32039) == "32039");
  assert(String::fromInt(-32994) == "-32994");
  assert(String::fromDouble(0.324, 10) == "0.324");
  assert(String::fromDouble(0.324, 1) == "0.3");
}

void test_filename()
{
  assert(String("C:\\foo\\main.cpp").getFilePath() == "C:\\foo");
  assert(String("C:\\foo\\main.cpp").getFileName() == "main.cpp");
  assert(String("C:\\foo\\main.cpp").getFileExtension() == "cpp");
  assert(String("C:\\foo\\main.cpp").getFileTitle() == "main");

  String path("C:\\foo");
  path = path.addPathComponent("src");
  assert(path == "C:\\foo\\src");
  path = path.addPathComponent("headers\\");
  assert(path == "C:\\foo\\src\\headers\\");
  path = path.addPathComponent("main.h");
  assert(path == "C:\\foo\\src\\headers\\main.h");
}

int main()
{
  test_equal();
  test_from();
  test_filename();
  return 0;
}
