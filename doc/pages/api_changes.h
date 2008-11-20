namespace Vaca {

/**

@page page_api_changes API Changes

@li @ref page_api_changes_0_0_8


@section page_api_changes_0_0_8 API Changes from 0.0.7 to 0.0.8

@beginChanges
@beginChange{String::addPathComponent - Semantic change}

Before:
@code
String String::addPathComponent(const String& component) const;
@endcode
Now:
@code
void String::addPathComponent(const String& component);
@endcode

Now String::addPathComponent adds the specified path-component to the
string instance used to call the method. The returned reference is a
"return *this".

@endChange
@beginChange{VACA_MIN/MAX/CLAMP - Renamed}

Now the three macros VACA_MIN, VACA_MAX, and VACA_CLAMP where replaced
by Vaca::min_value, Vaca::max_value and Vaca::clamp_value inline functions.

@endChange
@beginChange{ToolSet::loadStandardImageList - Removed}

This method was removed (it was too much Win32 specific). Also
ToolBar::loadStandardImageList was removed.

@endChange
@beginChange{Bix() - Signature Changed}
Old signature:
@code
Bix::Bix(Widget* container, int flags, int matrixColumns)
@endcode
New signature:
@code
Bix::Bix(int flags, int matrixColumns)
@endcode
@endChange
@beginChange{onScroll - Signature Changed}

Old signature:
@code
virtual void Widget::onScroll(Orientation orientation, int code);
@endcode

New signature:
@code
virtual void Widget::onScroll(ScrollEvent& ev);
...
Orientation orientation = ev.getOrientation();
ScrollRequest code = ev.getRequest();
@endcode

@ref ScrollRequest is a new enumeration to wrap Win32 constants.

@endChange
@beginChange{New Stuff}

@li String::encodeUrl
@li String::decodeUrl
@li Thread::sleep
@li Added new header file "Vaca/ScrollEvent.h"
@li New widgets: SplitBar

@endChange
@endChanges

*/

}
