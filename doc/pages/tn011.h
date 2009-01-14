namespace Vaca {

/**

@page page_tn_011 TN011: Layout Managers

When you use @link Vaca::Widget#setLayout setLayout@endlink or
@link Vaca::Widget#setConstraint setConstraint@endlink, you must to specify a
pointer to a @link Vaca::Layout Layout@endlink or a
@link Vaca::Constraint Constraint@endlink respectively. These pointers are
SmartPtr, they are deleted automatically when the widget is destroyed.

@code
  {
    Layout* betterLayout = new MyBetterLayout();
    ...
    {
      Frame frame(...);
      frame.setLayout(new MyLayout(...)); // <-- Here the smart pointer is created
      frame.setLayout(betterLayout);      // <-- MyLayout is destroyed automatically
    }
    ...
    // here betterLayout doesn't exist (was automatically deleted
    // in the Frame destructor)
  }
@endcode

If you want to make @c betterLayout to live more time than @c frame,
you can use a SmartPtr:

@code
  {
    LayoutPtr betterLayout(new MyBetterLayout());
    ...
    {
      Frame frame(...);
      frame.setLayout(betterLayout);
    }
    ...
    // here betterLayout still exists
  }
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

@see Layout, Constraint, Widget#setLayout, Widget#setConstraint

*/

}
