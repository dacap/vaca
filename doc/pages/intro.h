namespace Vaca {

/**

@page page_intro Introduction

@li @ref page_intro_whatis
@li @ref page_intro_win32
@li @ref page_intro_objectives
@li @ref page_intro_alternatives


@section page_intro_whatis What is Vaca?

Vaca is a library to develop GUI applications with C++.
It uses templates and STL, and has some special features like
dockable tool bars and layout managers.


@section page_intro_win32 A Win32 Wrapper

Vaca is a wrapper for the Win32 API. What is the Win32 API? You don't
need to know about it, but it is the set of routines that Microsoft
gives you to program applications on Windows. So whenever you see
"Win32" in this manual, you'll know that we are talking about
internals of Windows.

Also when you see "W32" above a word, like @msdn{CreateWindow}, this
means that we are talking about a routine of Win32 API. You can click
in the "W32" to search the term in the
<a href="http://msdn.microsoft.com/">Microsoft Developer Network</a> (MSDN) site.


@section page_intro_objectives Objectives

The following are some of the objectives to achieve with Vaca:
@li Make an intuitive and easy to use API.
@li Make a lightweight library that wraps Win32 API.
@li Wrap so much the Win32 API that the user should not see or know nothing about it.
@li Constructors initialize resources, and windows/controls/widgets are resources. If you do @c "new MyWindow()", then your window should be created.
@li Documentation for every tricky situation, with the solution and examples.
@li @link page_tn Technical Notes@endlink about internal behavior of Vaca.

@see @ref page_tn_002


@section page_intro_alternatives Alternatives

There are a lot of libraries to program GUI applications using C++.
If you have to develop a serious project, I recommend you to use
<a href="http://www.wxwidgets.org/">wxWidgets</a>, but there are
a lot of
@wikipedia{List_of_widget_toolkits, toolkits}
around.

*/

}
