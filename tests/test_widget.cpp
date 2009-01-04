#include <cassert>

#include "Vaca/Vaca.h"

using namespace Vaca;

template<class T>
void test_basic()
{
  // create with default style
  {
    Frame parent(L"title");
    T a(L"caption", &parent);
    T b(L"caption", &parent, T::Styles::Default);

    assert(a.getParent() == &parent);
    assert(b.getParent() == &parent);
    assert(a.getStyle() == b.getStyle());
    assert(a.getId() == 0);
    assert(b.getId() == 0);

    // check if get/setText work
    a.setText(L"bbb");
    assert(a.getText() == L"bbb");

    // check if get/setId work
    a.setId(32);
    b.setId(33);
    assert(a.getId() == 32);
    assert(b.getId() == 33);
  }

  // create without parent
  {
    T a(L"caption", NULL, T::Styles::Default - Widget::Styles::Visible);
    assert(a.getText() == L"caption");
    assert(a.getParent() == NULL);
  }
}

void test_nonexistent_class()
{
  try {
    Widget a(WidgetClassName(L"Vaca.NonExistentClass"), NULL, Widget::Styles::None);

    // CreateWidgetException wasn't throw
    assert(false);
  }
  catch (CreateWidgetException& e) {
    // expected behavior
  }
}

int main()
{
  Application app;

  test_basic<Label>();
  test_basic<Button>();
  test_basic<TextEdit>();
  test_basic<CheckBox>();
  test_basic<ToggleButton>();
  test_basic<GroupBox>();
  test_basic<LinkLabel>();

  test_nonexistent_class();

  return 0;
}
