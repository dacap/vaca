#include <cassert>

#include "Vaca/Vaca.h"

using namespace Vaca;

void test_double_wrapper()
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
    assert(false);		// other exception!!
  }
}

int main()
{
  Application app;
  test_double_wrapper();
  return 0;
}
