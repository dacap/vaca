namespace vaca {

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
#include "vaca/Bind.h"

int func(int a, int b)
{
  return a + b;
}

int main()
{
  int a = vaca::Bind<int>(&func, 1, 2)(2.3, "Hello", 200);
  std::cout << a << std::endl;
  return 0;
}
@endcode
Prints the value
@code
3
@endcode

Why? There are two things in the example:

@li First we call @em Bind<int>(&func, 1, 2) constructor, which creates
    a BindAdapter object that allocates memory to hold the address of 
    @em func and two integers (1 and 2 in this case).
@li Then we are calling the @em operator()(double, const char*, int) of
    that new BindAdapter. The BindAdapter completelly ignores those
    arguments (2.3, "Hello", 200) and a call to @em func(1, 2) is made
    instead.

Is the same to call BindAdapter::operator() without parameters or
with any kind and number of parameters, they will be ignored anyway.
The BindAdapter object (created with @link vaca::Bind Bind@endlink function)
always will remember the arguments specified in its constructor, and these arguments
will be used in each operator() call.

The following example shows you how to create an adapter directly (without
the help of @link vaca::Bind Bind@endlink function):
@code
#include <iostream>
#include "vaca/Bind.h"

int func(int a, int b)
{
  return a + b;
}

int main()
{
  vaca::BindAdapter2_fun<int, int(*)(int,int), int, int>
    adapter(&func, 1, 2);

  std::cout << adapter() << std::endl;
  std::cout << adapter(2.3) << std::endl;
  std::cout << adapter("Hello") << std::endl;
  std::cout << adapter(200, 2.3) << std::endl;
  return 0;
}
@endcode
It will print:
@code
3
3
3
3
@endcode
Arguments in each call are ignored, just the values passed in the
constructor of BindAdapter2_fun() are used.


@section page_bind_signals How to Use Bind With Signals?


@section page_bind_details Details About Bind

*/

}
