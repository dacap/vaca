#USE_STATIC = 1
#USE_UNICODE = 1
#USE_THREAD = 1 deprecated

ifndef BOOSTDIR
BOOSTDIR = c:/boost_1_33_1
endif

# ifdef USE_THREAD
# BOOSTLIBS = \
#   $(BOOSTDIR)/bin/boost/libs/signals/build/boost_signals.dll/gcc/release/threading-multi/boost_signals-gcc-mt-1_33_1.lib \
#   $(BOOSTDIR)/bin/boost/libs/thread/build/boost_thread.dll/gcc/release/threading-multi/boost_thread-gcc-mt-1_33_1.lib
# else
ifdef USE_STATIC
BOOSTLIBS = \
  $(BOOSTDIR)/bin/boost/libs/signals/build/libboost_signals.lib/gcc/release/runtime-link-static/libboost_signals-gcc-s-1_33_1.lib
else
BOOSTLIBS = \
  $(BOOSTDIR)/bin/boost/libs/signals/build/boost_signals.dll/gcc/release/boost_signals-gcc-1_33_1.lib
endif
# endif

ifdef USE_STATIC
VACA_LIB = lib/libvaca_static.a
VACA_DLL = 
VACA_LIB_FLAGS = -DVACA_STATIC
VACA_EXE_FLAGS = -DVACA_STATIC
else
VACA_LIB = lib/libvaca_shared.a
VACA_DLL = bin/vaca.dll
VACA_LIB_FLAGS = -DVACA_SRC
VACA_EXE_FLAGS = 
endif

# for Win98/Me (without Unicode support)
CXXFLAGS = -I$(BOOSTDIR) -Iinclude -I. -s -DBOOST_BIND_NO_PLACEHOLDERS \
	   -DWINVER=0x0500 -D_WIN32_IE=0x0500 -D_WIN32_WINNT=0x0500

# for Win2K/WinXP (with Unicode support)
ifdef USE_UNICODE
CXXFLAGS += -DUNICODE -D_UNICODE
endif

LFLAGS = -s -mwindows -Lobj
#LIBS = -lcomctl32 -lsecur32 $(BOOSTLIBS)
LIBS = -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lwinspool -lwinmm -lshell32 -lcomctl32 -lole32 -loleaut32 -luuid -lrpcrt4 -ladvapi32 -lwsock32 -lodbc32 -lcomctl32 -lsecur32 -lmsimg32 $(BOOSTLIBS)

ifdef USE_STATIC
EXE_LIBS = $(LIBS)
else
EXE_LIBS = $(BOOSTLIBS)
endif

LIB_OBJS = \
	obj/AnchorLayout.o \
	obj/Application.o \
	obj/BandedDockArea.o \
	obj/BasicDockArea.o \
	obj/BoxLayout.o \
	obj/Button.o \
	obj/ButtonBase.o \
	obj/CheckBox.o \
	obj/ClientLayout.o \
	obj/Clipboard.o \
	obj/Color.o \
	obj/ColorDialog.o \
	obj/Command.o \
	obj/CommonDialog.o \
	obj/Cursor.o \
	obj/CustomButton.o \
	obj/CustomLabel.o \
	obj/Debug.o \
	obj/Dialog.o \
	obj/DockArea.o \
	obj/DockBar.o \
	obj/DockFrame.o \
	obj/Edit.o \
	obj/FileDialog.o \
	obj/FindTextDialog.o \
	obj/Font.o \
	obj/FontDialog.o \
	obj/Frame.o \
	obj/Graphics.o \
	obj/GroupBox.o \
	obj/Icon.o \
	obj/Image.o \
	obj/ImageList.o \
	obj/Keys.o \
	obj/Label.o \
	obj/Layout.o \
	obj/LinkLabel.o \
	obj/ListBox.o \
	obj/Mdi.o \
	obj/Menu.o \
	obj/Mutex.o \
	obj/Panel.o \
	obj/Point.o \
	obj/ProgressBar.o \
	obj/RadioButton.o \
	obj/ReBar.o \
	obj/Rect.o \
	obj/Scintilla.o \
	obj/Size.o \
	obj/Slider.o \
	obj/SpinButton.o \
	obj/Spinner.o \
	obj/String.o \
	obj/Style.o \
	obj/System.o \
	obj/Tab.o \
	obj/Thread.o \
	obj/ToggleButton.o \
	obj/ToolBar.o \
	obj/TreeNode.o \
	obj/TreeView.o \
	obj/Vaca.o \
	obj/Widget.o

EXAMPLES = \
	Anchors \
        Buttons \
        Calendar \
        ColoredButton \
        EditableListBox \
        Edits \
	EyeDropper \
        HelloWorld \
        Labels \
        Primitives \
        ProgressBars \
        Scribble \
        SimpleWorld \
        Sliders \
        Spinners \
        Sudoku \
        Tabs \
        TextEditor \
        Threads \
        TimerThread \
        ToolBars \
        Trees

# EXAMPLES = Scribble
# EXAMPLES = ToolBars

EXAMPLES_EXE = $(addprefix bin/Example., $(addsuffix .exe, $(EXAMPLES)))
EXAMPLES_DIRS_WITH_RC = $(dir $(wildcard examples/*/*.rc))

UTILITIES_EXE = \
	bin/VacaDoc.exe \
	bin/VacaStudio.exe

default: all

all:    stdvaca.h.gch \
	$(VACA_DLL) \
	$(EXAMPLES_EXE) \
	$(UTILITIES_EXE)

# all:    stdvaca.h.gch \
# 	$(VACA_DLL) \
# 	bin/Example.ProgressBars.exe

# all:    stdvaca.h.gch \
# 	$(VACA_DLL) \
# 	bin/Example.Spinners.exe

# all:    stdvaca.h.gch \
# 	$(VACA_DLL) \
# 	bin/Example.Labels.exe

# all:    stdvaca.h.gch \
# 	$(VACA_DLL) \
# 	bin/VacaStudio.exe

# all:    stdvaca.h.gch \
# 	$(VACA_DLL) \
# 	bin/VacaDoc.exe

vpath %.cpp $(addprefix examples/, $(EXAMPLES))
vpath %.rc $(EXAMPLES_DIRS_WITH_RC)

obj/%.o: src/%.cpp
	g++ $(CXXFLAGS) $(VACA_LIB_FLAGS) -o $@ -c $<

obj/Example.res: examples/Example.rc
	windres -Iexamples/ -O coff -o $@ -i $<

obj/Example.%.res: %.rc
	windres $(addprefix -I, $(EXAMPLES_DIRS_WITH_RC)) -O coff -o $@ -i $<

obj/Example.%.o: %.cpp
	g++ $(CXXFLAGS) $(VACA_EXE_FLAGS) -o $@ -c $<

obj/VacaDoc.%.o: utilities/VacaDoc/%.cpp
	g++ $(CXXFLAGS) $(VACA_EXE_FLAGS) -o $@ -c $<

obj/VacaStudio.%.o: utilities/VacaStudio/%.cpp
	g++ $(CXXFLAGS) $(VACA_EXE_FLAGS) -o $@ -c $<

%.h.gch: %.h
	g++ $(CXXFLAGS) -o $@ -c $<

lib/libvaca_static.a: stdvaca.h.gch $(LIB_OBJS)
	rm -f $@
	ar crs $(VACA_LIB) $(LIB_OBJS)

lib/libvaca_shared.a: stdvaca.h.gch $(LIB_OBJS)
	rm -f $@
	g++ -s -shared -fPIC -o bin/vaca.dll $(LIB_OBJS) -Wl,--out-implib,lib/libvaca_shared.a $(LIBS)

bin/vaca.dll: lib/libvaca_shared.a

bin/Example.%.exe: obj/Example.%.o obj/Example.res $(VACA_LIB)
	g++ $(LFLAGS) -o $@ $^ $(EXE_LIBS)

bin/Example.EyeDropper.exe: obj/Example.EyeDropper.o obj/Example.EyeDropper.res $(VACA_LIB)
	g++ $(LFLAGS) -o $@ $^ $(EXE_LIBS)

bin/Example.TextEditor.exe: obj/Example.TextEditor.o obj/Example.TextEditor.res $(VACA_LIB)
	g++ $(LFLAGS) -o $@ $^ $(EXE_LIBS)

bin/Example.ToolBars.exe: obj/Example.ToolBars.o obj/Example.ToolBars.res $(VACA_LIB)
	g++ $(LFLAGS) -o $@ $^ $(EXE_LIBS)

bin/VacaDoc.exe: \
	obj/VacaDoc.VacaDoc.o \
	obj/VacaDoc.XmlNode.o \
	obj/VacaDoc.XmlParser.o \
	obj/VacaDoc.XmlRawParser.o \
	obj/VacaDoc.XmlString.o \
	$(VACA_LIB)
	g++ $(LFLAGS) -o $@ $^ $(EXE_LIBS)

bin/VacaStudio.exe: \
	obj/VacaStudio.FRFiles.o \
	obj/VacaStudio.main.o \
	obj/VacaStudio.MainFrame.o \
	obj/VacaStudio.VacaStudio.o \
	obj/VacaStudio.VsMdi.o \
	$(VACA_LIB)
	g++ $(LFLAGS) -o $@ $^ $(EXE_LIBS)

deps:
	g++ -MM $(CXXFLAGS) src/*.cpp | sed -e 's|^\([A-Za-z_0-9]\)|obj/\1|' > .deps
	g++ -MM $(CXXFLAGS) examples/*/*.cpp | sed -e 's|^\([A-Za-z_0-9]\)|obj/Example.\1|' >> .deps
	g++ -MM $(CXXFLAGS) utilities/VacaDoc/*.cpp | sed -e 's|^\([A-Za-z_0-9]\)|obj/VacaDoc.\1|' >> .deps
	g++ -MM $(CXXFLAGS) utilities/VacaStudio/*.cpp | sed -e 's|^\([A-Za-z_0-9]\)|obj/VacaStudio.\1|' >> .deps

clean:
	-rm $(LIB_OBJS) lib/libvaca_shared.a bin/*.exe

-include .deps
