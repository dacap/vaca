#include <cassert>

#include "Vaca/String.h"

using namespace Vaca;

void test_basic()
{
  assert(String() == _T(""));
}
  
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
  assert(String("C:/foo/pack.tar.gz").getFilePath() == _T("C:/foo"));
  assert(String("./main.cpp").getFilePath() == _T("."));
  assert(String(".\\main.cpp").getFilePath() == _T("."));
  assert(String("\\main.cpp").getFilePath() == _T(""));
  assert(String("main.cpp").getFilePath() == _T(""));
  assert(String("main.").getFilePath() == _T(""));
  assert(String("main").getFilePath() == _T(""));
  assert(String("C:/foo/").getFilePath() == _T("C:/foo"));
  assert(String("C:\\").getFilePath() == _T("C:"));
  assert(String("C:\\.cpp").getFilePath() == _T("C:"));
  assert(String(".cpp").getFilePath() == _T(""));
  assert(String("").getFilePath() == _T(""));

  assert(String("C:\\foo\\main.cpp").getFileName() == _T("main.cpp"));
  assert(String("C:/foo/pack.tar.gz").getFileName() == _T("pack.tar.gz"));
  assert(String("./main.cpp").getFileName() == _T("main.cpp"));
  assert(String(".\\main.cpp").getFileName() == _T("main.cpp"));
  assert(String("\\main.cpp").getFileName() == _T("main.cpp"));
  assert(String("main.cpp").getFileName() == _T("main.cpp"));
  assert(String("main.").getFileName() == _T("main."));
  assert(String("main").getFileName() == _T("main"));
  assert(String("C:/foo/").getFileName() == _T(""));
  assert(String("C:\\").getFileName() == _T(""));
  assert(String("C:\\.cpp").getFileName() == _T(".cpp"));
  assert(String(".cpp").getFileName() == _T(".cpp"));
  assert(String("").getFileName() == _T(""));

  assert(String("C:\\foo\\main.cpp").getFileExtension() == _T("cpp"));
  assert(String("C:/foo/pack.tar.gz").getFileExtension() == _T("gz"));
  assert(String("./main.cpp").getFileExtension() == _T("cpp"));
  assert(String(".\\main.cpp").getFileExtension() == _T("cpp"));
  assert(String("\\main.cpp").getFileExtension() == _T("cpp"));
  assert(String("main.cpp").getFileExtension() == _T("cpp"));
  assert(String("main.").getFileExtension() == _T(""));
  assert(String("main").getFileExtension() == _T(""));
  assert(String("C:/foo/").getFileExtension() == _T(""));
  assert(String("C:\\").getFileExtension() == _T(""));
  assert(String("C:\\.cpp").getFileExtension() == _T("cpp"));
  assert(String(".cpp").getFileExtension() == _T("cpp"));
  assert(String("").getFileExtension() == _T(""));

  assert(String("C:\\foo\\main.cpp").getFileTitle() == _T("main"));
  assert(String("C:/foo/pack.tar.gz").getFileTitle() == _T("pack.tar"));
  assert(String("./main.cpp").getFileTitle() == _T("main"));
  assert(String(".\\main.cpp").getFileTitle() == _T("main"));
  assert(String("\\main.cpp").getFileTitle() == _T("main"));
  assert(String("main.cpp").getFileTitle() == _T("main"));
  assert(String("main.").getFileTitle() == _T("main"));
  assert(String("main").getFileTitle() == _T("main"));
  assert(String("C:/foo/").getFileTitle() == _T(""));
  assert(String("C:\\").getFileTitle() == _T(""));
  assert(String("C:\\.cpp").getFileTitle() == _T(""));
  assert(String(".cpp").getFileTitle() == _T(""));
  assert(String("").getFileTitle() == _T(""));

  String path("C:\\foo");
  path.addPathComponent("src");
  assert(path == _T("C:\\foo\\src"));
  path.addPathComponent("headers\\");
  assert(path == _T("C:\\foo\\src\\headers\\"));
  path.addPathComponent("main.h");
  assert(path == _T("C:\\foo\\src\\headers\\main.h"));
}

void test_trim()
{
  assert(String().trim() == _T(""));
  assert(String("No trim").trim() == _T("No trim"));
  assert(String("Front    ").trim() == _T("Front"));
  assert(String("   End").trim() == _T("End"));
  assert(String("       ").trim() == _T(""));
  assert(String(" \n \r  \t \r \n \r\n \t   ").trim() == _T(""));
  assert(String(" \n \r Word \t   ").trim() == _T("Word"));
  assert(String("\tNo \n \r \ttrim \n ").trim() == _T("No \n \r \ttrim"));
}

void test_format()
{
  assert(String::format(  ("")) == _T(""));
  assert(String::format(_T("")) == _T(""));

  assert(String::format(  ("Hello")) == _T("Hello"));
  assert(String::format(_T("Hello")) == _T("Hello"));

  assert(String::format(  ("%d"), 2) == _T("2"));
  assert(String::format(_T("%d"), 2) == _T("2"));

  assert(String::format(  ("%.02f"), 20.3248) == _T("20.32"));
  assert(String::format(_T("%.02f"), 20.3248) == _T("20.32"));

  assert(String::format(  ("%5d"), 202) == _T("  202"));
  assert(String::format(_T("%5d"), 202) == _T("  202"));

  assert(String::format(  ("%05d"), 202) == _T("00202"));
  assert(String::format(_T("%05d"), 202) == _T("00202"));
}

void test_format_overflow()
{
  for (int n=1024; n<1026; ++n) {
    char* buf(new char[n]);

    for (int d=10; d<1000; d*=10) {
      for (int c=0; c<n-3; ++c)
	buf[c] = 'x';
      buf[n-3] = '%';
      buf[n-2] = 'd';
      buf[n-1] = 0;

      assert(String::format(buf, d) == std::string(n-3, 'x') + String::fromInt(d));
    }

    delete buf;
  }
}

int main()
{
  test_basic();
  test_equal();
  test_from();
  test_parse();
  test_filename();
  test_trim();
  test_format();
  test_format_overflow();
  return 0;
}
