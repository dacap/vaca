#include <cassert>

#include "Vaca/Vaca.h"

using namespace Vaca;

void test_double_wrapper_fail()
{
  Frame a(L"first wrapper");

  try {
    Widget b(a.getHandle());	// second wrapper

    // CreateWidgetException wasn't throw
    assert(false);
  }
  catch (CreateWidgetException& e) {
    // expected behavior
  }
  catch (...) {
    assert(false);		// other kind of exception!!
  }
}

int main()
{
  Application app;
  test_double_wrapper_fail();
  return 0;
}
