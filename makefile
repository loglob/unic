INSTALL_DIR=/usr/include/unic

lib/unic: src/unic.c src/*.h include/unic.h
	mkdir -p lib
	gcc -Ofast -Wall -Wextra -c $< -o $@

precompile: uchar.gch util.gch utf8.gch u8string.gch

%.gch: %.h 
	c99 -Wall -Wextra $<

install:
	mkdir -p $(INSTALL_DIR)
	cp $(CURDIR)/*.*h $(INSTALL_DIR)

clean:
	rm -r lib

uninstall:
	rm -r $(INSTALL_DIR)

out/test: test/test.c lib/unic
	mkdir -p out
	c99 -Wall -Wextra $< ./lib/unic -o $@
	./$@