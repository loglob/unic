INSTALL_DIR=/usr/include/unic

precompile: uchar.gch util.gch utf8.gch

%.gch: %.h 
	c99 -Wall -Wextra $<

install:
	mkdir -p $(INSTALL_DIR)
	cp $(CURDIR)/*.*h $(INSTALL_DIR)

clean:
	rm *.gch

uninstall:
	rm -r $(INSTALL_DIR)

