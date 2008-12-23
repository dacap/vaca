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
string instance used to call the method.

@endChange
@beginChange{VACA_MIN/MAX/CLAMP - Renamed}

Now the three macros VACA_MIN, VACA_MAX, and VACA_CLAMP where replaced
by Vaca::min_value, Vaca::max_value and Vaca::clamp_value inline functions.

Include "Vaca/backward.h" header file if you were using those macros.

@endChange
@beginChange{Spinner::getEdit - Renamed}

Spinner::getEdit renamed to Spinner::getTextEdit.

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
@beginChange{About Styles}

All style stuff had change. Before there were a lot of macros to
define styles for each widget (e.g., ButtonStyle, RadioButtonStyle,
etc.).  Now each class has constants for their supported styles.

If you were using something like this:
@code
Dialog dlg("", NULL, DialogStyle + ResizableFrameStyle);
Button ok("OK", &dlg, ButtonStyle);
Button cancel("Cancel", &dlg); // <-- uses the default style
ComboBox box(&dlg, EditComboBoxStyle);
@endcode

Now you should to change to:
@code
Dialog dlg("", NULL, Dialog::Styles::Default +
                     Frame::Styles::ResizableFrameStyle);
Button ok("OK", &dlg, Button::Styles::Default);
Button cancel("Cancel", &dlg); // <-- uses the default style
ComboBox box(&dlg, ComboBox::Styles::Editable);
@endcode

You can still use the old style macros including the "Vaca/backward.h"
header file.

@endChange
@beginChange{New Classes/Methods}

@li String::encodeUrl
@li String::decodeUrl
@li Thread::sleep
@li Added new header file "Vaca/ScrollEvent.h"
@li New widgets: SplitBar

@endChange
@endChanges

*/

}
