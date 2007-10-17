# ----------------------------------------------------------------------
# Configuration

# DEBUG = 1
# STATIC = 1
# PROFILE = 1
# UNICODE = 1

ifndef BOOSTDIR
  BOOSTDIR = c:/boost_1_33_1
endif

# ----------------------------------------------------------------------
# Programs

AR	= ar
GXX	= g++
RM	= rm
CP	= cp
RMDIR	= rmdir
MKDIR	= mkdir
WINDRES = windres

# ----------------------------------------------------------------------
# Sources & Examples

LIB_SOURCES = Anchor AnchorLayout Application BandedDockArea		\
	      BasicDockArea BoxConstraint BoxLayout Brush Button	\
	      ButtonBase CancelableEvent CheckBox ClientLayout		\
	      Clipboard CloseEvent Color ColorDialog ComboBox Command	\
	      CommonDialog Component Constraint Cursor CustomButton	\
	      CustomLabel Debug Dialog DockArea DockBar DockFrame	\
	      DropFilesEvent Edit Event FileDialog FindTextDialog Font	\
	      FontDialog Frame Graphics GroupBox Icon Image ImageList	\
	      KeyEvent Keys Label Layout LinkLabel ListBox ListView	\
	      Mdi Menu MenuItemEvent MouseEvent Mutex Panel Pen Point	\
	      ProgressBar RadioButton ReBar Rect Region Scintilla Size	\
	      Slider SpinButton Spinner String Style System Tab Thread	\
	      Timer ToggleButton ToolBar TreeNode TreeView		\
	      TreeViewEvent Vaca Widget

EXAMPLES = AnchorLayouts AutoCompletion BouncingBalls BoxLayouts	\
	   Buttons ColoredButton ComboBoxes CommandsAlt DataGrids	\
	   EditableListBox Edits EyeDropper FontMetrics FreeOfLayout	\
	   Hashing HelloWorld Images Labels Maths MenuResource		\
	   MiniExplorer PensBrushes Primitives ProgressBars Regions	\
	   Scribble Sliders Spinners StdCommands Sudoku			\
	   SystemImageList Tabs TextEditor ThreadKiller Threads		\
	   ToolBars Trees

ADDRESSBOOK_SOURCES = AddressBook LightingPanel SectionHeader	\
                      SectionContent

SIZEOF_SOURCES = sizeof

TORO_SOURCES = Document FormDocument FormView TextDocument TextView	\
               ToroApp ToroFrame ToroTab View

VACADOC_SOURCES = CppNode VacaDoc XmlNode XmlParser XmlRawParser	\
		  XmlString

# ----------------------------------------------------------------------
# Flags

CXXFLAGS = -I$(BOOSTDIR) -Iinclude -I. -Ithird_party -DBOOST_BIND_NO_PLACEHOLDERS \
	   -DWINVER=0x0500 -D_WIN32_IE=0x0500 -D_WIN32_WINNT=0x0500 -W -Wall -Wno-unused

ifdef UNICODE
  CXXFLAGS += -DUNICODE -D_UNICODE
endif

DLLFLAGS =
LFLAGS = -mwindows -Lobj
LIBS = -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lwinspool -lwinmm -lshell32 -lshlwapi -lcomctl32 -lole32 -loleaut32 -luuid -lrpcrt4 -ladvapi32 -lwsock32 -lodbc32 -lcomctl32 -lsecur32 -lmsimg32 $(BOOSTLIBS)
EXE_LIBS = $(LIBS)

# ----------------------------------------------------------------------
# STATIC or SHARED?

ifdef STATIC

  BOOSTLIBS = $(BOOSTDIR)/bin/boost/libs/signals/build/libboost_signals.lib/gcc/release/runtime-link-static/libboost_signals-gcc-s-1_33_1.lib

  VACA_LIB_FLAGS = -DVACA_STATIC
  VACA_EXE_FLAGS = -DVACA_STATIC

  ifdef DEBUG
    SUFFIX = _ds
  else
    SUFFIX = _s
  endif

else

  BOOSTLIBS = $(BOOSTDIR)/bin/boost/libs/signals/build/boost_signals.dll/gcc/release/boost_signals-gcc-1_33_1.lib

  VACA_LIB_FLAGS = -DVACA_SRC
  VACA_EXE_FLAGS = 

  ifdef DEBUG
    SUFFIX = _d
  endif

endif

# ----------------------------------------------------------------------
# PROFILE?

ifdef PROFILE
  CXXFLAGS += -pg
  LFLAGS += -pg
  DLLFLAGS += -pg

  ifdef SUFFIX
    SUFFIX := $(SUFFIX)p
  else
    SUFFIX := _p
  endif
endif

# ----------------------------------------------------------------------
# UNICODE

ifdef UNICODE
  ifdef SUFFIX
    SUFFIX := $(SUFFIX)u
  else
    SUFFIX := _u
  endif
endif

# ----------------------------------------------------------------------
# DEBUG or NDEBUG?

ifdef DEBUG
  VACA_LIB_FLAGS += -D_DEBUG
  VACA_EXE_FLAGS += -D_DEBUG
  CXXFLAGS += -g
  LFLAGS += -g
  DLLFLAGS += -g
else
#   VACA_LIB_FLAGS += -DNDEBUG
#   VACA_EXE_FLAGS += -DNDEBUG
  CXXFLAGS += -s
  LFLAGS += -s
  DLLFLAGS += -s
endif

# ----------------------------------------------------------------------
# Files

VACA_LIB = lib/libvaca$(SUFFIX).a
ifdef UNICODE
  VACA_DLL = bin/VacaUnicode.dll
else
  VACA_DLL = bin/Vaca.dll
endif

LIB_OBJS = $(addprefix obj/Library., $(addsuffix $(SUFFIX).o, $(LIB_SOURCES)))
EXAMPLES_EXE = $(addprefix bin/, $(addsuffix $(SUFFIX).exe, $(EXAMPLES)))

EXAMPLES_DIRS_WITH_RC = $(dir $(wildcard examples/*/*.rc))

UTILITIES_EXE = \
	bin/AddressBook$(SUFFIX).exe \
	bin/sizeof$(SUFFIX).exe \
	bin/Toro$(SUFFIX).exe \
	bin/VacaDoc$(SUFFIX).exe

ADDRESSBOOK_OBJS = $(addprefix obj/AddressBook., $(addsuffix $(SUFFIX).o, $(ADDRESSBOOK_SOURCES)))
SIZEOF_OBJS = $(addprefix obj/sizeof., $(addsuffix $(SUFFIX).o, $(SIZEOF_SOURCES)))
TORO_OBJS = $(addprefix obj/Toro., $(addsuffix $(SUFFIX).o, $(TORO_SOURCES)))
VACADOC_OBJS = $(addprefix obj/VacaDoc., $(addsuffix $(SUFFIX).o, $(VACADOC_SOURCES)))

ALL_TARGETS = stdvaca.h.gch $(VACA_DLL) $(EXAMPLES_EXE) $(UTILITIES_EXE)
# ALL_TARGETS = stdvaca.h.gch $(VACA_DLL) bin/Toro$(SUFFIX).exe
# ALL_TARGETS = stdvaca.h.gch $(VACA_DLL) bin/AddressBook$(SUFFIX).exe bin/BoxLayouts$(SUFFIX).exe
# ALL_TARGETS = stdvaca.h.gch $(VACA_DLL) bin/AddressBook$(SUFFIX).exe bin/BoxLayouts$(SUFFIX).exe bin/BouncingBalls$(SUFFIX).exe
# ALL_TARGETS = stdvaca.h.gch $(VACA_DLL) bin/Bixes$(SUFFIX).exe

# ----------------------------------------------------------------------
# Rules

default: all

all: $(ALL_TARGETS)

vpath %.cpp $(addprefix examples/, $(EXAMPLES))
vpath %.rc $(EXAMPLES_DIRS_WITH_RC)

obj/Library.%$(SUFFIX).o: src/%.cpp
	$(GXX) $(CXXFLAGS) $(VACA_LIB_FLAGS) -o $@ -c $<

obj/Example.%$(SUFFIX).o: %.cpp
	$(GXX) $(CXXFLAGS) $(VACA_EXE_FLAGS) -o $@ -c $<

obj/Example.res: examples/Example.rc
	$(WINDRES) -Iexamples/ -O coff -o $@ -i $<

obj/Example.%.res: %.rc
	$(WINDRES) $(addprefix -I, $(EXAMPLES_DIRS_WITH_RC)) -O coff -o $@ -i $<

obj/AddressBook.%$(SUFFIX).o: utilities/AddressBook/%.cpp
	$(GXX) $(CXXFLAGS) $(VACA_EXE_FLAGS) -o $@ -c $<

obj/AddressBook.AddressBook.res: utilities/AddressBook/AddressBook.rc
	$(WINDRES) -Iutilities/AddressBook -O coff -o $@ -i $<

obj/sizeof.%$(SUFFIX).o: utilities/sizeof/%.cpp
	$(GXX) $(CXXFLAGS) $(VACA_EXE_FLAGS) -o $@ -c $<

obj/Toro.%$(SUFFIX).o: utilities/Toro/%.cpp
	$(GXX) $(CXXFLAGS) $(VACA_EXE_FLAGS) -o $@ -c $<

obj/Toro.%.res: utilities/Toro/%.rc
	$(WINDRES) -Iutilities/Toro -O coff -o $@ -i $<

obj/VacaDoc.%$(SUFFIX).o: utilities/VacaDoc/%.cpp
	$(GXX) $(CXXFLAGS) $(VACA_EXE_FLAGS) -o $@ -c $<

%.h.gch: %.h
	$(GXX) $(CXXFLAGS) -o $@ -c $<

ifdef STATIC
$(VACA_LIB): stdvaca.h.gch $(LIB_OBJS)
	$(RM) -f $@
	$(AR) crs $(VACA_LIB) $(LIB_OBJS)
else
$(VACA_LIB): stdvaca.h.gch $(LIB_OBJS)
	$(RM) -f $@

$(VACA_DLL): $(VACA_LIB)
	$(GXX) $(DLLFLAGS) -shared -fPIC -o $(VACA_DLL) $(LIB_OBJS) -Wl,--out-implib,$(VACA_LIB) $(LIBS)
endif

# general examples

bin/%.exe: obj/Example.%.o obj/Example.res $(VACA_LIB)
	$(GXX) $(LFLAGS) -o $@ $^ $(EXE_LIBS)

# special examples

bin/EyeDropper$(SUFFIX).exe: obj/Example.EyeDropper$(SUFFIX).o obj/Example.EyeDropper.res $(VACA_LIB)
	$(GXX) $(LFLAGS) -o $@ $^ $(EXE_LIBS)

bin/Hashing$(SUFFIX).exe: obj/Example.Hashing$(SUFFIX).o obj/Example.md5$(SUFFIX).o obj/Example.sha1$(SUFFIX).o obj/Example.res $(VACA_LIB)
	$(GXX) $(LFLAGS) -o $@ $^ $(EXE_LIBS)

bin/Images$(SUFFIX).exe: obj/Example.Images$(SUFFIX).o obj/Example.Images.res $(VACA_LIB)
	$(GXX) $(LFLAGS) -o $@ $^ $(EXE_LIBS)

bin/MenuResource$(SUFFIX).exe: obj/Example.MenuResource$(SUFFIX).o obj/Example.MenuResource.res $(VACA_LIB)
	$(GXX) $(LFLAGS) -o $@ $^ $(EXE_LIBS)

bin/TextEditor$(SUFFIX).exe: obj/Example.TextEditor$(SUFFIX).o obj/Example.TextEditor.res $(VACA_LIB)
	$(GXX) $(LFLAGS) -o $@ $^ $(EXE_LIBS)

bin/ToolBars$(SUFFIX).exe: obj/Example.ToolBars$(SUFFIX).o obj/Example.ToolBars.res $(VACA_LIB)
	$(GXX) $(LFLAGS) -o $@ $^ $(EXE_LIBS)

# utilities

bin/AddressBook$(SUFFIX).exe: $(ADDRESSBOOK_OBJS) obj/AddressBook.AddressBook.res $(VACA_LIB)
	$(GXX) $(LFLAGS) -s -mwindows -Lobj -o $@ $^ $(EXE_LIBS)

bin/sizeof$(SUFFIX).exe: $(SIZEOF_OBJS) $(VACA_LIB)
	$(GXX) $(LFLAGS) -s -mconsole -Lobj -o $@ $^ $(EXE_LIBS)

bin/Toro$(SUFFIX).exe: $(TORO_OBJS) obj/Toro.resource.res $(VACA_LIB)
	$(GXX) $(LFLAGS) -s -mwindows -Lobj -o $@ $^ $(EXE_LIBS)

bin/VacaDoc$(SUFFIX).exe: $(VACADOC_OBJS) $(VACA_LIB)
	$(GXX) $(LFLAGS) -s -mwindows -Lobj -o $@ $^ $(EXE_LIBS)

deps:
	$(GXX) -MM $(CXXFLAGS) src/*.cpp | sed -e 's|^\([A-Za-z_0-9]\+\)|obj/Library.\1\$$\(SUFFIX\)|' > .deps
	$(GXX) -MM $(CXXFLAGS) examples/*/*.cpp | sed -e 's|^\([A-Za-z_0-9]\+\)|obj/Example.\1\$$\(SUFFIX\)|' >> .deps
	$(GXX) -MM $(CXXFLAGS) utilities/AddressBook/*.cpp | sed -e 's|^\([A-Za-z_0-9]\+\)|obj/AddressBook.\1\$$\(SUFFIX\)|' >> .deps
	$(GXX) -MM $(CXXFLAGS) utilities/Toro/*.cpp | sed -e 's|^\([A-Za-z_0-9]\+\)|obj/Toro.\1\$$\(SUFFIX\)|' >> .deps
	$(GXX) -MM $(CXXFLAGS) utilities/VacaDoc/*.cpp | sed -e 's|^\([A-Za-z_0-9]\+\)|obj/VacaDoc.\1\$$\(SUFFIX\)|' >> .deps

clean:
	-$(RM) -f obj/*.o obj/*.res

distclean: clean
	-$(RM) -f lib/*.a bin/*.exe $(VACA_DLL)

ifdef MINGWDIR
install: $(VACA_LIB) $(VACA_DLL)
	-$(MKDIR) $(MINGWDIR)/include/Vaca
	$(CP) $(wildcard include/Vaca/*.h) $(MINGWDIR)/include/Vaca
	$(CP) $(VACA_LIB) $(MINGWDIR)/lib

uninstall:
	@echo $(RM) $(MINGWDIR)/$(VACA_LIB)
	@echo $(RM) $(wildcard $(MINGWDIR)/include/Vaca/*.h)
	@echo $(RMDIR) $(MINGWDIR)/include/Vaca
else
install:
	@echo You must to setup your MINGWDIR enviroment variable

uninstall:
	@echo You must to setup your MINGWDIR enviroment variable
endif

# ----------------------------------------------------------------------
# Dependencies

-include .deps
