DOXYGEN = "$(PROGRAMFILES)/doxygen/bin/doxygen.exe"

all: html chm private

html:
	$(DOXYGEN) Doxyfile

chm:
	$(DOXYGEN) Doxyfile_chm

private:
	$(DOXYGEN) Doxyfile_private

gen1:
	$(DOXYGEN) -g new_Doxyfile

gen2:
	$(DOXYGEN) -w html new_header.html new_footer.html new_style.css new_Doxyfile

clean:
	-$(RM) html/*.html html/*.css html/*.gif html/*.png html/index.*
