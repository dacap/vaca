namespace vaca {

/**

@page page_devel Development Tips

@li @ref page_devel_headers
@li @ref page_devel_static
@li @ref page_devel_dynamic
@li @subpage page_debug
@li @ref page_devel_svn
@li @subpage page_hacking


@section page_devel_headers Header Files

You should include the &lt;vaca/vaca.h&gt; file:
@code
#include <vaca/vaca.h>
@endcode

But if you want to make the compilation faster, you can include the
header file of the class that you use. E.g.:
@code
#include <vaca/Color.h>
void my_routine(const vaca::Color& color)
{
  ...
}
@endcode

Also it's a good idea to include the vaca namespace in .cpp files to
get quick access to all Vaca classes:
@code
using namespace vaca;
@endcode


@section page_devel_static Static Library

If you are going to use the static version of Vaca (non-shared),
remember to compile your application using the @c -DVACA_STATIC flag.


@section page_devel_dynamic Dynamic Library

If you are going to use the dynamic version of Vaca (shared version),
you have to distribute the @c vaca.dll with your executable file.

Also, remember to compile your source code @b without the
@c -DVACA_STATIC flag. When VACA_STATIC is not defined, &lt;vaca/base.h&gt;
defines VACA_DLL as @c __declspec(dllimport) to import each class from the @c DLL.


@section page_devel_svn SVN Repository

You can access to the latest version of Vaca's source code under development using
<a href="http://subversion.tigris.org/">Subversion</a>:

@code
svn co https://vaca.svn.sourceforge.net/svnroot/vaca/trunk vaca
@endcode

Or you can <a href="http://vaca.svn.sourceforge.net/viewvc/vaca/trunk">browse the SVN
repository</a> with your web-browser.

*/

}
