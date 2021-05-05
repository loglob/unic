INSTALL_DIR=/usr/include/unic

lib/libunic.so: lib/unic.o
	cc -shared -o $@ $<

lib/unic.o: src/unic.c src/*.h include/unic.h
	mkdir -p lib
	cc -fpic -Ofast -Wall -Wextra -c $< -o $@

precompile: uchar.gch util.gch utf8.gch u8string.gch

%.gch: %.h 
	c99 -Wall -Wextra $<

install:
	mkdir -p $(INSTALL_DIR)
	cp $(CURDIR)/*.*h $(INSTALL_DIR)

clean:
	rm -fr lib out

uninstall:
	rm -r $(INSTALL_DIR)

.PHONY: test
test: out/test
	./$<

out/test: test/test.c lib/libunic.so test/*.h
	mkdir -p out
	cc -L./lib/ -Wl,-rpath=./lib -Wall -Wextra -g $< -o $@ -lunic