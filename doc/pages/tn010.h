namespace Vaca {

/**

@page page_tn_010 TN010: Auto-deleted objects

Vaca deletes some objects automatically that you create. This means
that there are cases where you create the object using the @c new
operator, but you never deletes it because Vaca makes that job for you.

Here is a list of some classes/methods with this behavior:

- Frame#setMenuBar
- Menu#add(MenuItem*)
- CommandsClient#addCommand
- Frame#addDockArea
- MdiClient#setMdiClient

Automatically handled with SmartPtr:

- Widget#setLayout
- Widget#setConstraint

*/

}
