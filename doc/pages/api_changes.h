namespace Vaca {

/**

@page page_api_changes API Changes

@li @ref page_api_changes_0_0_8


@section page_api_changes_0_0_8 API Changes from 0.0.7 to 0.0.8

@beginChanges
@beginChange{String - Now it is a std::wstring}

There were problems using the old String (a derived class of
std::basic_string<Character>). So now String is just a std::wstring.
This has two implications:
@li You cannot directly create a String from an ASCII string. So if you
    were using String("Hi") now you have to change it to String(L"Hi").
    Also you can use the convert_to function to convert an
    std::string to a std::wstring (String).
@li Now Unicode is not optional.

The old string class is inside the "Vaca/backward.h" under the name
of _String. It will be completelly removed in the next version of Vaca.
@endChange
@beginChange{ChildStyle - Removed}
Now ChildStyle is deprecated. You should replace all places
where you were using ChildStyle with Widget::Styles::Visible
(yes, Visible, it is not a typo, because ChildStyle was
WS_CHILD | WS_VISIBLE).

The WS_CHILD style is automatically controlled by Vaca if you
specify or not a parent when create a Widget. You don't have to
worry about it.
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
@beginChange{Thread::setPriority - Renamed/Signature Changed}
Before the "priority" argument was from Win32 API:
@code
void Thread::setPriority(int priority);
Where:
  priority = THREAD_PRIORITY_IDLE
  priority = THREAD_PRIORITY_LOWEST
  priority = THREAD_PRIORITY_BELOW_NORMAL
  priority = THREAD_PRIORITY_NORMAL
  priority = THREAD_PRIORITY_ABOVE_NORMAL
  priority = THREAD_PRIORITY_HIGHEST
  priority = THREAD_PRIORITY_TIME_CRITICAL
@endcode
Now it is:
@code
void Thread::setThreadPriority(ThreadPriority priority);
Where:
  priority = ThreadPriority::Idle
  priority = ThreadPriority::Lowest
  priority = ThreadPriority::Low
  priority = ThreadPriority::Normal
  priority = ThreadPriority::High
  priority = ThreadPriority::Highest
  priority = ThreadPriority::TimeCritical
@endcode
@endChange
@beginChange{onGotFocus/onLostFocus - Renamed}
Now they are onFocusEnter and onFocusLeave (the same for Widget#FocusEnter/Widget#FocusLeave signals).
@endChange
@beginChange{onBeforePosChange/onAfterPosChange - Removed}
Use Widget#onResize, and Frame#onResizing.
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
Dialog dlg(L"", NULL, Dialog::Styles::Default +
                      Frame::Styles::ResizableFrameStyle);
Button ok(L"OK", &dlg, Button::Styles::Default);
Button cancel(L"Cancel", &dlg); // <-- uses the default style
ComboBox box(&dlg, ComboBox::Styles::Editable);
@endcode

You can still use the old style macros including the "Vaca/backward.h"
header file.

@endChange
@endChanges

*/

}
