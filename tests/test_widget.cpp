#include <gtest/gtest.h>

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

    EXPECT_TRUE(a.getParent() == &parent);
    EXPECT_TRUE(b.getParent() == &parent);
    EXPECT_TRUE(a.getStyle() == b.getStyle());
    EXPECT_EQ(0, a.getId());
    EXPECT_EQ(0, b.getId());

    // check if get/setText work
    a.setText(L"bbb");
    EXPECT_EQ(L"bbb", a.getText());

    // check if get/setId work
    a.setId(32);
    b.setId(33);
    EXPECT_EQ(32, a.getId());
    EXPECT_EQ(33, b.getId());
  }

  // create without parent
  {
    T a(L"caption", NULL, T::Styles::Default - Widget::Styles::Visible);
    EXPECT_EQ(L"caption", a.getText());
    EXPECT_EQ(NULL, a.getParent());
  }
}

TEST(Widget, BasicBehavior)
{
  Application app;
  test_basic<Label>();
  test_basic<Button>();
  test_basic<TextEdit>();
  test_basic<CheckBox>();
  test_basic<ToggleButton>();
  test_basic<GroupBox>();
  test_basic<LinkLabel>();
}

TEST(Widget, NonExistentClass)
{
  Application app;

  EXPECT_THROW(Widget a(WidgetClassName(L"Vaca.NonExistentClass"), NULL, Widget::Styles::None),
	       CreateWidgetException);
}
