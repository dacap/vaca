// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
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
// * Neither the name of the author nor the names of its contributors
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

#ifndef VACA_SCINTILLA_H
#define VACA_SCINTILLA_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"

namespace Vaca {

/**
 * Loads the Scintilla DLL.
 *
 * First it tries to load @c SciLexer.dll and then if it fails, it
 * will try to load @c Scintilla.dll
 */
class VACA_DLL SciRegister
{
  static HINSTANCE hmod;

public:
  SciRegister();
};

/**
 * Controls a Scintilla text editor. This is the class with more
 * methods, but each method only wrap an Scintilla (@c SCI_) message.
 */
class VACA_DLL SciEdit : public SciRegister, public Widget
{
public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  SciEdit(Widget* parent, Style style = Styles::Default);
  virtual ~SciEdit();

  virtual void setFont(Font font);

  //////////////////////////////////////////////////////////////////////
  // Text retrieval and modification
  virtual String getText() const;
  virtual void setText(const String& str);
  void setSavePoint();
  String getLine(int line) const;
  void replaceSel(const String& str);
  void setReadOnly(bool readOnly);
  bool getReadOnly() const;
// SCI_GETTEXTRANGE(<unused>, TextRange* tr)
// SCI_GETSTYLEDTEXT(<unused>, TextRange* tr)
// SCI_ALLOCATE(int bytes, <unused>)
  void addText(const char* str, int length);
  void addText(const String& str);
// SCI_ADDSTYLEDTEXT(int length, cell* s)
  void appendText(const char* str, int length);
  void appendText(const String& str);
  void insertText(int pos, const String& str);
  void clearAll();
// SCI_CLEARDOCUMENTSTYLE
  char getCharAt(int pos) const;
// SCI_GETSTYLEAT(int pos)
// SCI_SETSTYLEBITS(int bits)
// SCI_GETSTYLEBITS
  
  //////////////////////////////////////////////////////////////////////
  // Searching
// SCI_FINDTEXT(int flags, TextToFind* ttf)
  void searchAnchor();
  bool searchNext(int flags, String& str);
  bool searchPrev(int flags, String& str);

  //////////////////////////////////////////////////////////////////////
  // Searching and replace using target
// SCI_SETTARGETSTART(int pos)
// SCI_GETTARGETSTART
// SCI_SETTARGETEND(int pos)
// SCI_GETTARGETEND
// SCI_TARGETFROMSELECTION
// SCI_SETSEARCHFLAGS(int searchFlags)
// SCI_GETSEARCHFLAGS
// SCI_SEARCHINTARGET(int length, const char* text)
// SCI_REPLACETARGET(int length, const char* text)
// SCI_REPLACETARGETRE(int length, const char* text)

  //////////////////////////////////////////////////////////////////////
  // Overtype (overwrite-mode)
  void setOverwriteMode(bool state);
  bool getOverwriteMode() const;

  //////////////////////////////////////////////////////////////////////
  // Cut, copy and paste
  void cutTextToClipboard();
  void copyTextToClipboard();
  void pasteTextFromClipboard();
  void clearText();
// SCI_CANPASTE
// SCI_COPYRANGE(int start, int end)
// SCI_COPYTEXT(int length, const char* text)
// SCI_SETPASTECONVERTENDINGS(bool convert)
// SCI_GETPASTECONVERTENDINGS

  //////////////////////////////////////////////////////////////////////
  // Error handling
// SCI_SETSTATUS(int status)
// SCI_GETSTATUS

  //////////////////////////////////////////////////////////////////////
  // Undo and Redo
  void undo();
  bool canUndo() const;
  void redo();
  bool canRedo() const;
  void emptyUndoBuffer();
// SCI_SETUNDOCOLLECTION(bool collectUndo)
// SCI_GETUNDOCOLLECTION
  void beginUndoAction();
  void endUndoAction();

  //////////////////////////////////////////////////////////////////////
  // Selection and information
  int getTextLength() const;
  int getLineCount() const;
  int getFirstVisibleLine() const;
  int getLinesOnScreen() const;
  bool isModified() const;
// SCI_SETSEL(int anchorPos, int currentPos)
  void goToPos(int pos);
  void goToLine(int line);
  void setCurrentPos(int pos);
  int getCurrentPos() const;
  void setAnchor(int pos);
  int getAnchor() const;
// SCI_SETSELECTIONSTART(int position)
// SCI_SETSELECTIONEND(int position)
  int getSelectionStart() const;
  int getSelectionEnd() const;
// SCI_SELECTALL
// SCI_LINEFROMPOSITION(int position)
// SCI_POSITIONFROMLINE(int line)
// SCI_GETLINEENDPOSITION(int line)
  int getLineLength(int line) const;
  String getSelText() const;
// SCI_GETCURLINE(int textLen, char* text)
// SCI_SELECTIONISRECTANGLE
// SCI_SETSELECTIONMODE(int mode)
// SCI_GETSELECTIONMODE
// SCI_GETLINESELSTARTPOSITION(int line)
// SCI_GETLINESELENDPOSITION(int line)
// SCI_MOVECARETINSIDEVIEW
// SCI_WORDENDPOSITION(int position, bool onlyWordCharacters)
// SCI_WORDSTARTPOSITION(int position, bool onlyWordCharacters)
// SCI_POSITIONBEFORE(int position)
// SCI_POSITIONAFTER(int position)
// SCI_TEXTWIDTH(int styleNumber, const char* text)
// SCI_TEXTHEIGHT(int line)
// SCI_GETCOLUMN(int position)
// SCI_FINDCOLUMN(int line, int column)
// SCI_POSITIONFROMPOINT(int x, int y)
// SCI_POSITIONFROMPOINTCLOSE(int x, int y)
// SCI_POINTXFROMPOSITION(<unused>, int position)
// SCI_POINTYFROMPOSITION(<unused>, int position)
// SCI_HIDESELECTION(bool hide)
// SCI_CHOOSECARETX

  //////////////////////////////////////////////////////////////////////
  // Scrolling and automatic scrolling
// SCI_LINESCROLL(int column, int line)
// SCI_SCROLLCARET
// SCI_SETXCARETPOLICY(int caretPolicy, int caretSlop)
// SCI_SETYCARETPOLICY(int caretPolicy, int caretSlop)
// SCI_SETVISIBLEPOLICY(int caretPolicy, int caretSlop)
// SCI_SETHSCROLLBAR(bool visible)
// SCI_GETHSCROLLBAR
// SCI_SETVSCROLLBAR(bool visible)
// SCI_GETVSCROLLBAR
// SCI_GETXOFFSET
// SCI_SETXOFFSET(int xOffset)
// SCI_SETSCROLLWIDTH(int pixelWidth)
// SCI_GETSCROLLWIDTH
// SCI_SETENDATLASTLINE(bool endAtLastLine)
// SCI_GETENDATLASTLINE
  
  //////////////////////////////////////////////////////////////////////
  // White space
// SCI_SETVIEWWS(int wsMode)
// SCI_GETVIEWWS
// SCI_SETWHITESPACEFORE(bool useWhitespaceForeColour, int colour)
// SCI_SETWHITESPACEBACK(bool useWhitespaceBackColour, int colour)

  //////////////////////////////////////////////////////////////////////
  // Cursor
// SCI_SETCURSOR(int curType)
// SCI_GETCURSOR

  //////////////////////////////////////////////////////////////////////
  // Mouse capture
// SCI_SETMOUSEDOWNCAPTURES(bool captures)
// SCI_GETMOUSEDOWNCAPTURES

  //////////////////////////////////////////////////////////////////////
  // Line endings
  void setEolMode(int eolMode);
  int getEolMode() const;
  void convertEols(int eolMode);
  void setViewEol(bool visible);
  bool getViewEol() const;

  //////////////////////////////////////////////////////////////////////
  // Styling
// SCI_GETENDSTYLED
// SCI_STARTSTYLING(int position, int mask)
// SCI_SETSTYLING(int length, int style)
// SCI_SETSTYLINGEX(int length, const char* styles)
// SCI_SETLINESTATE(int line, int value)
// SCI_GETLINESTATE(int line)
// SCI_GETMAXLINESTATE

  //////////////////////////////////////////////////////////////////////
  // Style definition
// SCI_STYLERESETDEFAULT
// SCI_STYLECLEARALL
// SCI_STYLESETFONT(int styleNumber, char* fontName)
// SCI_STYLESETSIZE(int styleNumber, int sizeInPoints)
// SCI_STYLESETBOLD(int styleNumber, bool bold)
// SCI_STYLESETITALIC(int styleNumber, bool italic)
// SCI_STYLESETUNDERLINE(int styleNumber, bool underline)
// SCI_STYLESETFORE(int styleNumber, int colour)
// SCI_STYLESETBACK(int styleNumber, int colour)
// SCI_STYLESETEOLFILLED(int styleNumber, bool eolFilled)
// SCI_STYLESETCHARACTERSET(int styleNumber, int charSet)
// SCI_STYLESETCASE(int styleNumber, int caseMode)
// SCI_STYLESETVISIBLE(int styleNumber, bool visible)
// SCI_STYLESETCHANGEABLE(int styleNumber, bool changeable)
// SCI_STYLESETHOTSPOT(int styleNumber, bool hotspot)

  //////////////////////////////////////////////////////////////////////
  // Caret, selection, and hotspot styles
// SCI_SETSELFORE(bool useSelectionForeColour, int colour)
// SCI_SETSELBACK(bool useSelectionBackColour, int colour)
// SCI_SETCARETFORE(int colour)
// SCI_GETCARETFORE
// SCI_SETCARETLINEVISIBLE(bool show)
// SCI_GETCARETLINEVISIBLE
// SCI_SETCARETLINEBACK(int colour)
// SCI_GETCARETLINEBACK
// SCI_SETCARETPERIOD(int milliseconds)
// SCI_GETCARETPERIOD
// SCI_SETCARETWIDTH(int pixels)
// SCI_GETCARETWIDTH
// SCI_SETHOTSPOTACTIVEFORE
// SCI_SETHOTSPOTACTIVEBACK
// SCI_SETHOTSPOTACTIVEUNDERLINE
// SCI_SETHOTSPOTSINGLELINE
// SCI_SETCONTROLCHARSYMBOL(int symbol)
// SCI_GETCONTROLCHARSYMBOL
// SCI_SETCARETSTICKY
// SCI_GETCARETSTICKY
// SCI_TOGGLECARETSTICKY

  //////////////////////////////////////////////////////////////////////
  // Margins
// SCI_SETMARGINTYPEN(int margin, int type)
// SCI_GETMARGINTYPEN(int margin)
// SCI_SETMARGINWIDTHN(int margin, int pixelWidth)
// SCI_GETMARGINWIDTHN(int margin)
// SCI_SETMARGINMASKN(int margin, int mask)
// SCI_GETMARGINMASKN(int margin)
// SCI_SETMARGINSENSITIVEN(int margin, bool sensitive)
// SCI_GETMARGINSENSITIVEN(int margin)
// SCI_SETMARGINLEFT(<unused>, int pixels)
// SCI_GETMARGINLEFT
// SCI_SETMARGINRIGHT(<unused>, int pixels)
// SCI_GETMARGINRIGHT
// SCI_SETFOLDMARGINCOLOUR(bool useSetting, int colour)
// SCI_SETFOLDMARGINHICOLOUR(bool useSetting, int colour)

  //////////////////////////////////////////////////////////////////////
  // Other settings
// SCI_SETUSEPALETTE(bool allowPaletteUse)
// SCI_GETUSEPALETTE
// SCI_SETBUFFEREDDRAW(bool isBuffered)
// SCI_GETBUFFEREDDRAW
// SCI_SETTWOPHASEDRAW(bool twoPhase)
// SCI_GETTWOPHASEDRAW
// SCI_SETCODEPAGE(int codePage)
// SCI_GETCODEPAGE
// SCI_SETWORDCHARS(<unused>, const char* chars)
// SCI_SETWHITESPACECHARS(<unused>, const char* chars)
// SCI_SETCHARSDEFAULT
// SCI_GRABFOCUS
// SCI_SETFOCUS(bool focus)
// SCI_GETFOCUS

  //////////////////////////////////////////////////////////////////////
  // Brace highlighting
// SCI_BRACEHIGHLIGHT(int pos1, int pos2)
// SCI_BRACEBADLIGHT(int pos1)
// SCI_BRACEMATCH(int position, int maxReStyle)

  //////////////////////////////////////////////////////////////////////
  // Tabs and Indentation Guides
// SCI_SETTABWIDTH(int widthInChars)
// SCI_GETTABWIDTH
// SCI_SETUSETABS(bool useTabs)
// SCI_GETUSETABS
// SCI_SETINDENT(int widthInChars)
// SCI_GETINDENT
// SCI_SETTABINDENTS(bool tabIndents)
// SCI_GETTABINDENTS
// SCI_SETBACKSPACEUNINDENTS(bool bsUnIndents)
// SCI_GETBACKSPACEUNINDENTS
// SCI_SETLINEINDENTATION(int line, int indentation)
// SCI_GETLINEINDENTATION(int line)
// SCI_GETLINEINDENTPOSITION(int line)
// SCI_SETINDENTATIONGUIDES(bool view)
// SCI_GETINDENTATIONGUIDES
// SCI_SETHIGHLIGHTGUIDE(int column)
// SCI_GETHIGHLIGHTGUIDE

  //////////////////////////////////////////////////////////////////////
  // Markers
// SCI_MARKERDEFINE(int markerNumber, int markerSymbols)
// SCI_MARKERDEFINEPIXMAP(int markerNumber, const char* xpm)
// SCI_MARKERSETFORE(int markerNumber, int colour)
// SCI_MARKERSETBACK(int markerNumber, int colour)
// SCI_MARKERADD(int line, int markerNumber)
// SCI_MARKERADDSET(int line, int markerMask)
// SCI_MARKERDELETE(int line, int markerNumber)
// SCI_MARKERDELETEALL(int markerNumber)
// SCI_MARKERGET(int line)
// SCI_MARKERNEXT(int lineStart, int markerMask)
// SCI_MARKERPREVIOUS(int lineStart, int markerMask)
// SCI_MARKERLINEFROMHANDLE(int handle)
// SCI_MARKERDELETEHANDLE(int handle)

  //////////////////////////////////////////////////////////////////////
  // Indicators
// SCI_INDICSETSTYLE(int indicatorNumber, int indicatorStyle)
// SCI_INDICGETSTYLE(int indicatorNumber)
// SCI_INDICSETFORE(int indicatorNumber, int colour)
// SCI_INDICGETFORE(int indicatorNumber)

  //////////////////////////////////////////////////////////////////////
  // Autocompletion
// SCI_AUTOCSHOW(int lenEntered, const char* list)
// SCI_AUTOCCANCEL
// SCI_AUTOCACTIVE
// SCI_AUTOCPOSSTART
// SCI_AUTOCCOMPLETE
// SCI_AUTOCSTOPS(<unused>, const char* chars)
// SCI_AUTOCSETSEPARATOR(char separator)
// SCI_AUTOCGETSEPARATOR
// SCI_AUTOCSELECT(<unused>, const char* select)
// SCI_AUTOCGETCURRENT
// SCI_AUTOCSETCANCELATSTART(bool cancel)
// SCI_AUTOCGETCANCELATSTART
// SCI_AUTOCSETFILLUPS(<unused>, const char* chars)
// SCI_AUTOCSETCHOOSESINGLE(bool chooseSingle)
// SCI_AUTOCGETCHOOSESINGLE
// SCI_AUTOCSETIGNORECASE(bool ignoreCase)
// SCI_AUTOCGETIGNORECASE
// SCI_AUTOCSETAUTOHIDE(bool autoHide)
// SCI_AUTOCGETAUTOHIDE
// SCI_AUTOCSETDROPRESTOFWORD(bool dropRestOfWord)
// SCI_AUTOCGETDROPRESTOFWORD
// SCI_REGISTERIMAGE
// SCI_CLEARREGISTEREDIMAGES
// SCI_AUTOCSETTYPESEPARATOR(char separatorCharacter)
// SCI_AUTOCGETTYPESEPARATOR
// SCI_AUTOCSETMAXHEIGHT(int rowCount)
// SCI_AUTOCGETMAXHEIGHT
// SCI_AUTOCSETMAXWIDTH(int characterCount)
// SCI_AUTOCGETMAXWIDTH

  //////////////////////////////////////////////////////////////////////
  // User lists
// SCI_USERLISTSHOW(int listType, const char* list)

  //////////////////////////////////////////////////////////////////////
  // Call tips
// SCI_CALLTIPSHOW(int posStart, const char* definition)
// SCI_CALLTIPCANCEL
// SCI_CALLTIPACTIVE
// SCI_CALLTIPPOSSTART
// SCI_CALLTIPSETHLT(int highlightStart, int highlightEnd)
// SCI_CALLTIPSETBACK(int colour)
// SCI_CALLTIPSETFORE(int colour)
// SCI_CALLTIPSETFOREHLT(int colour)

  //////////////////////////////////////////////////////////////////////
  // Keyboard commands
// SCI_LINEDOWN
// SCI_LINEDOWNEXTEND
// SCI_LINEDOWNRECTEXTEND
// SCI_LINESCROLLDOWN	
// SCI_LINEUP
// SCI_LINEUPEXTEND
// SCI_LINEUPRECTEXTEND
// SCI_LINESCROLLUP	
// SCI_PARADOWN
// SCI_PARADOWNEXTEND
// SCI_PARAUP
// SCI_PARAUPEXTEND	
// SCI_CHARLEFT
// SCI_CHARLEFTEXTEND
// SCI_CHARLEFTRECTEXTEND
// SCI_CHARRIGHT
// SCI_CHARRIGHTEXTEND
// SCI_CHARRIGHTRECTEXTEND
// SCI_WORDLEFT
// SCI_WORDLEFTEXTEND
// SCI_WORDRIGHT
// SCI_WORDRIGHTEXTEND	
// SCI_WORDLEFTEND
// SCI_WORDLEFTENDEXTEND
// SCI_WORDRIGHTEND
// SCI_WORDRIGHTENDEXTEND	
// SCI_WORDPARTLEFT
// SCI_WORDPARTLEFTEXTEND
// SCI_WORDPARTRIGHT
// SCI_WORDPARTRIGHTEXTEND	
// SCI_HOME
// SCI_HOMEEXTEND
// [SCI_HOMERECTEXTEND]
// SCI_HOMEDISPLAY
// SCI_HOMEDISPLAYEXTEND
// SCI_HOMEWRAP
// SCI_HOMEWRAPEXTEND	
// SCI_VCHOME
// SCI_VCHOMEEXTEND
// SCI_VCHOMERECTEXTEND
// SCI_VCHOMEWRAP
// SCI_VCHOMEWRAPEXTEND
// SCI_LINEEND
// SCI_LINEENDEXTEND
// SCI_LINEENDRECTEXTEND	
// SCI_LINEENDDISPLAY
// SCI_LINEENDDISPLAYEXTEND
// SCI_LINEENDWRAP
// SCI_LINEENDWRAPEXTEND	
// SCI_DOCUMENTSTART
// SCI_DOCUMENTSTARTEXTEND
// SCI_DOCUMENTEND
// SCI_DOCUMENTENDEXTEND	
// SCI_PAGEUP
// SCI_PAGEUPEXTEND
// SCI_PAGEUPRECTEXTEND
// SCI_PAGEDOWN
// SCI_PAGEDOWNEXTEND
// SCI_PAGEDOWNRECTEXTEND
// SCI_STUTTEREDPAGEUP
// SCI_STUTTEREDPAGEUPEXTEND
// SCI_STUTTEREDPAGEDOWN
// SCI_STUTTEREDPAGEDOWNEXTEND	
// SCI_DELETEBACK
// SCI_DELETEBACKNOTLINE
// SCI_DELWORDLEFT
// SCI_DELWORDRIGHT	
// SCI_DELLINELEFT
// SCI_DELLINERIGHT
// SCI_LINEDELETE
// SCI_LINECUT
// SCI_LINECOPY
// SCI_LINETRANSPOSE
// SCI_LINEDUPLICATE	
// SCI_LOWERCASE
// SCI_UPPERCASE
// SCI_CANCEL
// SCI_EDITTOGGLEOVERTYPE	
// SCI_NEWLINE
// SCI_FORMFEED
// SCI_TAB
// SCI_BACKTAB

  //////////////////////////////////////////////////////////////////////
  // Key bindings
// SCI_ASSIGNCMDKEY(int keyDefinition, int sciCommand)
// SCI_CLEARCMDKEY(int keyDefinition)
// SCI_CLEARALLCMDKEYS
// SCI_NULL

  //////////////////////////////////////////////////////////////////////
  // Popup edit menu
// SCI_USEPOPUP(bool bEnablePopup)

  //////////////////////////////////////////////////////////////////////
  // Macro recording
// SCI_STARTRECORD
// SCI_STOPRECORD

  //////////////////////////////////////////////////////////////////////
  // Printing
// SCI_FORMATRANGE(bool bDraw, RangeToFormat* pfr)
// SCI_SETPRINTMAGNIFICATION(int magnification)
// SCI_GETPRINTMAGNIFICATION
// SCI_SETPRINTCOLOURMODE(int mode)
// SCI_GETPRINTCOLOURMODE
// SCI_SETPRINTWRAPMODE
// SCI_GETPRINTWRAPMODE

  //////////////////////////////////////////////////////////////////////
  // Direct access (dacap: don't provide this)
// SCI_GETDIRECTFUNCTION
// SCI_GETDIRECTPOINTER

  //////////////////////////////////////////////////////////////////////
  // Multiple views
  void* getDocPointer();
  void setDocPointer(void* doc);
// SCI_CREATEDOCUMENT
// SCI_ADDREFDOCUMENT(<unused>, document* pDoc)
// SCI_RELEASEDOCUMENT(<unused>, document* pDoc)

  //////////////////////////////////////////////////////////////////////
  // Folding
// SCI_VISIBLEFROMDOCLINE(int docLine)
// SCI_DOCLINEFROMVISIBLE(int displayLine)
// SCI_SHOWLINES(int lineStart, int lineEnd)
// SCI_HIDELINES(int lineStart, int lineEnd)
// SCI_GETLINEVISIBLE(int line)
// SCI_SETFOLDLEVEL(int line, int level)
// SCI_GETFOLDLEVEL(int line)
// SCI_SETFOLDFLAGS(int flags)
// SCI_GETLASTCHILD(int line, int level)
// SCI_GETFOLDPARENT(int line)
// SCI_SETFOLDEXPANDED(int line, bool expanded)
// SCI_GETFOLDEXPANDED(int line)
// SCI_TOGGLEFOLD(int line)
// SCI_ENSUREVISIBLE(int line)
// SCI_ENSUREVISIBLEENFORCEPOLICY(int line)

  //////////////////////////////////////////////////////////////////////
  // Line wrapping
// SCI_SETWRAPMODE(int wrapMode)
// SCI_GETWRAPMODE
// SCI_SETWRAPVISUALFLAGS(int wrapVisualFlags)
// SCI_GETWRAPVISUALFLAGS
// SCI_SETWRAPSTARTINDENT(int indent)
// SCI_GETWRAPSTARTINDENT
// SCI_SETLAYOUTCACHE(int cacheMode)
// SCI_GETLAYOUTCACHE
// SCI_LINESSPLIT(int pixelWidth)
// SCI_LINESJOIN
// SCI_WRAPCOUNT(int docLine)

  //////////////////////////////////////////////////////////////////////
  // Zooming
  void zoomIn();
  void zoomOut();
  void setZoom(int zoomInPoints);
  int getZoom();

  //////////////////////////////////////////////////////////////////////
  // Long lines
// SCI_SETEDGEMODE(int mode)
// SCI_GETEDGEMODE
// SCI_SETEDGECOLUMN(int column)
// SCI_GETEDGECOLUMN
// SCI_SETEDGECOLOUR(int colour)
// SCI_GETEDGECOLOUR

  //////////////////////////////////////////////////////////////////////
  // Lexer
// SCI_SETLEXER(int lexer)
// SCI_GETLEXER
// SCI_SETLEXERLANGUAGE(<unused>, char* name)
// SCI_LOADLEXERLIBRARY(<unused>, char* path)
// SCI_COLOURISE(int start, int end)
// SCI_SETPROPERTY(const char* key, const char* value)
// SCI_GETPROPERTY(const char* key, char* value)
// SCI_GETPROPERTYEXPANDED(const char* key, char* value)
// SCI_GETPROPERTYINT(const char* key, int default)
// SCI_SETKEYWORDS(int keyWordSet, const char* keyWordList)
// SCI_GETSTYLEBITSNEEDED

  //////////////////////////////////////////////////////////////////////
  // Notifications

  // signals
  Signal0<void> UpdateUI; ///< @see onUpdateUI

protected:
  // reflection
  virtual bool onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult);

  // new events
// SCN_STYLENEEDED
// SCN_CHARADDED
// SCN_SAVEPOINTREACHED
// SCN_SAVEPOINTLEFT
// SCN_MODIFYATTEMPTRO
// SCN_KEY
// SCN_DOUBLECLICK
  virtual void onUpdateUI();
// SCN_MODIFIED
// SCN_MACRORECORD
// SCN_MARGINCLICK
// SCN_NEEDSHOWN
// SCN_PAINTED
// SCN_USERLISTSELECTION
// SCN_URIDROPPED
// SCN_DWELLSTART
// SCN_DWELLEND
// SCN_ZOOM
// SCN_HOTSPOTCLICK
// SCN_HOTSPOTDOUBLECLICK
// SCN_CALLTIPCLICK
// SCN_AUTOCSELECTION
// SCI_SETMODEVENTMASK(int eventMask)
// SCI_GETMODEVENTMASK
// SCI_SETMOUSEDWELLTIME
// SCI_GETMOUSEDWELLTIME
// SCEN_CHANGE
// SCEN_SETFOCUS
// SCEN_KILLFOCUS

};

} // namespace Vaca

#endif // VACA_SCINTILLA_H
