namespace Vaca {

/**

@page page_tn_015 TN015: Widgets and Threads

Each widget belongs to the thread on which it was created. You cannot
use a Widget directly from another different thread (unless you really
know what you are doing, generally, knowing Vaca internals).

The only encourage way to communicate with a widget of another thread
is using Widget#enqueueMessage method. This method receives as parameter
a Message (a custom message). The destination widget had to override
the Widget#preTranslateMessage so it can catch the message and do what
it want with it.

Example (based on @link page_examples Threads example@endlink):
@code
// Here we create a custom message to be send to the MainFrame
// from a working thread
Message my_custom_message(L"My.Custom.Message");

// The following function is executed in a different thread that
// is not the "dest" owner thread
void working_thread(Widget* dest)
{
  ...
  // Here we can enqueue the custom message to "dest" widget.
  // Note that at this point we have to be sure that "dest" is
  // still "alive" (not deleted)
  dest->enqueueMessage(Message(my_custom_message, (void*)L"The End"));
}

class MainFrame : public Frame
{
public:
  ...
protected:
  // Override preTranslateMessage so we can intercept
  // our custom message
  virtual bool preTranslateMessage(Message& message)
  {
    // Call base implementation here
    if (Frame::preTranslateMessage(message))
      return true;

    // Is it our custom message?
    if (message == my_custom_message) {
      // Here we get the data that the working thread sent us
      // in the payload field
      String text = (const wchar_t*)message.getPayload();

      // Do what you want with the data (remember that
      // the payload could be a shared resource, so remember
      // to "mutex it" and free it in the right place)
      MsgBox::show(this, L"Message received", text);

      // We have used this message, so we return true
      return true;
    }

    // We didn't use this message
    return false;
  }
};

int VACA_MAIN()
{
  ...
  MainFrame frm;

  // Here we create the working thread and
  // we pass "&frm" as "dest" argument for the "working_thread"
  // (so the working thread can send messages to our MainFrame)
  Thread* newThread = new Thread(Bind<void>(&working_thread, &frm));
  ...
}
@endcode

@see Widget#enqueueMessage,
     Message#Message(const String&),
     Message#Message(const Message&, void*),
     @link page_examples Threads example@endlink

*/

}
