# Vaca - Visual Application Components Abstraction
# Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
# All rights reserved.

all:
	@echo --- Compiler ---
	@echo Use "make -f Makefile.mgw" for MinGW
	@echo Use "nmake -f Makefile.msc" for Microsoft Visual C
	@echo --- Macros you can define ---
	@echo RELEASE=1 ... build for release, without assertions
	@echo DEBUG=1 ..... build for debug, with debug information
	@echo STATIC=1 .... build the static version, no DLL
	@echo PROFILE=1 ... build the version to do profiling, e.g. with gprof
	@echo UNICODE=1 ... build the library with Unicode support
	@echo --- Targets ---
	@echo all: ........ build everything
	@echo lib: ........ build the library
	@echo examples: ... build the examples
	@echo tests: ...... build the tests
	@echo runtests: ... run the test
	@echo clean: ...... remove .o .obj .res files
	@echo distclean: .. remove .exe .lib .dll files
	@echo --- Example ---
	@echo make -f Makefile.mgw DEBUG=1 UNICODE=1
	@echo Builds the DLL with debug information with Unicode support.
