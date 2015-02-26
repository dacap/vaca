namespace vaca {

/**

@page page_tn_002 TN002: CreateWindow and DestroyWindow (Win32)

@li @ref page_tn_002_hwnd_creation
@li @ref page_tn_002_hwnd_life_time
@li @ref page_tn_002_override_create_handle


@section page_tn_002_hwnd_creation Where are HWND created?

All @msdn{HWND} are created inside
@ref Widget#createHandle method using the @msdn{CreateWindowEx} routine.
But @c createHandle is privated, you can't use it (although you can
override it), the only one that calls this method is
@ref Widget#create. Its main task
is to setup the @ref Widget#m_handle member
and to do the common Win32 subclassing:
@code
m_baseWndProc = (WNDPROC)
  SetWindowLongPtr(m_handle,
		   GWLP_WNDPROC,
		   (LONG_PTR)getGlobalWndProc());
@endcode
See @ref Widget#subClass for more information.

@image html CreateDestroyWindows.png


@section page_tn_002_hwnd_life_time HWND Life Time

All HWND have the same life time that its container
Widget. For example the next code shows you when a HWND
is created and destroyed:
@code
  Frame frame(...);
  {
    Button myButton("OK", &frame); // the HWND is created
    ...
  } // the Button's destructor calls DestroyWindow
  ...
  // at this point myButton (and its HWND) doesn't exist anymore
@endcode


@section page_tn_002_override_create_handle Overriding Widget::createHandle Method

This shouldn't be your case, but if you want to make something like
@ref MdiClient#createHandle, in other words,
to make your customized @msdn{CreateWindowEx} call, you can override
@ref Widget#createHandle method.

Remember that you must to call
@ref Widget#create inside the constructor of
your own class, and give WidgetClassName#None as @a className in
@link Widget#Widget Widget's constructor@endlink.

Here is an example that shows you how to override @c createHandle:
@code
class MyWidgetClass : public WidgetClass
{
public:
  // This returns the name of your class for the Win32 API
  static MyWidgetClass getClassName()
  { return MyWidgetClass(L"Vaca.MyWidgetClass"); }
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
    return CreateWindowEx(style.extended, className, L"",
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

The @ref page_examples_webcam overrides the @c createHandle method
to use a @msdn{HWND} created from other routine that is not @msdn{CreateWindowEx}.

@see @ref page_tn_001, Widget#Widget, Widget#~Widget

*/

}
