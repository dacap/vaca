DOXYGEN = "$(PROGRAMFILES)/doxygen/bin/doxygen.exe"

default: all

all: html chm private

_norule:

html: _norule
	-mkdir html
	$(DOXYGEN) Doxyfile

chm:
	-mkdir html
	$(DOXYGEN) Doxyfile_chm

private:
	-mkdir html
	$(DOXYGEN) Doxyfile_pri

web:
	-mkdir html
	$(DOXYGEN) Doxyfile_web

gen1:
	$(DOXYGEN) -g new_Doxyfile

gen2:
	$(DOXYGEN) -w html new_header.html new_footer.html new_style.css new_Doxyfile

clean:
	-$(RM) -r html
