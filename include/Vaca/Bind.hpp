// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

  template<typename R, typename F>
  struct BindAdapter0_fun
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

  template<typename F>
  struct BindAdapter0_fun<void, F>
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

  template<typename R, typename F>
  BindAdapter0_fun<R, F>
  Bind(const F& f)
  {
    return BindAdapter0_fun<R, F>(f);
  }

  //////////////////////////////////////////////////////////////////////

  template<typename R, typename T>
  struct BindAdapter0_mem
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

  template<typename T>
  struct BindAdapter0_mem<void, T>
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

  template<typename R, typename T, typename T2>
  BindAdapter0_mem<R, T>
  Bind(R (T::*m)(), T2* t)
  {
    return BindAdapter0_mem<R, T>(m, t);
  }

  //////////////////////////////////////////////////////////////////////

  template<typename R, typename F, typename B1>
  struct BindAdapter1_fun
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

  template<typename F, typename B1>
  struct BindAdapter1_fun<void, F, B1>
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

  template<typename R, typename F, typename B1>
  BindAdapter1_fun<R, F, B1>
  Bind(const F& f, B1 b1)
  {
    return BindAdapter1_fun<R, F, B1>(f, b1);
  }

  //////////////////////////////////////////////////////////////////////

  template<typename R, typename T, typename B1>
  struct BindAdapter1_mem
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

  template<typename T, typename B1>
  struct BindAdapter1_mem<void, T, B1>
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

  template<typename R, typename T, typename T2, typename B1>
  BindAdapter1_mem<R, T, B1>
  Bind(R (T::*m)(B1), T2* t, B1 b1)
  {
    return BindAdapter1_mem<R, T, B1>(m, t, b1);
  }

  //////////////////////////////////////////////////////////////////////

  template<typename R, typename F, typename B1, typename B2>
  struct BindAdapter2_fun
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

  template<typename F, typename B1, typename B2>
  struct BindAdapter2_fun<void, F, B1, B2>
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

  template<typename R, typename F, typename B1, typename B2>
  BindAdapter2_fun<R, F, B1, B2>
  Bind(const F& f, B1 b1, B2 b2)
  {
    return BindAdapter2_fun<R, F, B1, B2>(f, b1, b2);
  }

  //////////////////////////////////////////////////////////////////////

  template<typename R, typename T, typename B1, typename B2>
  struct BindAdapter2_mem
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

  template<typename T, typename B1, typename B2>
  struct BindAdapter2_mem<void, T, B1, B2>
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
  };

  template<typename R, typename T, typename T2, typename B1, typename B2>
  BindAdapter2_mem<R, T, B1, B2>
  Bind(R (T::*m)(B1, B2), T2* t, B1 b1, B2 b2)
  {
    return BindAdapter2_mem<R, T, B1, B2>(m, t, b1, b2);
  }

  //////////////////////////////////////////////////////////////////////

} // namespace Vaca

#endif
