namespace Vaca {

/**

@page page_devel Development Tips

@li @ref page_devel_headers
@li @ref page_devel_static
@li @ref page_devel_dynamic
@li @subpage page_debug
@li @ref page_devel_svn
@li @subpage page_hacking


@section page_devel_headers Header Files

You should include the &lt;Vaca/Vaca.h&gt; file:
@code
#include <Vaca/Vaca.h>
@endcode

But if you want to make the compilation faster, you can include the
header file of the class that you use. E.g.:
@code
#include <Vaca/String.h>
void my_routine(const Vaca::String& str);
@endcode

Also it's a good idea to include the Vaca namespace to get quick access
to all Vaca classes:
@code
using namespace Vaca;
@endcode


@section page_devel_static Static Library

If you are going to use the static version of Vaca (@c libvaca_s.a),
remember to compiling your application using the @c -DVACA_STATIC flag.


@section page_devel_dynamic Dynamic Library

If you are going to use the dynamic version of Vaca (@c libvaca.a and @c Vaca.dll),
you have to distribute the @c Vaca.dll with your executable file.

Also, remember to compiling your source code @b without the
@c -DVACA_STATIC flag. When VACA_STATIC is not defined, &lt;Vaca/base.h&gt;
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
