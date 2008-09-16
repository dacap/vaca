namespace Vaca {

/**

@page page_tn_009 TN009: Reflection

Vaca supports reflection of @c WM_COMMAND, @c WM_NOTIFY and @c WM_DRAWITEM
using the #Vaca::Widget methods:

@li @link Vaca::Widget#onReflectedCommand Widget::onReflectedCommand@endlink
@li @link Vaca::Widget#onReflectedNotify Widget::onReflectedNotify@endlink
@li @link Vaca::Widget#onReflectedDrawItem Widget::onReflectedDrawItem@endlink

When a widget (e.g. @link Vaca::Button Button@endlink) sends a notification
(e.g. @c BN_CLICKED) to its parent widget (e.g. @link Vaca::Frame Frame@endlink),
the @c Frame processes the @c WM_COMMAND and extracts the handle of the button
from the @c lParam (a @c HWND). Then uses the @link Vaca::Widget::fromHandle Widget::fromHandle@endlink
to get the @c "Button*" pointer, and finally calls
@link Vaca::ButtonBase::onReflectedCommand ButtonBase::onReflectedCommand@endlink(...).

@see @link Vaca::Widget#wndProc Widget::wndProc@endlink,
     @link Vaca::Widget#fromHandle Widget::fromHandle@endlink.

*/

}
