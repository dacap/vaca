namespace Vaca {

/**

@page page_mess MESS Architecture

MESS stands for Message-Event-Signal/Slot.

Definitions:

@li A @em message is a message received from Windows. E.g. @c WM_KEYUP.
@li An @em event is a virtual method. E.g. @link Widget#onKeyUp onKeyUp@endlink method.
@li A @em signal is a variable member which anybody can connect their callbacks
    E.g. @link Widget#KeyUp KeyUp@endlink signal.
@li A @em slot is a instance of @link Slot0 Slot@endlink that wraps functions,
    function objects (@e functors), or classes' methods.
    They are automatically created/connected to signals when you
    use functions like @link Signal0_base#connect Signal::connect@endlink.

General behavior:

@li A @em message is received from Windows OS. E.g. @c WM_PAINT.
@li A global WndProc (@link Widget#getGlobalWndProc getGlobalWndProc@endlink)
    extracts the @c GWL_USERDATA from the @c HWND that received the message. Note:
    New versions of Vaca stores the Widget pointer in the "VacaAtom" property, see
    @link Widget#fromHandle fromHandle@endlink for more information.
@li In the @c GWL_USERDATA is located a pointer to a @ref Widget.
@li This instance is used to call the @link Widget#wndProc wndProc@endlink
    method to process the message.
@li Depending the message received by @c wndProc, it calls other method (an @em event)
    that can be overridden (like @link Widget#onPaint onPaint@endlink,
    @link Widget#onResize onResize@endlink, etc.).
@li Default implementations of the events fire a @em signal related to that event.
@li The signal can be intercepted by anyone (like a C# delegate). Each connection
    to a signal is called a @link Slot0 slot@endlink.
@li Signals are the only variable members of classes that should be public, and their
    names must begin with a capital letter (like @link Button#Action Action@endlink).

Generally in Vaca we define virtual methods to handle events in the
widget itself, and signals for events that should be intercepted by
the parent window. Maybe in the future, signals will exist for every
event (like events and delegates in .NET framework).

@image html MESS.png

*/

}
