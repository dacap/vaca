namespace vaca {

/**

@page page_tn_011 TN011: Layout Managers

When you use Widget#setLayout or Widget#setConstraint, you
must to specify a pointer to a Layout or a Constraint respectively.
These pointers are SmartPtr, they are deleted automatically when
they are not referenced anymore (generally when the widget is destroyed).

Example:
@code
{
  LayoutPtr longLived(new ClientLayout());
  {
    Frame frame(...);
    frame.setLayout(longLived);
  }
  // Here longLived still exists (was not destroyed by ~Frame)
  ...
} // Here longLived is automatically destroyed because there aren't
  // more SmartPtr pointing to it
@endcode

Example of how a raw pointer is converted to a SmartPtr in the
same setLayout call (you should avoid raw pointer to layouts/constraints):
@code
Layout* noSmart = new ClientLayout(); // <-- this is not a smart pointer
...
{
  Frame frame(...);
  frame.setLayout(new BoxLayout(...)); // Here one smart pointer is created
  frame.setLayout(noSmart);    // Here BoxLayout is destroyed automatically
                               // and noSmart is converted to a SmartPtr!
}
...
// here noSmart doesn't exist anymore (was automatically deleted
// in the Frame destructor)
@endcode

@warning You cannot use layouts in stack:
@code
  {
    Frame frame(...);
    ClientLayout layout;      // <-- a layout in stack
    frame.setLayout(&layout); // <-- here we're making a SmartPtr to stack!!!
    ...
  } // <-- runtime error: double deletion here
@endcode

@see Layout, Constraint, Widget#setLayout, Widget#setConstraint,
     LayoutPtr, ConstraintPtr

*/

}
