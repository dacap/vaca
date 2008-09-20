namespace Vaca {

/**

@page page_tn_013 TN013: All MenuItems must have their own ID

Every @ref MenuItem that you create must have
its own @ref CommandId. See the @c Undo example
to see how to create a @ref Menu where its items change
over time.

This also is true if you don't use commands. For example, if you
override the @ref MenuItem#onAction to control the actions, the items
need a unique ID anyway.

*/

}
