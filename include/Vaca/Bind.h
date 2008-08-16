// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the Vaca nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef VACA_BIND_HPP
#define VACA_BIND_HPP

namespace Vaca {

  //////////////////////////////////////////////////////////////////////

  /**
   * A class to adapt a callable function or @e functor with
   * @c "ReturnType fn()" signature to another signature with the same
   * @c ReturnType.
   *
   * Example: 
   * @code
   * int fn() { ... }
   * ...
   * // R=int, F=void (*)()
   * BindAdapter0_fun<int, int (*)()> newfn(&fn); 
   * newfn();
   * newfn(1);          // these arguments are ignored,
   * newfn(1, 2);       // each of these calls just invoke fn()
   * newfn(1, 2, 3);
   * newfn("Hello");
   * @endcode
   *
   * @see BindAdapter0_fun<void,F>, Bind<R,F>
   */
  template<typename R, typename F>
  class BindAdapter0_fun
  {
    F f;
  public:
    BindAdapter0_fun(const F& f) : f(f) { }

    R operator()() { return f(); }

    template<typename A1>
    R operator()(A1 a1) { return f(); }

    template<typename A1, typename A2>
    R operator()(A1 a1, A2 a2) { return f(); }

    template<typename A1, typename A2, typename A3>
    R operator()(A1 a1, A2 a2, A3 a3) { return f(); }

    template<typename A1, typename A2, typename A3, typename A4>
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4) { return f(); }
  };

  /**
   * Specialization of @link Vaca::BindAdapter0_fun BindAdapter0_fun<R,F>@endlink for R=void.
   * 
   * @see Bind<R,F>
   */
  template<typename F>
  class BindAdapter0_fun<void, F>
  {
    F f;
  public:
    BindAdapter0_fun(const F& f) : f(f) { }

    void operator()() { f(); }

    template<typename A1>
    void operator()(A1 a1) { f(); }

    template<typename A1, typename A2>
    void operator()(A1 a1, A2 a2) { f(); }

    template<typename A1, typename A2, typename A3>
    void operator()(A1 a1, A2 a2, A3 a3) { f(); }

    template<typename A1, typename A2, typename A3, typename A4>
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4) { f(); }
  };

  /**
   * Creates a @link Vaca::BindAdapter0_fun BindAdapter0_fun<R,F>@endlink
   * deducing the F type through
   * the @a f argument. The return type R can't be deduced, so you
   * have to specified it as a template parameter.
   *
   * Example:
   * @code
   * int fn() { ... }
   * ...
   * Bind<int>(&fn)(1, "Hello"); // call fn() ignoring arguments
   * @endcode
   * 
   * @tparam R Return type of F function.
   * @tparam F Pointer to a function or a functor that can be invoked without parameters.
   */
  template<typename R, typename F>
  BindAdapter0_fun<R, F>
  Bind(const F& f)
  {
    return BindAdapter0_fun<R, F>(f);
  }

  //////////////////////////////////////////////////////////////////////

  /**
   * A class to adapt a method of T class with
   * @c "ReturnType T::method()" signature to another signature
   * with the same @c ReturnType.
   *
   * Example: 
   * @code
   * class MyClass {
   * public:
   *   int method() { ... }
   * };
   * ...
   * MyClass instance;
   * // R=int, T=MyClass
   * BindAdapter0_mem<int, MyClass> newfn(&MyClass::method, &instance);
   * newfn();
   * newfn(1);          // these arguments are ignored,
   * newfn(1, 2);       // each of these calls just invoke "instance.method()"
   * newfn(1, 2, 3);
   * newfn("Hello");
   * @endcode
   * 
   * @see BindAdapter0_mem<void,T>, Bind<R,T,T2>
   */
  template<typename R, typename T>
  class BindAdapter0_mem
  {
    R (T::*m)();
    T* t;
  public:
    template<typename T2>
    BindAdapter0_mem(R (T::*m)(), T2* t) : m(m), t(t) { }

    R operator()() { return (t->*m)(); }

    template <typename A1>
    R operator()(A1 a1) { return (t->*m)(); }

    template <typename A1, typename A2>
    R operator()(A1 a1, A2 a2) { return (t->*m)(); }

    template <typename A1, typename A2, typename A3>
    R operator()(A1 a1, A2 a2, A3 a3) { return (t->*m)(); }

    template <typename A1, typename A2, typename A3, typename A4>
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4) { return (t->*m)(); }
  };

  /**
   * Specialization of @link Vaca::BindAdapter0_mem BindAdapter0_mem<R,T>@endlink for R=void.
   * 
   * @see Bind<R,T,T2>
   */
  template<typename T>
  class BindAdapter0_mem<void, T>
  {
    void (T::*m)();
    T* t;
  public:
    template<typename T2>
    BindAdapter0_mem(void (T::*m)(), T2* t) : m(m), t(t) { }

    void operator()() { (t->*m)(); }

    template <typename A1>
    void operator()(A1 a1) { (t->*m)(); }

    template <typename A1, typename A2>
    void operator()(A1 a1, A2 a2) { (t->*m)(); }

    template <typename A1, typename A2, typename A3>
    void operator()(A1 a1, A2 a2, A3 a3) { (t->*m)(); }

    template <typename A1, typename A2, typename A3, typename A4>
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4) { (t->*m)(); }
  };

  /**
   * Creates a @ref BindAdapter0_mem<R,T> deducing the R and T through
   * @a m and @a t arguments.
   */
  template<typename R, typename T, typename T2>
  BindAdapter0_mem<R, T>
  Bind(R (T::*m)(), T2* t)
  {
    return BindAdapter0_mem<R, T>(m, t);
  }

  //////////////////////////////////////////////////////////////////////

  /**
   * A class to adapt a callable function or @e functor with
   * @c "ReturnType fn(B1)" signature to another signature with the same
   * @c ReturnType.
   *
   * Example: 
   * @code
   * int fn(double x) { ... }
   * ...
   * // R=int, F=int (*)(double), B1=double
   * BindAdapter1_fun<int, int (*)(double), double> newfn(&fn, 5.5);
   * newfn();
   * newfn(1);          // these arguments are ignored,
   * newfn(1, 2);       // each of these calls just invoke fn(5.5)
   * newfn(1, 2, 3);
   * newfn("Hello");
   * @endcode
   *
   * @see BindAdapter0_fun<void,F>, Bind<R,F>
   */
  template<typename R, typename F, typename B1>
  class BindAdapter1_fun
  {
    F f;
    B1 b1;
  public:
    BindAdapter1_fun(const F& f, B1 b1) : f(f), b1(b1) { }

    R operator()() { return f(b1); }

    template<typename A1>
    R operator()(A1 a1) { return f(b1); }

    template<typename A1, typename A2>
    R operator()(A1 a1, A2 a2) { return f(b1); }

    template<typename A1, typename A2, typename A3>
    R operator()(A1 a1, A2 a2, A3 a3) { return f(b1); }

    template<typename A1, typename A2, typename A3, typename A4>
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4) { return f(b1); }
  };

  /**
   * Specialization of @ref BindAdapter1_fun<R,F,B1> for R=void.
   * 
   * @see Bind<R,F,B1>
   */
  template<typename F, typename B1>
  class BindAdapter1_fun<void, F, B1>
  {
    F f;
    B1 b1;
  public:
    BindAdapter1_fun(const F& f, B1 b1) : f(f), b1(b1) { }

    void operator()() { f(b1); }

    template<typename A1>
    void operator()(A1 a1) { f(b1); }

    template<typename A1, typename A2>
    void operator()(A1 a1, A2 a2) { f(b1); }

    template<typename A1, typename A2, typename A3>
    void operator()(A1 a1, A2 a2, A3 a3) { f(b1); }

    template<typename A1, typename A2, typename A3, typename A4>
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4) { f(b1); }
  };

  /**
   * Creates a @ref BindAdapter1_fun<R,F,B1> deducing F and B1 through
   * @a f and @a b1 arguments. The return type R can't be deduced, so you
   * have to specified it as a template parameter.
   *
   * Example:
   * @code
   * int fn(double x) { ... }
   * ...
   * Bind<int>(&fn, 5.5)(1, "Hello"); // call fn(5.5) ignoring arguments
   * @endcode
   */
  template<typename R, typename F, typename B1>
  BindAdapter1_fun<R, F, B1>
  Bind(const F& f, B1 b1)
  {
    return BindAdapter1_fun<R, F, B1>(f, b1);
  }

  //////////////////////////////////////////////////////////////////////

  /**
   * A class to adapt a method of T class with
   * @c "ReturnType T::method(B1)" signature to another signature
   * with the same @c ReturnType.
   *
   * Example: 
   * @code
   * class MyClass {
   * public:
   *   int method(double x) { ... }
   * };
   * ...
   * MyClass instance;
   * // R=int, T=MyClass, B1=int
   * BindAdapter1_mem<int, MyClass, double> newfn(&MyClass::method, &instance, 5.5);
   * newfn();
   * newfn(1);          // these arguments are ignored,
   * newfn(1, 2);       // each of these calls just invoke "instance.method(5.5)"
   * newfn(1, 2, 3);
   * newfn("Hello");
   * @endcode
   * 
   * @see BindAdapter1_mem<void,T,B1>, Bind<R,T,T2,B1>
   */
  template<typename R, typename T, typename B1>
  class BindAdapter1_mem
  {
    R (T::*m)(B1);
    T* t;
    B1 b1;
  public:
    template<typename T2>
    BindAdapter1_mem(R (T::*m)(B1), T2* t, B1 b1) : m(m), t(t), b1(b1) { }

    R operator()() { return (t->*m)(b1); }

    template <typename A1>
    R operator()(A1 a1) { return (t->*m)(b1); }

    template <typename A1, typename A2>
    R operator()(A1 a1, A2 a2) { return (t->*m)(b1); }

    template <typename A1, typename A2, typename A3>
    R operator()(A1 a1, A2 a2, A3 a3) { return (t->*m)(b1); }

    template <typename A1, typename A2, typename A3, typename A4>
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4) { return (t->*m)(b1); }
  };

  /**
   * Specialization of @link Vaca::BindAdapter1_mem BindAdapter1_mem<R,T,B1>@endlink for R=void.
   *
   * @see Bind<R,T,T2,B1>
   */
  template<typename T, typename B1>
  class BindAdapter1_mem<void, T, B1>
  {
    void (T::*m)(B1);
    T* t;
    B1 b1;
  public:
    template<typename T2>
    BindAdapter1_mem(void (T::*m)(B1), T2* t, B1 b1) : m(m), t(t), b1(b1) { }

    void operator()() { (t->*m)(b1); }

    template <typename A1>
    void operator()(A1 a1) { (t->*m)(b1); }

    template <typename A1, typename A2>
    void operator()(A1 a1, A2 a2) { (t->*m)(b1); }

    template <typename A1, typename A2, typename A3>
    void operator()(A1 a1, A2 a2, A3 a3) { (t->*m)(b1); }

    template <typename A1, typename A2, typename A3, typename A4>
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4) { (t->*m)(b1); }
  };

  /**
   * Creates a @ref BindAdapter1_mem<R,T,B1> deducing the R, T, B1 through
   * @a m, @a t and @a b1 arguments.
   */
  template<typename R, typename T, typename T2, typename B1>
  BindAdapter1_mem<R, T, B1>
  Bind(R (T::*m)(B1), T2* t, B1 b1)
  {
    return BindAdapter1_mem<R, T, B1>(m, t, b1);
  }

  //////////////////////////////////////////////////////////////////////

  /**
   * A class to adapt a callable function or @e functor with
   * @c "ReturnType fn(B1,B2)" signature to another signature with the same
   * @c ReturnType.
   *
   * Example: 
   * @code
   * int fn(double x, int y) { ... }
   * ...
   * // R=int, F=int (*)(double, int), B1=double, B2=int
   * BindAdapter1_fun<int, int (*)(double, int),
   *                  double, int> newfn(&fn, 5.5, 8);
   * newfn();
   * newfn(1);          // these arguments are ignored,
   * newfn(1, 2);       // each of these calls just invoke fn(5.5, 8)
   * newfn(1, 2, 3);
   * newfn("Hello");
   * @endcode
   *
   * @see BindAdapter2_fun<void,F,B1,B2>, Bind<R,F,B1,B2>
   */
  template<typename R, typename F, typename B1, typename B2>
  class BindAdapter2_fun
  {
    F f;
    B1 b1;
    B2 b2;
  public:
    BindAdapter2_fun(const F& f, B1 b1, B2 b2) : f(f), b1(b1), b2(b2) { }

    R operator()() { return f(b1, b2); }

    template<typename A1>
    R operator()(A1 a1) { return f(b1, b2); }

    template<typename A1, typename A2>
    R operator()(A1 a1, A2 a2) { return f(b1, b2); }

    template<typename A1, typename A2, typename A3>
    R operator()(A1 a1, A2 a2, A3 a3) { return f(b1, b2); }

    template<typename A1, typename A2, typename A3, typename A4>
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4) { return f(b1, b2); }
  };

  /**
   * Specialization of @link Vaca::BindAdapter2_fun BindAdapter2_fun<R,F,B1,B2>@endlink for R=void.
   * 
   * @see Bind<R,F,B1,B2>
   */
  template<typename F, typename B1, typename B2>
  class BindAdapter2_fun<void, F, B1, B2>
  {
    F f;
    B1 b1;
    B2 b2;
  public:
    BindAdapter2_fun(const F& f, B1 b1, B2 b2) : f(f), b1(b1), b2(b2) { }

    void operator()() { f(b1, b2); }

    template<typename A1>
    void operator()(A1 a1) { f(b1, b2); }

    template<typename A1, typename A2>
    void operator()(A1 a1, A2 a2) { f(b1, b2); }

    template<typename A1, typename A2, typename A3>
    void operator()(A1 a1, A2 a2, A3 a3) { f(b1, b2); }

    template<typename A1, typename A2, typename A3, typename A4>
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4) { f(b1, b2); }
  };

  /**
   * Creates a @ref BindAdapter2_fun<R,F,B1,b"> deducing F, B1 and B2 through
   * @a f, @a b1 and @a b2 arguments. The return type R can't be deduced, so you
   * have to specified it as a template parameter.
   *
   * Example:
   * @code
   * int fn(double x, int y) { ... }
   * ...
   * Bind<int>(&fn, 5.5, 8)(1, "Hello"); // call fn(5.5, 8) ignoring arguments
   * @endcode
   */
  template<typename R, typename F, typename B1, typename B2>
  BindAdapter2_fun<R, F, B1, B2>
  Bind(const F& f, B1 b1, B2 b2)
  {
    return BindAdapter2_fun<R, F, B1, B2>(f, b1, b2);
  }

  //////////////////////////////////////////////////////////////////////

  /**
   * A class to adapt a method of T class with
   * @c "ReturnType T::method(B1,B2)" signature to another signature
   * with the same @c ReturnType.
   *
   * Example: 
   * @code
   * class MyClass {
   * public:
   *   int method(double x, int y) { ... }
   * };
   * ...
   * MyClass instance;
   * // R=int, T=MyClass, B1=double, B2=int
   * BindAdapter2_mem<int, MyClass,
   *                  double, int> newfn(&MyClass::method, &instance, 5.5, 8);
   * newfn();
   * newfn(1);       // these arguments are ignored,
   * newfn(1, 2);    // each of these calls just invoke "instance.method(5.5, 8)"
   * newfn(1, 2, 3);
   * newfn("Hello");
   * @endcode
   * 
   * @see BindAdapter2_mem<void,T,B1,B2>, Bind<R,T,T2,B1,B2>
   */
  template<typename R, typename T, typename B1, typename B2>
  class BindAdapter2_mem
  {
    R (T::*m)(B1, B2);
    T* t;
    B1 b1;
    B2 b2;
  public:
    template<typename T2>
    BindAdapter2_mem(R (T::*m)(B1, B2), T2* t, B1 b1, B2 b2) : m(m), t(t), b1(b1), b2(b2) { }

    R operator()() { return (t->*m)(b1, b2); }

    template<typename A1>
    R operator()(A1 a1) { return (t->*m)(b1, b2); }

    template<typename A1, typename A2>
    R operator()(A1 a1, A2 a2) { return (t->*m)(b1, b2); }

    template<typename A1, typename A2, typename A3>
    R operator()(A1 a1, A2 a2, A3 a3) { return (t->*m)(b1, b2); }

    template<typename A1, typename A2, typename A3, typename A4>
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4) { return (t->*m)(b1, b2); }
  };

  /**
   * Specialization of @link Vaca::BindAdapter2_mem BindAdapter2_mem<R,T,B1,B2>@endlink for R=void.
   *
   * @see Bind<R,T,T2,B1,B2>
   */
  template<typename T, typename B1, typename B2>
  class BindAdapter2_mem<void, T, B1, B2>
  {
    void (T::*m)(B1, B2);
    T* t;
    B1 b1;
    B2 b2;
  public:
    template<typename T2>
    BindAdapter2_mem(void (T::*m)(B1, B2), T2* t, B1 b1, B2 b2) : m(m), t(t), b1(b1), b2(b2) { }

    void operator()() { (t->*m)(b1, b2); }

    template<typename A1>
    void operator()(A1 a1) { (t->*m)(b1, b2); }

    template<typename A1, typename A2>
    void operator()(A1 a1, A2 a2) { (t->*m)(b1, b2); }

    template<typename A1, typename A2, typename A3>
    void operator()(A1 a1, A2 a2, A3 a3) { (t->*m)(b1, b2); }

    template<typename A1, typename A2, typename A3, typename A4>
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4) { (t->*m)(b1, b2); }
  };

  /**
   * Creates a @ref BindAdapter2_mem<R,T,B1,B2> deducing the R, T, B1, B2 through
   * @a m, @a t, @a b1 and @a b2 arguments.
   */
  template<typename R, typename T, typename T2, typename B1, typename B2>
  BindAdapter2_mem<R, T, B1, B2>
  Bind(R (T::*m)(B1, B2), T2* t, B1 b1, B2 b2)
  {
    return BindAdapter2_mem<R, T, B1, B2>(m, t, b1, b2);
  }

  //////////////////////////////////////////////////////////////////////

} // namespace Vaca

#endif
