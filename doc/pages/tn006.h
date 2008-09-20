namespace Vaca {

/**

@page page_tn_006 TN006: Deleting a Widget inside its event

@li @ref page_tn_006_problem
@li @ref page_tn_006_solution
@li @ref page_tn_006_example


@section page_tn_006_problem The Problem

There are a problem with signals:
@warning YOU CAN'T DELETE A WIDGET INSIDE AN EVENT WHICH THE WIDGET BY
ITSELF GENERATED.

It means that if a widget's event is activated
(e.g. Widget#onKeyDown), then you can't delete that widget in your own
event handler (or a slot connected to the Widget#KeyDown signal).
The widget must be deleted after the original message is processed
(it is when the widget is not referenced anymore).

@see @ref page_mess


@section page_tn_006_solution The Solution

The solution is simple: Use the #delete_widget function.

@image html DeleteInEvent.png

@see delete_widget


@section page_tn_006_example An Example

See the @ref page_examples_texteditor, it uses the #delete_widget to
delete an #MdiChild when its close button is pushed by the user. I think
that it's the only case where @ref delete_widget could be necessary to use.

*/

}
