.PHONY: test unic install uninstall

unic: lib/libunic.so

test: out/test
	./$<

out/test: test/test.c out/debug.o test/*.h
	mkdir -p out
	cc -Wall -Wextra -g $< out/debug.o -o $@ -lexplain

lib/libunic.so: out/unic.o
	mkdir -p lib
	cc -shared $< -o $@

out/unic.o: src/unic.c src/*.h include/unic.h
	mkdir -p out
	cc -fpic -Ofast -Wall -Wextra -c $< -o $@

out/debug.o: src/unic.c src/*.h include/unic.h
	mkdir -p out
	cc -g -Wall -Wextra -c $< -o $@

install: lib/libunic.so
	cp $< /usr/lib/
	cp include/unic.h /usr/include

uninstall:
	rm -f /usr/lib/libunic.so /usr/include/unic.h

clean:
	rm -fr lib out
