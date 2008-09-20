namespace Vaca {

/**

@page page_tn_010 TN010: Auto-deleted objects

Vaca deletes some objects automatically that you create. This means
that there are cases where you create the object using the @c new
operator, but you never deletes it because Vaca makes that job for you.

Here is a list of some classes/methods with this behavior:

- Widget#setLayout
- Widget#setConstraint

@todo the next of the list is deprecated, need an update

- Frame#setMenuBar
- Menu#add(const String&, Keys#Type),
  * Menu#addSeparator(),
  * Menu#insert(const String&, int),
  * Menu#insertSeparator() deletes MenuItem's automatically.
- Frame#addCommand
- Frame#addDockArea
- MdiClient#setMdiClient

*/

}
