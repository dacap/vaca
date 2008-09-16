namespace Vaca {

/**

@page page_tn_002 TN002: CreateWindow and DestroyWindow

@image html CreateDestroyWindows.png

@section tn002_hwnd_creation Where are HWNDs created?

All HWNDs are created inside
@link Vaca::Widget#createHandle Widget::createHandle@endlink
method using the Win32 API routine @c CreateWindowEx. But this
method is privated, you can't call it (although you can
override it), the only one that calls this method is
@link Vaca::Widget#create Widget::create@endlink. Its main task
is to setup the @a m_HWND member of @link Vaca::Widget Widget@endlink
instances and to do the classic Win32 subclassing (setting the
GWL_WNDPROC property):
@code
baseWndProc = (WNDPROC)
  SetWindowLongPtr(m_HWND,
		   GWLP_WNDPROC,
		   (LONG_PTR) getGlobalWndProc());
@endcode
See @link Vaca::Widget#subClass() Widget::subClass()@endlink
for more information.

@section tn002_hwnd_life_time HWND life time

All windows/HWNDs have the same life time that its container
Widget. That is if we create a button:
@code
  Frame frame(...)
  {
    Button myButton("OK", frame); // the HWND is created
    ...
  }
  // here myButton (and its HWND) doesn't exist
@endcode

@section tn002_override_create_handle You can override createHandle

This shouldn't be your case, but if you want to make something like
@link Vaca::MdiClient#createHandle MdiClient::createHandle@endlink, it is,
to make your customized @c CreateWindowEx call, you can override @c createHandle,
but remember that you must to call
@link Vaca::Widget#create create@endlink inside the constructor of
your own class, and give WidgetClassName#None as @a className in
@link Vaca::Widget#Widget Widget's constructor@endlink.
This is a little example:
@code
class MyWidgetClass : public WidgetClass
{
public:
  // This returns the name of your class for the Win32's API
  static MyWidgetClass getClassName()
  { return MyWidgetClass("Vaca.MyWidgetClass"); }
};

class MyWidget : public Register<MyWidgetClass>
	       , public Widget
{
public:

  MyWidget(Widget* parent)
    : Widget(WidgetClassName::None, parent, NoStyle)
  {
    create(MyWidgetClass::getClassName(),
	   parent, ChildStyle);
  }

protected:

  // customized call to CreateWindowEx
  virtual HWND createHandle(LPCTSTR className,
                            Widget* parent,
                            Style style)
  {
    return CreateWindowEx(style.extended, className, _T(""),
			  style.regular,
			  CW_USEDEFAULT, CW_USEDEFAULT,
			  CW_USEDEFAULT, CW_USEDEFAULT,
			  parent ? parent->getHandle(): (HWND)NULL,
			  (HMENU)NULL,
			  Application::getHandle(),
			  reinterpret_cast<LPVOID>(this));
  }
};
@endcode

@see @ref page_tn_001,
     @link Vaca::Widget#Widget Widget()@endlink,
     @link Vaca::Widget#~Widget ~Widget()@endlink.

*/

}
