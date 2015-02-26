namespace vaca {

/**

@page page_tn_009 TN009: Reflection

Vaca supports reflection of @c WM_COMMAND, @c WM_NOTIFY and @c WM_DRAWITEM
using the #vaca::Widget methods:

@li @link vaca::Widget#onReflectedCommand Widget::onReflectedCommand@endlink
@li @link vaca::Widget#onReflectedNotify Widget::onReflectedNotify@endlink
@li @link vaca::Widget#onReflectedDrawItem Widget::onReflectedDrawItem@endlink

When a widget (e.g. @link vaca::Button Button@endlink) sends a notification
(e.g. @c BN_CLICKED) to its parent widget (e.g. @link vaca::Frame Frame@endlink),
the @c Frame processes the @c WM_COMMAND and extracts the handle of the button
from the @c lParam (a @c HWND). Then uses the @link vaca::Widget::fromHandle Widget::fromHandle@endlink
to get the @c "Button*" pointer, and finally calls
@link vaca::ButtonBase::onReflectedCommand ButtonBase::onReflectedCommand@endlink(...).

@see @link vaca::Widget#wndProc Widget::wndProc@endlink,
     @link vaca::Widget#fromHandle Widget::fromHandle@endlink.

*/

}
