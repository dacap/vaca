namespace vaca {

/**

@page page_tn_001 TN001: RegisterClass process (Win32)

Vaca creates HWNDs in C++ constructors. One implication of this is
that the @msdn{RegisterClass} must be called before the @ref Widget
constructor.

Other libraries use a @c Create method to really create the widget, it
is calling @msdn{CreateWindowEx}. I don't like it, mainly because you
have to do something like this (in the worse scenario):

@code
class MyDialog : public FooDialog {
  Label* label;
  Button* ok;
public:
  MyDialog() {
    label = NULL;
    ok = NULL;
    ...
  }
  ~MyDialog() {
    delete label;
    delete ok;
    ...
  }
  void Create(...) {
    label = new FooLabel();
    label->Create(...);
    ok = new FooButton();
    ok->Create(...);
    ...
  }
};
@endcode

With that API you have to create two times everything: the first time using the
constructor (@c new @c Foo), and then using the @c Create method.
But with Vaca you create all with constructors, there aren't a @c Create
method or things like that. Also this is better for @wikipedia{Resource_acquisition_is_initialization,exception handling}.

Using Vaca you can write code like this:

@code
class MyDialog : public Dialog {
  Label label;
  Button ok;
public:
  MyDialog() : Dialog(...)
             , label(...) {
             , ok(...) {
    // done
  }
};
@endcode

It looks great. But this has serious implications for the internal Vaca
implementation, making the process to register a Win32 class a
"little" tricky.

The problem is this:
@li @ref Widget#Widget "Constructor of Widget" creates
the @msdn{HWND} calling @msdn{CreateWindowEx},
@li to create a @msdn{HWND} we have to register the Win32 classes
    with @msdn{RegisterClass} or @msdn{RegisterClassEx} before,
@li but we are in a C++ class constructor!

The only way to call @msdn{RegisterClass} automatically and before
a constructor is to inherit first from another class which calls @msdn{RegisterClass}.
This is the reason, for example, that a Frame has multiple inheritance:
@li from Register<FrameClass>, and
@li from Widget.

Register class calls @msdn{RegisterClass} and Widget calls @msdn{CreateWindowEx}.

@see @ref page_tn_002

*/

}
