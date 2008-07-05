// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008 David A. Capello
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

#ifndef VACA_SLOT_HPP_INCLUDED
#define VACA_SLOT_HPP_INCLUDED

#include "Vaca/base.hpp"

namespace Vaca {

  //////////////////////////////////////////////////////////////////////
  // Slot0

  template<typename R>
  class Slot0
  {
  public:
    Slot0() { }
    Slot0(const Slot0& s) { (void)s; }
    virtual ~Slot0() { }
    virtual R operator()() = 0;
    virtual Slot0* clone() const = 0;
  };

  //////////////////////////////////////////////////////////////////////
  // Slot0_call - hold a T instance and use the function call operator

  template<typename R, typename T>
  class Slot0_call : public Slot0<R>
  {
    T m_func;
  public:
    Slot0_call(const T& t) : m_func(t) { }
    Slot0_call(const Slot0_call& s) : Slot0<R>(s)
				    , m_func(s.m_func) { }
    ~Slot0_call() { }
    R operator()() { return m_func(); }
    Slot0_call* clone() const { return new Slot0_call(*this); }
  };
  
  //////////////////////////////////////////////////////////////////////
  // Slot0_mem - pointer to a member function of the T class

  template<typename R, class T>
  class Slot0_mem : public Slot0<R>
  {
    T* m_instance;
    R (T::*m_method)();
  public:
    Slot0_mem(T* t, R (T::*m)()) : m_instance(t)
				 , m_method(m) { }
    Slot0_mem(const Slot0_mem& s) : Slot0<R>(s)
				  , m_instance(s.m_instance)
				  , m_method(s.m_method) { }
    ~Slot0_mem() { }
    R operator()() { return (m_instance->*m_method)(); }
    Slot0_mem* clone() const { return new Slot0_mem(*this); }
  };

  //////////////////////////////////////////////////////////////////////
  // Slot1

  template<typename R, typename A1>
  class Slot1
  {
  public:
    Slot1() { }
    Slot1(const Slot1& s) { (void)s; }
    virtual ~Slot1() { }
    virtual R operator()(A1 a1) = 0;
    virtual Slot1* clone() const = 0;
  };

  //////////////////////////////////////////////////////////////////////
  // Slot1_call - hold a T instance and use the function call operator

  template<typename R, typename T, typename A1>
  class Slot1_call : public Slot1<R, A1>
  {
    T m_func;
  public:
    Slot1_call(const T& t) : m_func(t) { }
    Slot1_call(const Slot1_call& s) : Slot1<R, A1>(s)
				    , m_func(s.m_func) { }
    ~Slot1_call() { }
    R operator()(A1 a1) { return m_func(a1); }
    Slot1_call* clone() const { return new Slot1_call(*this); }
  };
  
  //////////////////////////////////////////////////////////////////////
  // Slot1_mem - pointer to a member function of the T class

  template<typename R, class T, typename A1>
  class Slot1_mem : public Slot1<R, A1>
  {
    T* m_instance;
    R (T::*m_method)(A1);
  public:
    Slot1_mem(T* t, R (T::*m)(A1)) : m_instance(t)
				   , m_method(m) { }
    Slot1_mem(const Slot1_mem& s) : Slot1<R, A1>(s)
				  , m_instance(s.m_instance)
				  , m_method(s.m_method) { }
    ~Slot1_mem() { }
    R operator()(A1 a1) { return (m_instance->*m_method)(a1); }
    Slot1_mem* clone() const { return new Slot1_mem(*this); }
  };

  //////////////////////////////////////////////////////////////////////
  // Slot2

  template<typename R, typename A1, typename A2>
  class Slot2
  {
  public:
    Slot2() { }
    Slot2(const Slot2& s) { (void)s; }
    virtual ~Slot2() { }
    virtual R operator()(A1 a1, A2 a2) = 0;
    virtual Slot2* clone() const = 0;
  };

  //////////////////////////////////////////////////////////////////////
  // Slot2_call - hold a T instance and use the function call operator

  template<typename R, typename T, typename A1, typename A2>
  class Slot2_call : public Slot2<R, A1, A2>
  {
    T m_func;
  public:
    Slot2_call(const T& t) : m_func(t) { }
    Slot2_call(const Slot2_call& s) : Slot2<R, A1, A2>(s)
				    , m_func(s.m_func) { }
    ~Slot2_call() { }
    R operator()(A1 a1, A2 a2) { return m_func(a1, a2); }
    Slot2_call* clone() const { return new Slot2_call(*this); }
  };
  
  //////////////////////////////////////////////////////////////////////
  // Slot2_mem - pointer to a member function of the T class

  template<typename R, class T, typename A1, typename A2>
  class Slot2_mem : public Slot2<R, A1, A2>
  {
    T* m_instance;
    R (T::*m_method)(A1, A2);
  public:
    Slot2_mem(T* t, R (T::*m)(A1, A2)) : m_instance(t)
				       , m_method(m) { }
    Slot2_mem(const Slot2_mem& s) : Slot2<R, A1, A2>(s)
				  , m_instance(s.m_instance)
				  , m_method(s.m_method) { }
    ~Slot2_mem() { }
    R operator()(A1 a1, A2 a2) { return (m_instance->*m_method)(a1, a2); }
    Slot2_mem* clone() const { return new Slot2_mem(*this); }
  };

  //////////////////////////////////////////////////////////////////////

} // namespace Vaca

#endif // VACA_SLOT_HPP_INCLUDED
