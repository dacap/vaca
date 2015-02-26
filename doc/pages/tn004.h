namespace vaca {

/**

@page page_tn_004 TN004: Signals Are Public

@li @ref page_tn_004_intro
@li @ref page_tn_004_access


@section page_tn_004_intro Signals

Maybe OOP fanatics'll explode of hungry when they see the @em public
data members like @c Close or @c Action for every signal. But I think
that it is the better way to access to signals because we don't need to
create/known a lot of routines to access/modify/update bindings for
the signal.

Let us see an alternative:

@code
class GoodFrame
{
  Signal m_close;
  
public:
  Signal& getCloseSignal() { return m_close; }
};

GoodFrame frame;
frame.getCloseSignal().connect(...);
@endcode

That code is the same to have m_close as public member, because we have
full access to the signal through it's reference. Another alternative
is to have methods like connectToCloseSignal and
disconnectToCloseSignal, but it only difficults the programming.

Vaca uses signals just like this:

@code
class Frame
{
public:
  Signal Close;
};

Frame frame;
frame.Close.connect(...);
@endcode

What is the advantage? You write less code and you have to know only
one entry point: the Close member.

What is the disadvantage? It's a public data member (is that a disadvantage? :)

@section page_tn_004_access General Access Rules

All data members must be private, only signals members must be public
and must begin with a capital letter. Don't use
protected data members, instead use a protected interface to access to
them.

*/

}
