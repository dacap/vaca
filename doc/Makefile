DOXYGEN = "$(PROGRAMFILES)/doxygen/bin/doxygen.exe"

all:
	$(DOXYGEN) Doxyfile

gen1:
	$(DOXYGEN) -g new_Doxyfile

gen2:
	$(DOXYGEN) -w html new_header.html new_footer.html new_style.css new_Doxyfile

clean:
	-$(RM) html/*.html html/*.css html/*.gif html/*.png html/index.*
