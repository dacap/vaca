#include <cassert>

#include "Vaca/Bind.h"
#include "Vaca/Signal.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

int void0fun_hits;
int void1fun_hits;
int void2fun_hits;
int int0fun_hits;
int int1fun_hits;
int int2fun_hits;

int void0cal_hits;
int void1cal_hits;
int void2cal_hits;
int int0cal_hits;
int int1cal_hits;
int int2cal_hits;

int void0mem_hits;
int void1mem_hits;
int void2mem_hits;
int int0mem_hits;
int int1mem_hits;
int int2mem_hits;

int ctor_hits;
int copy_ctor_hits;

void reset_hits()
{
  void0fun_hits = void1fun_hits = void2fun_hits = 0;
  void0cal_hits = void1cal_hits = void2cal_hits = 0;
  void0mem_hits = void1mem_hits = void2mem_hits = 0;
  int0fun_hits = int1fun_hits = int2fun_hits = 0;
  int0cal_hits = int1cal_hits = int2cal_hits = 0;
  int0mem_hits = int1mem_hits = int2mem_hits = 0;
  ctor_hits = copy_ctor_hits = 0;
}

//////////////////////////////////////////////////////////////////////

namespace test1 {

void f0() { ++void0fun_hits; }
void f1(int a) { ++void1fun_hits; assert(a == 5); }
void f2(int a, int b) { ++void2fun_hits; assert(a == b); }

int f0_int() { ++int0fun_hits; return 0; }
int f1_int(int a) { ++int1fun_hits; assert(a == 5); return 0; }
int f2_int(int a, int b) { ++int2fun_hits; assert(a == b); return 0; }

struct T0 {
  void operator()() { ++void0cal_hits; }
};
struct T0_int {
  int operator()() { ++int0cal_hits; return 0; }
};
struct T1 {
  void operator()(int a) { ++void1cal_hits; assert(a == 5); }
};
struct T1_int {
  int operator()(int a) { ++int1cal_hits; assert(a == 5); return a; }
};
struct T2 {
  void operator()(int a, int b) { ++void2cal_hits; assert(a == b); }
};
struct T2_int {
  int operator()(int a, int b) { ++int2cal_hits; assert(a == b); return a+b; }
};
struct T3 {
  void method0() { ++void0mem_hits; }
  void method1(int a) { ++void1mem_hits; assert(a == 5); }
  void method2(int a, int b) { ++void2mem_hits; assert(a == b); }
  int method0_int() { ++int0mem_hits; return 0; }
  int method1_int(int a) { ++int1mem_hits; assert(a == 5); return a; }
  int method2_int(int a, int b) { ++int2mem_hits; assert(a == b); return a+b; }
};

void test0_void()
{
  Signal0<void> s0;
  s0.connect(&f0);		// direct connection (Slot0_fun)
  s0.connect(Bind<void>(&f0));	// dummy adapter
  s0.connect(Bind<void>(&f1, 5));
  s0.connect(Bind<void>(&f2, 5, 5));
  s0.connect(Bind<void>(&f0_int));
  s0.connect(Bind<void>(&f1_int, 5));
  s0.connect(Bind<void>(&f2_int, 5, 5));
  s0.connect(T0());		// direct connection (Slot0_fun)
  s0.connect(Bind<void>(T0()));
  s0.connect(Bind<void>(T1(), 5));
  s0.connect(Bind<void>(T2(), 5, 5));
  s0.connect(Bind<void>(T0_int()));
  s0.connect(Bind<void>(T1_int(), 5));
  s0.connect(Bind<void>(T2_int(), 5, 5));
  T3 a;
  s0.connect(&T3::method0, &a);	      // direct connection (Slot0_mem)
  s0.connect(Bind(&T3::method0, &a)); // dummy adapter
  s0.connect(Bind(&T3::method1, &a, 5));
  s0.connect(Bind(&T3::method2, &a, 5, 5));
  s0.connect(Bind(&T3::method0_int, &a));
  s0.connect(Bind(&T3::method1_int, &a, 5));
  s0.connect(Bind(&T3::method2_int, &a, 5, 5));
  s0();

  assert(void0fun_hits == 2);
  assert(void1fun_hits == 1);
  assert(void2fun_hits == 1);
  assert(int0fun_hits == 1);
  assert(int1fun_hits == 1);
  assert(int2fun_hits == 1);
  assert(void0cal_hits == 2);
  assert(void1cal_hits == 1);
  assert(void2cal_hits == 1);
  assert(int0cal_hits == 1);
  assert(int1cal_hits == 1);
  assert(int2cal_hits == 1);
  assert(void0mem_hits == 2);
  assert(void1mem_hits == 1);
  assert(void2mem_hits == 1);
  assert(int0mem_hits == 1);
  assert(int1mem_hits == 1);
  assert(int2mem_hits == 1);
}

void test0_int()
{
  Signal0<int> s0;
  s0.connect(&f0_int);		  // direct connection (Slot0_fun)
  s0.connect(Bind<int>(&f0_int)); // dummy adapter
  s0.connect(Bind<int>(&f1_int, 5));
  s0.connect(Bind<int>(&f2_int, 5, 5));
  s0.connect(T0_int());		// direct connection (Slot0_fun)
  s0.connect(Bind<int>(T0_int()));
  s0.connect(Bind<int>(T1_int(), 5));
  s0.connect(Bind<int>(T2_int(), 5, 5));
  T3 a;
  s0.connect(&T3::method0_int, &a);	  // direct connection (Slot0_mem)
  s0.connect(Bind(&T3::method0_int, &a)); // dummy adapter
  s0.connect(Bind(&T3::method1_int, &a, 5));
  s0.connect(Bind(&T3::method2_int, &a, 5, 5));
  s0();

  assert(void0fun_hits == 0);
  assert(void1fun_hits == 0);
  assert(void2fun_hits == 0);
  assert(int0fun_hits == 2);
  assert(int1fun_hits == 1);
  assert(int2fun_hits == 1);
  assert(void0cal_hits == 0);
  assert(void1cal_hits == 0);
  assert(void2cal_hits == 0);
  assert(int0cal_hits == 2);
  assert(int1cal_hits == 1);
  assert(int2cal_hits == 1);
  assert(void0mem_hits == 0);
  assert(void1mem_hits == 0);
  assert(void2mem_hits == 0);
  assert(int0mem_hits == 2);
  assert(int1mem_hits == 1);
  assert(int2mem_hits == 1);
}

void test1_void()
{
  Signal1<void, int> s1;
  s1.connect(&f1);		// direct connection (Slot1_fun)
  s1.connect(Bind<void>(&f0));
  s1.connect(Bind<void>(&f1, 5)); // this isn't a dummy adapter ('f1' is called with this 5)
  s1.connect(Bind<void>(&f2, 5, 5));
  s1.connect(Bind<void>(&f0_int));
  s1.connect(Bind<void>(&f1_int, 5));
  s1.connect(Bind<void>(&f2_int, 5, 5));
  s1.connect(T1());		// direct connection (Slot1_fun)
  s1.connect(Bind<void>(T0()));
  s1.connect(Bind<void>(T1(), 5));
  s1.connect(Bind<void>(T2(), 5, 5));
  s1.connect(Bind<void>(T0_int()));
  s1.connect(Bind<void>(T1_int(), 5));
  s1.connect(Bind<void>(T2_int(), 5, 5));
  T3 a;
  s1.connect(&T3::method1, &a);		 // direct connection (Slot1_mem)
  s1.connect(Bind(&T3::method0, &a));
  s1.connect(Bind(&T3::method1, &a, 5)); // this isn't a dummy adapter ('method1' is called with this 5)
  s1.connect(Bind(&T3::method2, &a, 5, 5));
  s1.connect(Bind(&T3::method0_int, &a));
  s1.connect(Bind(&T3::method1_int, &a, 5));
  s1.connect(Bind(&T3::method2_int, &a, 5, 5));
  s1(5);

  assert(void0fun_hits == 1);
  assert(void1fun_hits == 2);
  assert(void2fun_hits == 1);
  assert(int0fun_hits == 1);
  assert(int1fun_hits == 1);
  assert(int2fun_hits == 1);
  assert(void0cal_hits == 1);
  assert(void1cal_hits == 2);
  assert(void2cal_hits == 1);
  assert(int0cal_hits == 1);
  assert(int1cal_hits == 1);
  assert(int2cal_hits == 1);
  assert(void0mem_hits == 1);
  assert(void1mem_hits == 2);
  assert(void2mem_hits == 1);
  assert(int0mem_hits == 1);
  assert(int1mem_hits == 1);
  assert(int2mem_hits == 1);
}

void test1_int()
{
  Signal1<int, int> s1;
  s1.connect(&f1_int);		// direct connection (Slot1_fun)
  s1.connect(Bind<int>(&f0_int));
  s1.connect(Bind<int>(&f1_int, 5));
  s1.connect(Bind<int>(&f2_int, 5, 5));
  s1.connect(T1_int());		// direct connection (Slot1_fun)
  s1.connect(Bind<int>(T0_int()));
  s1.connect(Bind<int>(T1_int(), 5));
  s1.connect(Bind<int>(T2_int(), 5, 5));
  T3 a;
  s1.connect(&T3::method1_int, &a); // direct connection (Slot1_mem)
  s1.connect(Bind(&T3::method0_int, &a));
  s1.connect(Bind(&T3::method1_int, &a, 5)); // this isn't a dummy adapter ('method1_int' is called with this 5)
  s1.connect(Bind(&T3::method2_int, &a, 5, 5));
  s1(5);

  assert(void0fun_hits == 0);
  assert(void1fun_hits == 0);
  assert(void2fun_hits == 0);
  assert(int0fun_hits == 1);
  assert(int1fun_hits == 2);
  assert(int2fun_hits == 1);
  assert(void0cal_hits == 0);
  assert(void1cal_hits == 0);
  assert(void2cal_hits == 0);
  assert(int0cal_hits == 1);
  assert(int1cal_hits == 2);
  assert(int2cal_hits == 1);
  assert(void0mem_hits == 0);
  assert(void1mem_hits == 0);
  assert(void2mem_hits == 0);
  assert(int0mem_hits == 1);
  assert(int1mem_hits == 2);
  assert(int2mem_hits == 1);
}

void test2_void()
{
  Signal2<void, int, int> s2;
  s2.connect(&f2);		     // direct connection (Slot2_fun)
  s2.connect(Bind<void>(&f0));
  s2.connect(Bind<void>(&f1, 5));
  s2.connect(Bind<void>(&f2, 5, 5)); // this isn't a dummy adapter
  s2.connect(Bind<void>(&f0_int));
  s2.connect(Bind<void>(&f1_int, 5));
  s2.connect(Bind<void>(&f2_int, 5, 5));
  s2.connect(T2());		      // direct connection (Slot2_fun)
  s2.connect(Bind<void>(T0()));
  s2.connect(Bind<void>(T1(), 5));
  s2.connect(Bind<void>(T2(), 5, 5)); // this isn't a dummy adapter
  s2.connect(Bind<void>(T0_int()));
  s2.connect(Bind<void>(T1_int(), 5));
  s2.connect(Bind<void>(T2_int(), 5, 5));
  T3 a;
  s2.connect(&T3::method2, &a);		 // direct connection (Slot2_mem)
  s2.connect(Bind(&T3::method0, &a));
  s2.connect(Bind(&T3::method1, &a, 5));
  s2.connect(Bind(&T3::method2, &a, 5, 5)); // this isn't a dummy adapter
  s2.connect(Bind(&T3::method0_int, &a));
  s2.connect(Bind(&T3::method1_int, &a, 5));
  s2.connect(Bind(&T3::method2_int, &a, 5, 5));
  s2(5, 5);

  assert(void0fun_hits == 1);
  assert(void1fun_hits == 1);
  assert(void2fun_hits == 2);
  assert(int0fun_hits == 1);
  assert(int1fun_hits == 1);
  assert(int2fun_hits == 1);
  assert(void0cal_hits == 1);
  assert(void1cal_hits == 1);
  assert(void2cal_hits == 2);
  assert(int0cal_hits == 1);
  assert(int1cal_hits == 1);
  assert(int2cal_hits == 1);
  assert(void0mem_hits == 1);
  assert(void1mem_hits == 1);
  assert(void2mem_hits == 2);
  assert(int0mem_hits == 1);
  assert(int1mem_hits == 1);
  assert(int2mem_hits == 1);
}

void test2_int()
{
  Signal2<int, int, int> s2;
  s2.connect(&f2_int);		// direct connection (Slot2_fun)
  s2.connect(Bind<int>(&f0_int));
  s2.connect(Bind<int>(&f1_int, 5));
  s2.connect(Bind<int>(&f2_int, 5, 5));
  s2.connect(T2_int());		// direct connection (Slot2_fun)
  s2.connect(Bind<int>(T0_int()));
  s2.connect(Bind<int>(T1_int(), 5));
  s2.connect(Bind<int>(T2_int(), 5, 5));
  T3 a;
  s2.connect(&T3::method2_int, &a); // direct connection (Slot2_mem)
  s2.connect(Bind(&T3::method0_int, &a));
  s2.connect(Bind(&T3::method1_int, &a, 5));
  s2.connect(Bind(&T3::method2_int, &a, 5, 5));
  s2(5, 5);

  assert(void0fun_hits == 0);
  assert(void1fun_hits == 0);
  assert(void2fun_hits == 0);
  assert(int0fun_hits == 1);
  assert(int1fun_hits == 1);
  assert(int2fun_hits == 2);
  assert(void0cal_hits == 0);
  assert(void1cal_hits == 0);
  assert(void2cal_hits == 0);
  assert(int0cal_hits == 1);
  assert(int1cal_hits == 1);
  assert(int2cal_hits == 2);
  assert(void0mem_hits == 0);
  assert(void1mem_hits == 0);
  assert(void2mem_hits == 0);
  assert(int0mem_hits == 1);
  assert(int1mem_hits == 1);
  assert(int2mem_hits == 2);
}

} // test1

//////////////////////////////////////////////////////////////////////

namespace test2 {

class Int
{
  int a;
public:
  Int(int a) : a(a) { ++ctor_hits; }
  Int(const Int& b) : a(b.a) { ++copy_ctor_hits; }
  int value() const { return a; }
  bool operator==(int a) const { return this->a == a; }
  bool operator==(const Int& b) const { return a == b.a; }
  int operator+(const Int& b) const { return a + b.a; }
};

void f1(const Int& a) { ++void1fun_hits; assert(a == 5); }
void f2(const Int& a, const Int& b) { ++void2fun_hits; assert(a == b); }

int f1_int(const Int& a) { ++int1fun_hits; assert(a == 5); return 0; }
int f2_int(const Int& a, const Int& b) { ++int2fun_hits; assert(a == b); return 0; }

struct T1 {
  void operator()(const Int& a) { ++void1cal_hits; assert(a == 5); }
};
struct T1_int {
  int operator()(const Int& a) { ++int1cal_hits; assert(a == 5); return a.value(); }
};
struct T2 {
  void operator()(const Int& a, const Int& b) { ++void2cal_hits; assert(a == b); }
};
struct T2_int {
  int operator()(const Int& a, const Int& b) { ++int2cal_hits; assert(a == b); return a+b; }
};
struct T3 {
  void method1(const Int& a) { ++void1mem_hits; assert(a == 5); }
  void method2(const Int& a, const Int& b) { ++void2mem_hits; assert(a == b); }
  int method1_int(const Int& a) { ++int1mem_hits; assert(a == 5); return a.value(); }
  int method2_int(const Int& a, const Int& b) { ++int2mem_hits; assert(a == b); return a+b; }
};

void test1_void()
{
  Int n(5);
  Signal1<void, const Int&> s1;
  s1.connect(&f1);		// direct connection (Slot1_fun)
  s1.connect(Bind<void>(&f1, Ref(n))); // this isn't a dummy adapter
  s1.connect(Bind<void>(&f2, Ref(n), Ref(n)));
  s1.connect(Bind<void>(&f1_int, Ref(n)));
  s1.connect(Bind<void>(&f2_int, Ref(n), Ref(n)));
  s1.connect(T1());		// direct connection (Slot1_fun)
  s1.connect(Bind<void>(T1(), Ref(n)));
  s1.connect(Bind<void>(T2(), Ref(n), Ref(n)));
  s1.connect(Bind<void>(T1_int(), Ref(n)));
  s1.connect(Bind<void>(T2_int(), Ref(n), Ref(n)));
  T3 a;
  s1.connect(&T3::method1, &a);		 // direct connection (Slot1_mem)
  s1.connect(Bind(&T3::method1, &a, Ref(n))); // this isn't a dummy adapter
  s1.connect(Bind(&T3::method2, &a, Ref(n), Ref(n)));
  s1.connect(Bind(&T3::method1_int, &a, Ref(n)));
  s1.connect(Bind(&T3::method2_int, &a, Ref(n), Ref(n)));
  s1(n);

  assert(ctor_hits == 1);
  assert(copy_ctor_hits == 0);
  assert(void1fun_hits == 2);
  assert(void2fun_hits == 1);
  assert(int1fun_hits == 1);
  assert(int2fun_hits == 1);
  assert(void1cal_hits == 2);
  assert(void2cal_hits == 1);
  assert(int1cal_hits == 1);
  assert(int2cal_hits == 1);
  assert(void1mem_hits == 2);
  assert(void2mem_hits == 1);
  assert(int1mem_hits == 1);
  assert(int2mem_hits == 1);
}

void test1_int()
{
  Int n(5);
  Signal1<int, const Int&> s1;
  s1.connect(&f1_int);		// direct connection (Slot1_fun)
  s1.connect(Bind<int>(&f1_int, Ref(n)));
  s1.connect(Bind<int>(&f2_int, Ref(n), Ref(n)));
  s1.connect(T1_int());		// direct connection (Slot1_fun)
  s1.connect(Bind<int>(T1_int(), Ref(n)));
  s1.connect(Bind<int>(T2_int(), Ref(n), Ref(n)));
  T3 a;
  s1.connect(&T3::method1_int, &a); // direct connection (Slot1_mem)
  s1.connect(Bind(&T3::method1_int, &a, Ref(n))); // this isn't a dummy adapter
  s1.connect(Bind(&T3::method2_int, &a, Ref(n), Ref(n)));
  s1(n);

  assert(ctor_hits == 1);
  assert(copy_ctor_hits == 0);
  assert(void1fun_hits == 0);
  assert(void2fun_hits == 0);
  assert(int1fun_hits == 2);
  assert(int2fun_hits == 1);
  assert(void1cal_hits == 0);
  assert(void2cal_hits == 0);
  assert(int1cal_hits == 2);
  assert(int2cal_hits == 1);
  assert(void1mem_hits == 0);
  assert(void2mem_hits == 0);
  assert(int1mem_hits == 2);
  assert(int2mem_hits == 1);
}

void test2_void()
{
  Int n(5);
  Signal2<void, const Int&, const Int&> s2;
  s2.connect(&f2);		     // direct connection (Slot2_fun)
  s2.connect(Bind<void>(&f1, Ref(n)));
  s2.connect(Bind<void>(&f2, Ref(n), Ref(n))); // this isn't a dummy adapter
  s2.connect(Bind<void>(&f1_int, Ref(n)));
  s2.connect(Bind<void>(&f2_int, Ref(n), Ref(n)));
  s2.connect(T2());		      // direct connection (Slot2_fun)
  s2.connect(Bind<void>(T1(), Ref(n)));
  s2.connect(Bind<void>(T2(), Ref(n), Ref(n))); // this isn't a dummy adapter
  s2.connect(Bind<void>(T1_int(), Ref(n)));
  s2.connect(Bind<void>(T2_int(), Ref(n), Ref(n)));
  T3 a;
  s2.connect(&T3::method2, &a);		 // direct connection (Slot2_mem)
  s2.connect(Bind(&T3::method1, &a, Ref(n)));
  s2.connect(Bind(&T3::method2, &a, Ref(n), Ref(n))); // this isn't a dummy adapter
  s2.connect(Bind(&T3::method1_int, &a, Ref(n)));
  s2.connect(Bind(&T3::method2_int, &a, Ref(n), Ref(n)));
  s2(n, n);

  assert(ctor_hits == 1);
  assert(copy_ctor_hits == 0);
  assert(void1fun_hits == 1);
  assert(void2fun_hits == 2);
  assert(int1fun_hits == 1);
  assert(int2fun_hits == 1);
  assert(void1cal_hits == 1);
  assert(void2cal_hits == 2);
  assert(int1cal_hits == 1);
  assert(int2cal_hits == 1);
  assert(void1mem_hits == 1);
  assert(void2mem_hits == 2);
  assert(int1mem_hits == 1);
  assert(int2mem_hits == 1);
}

void test2_int()
{
  Int n(5);
  Signal2<int, const Int&, const Int&> s2;
  s2.connect(&f2_int);		// direct connection (Slot2_fun)
  s2.connect(Bind<int>(&f1_int, Ref(n)));
  s2.connect(Bind<int>(&f2_int, Ref(n), Ref(n)));
  s2.connect(T2_int());		// direct connection (Slot2_fun)
  s2.connect(Bind<int>(T1_int(), Ref(n)));
  s2.connect(Bind<int>(T2_int(), Ref(n), Ref(n)));
  T3 a;
  s2.connect(&T3::method2_int, &a); // direct connection (Slot2_mem)
  s2.connect(Bind(&T3::method1_int, &a, Ref(n)));
  s2.connect(Bind(&T3::method2_int, &a, Ref(n), Ref(n)));
  s2(n, n);

  assert(ctor_hits == 1);
  assert(copy_ctor_hits == 0);
  assert(void1fun_hits == 0);
  assert(void2fun_hits == 0);
  assert(int1fun_hits == 1);
  assert(int2fun_hits == 2);
  assert(void1cal_hits == 0);
  assert(void2cal_hits == 0);
  assert(int1cal_hits == 1);
  assert(int2cal_hits == 2);
  assert(void1mem_hits == 0);
  assert(void2mem_hits == 0);
  assert(int1mem_hits == 1);
  assert(int2mem_hits == 2);
}

}

int main()
{
  reset_hits(); test1::test0_void();
  reset_hits(); test1::test0_int();
  reset_hits(); test1::test1_void();
  reset_hits(); test1::test1_int();
  reset_hits(); test1::test2_void();
  reset_hits(); test1::test2_int();
  // test references
  reset_hits(); test2::test1_void();
  reset_hits(); test2::test1_int();
  reset_hits(); test2::test2_void();
  reset_hits(); test2::test2_int();
  return 0;
}
