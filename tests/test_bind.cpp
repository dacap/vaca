#include <cassert>

#include "Vaca/Bind.h"
#include "Vaca/Signal.h"

using namespace Vaca;

void f0() { }
void f1(int a) { assert(a == 5); }
void f2(int a, int b) { assert(a == b); }

int f0_int() { return 0; }
int f1_int(int a) { assert(a == 5); return 0; }
int f2_int(int a, int b) { assert(a == b); return 0; }

struct T0 {
  void operator()() { }
};
struct T0_int {
  int operator()() { return 0; }
};
struct T1 {
  void operator()(int a) { assert(a == 5); }
};
struct T1_int {
  int operator()(int a) { assert(a == 5); return a; }
};
struct T2 {
  void operator()(int a, int b) { assert(a == b); }
};
struct T2_int {
  int operator()(int a, int b) { assert(a == b); return a+b; }
};
struct T3 {
  void method0() { }
  void method1(int a) { assert(a == 5); }
  void method2(int a, int b) { assert(a == b); }
  int method0_int() { return 0; }
  int method1_int(int a) { assert(a == 5); return a; }
  int method2_int(int a, int b) { assert(a == b); return a+b; }
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
}

void test_raw_bind_classes()
{
  BindAdapter0_fun<void, void (*)()>		       void0fun(&f0);
  BindAdapter1_fun<void, void (*)(int), int>	       void1fun(&f1, 5);
  BindAdapter2_fun<void, void (*)(int, int), int, int> void2fun(&f2, 5, 5);
  BindAdapter0_fun<int, int (*)()>		       int0fun(&f0_int);
  BindAdapter1_fun<int, int (*)(int), int>	       int1fun(&f1_int, 5);
  BindAdapter2_fun<int, int (*)(int, int), int, int>   int2fun(&f2_int, 5, 5);

  T3 t3;
  BindAdapter0_mem<void, T3>	       void0mem(&T3::method0, &t3);
  BindAdapter1_mem<void, T3, int>      void1mem(&T3::method1, &t3, 5);
  BindAdapter2_mem<void, T3, int, int> void2mem(&T3::method2, &t3, 5, 5);
  BindAdapter0_mem<int, T3>	       int0mem(&T3::method0_int, &t3);
  BindAdapter1_mem<int, T3, int>       int1mem(&T3::method1_int, &t3, 5);
  BindAdapter2_mem<int, T3, int, int>  int2mem(&T3::method2_int, &t3, 5, 5);

  void0fun(); void0fun(0); void0fun(0,0); void0fun(0,0,0); void0fun(0,0,0,0);
  void1fun(); void1fun(0); void1fun(0,0); void1fun(0,0,0); void1fun(0,0,0,0);
  void2fun(); void2fun(0); void2fun(0,0); void2fun(0,0,0); void2fun(0,0,0,0);

  void0mem(); void0mem(0); void0mem(0,0); void0mem(0,0,0); void0mem(0,0,0,0);
  void1mem(); void1mem(0); void1mem(0,0); void1mem(0,0,0); void1mem(0,0,0,0);
  void2mem(); void2mem(0); void2mem(0,0); void2mem(0,0,0); void2mem(0,0,0,0);

  int0fun(); int0fun(0); int0fun(0,0); int0fun(0,0,0); int0fun(0,0,0,0);
  int1fun(); int1fun(0); int1fun(0,0); int1fun(0,0,0); int1fun(0,0,0,0);
  int2fun(); int2fun(0); int2fun(0,0); int2fun(0,0,0); int2fun(0,0,0,0);

  int0mem(); int0mem(0); int0mem(0,0); int0mem(0,0,0); int0mem(0,0,0,0);
  int1mem(); int1mem(0); int1mem(0,0); int1mem(0,0,0); int1mem(0,0,0,0);
  int2mem(); int2mem(0); int2mem(0,0); int2mem(0,0,0); int2mem(0,0,0,0);
}

int main()
{
  test0_void();
  test0_int();
  test1_void();
  test1_int();
  test2_void();
  test2_int();
  test_raw_bind_classes();
  return 0;
}
