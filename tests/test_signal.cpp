#include <gtest/gtest.h>
#include <string>

#include "vaca/Signal.h"

using namespace std;
using namespace vaca;

//////////////////////////////////////////////////////////////////////

namespace test0 {

  int count0_void = 0;
  int count1_void = 0;
  int count2_void = 0;
  int count0_int = 0;
  int count1_int = 0;
  int count2_int = 0;
  int count0_string = 0;
  int count1_string = 0;
  int count2_string = 0;

  void func0_void() { ++count0_void; }
  void func1_void(int a) { ++count1_void; }
  void func2_void(int a, string b) { ++count2_void; }
  int func0_int() { ++count0_int; return -1; }
  int func1_int(int a) { ++count1_int; return a; }
  int func2_int(int a, int b) { ++count2_int; return a+b; }
  string func0_string() { ++count0_string; return "empty"; }
  string func1_string(string a) { ++count1_string; return a; }
  string func2_string(string a, string b) { ++count2_string; return a+b; }

  struct struct0_void { void operator()() { ++count0_void; } };
  struct struct1_void { void operator()(int a) { ++count1_void; } };
  struct struct2_void { void operator()(int a, string b) { ++count2_void; } };
  struct struct0_int { int operator()() { ++count0_int; return -1; } };
  struct struct1_int { int operator()(int a) { ++count1_int; return a; } };
  struct struct2_int { int operator()(int a, int b) { ++count2_int; return a+b; } };
  struct struct0_string { string operator()() { ++count0_string; return "empty"; } };
  struct struct1_string { string operator()(string a) { ++count1_string; return a; } };
  struct struct2_string { string operator()(string a, string b) { ++count2_string; return a+b; } };

  class class0_void { public: void method() { ++count0_void; } };
  class class1_void { public: void method(int a) { ++count1_void; } };
  class class2_void { public: void method(int a, string b) { ++count2_void; } };
  class class0_int { public: int method() { ++count0_int; return -1; } };
  class class1_int { public: int method(int a) { ++count1_int; return a; } };
  class class2_int { public: int method(int a, int b) { ++count2_int; return a+b; } };
  class class0_string { public: string method() { ++count0_string; return "empty"; } };
  class class1_string { public: string method(string a) { ++count1_string; return a; } };
  class class2_string { public: string method(string a, string b) { ++count2_string; return a+b; } };

  TEST(Signal, Coverage)
  {
    Signal<void()>                 sig0_void;
    Signal<void(int)>              sig1_void;
    Signal<void(int, string)>      sig2_void;
    Signal<int()>                  sig0_int;
    Signal<int(int)>               sig1_int;
    Signal<int(int, int)>          sig2_int;
    Signal<string()>               sig0_string;
    Signal<string(string)>         sig1_string;
    Signal<string(string, string)> sig2_string;

    sig0_void.connect(&func0_void);
    sig1_void.connect(&func1_void);
    sig2_void.connect(&func2_void);
    sig0_int.connect(&func0_int);
    sig1_int.connect(&func1_int);
    sig2_int.connect(&func2_int);
    sig0_string.connect(&func0_string);
    sig1_string.connect(&func1_string);
    sig2_string.connect(&func2_string);

    sig0_void.connect(struct0_void());
    sig1_void.connect(struct1_void());
    sig2_void.connect(struct2_void());
    sig0_int.connect(struct0_int());
    sig1_int.connect(struct1_int());
    sig2_int.connect(struct2_int());
    sig0_string.connect(struct0_string());
    sig1_string.connect(struct1_string());
    sig2_string.connect(struct2_string());

    class0_void i1; sig0_void.connect(&class0_void::method, &i1);
    class1_void i2; sig1_void.connect(&class1_void::method, &i2);
    class2_void i3; sig2_void.connect(&class2_void::method, &i3);
    class0_int i4; sig0_int.connect([&]() -> int { return i4.method(); });
    class1_int i5; sig1_int.connect([&](int x) -> int { return i5.method(x); });
    class2_int i6; sig2_int.connect([&](int x, int y) -> int { return i6.method(x, y); });
    class0_string i7; sig0_string.connect([&]() -> string { return i7.method(); });
    class1_string i8; sig1_string.connect([&](string x) -> string { return i8.method(x); });
    class2_string i9; sig2_string.connect([&](string x, string y) -> string { return i9.method(x, y); });

    sig0_void();
    sig1_void(1);
    sig2_void(32, "32");
    sig0_int();
    sig1_int(1);
    sig2_int(43, 20);
    sig0_string();
    sig1_string("Hello World!");
    sig2_string("Hi", "Bye");

    EXPECT_EQ(3, count0_void);
    EXPECT_EQ(3, count1_void);
    EXPECT_EQ(3, count2_void);
    EXPECT_EQ(3, count0_int);
    EXPECT_EQ(3, count1_int);
    EXPECT_EQ(3, count2_int);
    EXPECT_EQ(3, count0_string);
    EXPECT_EQ(3, count1_string);
    EXPECT_EQ(3, count2_string);

    // with "default_result"
    sig0_int.callWithDefaultReturn(0);
    sig1_int.callWithDefaultReturn(0, 1);
    sig2_int.callWithDefaultReturn(0, 43, 20);
    sig0_string.callWithDefaultReturn("");
    sig1_string.callWithDefaultReturn("", "Hello World!");
    sig2_string.callWithDefaultReturn("", "Hi", "Bye");
  }

}

//////////////////////////////////////////////////////////////////////

namespace test1 {

  int func1_counter = 0;
  int op1_counter = 0;
  int ok_counter = 0;
  int cancel_counter = 0;

  void func1()
  {
    func1_counter++;
  }

  struct op1
  {
    int a;
    op1(int a) : a(a) { }
    void operator()() {
      if (a == 9)
	op1_counter++;
    }
  };

  class Q
  {
    int v;
  public:
    Q(int v) : v(v) { }
    void ok() { if (v == 1) ok_counter++; }
    void cancel() { if (v == 2) cancel_counter++; }
  };

  TEST(Signal, ReturnVoidNoArgs)
  {
    Signal<void()> s;
    {
      Slot<void()>* a = s.connect(&func1);
      s();
      s();
      s.disconnect(a);
      delete a;
    }
    s();
    s.connect(op1(9));
    s();
    {
      Q q1(1);
      Q q2(2);
      Slot<void()>* b = s.connect(&Q::ok, &q1);
      Slot<void()>* c = s.connect(&Q::cancel, &q2);
      s();
    }
    EXPECT_EQ(2, func1_counter);
    EXPECT_EQ(2, op1_counter);
    EXPECT_EQ(1, ok_counter);
    EXPECT_EQ(1, cancel_counter);
  }

}

//////////////////////////////////////////////////////////////////////

namespace test2 {

  int return_4() { return 4; }
  int return_6() { return 6; }

  TEST(Signal, ReturnIntNoArgs)
  {
    Signal<int()> s;
    s.connect(&return_4);
    EXPECT_EQ(4, s());
    s.connect(&return_6);
    EXPECT_EQ(6, s());		// Last returned value (first slot returned 4,
				// second one returned 6)
  }

}

//////////////////////////////////////////////////////////////////////

namespace test3 {

  void return_check_arg_eq_5(int arg)
  {
    EXPECT_EQ(5, arg);
  }

  struct checker
  {
    int a;
    checker(int a) : a(a) { }
    void operator()(int b) {
      EXPECT_TRUE(a < b);
    }
  };

  TEST(Signal, ReturnVoidOneArg)
  {
    Signal<void(int)> s;
    Slot<void(int)>* a = s.connect(&return_check_arg_eq_5);
    s(5);
    s.disconnect(a);
    delete a;

    s.connect(checker(4));
    s(5);
    s.connect(checker(9));
    s(10);
  }

}

//////////////////////////////////////////////////////////////////////

namespace test4 {

  struct members_eq_args
  {
    int a, b;
    members_eq_args(int a, int b) : a(a), b(b) { }
    void operator()(int c, int d) {
      EXPECT_TRUE(a+b == c+d);
    }
  };

  int sum_args(int a, int b)
  {
    return a+b;
  }

  class sum_of_squares
  {
  public:
    int sum;
    sum_of_squares() : sum(0) { }
    int add(int x, int m) {
      sum += (x-m)*(x-m);
      return sum;
    }
  };

  TEST(Signal, ReturnVoidTwoArgs)
  {
    Signal<void(int, int)> s1;
    s1.connect(members_eq_args(2, 4));
    s1(3, 3);
    s1.connect(members_eq_args(3, 3));
    s1.connect(members_eq_args(-1, 7));
    s1(5, 1);

    Signal<int(int, int)> s2;
    s2.connect(&sum_args);
    EXPECT_EQ(6, s2(2, 4));

    Signal<int(int, int)> s3;
    sum_of_squares i;
    s3.connect([&i](int x, int y) -> int { return i.add(x, y); });
    EXPECT_EQ(9,    s3(4, 1));
    EXPECT_EQ(9+25, s3(5, 0));
  }

}

//////////////////////////////////////////////////////////////////////
