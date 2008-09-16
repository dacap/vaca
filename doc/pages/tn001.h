namespace Vaca {

/**

@page page_tn_001 TN001: RegisterClass process

Vaca creates HWNDs in C++ constructors, one implication of this is
that the RegisterClass must be called before a
@ref Vaca::Widget "Widget" constructor.

Some libraries, uses a @c create method to really create the
widget (call the CreateWindowEx). I don't like it, mainly because
you can't do things like:

@code
class MyDialog : public Dialog {
  Label m_label;
  Button m_ok;
public:
  MyDialog() : Dialog(...)
             , m_label(...) {
             , m_ok(...) {
    // done
  }
};
@endcode

Generally, the most ugly case is this one:

@code
class MyDialog : public Dialog {
  Label *m_label;
  Button *m_ok;
public:
  MyDialog() {
    m_label = NULL;
    m_ok = NULL;
    ...
  }
  void create(...) {
    m_label = new Label();
    m_label->create(...);
    m_ok = new Button();
    m_ok->create(...);
    ...
  }
};
@endcode

Vaca avoid to do that, and try to make your life "easy" (like the
first example).

Anyway, this has serious implications for the internal Vaca
implementation, making the process to register a Win32 class a
"little" tricky. This is mainly because the
@ref Vaca::Widget::Widget() "constructor of the Vaca::Widget" class creates
the HWND (calls CreateWindowEx), so to create a HWND in the constructor of
a C++ class, we must register the Win32 classes with RegisterClass or
RegisterClassEx before that.

The only way to call RegisterClass @em automatically and before the
C++ class constructor is to inherit first from other class where its
constructor registers the Win32 class because it is called first.
With Vaca::Register and a customized Vaca::WidgetClass, you can
register your own widget class before to contruct it.

@see @ref page_tn_002

*/

}
