namespace Vaca {

/**

@page page_bind Bind

In this section we talk about the Bind function that is defined in the
Vaca namespace.

@li @ref page_bind_whatis
@li @ref page_bind_signals
@li @ref page_bind_details


@section page_bind_whatis What Bind is For?

Bind adapts functions or methods so they can be connected to
signals. Basically it creates a BindAdapter that converts the
signature of one function to another signature.

In this way, with Bind we can create an object (BindAdapter) that
has the @c operator() overloaded in such a way that can be called
with any parameters.

For example, the following code:
@code
#include <iostream>
#include "Vaca/Bind.h"

int func(int a, int b)
{
  return a+b;
}

int main()
{
  int a = Vaca::Bind<int>(&func, 1, 2)(2.3, "Hello", 200);
  std::cout << a << std::endl;
  return 0;
}
@endcode
Prints the value
@code
3
@endcode

Why? There are two things in the example above:

@li The @em Bind<int>(&func, 1, 2) call which creates a BindAdapter object.
@li A call to the @em operator()(double, const char*, int) of that BindAdapter.

Bind() creates the BindAdapter that allocates memory
to hold the address of @em func and the values 1 and 2.
Then, when the BindAdapter::operator() is used, the parameters
are completelly ignored (@c 2.3, @c "Hello", and @c 200 in this
case) and a call to @em func(1, 2) is made instead.

Is the same to call BindAdapter::operator() without parameters or
with any kind and number of parameters, they will be ignored.


@section page_bind_signals How to Use Bind With Signals?


@section page_bind_details Details About Bind

*/

}
