// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the Vaca nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include "Vaca/Vaca.h"
#include <cmath>
#include <ctime>
#include <boost/bind/placeholders.hpp>

#include "resource.h"
#include "Date.h"
#include "Calendar.h"
#include "SectionHeader.h"
#include "SectionContent.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

struct Contact {
  String name;
  String address;
  String telephone;
  String email;
  String homepage;

  bool operator<(const Contact &contact) const
  {
    return name < contact.name;
  }
};

struct Reminder {
  Date date;
  String desc;

  bool operator<(const Reminder &reminder) const
  {
    return date < reminder.date;
  }
};

//////////////////////////////////////////////////////////////////////

Font verdana("Verdana", 8);

void setup_widget(Widget *widget)
{
  widget->setFont(&verdana);
  widget->setBgColor(Color::White);

  Widget::Container descendants = widget->getChildren();
  std::for_each(descendants.begin(),
		descendants.end(),
		setup_widget);
}

//////////////////////////////////////////////////////////////////////

class ContactForm : public Panel
{
  Label mLabelName;
  Edit mEditName;
  Label mLabelAddress;
  Edit mEditAddress;
  Label mLabelTelephone;
  Edit mEditTelephone;
  Label mLabelEmail;
  Edit mEditEmail;
  Label mLabelHomePage;
  Edit mEditHomePage;
  Panel mBottom;
  Button mSave;
  Button mCancel;

public:

  ContactForm(Contact *contact, Widget *parent)
    : Panel(parent)
    , mLabelName("Name:", this)
    , mEditName(contact != NULL ? contact->name: "", this)
    , mLabelAddress("Address:", this)
    , mEditAddress(contact != NULL ? contact->address: "", this)
    , mLabelTelephone("Telephone:", this)
    , mEditTelephone(contact != NULL ? contact->telephone: "", this)
    , mLabelEmail("E-mail:", this)
    , mEditEmail(contact != NULL ? contact->email: "", this)
    , mLabelHomePage("Home page:", this)
    , mEditHomePage(contact != NULL ? contact->homepage: "", this)
    , mBottom(this)
    , mSave("Save", &mBottom)
    , mCancel("Cancel", &mBottom)
  {
    setLayout(new BoxLayout(Vertical, false));
    mBottom.setLayout(new BoxLayout(Horizontal, true, 0));

    mEditName.acquireFocus();
    mSave.setDefault(true);

    setup_widget(this);
  }

  Button &getSaveButton() { return mSave; }
  Button &getCancelButton() { return mCancel; }

  void fillContact(Contact *contact)
  {
    contact->name      = mEditName.getText();
    contact->address   = mEditAddress.getText();
    contact->telephone = mEditTelephone.getText();
    contact->email     = mEditEmail.getText();
    contact->homepage  = mEditHomePage.getText();
  }
  
};

//////////////////////////////////////////////////////////////////////

class ReminderPanel : public Panel
{
  Date *mDate;
  Label mLabel;
  MultilineEdit mEdit;
  
public:

  boost::signal<void (const Date &date, const String &desc)> Save;

  ReminderPanel(Widget *parent)
    : Panel(parent)
    , mDate(NULL)
    , mLabel("", this)
    , mEdit("", this, MultilineEditStyle
// 		      + FocusableStyle
		      - AutoHorizontalScrollEditStyle
		      + AutoVerticalScrollEditStyle)
  {
    setLayout(new BoxLayout(Vertical, false));
    mEdit.setConstraint(new BoxConstraint(true));
  }

  virtual ~ReminderPanel()
  {
    if (mDate != NULL) {
      Save(*mDate, mEdit.getText());
      delete mDate;
    }
  }

  bool setDate(const Date &date, const String &desc)
  {
    bool hide = false;

    if (mDate != NULL) {
      Save(*mDate, mEdit.getText());
      hide = date == *mDate;
      delete mDate;
      mDate = NULL;
    }

    if (hide)
      return false;

    mDate = new Date(date);
    mLabel.setText(weekdayNames[date.getDayOfWeek()] + ", " +
		   date.getMonthName() + " " +
		   String::fromInt(date.getDay()) + " " +
		   String::fromInt(date.getYear()));
    mEdit.setText(desc);
    mEdit.acquireFocus();
    mEdit.selectAll();
    return true;
  }
  
  boost::signal<void (Event &)> &getEditChangeSignal()
  {
    return mEdit.Change;
  }

//   void d()
//   {
//     mEdit.releaseFocus();
//   }

};

//////////////////////////////////////////////////////////////////////

String AddressBookFileName;

typedef std::vector<Reminder> Reminders;
typedef std::vector<Contact *> Contacts;
typedef std::vector<String> Todos;

class AddressBook
{
  Reminders mReminders;
  Contacts mContacts;
  Todos mTodos;

public:

  virtual ~AddressBook()
  {
    for (Contacts::iterator it = mContacts.begin();
	 it != mContacts.end(); ++it)
      delete *it;
  }

  //////////////////////////////////////////////////////////////////////
  // Contacts

  Contacts *getContacts()
  {
    return &mContacts;
  }

  void addContact(Contact *contact)
  {
    mContacts.push_back(contact);
  }
  
  //////////////////////////////////////////////////////////////////////
  // Reminders

  bool isValidReminder(Reminders::iterator it)
  {
    return it != mReminders.end();
  }

  Reminders::iterator getReminder(const Date &date)
  {
    Reminders::iterator it;

    for (it = mReminders.begin(); it != mReminders.end(); ++it)
      if (it->date == date)
	break;

    return it;
  }

  // returns true if mReminders was modified
  bool replaceReminder(const Date &date, const String &desc)
  {
    Reminders::iterator it = getReminder(date);

    if (it != mReminders.end()) {
      if (desc.empty())
	mReminders.erase(it);
      else if (it->desc != desc)
	it->desc = desc;
      else
	return false;		// no changes
    }
    else if (!desc.empty()) {
      Reminder reminder;
      reminder.date = date;
      reminder.desc = desc;
      mReminders.push_back(reminder);
    }
    else
      return false;		// no changes

    sort(mReminders.begin(), mReminders.end());
    return true;		// reminders changed
  }
  
  //////////////////////////////////////////////////////////////////////
  // Load and save

  void load()
  {
    FILE *f = fopen(AddressBookFileName.c_str(), "rt");
    if (f != NULL) {
      char buf[4096];
      int section = 0;

      while (fgets(buf, sizeof(buf), f)) {
	// clean trailing characters
	int len = strlen(buf);
	while ((len = strlen(buf)) > 0 &&
	       buf[len-1] == '\r' ||
	       buf[len-1] == '\n')
	  buf[len-1] = 0;

	// change of section
	if (String("CONTACTS") == buf)
	  section = 1;
	else if (String("REMINDERS") == buf)
	  section = 2;
	else if (String("TODOS") == buf)
	  section = 3;

	// in which section?
	switch (section) {

	  // Contacts
	  case 1: {
	    Contact *contact = new Contact;
	    int tokNum = 0;
	    String tok;
	    char *p;

	    for (char *p = buf; ; ++p) {
	      if (*p == '\t' || *p == 0) {
		switch (tokNum) {
		  case 1: contact->name = tok; break;
		  case 2: contact->address = tok; break;
		  case 3: contact->telephone = tok; break;
		  case 4: contact->email = tok; break;
		  case 5: contact->homepage = tok; break;
		}
		if (*p == 0)
		  break;

		tokNum++;
		tok.clear();
	      }
	      else {
		tok.push_back(*p);
	      }
	    }

	    if (!contact->name.empty())
	      mContacts.push_back(contact);
	    else
	      delete contact;
	    break;
	  }

	    // Reminders
	  case 2: {
	    Reminder reminder;
	    int tokNum = 0;
	    String tok;
	    char *p;

	    for (char *p = buf; ; ++p) {
	      if (*p == '\t' || *p == 0) {
		switch (tokNum) {
		  case 1: reminder.date = Date(tok); break;
		  case 2: reminder.desc = tok; break;
		}
		if (*p == 0)
		  break;

		tokNum++;
		tok.clear();
	      }
	      else {
		tok.push_back(*p);
	      }
	    }

	    if (tokNum == 2)
	      mReminders.push_back(reminder);
	    break;
	  }

	    // Todos
	  case 3: {
	    break;
	  }
	    
	}
      }
      fclose(f);
    }
  }

  void save()
  {
    FILE *f = fopen(AddressBookFileName.c_str(), "wt");
    if (f != NULL) {
      fprintf(f, "CONTACTS\n");
      for (Contacts::iterator it = mContacts.begin();
	   it != mContacts.end(); ++it) {
	fprintf(f, "\t%s\t%s\t%s\t%s\t%s\n",
		(*it)->name.c_str(),
		(*it)->address.c_str(),
		(*it)->telephone.c_str(),
		(*it)->email.c_str(),
		(*it)->homepage.c_str());
      }

      fprintf(f, "REMINDERS\n");
      for (Reminders::iterator it = mReminders.begin();
	   it != mReminders.end(); ++it) {
	fprintf(f, "\t%s\t%s\n",
		it->date.getISO().c_str(),
		it->desc.c_str());
      }

      fprintf(f, "TODOS\n");

      fclose(f);
    }
  }
  
};

//////////////////////////////////////////////////////////////////////

#if 0
class ContactLabel : public LightingPanel
{
  int mHot;

public:

  ContactLabel(const String &text, Widget *parent)
    : LightingPanel(parent, PanelStyle + FocusableStyle - ContainerStyle)
  {
    setText(text);
//     setLightSpeed(0.1);
    setLightSpeed(0.75);

    mHot = -1;
  }

  boost::signal<void ()> Action;

protected:

  virtual void onPreferredSize(Size &sz)
  {
    ScreenGraphics g;
    g.setFont(getFont());

    sz = g.measureString(getText()) + Size(2, 2);
  }
  
  virtual void onPaint(Graphics &g)
  {
    Rect bounds = getClientBounds();
    double light = getLightLevel();

    Color bgColor1 = Color::White;
    Color bgColor2 = Color(0, 140, 200);
    Color fgColor1 = Color::Black;
    Color fgColor2 = Color::White;

    Brush brush(bgColor1*(1-light) + bgColor2*light);
    g.fillRect(brush, bounds);

    g.setFont(getFont());
    g.setColor(fgColor1*(1-light) + fgColor2*light);
    g.drawString(getText(), bounds.x+1, bounds.y+1);
  }

  void onMouseDown(MouseEvent &ev)
  {
    LightingPanel::onMouseDown(ev);

    acquireFocus();
    Action();
  }

  virtual void onSetCursor(int hitTest)
  {
    if (hitTest = HTCLIENT)
      setCursor(Cursor(HandCursor));
    else
      LightingPanel::onSetCursor(hitTest);
  }
  
};
#endif

class ContactLabel : public LinkLabel
{
public:

  ContactLabel(const String &text, Widget *parent)
    : LinkLabel(text, parent, LinkLabelStyle)
  {
  }

protected:

  virtual void onPreferredSize(Size &sz)
  {
    ScreenGraphics g;
    g.setFont(getFont());

    sz = g.measureString(getText()) + Size(2, 2);
  }

  virtual void onPaint(Graphics &g)
  {
    Rect bounds = getClientBounds();
//     double light = getLightLevel();

    Color bgColor1 = Color::White;
    Color bgColor2 = Color(0, 140, 200);
    Color fgColor1 = Color::Black;
    Color fgColor2 = Color::White;

    Brush brush(hasMouse() ? bgColor2: bgColor1);
    g.fillRect(brush, bounds);

    g.setFont(getFont());
    g.setColor(hasMouse() ? fgColor2: fgColor1);
    g.drawString(getText(), bounds.x+1, bounds.y+1);
  }

  virtual void onMouseLeave()
  {
    LinkLabel::onMouseLeave();
    invalidate(true);
  }

//   void onMouseDown(MouseEvent &ev)
//   {
//     LightingPanel::onMouseDown(ev);

//     acquireFocus();
//     Action();
//   }

//   virtual void onSetCursor(int hitTest)
//   {
//     if (hitTest = HTCLIENT)
//       setCursor(Cursor(HandCursor));
//     else
//       LightingPanel::onSetCursor(hitTest);
//   }
  
};


//////////////////////////////////////////////////////////////////////

class MainFrame : public Dialog
{
  AddressBook mAddressBook;
//   ImageList mImageList;
  Font mHeaderFont;
  SectionHeader mContactsHeader;
  SectionContent mContactsContent;
  SectionHeader mCalendarHeader;
  SectionContent mCalendarContent;
  Calendar mCalendar;
  ReminderPanel mReminderPanel;
  SectionHeader mTodoHeader;
  SectionContent mTodoContent;
  SectionHeader *mSelectedHeader;

public:

  MainFrame()
    : Dialog("AddressBook for " + System::getUserName() + " (WIP)",
	     NULL,
	     DialogStyle
	     + ResizableFrameStyle
	     - ModalDialogStyle
	     + VerticalScrollStyle)
// 	     FrameStyle
// 	     - ResizableFrameStyle
// 	     - MaximizableFrameStyle)
//     , mImageList(IDB_IMAGELIST, 16, Color::White)
    , mHeaderFont("Verdana", 10, Font::Style::Bold)
    , mContactsHeader("Contacts", Color(100, 200, 255), this)
    , mContactsContent(this)
    , mCalendarHeader("Calendar", Color(100, 200, 100), this)
    , mCalendarContent(this)
    , mCalendar(&mCalendarContent)
    , mReminderPanel(&mCalendarContent)
    , mTodoHeader("To Do", Color(255, 120, 120), this)
    , mTodoContent(this)
    , mSelectedHeader(NULL)
  {
    setup_widget(this);
    setLayout(new BoxLayout(Vertical, false, 4, 1));
    setIcon(IDI_ADDBOOK);
    
    mContactsHeader.setFont(&mHeaderFont);
    mCalendarHeader.setFont(&mHeaderFont);
    mTodoHeader    .setFont(&mHeaderFont);

    mContactsHeader.Action.connect(Bind(&MainFrame::onSelectHeader, this));
    mCalendarHeader.Action.connect(Bind(&MainFrame::onSelectHeader, this));
    mTodoHeader    .Action.connect(Bind(&MainFrame::onSelectHeader, this));

    // New task
//     addEntry(&mTodoContent, "[ New task ]");

    // when a date is clicked in the calendar, onSelectDate is called
    mCalendar.Action.connect(Bind(&MainFrame::onSelectDate, this));
    mCalendar.GetDateInfo.connect(Bind(&MainFrame::onGetDateInfo, this));

    // when the reminder must be saved
    mReminderPanel.Save.connect(Bind(&MainFrame::onSaveReminder, this));

    // when edit a reminder
    mReminderPanel.getEditChangeSignal()
      .connect(Bind(&SectionContent::animateVisibility,
		    &mCalendarContent,
		    true));  

    mReminderPanel.setVisible(false);

    // hide all sections
    onSelectHeader(NULL);

    // load the address book data
    mAddressBook.load();

    // generate the list of contacts from the loaded data
    generateContactsContent();

    // put this frame in the right side of the work-area
    {
      Rect workBounds = System::getWorkAreaBounds();
      Size sz(getNonClientSize().w + 256, workBounds.h);
      setBounds(Rect(Point(workBounds.x+workBounds.w-sz.w,
			   workBounds.y+workBounds.h-sz.h),
		     sz));
    }
  }

  virtual ~MainFrame()
  {
    cleanContactsContent();
  }

  virtual void layout()
  {
    Dialog::layout();

    Size client = getClientBounds().getSize();
    Size pref = getPreferredSize();

    if (client.h >= pref.h)
      hideScrollBar(Vertical);
    else {
      ScrollInfo si;
      si.minPos = 0;
      si.maxPos = pref.h;
      si.pageSize = client.h;
      si.pos = getScrollPos(Vertical);
      setScrollInfo(Vertical, si);
    }
  }

  virtual Rect getLayoutBounds()
  {
    Rect rc = Dialog::getLayoutBounds();
    return Rect(rc.x, rc.y-getScrollPos(Vertical), rc.w, rc.h);
  }

protected:

  // events

  virtual void onScroll(Orientation orientation, int code)
  {
    ScrollInfo si = getScrollInfo(orientation);
    int oldPos = si.pos;

    switch (code) {
      case SB_LINELEFT: 
	si.pos -= 32;
	break;
      case SB_LINERIGHT: 
	si.pos += 32;
	break;
      case SB_PAGELEFT:
	si.pos -= si.pageSize;
	break;
      case SB_PAGERIGHT:
	si.pos += si.pageSize;
	break;
      case SB_THUMBTRACK: 
	si.pos = si.trackPos;
	break;
      default:
	break;
    }

    setScrollPos(orientation, si.pos);
    si.pos = getScrollPos(orientation);

    if (si.pos != oldPos) {
      ScrollWindowEx(getHWND(),
		     (orientation == Horizontal) ? oldPos - si.pos: 0,
		     (orientation == Vertical  ) ? oldPos - si.pos: 0,
		     NULL, NULL, NULL, NULL,
		     SW_ERASE | SW_INVALIDATE);

      layout();
//       invalidate(true);
    }
  }
  
private:
  
  void cleanContactsContent()
  {
    Container children = mContactsContent.getChildren();
    for_each(children.begin(), children.end(), delete_widget);
  }
  
  void generateContactsContent()
  {
    Contacts *contacts = mAddressBook.getContacts();
    ContactLabel *entry;

    for (Contacts::iterator it = contacts->begin();
	 it != contacts->end(); ++it) {
      Contact *contact = *it;

      // A new entry for each contact
      entry = new ContactLabel("> " + contact->name, &mContactsContent);
      entry->Action.connect(Bind(&MainFrame::onShowContact, this, contact));
    }

    // New Contact
    entry = new ContactLabel("[ New Contact ]", &mContactsContent);
    entry->Action.connect(Bind(&MainFrame::onNewContact, this));

    setup_widget(&mContactsContent);
  }
  
  void onSelectHeader(SectionHeader *header)
  {
    // restore old selected header
    if (mSelectedHeader != NULL)
      mSelectedHeader->back();

    // new selected header
    mSelectedHeader = header;

    // start animation for resizing Content...
    mContactsContent.animateVisibility(header == &mContactsHeader);
    mCalendarContent.animateVisibility(header == &mCalendarHeader);
    mTodoContent    .animateVisibility(header == &mTodoHeader);

    // disable ReminderPanel?
    mReminderPanel.setVisible(mSelectedHeader == &mCalendarHeader);
    mReminderPanel.setEnabled(mSelectedHeader == &mCalendarHeader);

    // relayout
    layout();
  }

  //////////////////////////////////////////////////////////////////////

  void onNewContact()
  {
    cleanContactsContent();

    ContactForm *form = new ContactForm(NULL, &mContactsContent);
    form->getSaveButton().Action.connect(Bind(&MainFrame::onSaveOrCancelNewContact, this, form, true));
    form->getCancelButton().Action.connect(Bind(&MainFrame::onSaveOrCancelNewContact, this, form, false));

    // mContactsContent size changes
    mContactsContent.animateVisibility(mSelectedHeader == &mContactsHeader);
  }

  void onSaveOrCancelNewContact(ContactForm *form, bool save)
  {
    if (save) {
      Contact *contact = new Contact;
      form->fillContact(contact);

      mAddressBook.addContact(contact);
      mAddressBook.save();
    }

    // regenerate mContactsContent
    cleanContactsContent();
    generateContactsContent();

    // mContactsContent size changes
    mContactsContent.animateVisibility(mSelectedHeader == &mContactsHeader);
  }

  //////////////////////////////////////////////////////////////////////
  
  void onShowContact(Contact *contact)
  {
    cleanContactsContent();
    
    ContactForm *form = new ContactForm(contact, &mContactsContent);
    form->getSaveButton().Action.connect(Bind(&MainFrame::onSaveOrCancelExistentContact, this, form, true, contact));
    form->getCancelButton().Action.connect(Bind(&MainFrame::onSaveOrCancelExistentContact, this, form, false, contact));

    // mContactsContent size changes
    mContactsContent.animateVisibility(mSelectedHeader == &mContactsHeader);
  }

  void onSaveOrCancelExistentContact(ContactForm *form, bool save, Contact *contact)
  {
    if (save) {
      form->fillContact(contact);
      mAddressBook.save();
    }

    // regenerate mContactsContent
    cleanContactsContent();
    generateContactsContent();

    // mContactsContent size changes
    mContactsContent.animateVisibility(mSelectedHeader == &mContactsHeader);
  }

  //////////////////////////////////////////////////////////////////////

  void onGetDateInfo(const Date &date, bool &hasReminder, bool &isBirthday)
  {
    Reminders::iterator reminder = mAddressBook.getReminder(date);

    if (mAddressBook.isValidReminder(reminder))
      hasReminder = true;

    // TODO isBrithday
  }

  void onSelectDate(const Date &date)
  {
    Reminders::iterator reminder = mAddressBook.getReminder(date);
    String desc = mAddressBook.isValidReminder(reminder) ? reminder->desc: "";

    mReminderPanel.setVisible(mReminderPanel.setDate(date, desc));
//     if (mReminderPanel.setDate(date, desc))
//       mReminderPanel.setVisible(true);
//     else {
//       mReminderPanel.d();
//       Beep(400, 100);
//     }
    layout();
    
    mCalendarContent.animateVisibility(mSelectedHeader == &mCalendarHeader);
  }

  void onSaveReminder(const Date &date, const String &desc)
  {
    if (mAddressBook.replaceReminder(date, desc))
      mAddressBook.save();

    // mReminderPanel.setVisible(false);
    layout();

    mCalendarContent.animateVisibility(mSelectedHeader == &mCalendarHeader);
  }
  
  //////////////////////////////////////////////////////////////////////

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
public:

  virtual void main(std::vector<String> args)
  {
    AddressBookFileName =
      args[0].getFilePath() +
      "\\AddressBook for " +
      System::getUserName() + ".txt";

    MainFrame mainFrame;
    mainFrame.setVisible(true);

    doMessageLoop();
  }

};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
 		   LPSTR lpCmdLine, int nCmdShow)
{
  Example app;
  app.run();
  return 0;
}
