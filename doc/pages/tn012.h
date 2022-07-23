namespace vaca {

/**

@page page_tn_012 TN012: Using Parent's Signals in Children's Constructors

You can't use signals that aren't constructed. This piece of code has
a bug:

@code
class Frm;

class But : public Button
{
public:
  But(Frm* parent);
private:
  void onSig();
};

class Frm : public Frame
{
  But but;
public:
  Frm();
  Signal<void()> Sig;
};

But::But(Frm* parent) : Button("...", parent)
{
  // here we are using "Sig", but it isn't constructed yet!
  parent->Sig.connect(&But::onSig, this);
}

void But::onSig() { }

Frm::Frm() : Frame("..."), but(this)
{
}
@endcode

Here you can't use @c Frm::Sig in @c But::But constructor because the signal
isn't constructed yet. You have some options:

1) Put all Widget members after signals:
@code
class Frm : public Frame
{
public:
  Frm();
  Signal<void()> Sig;
private:
  But but;
};
@endcode

2) Construct the Widget after the signal is created:
@code
class Frm : public Frame
{
  std::auto_ptr<But> but;
public:
  Frm();
  Signal<void()> Sig;
};
...
Frm::Frm() : Frame("...")
{
  but.reset(new But(this));
}
@endcode

*/

}
