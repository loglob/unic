CC=xargs -a compile_flags.txt -- cc

.PHONY: test unic install uninstall

out/libunic.so: out/unic.o
	cc -shared $< -o $@

test: out/test
	./$<

out/test: test/test.c out/debug.o test/*.h
	mkdir -p out
	$(CC) -g $< out/debug.o -o $@ -lexplain

out/unic.o: src/unic.c src/*.h include/unic.h
	mkdir -p out
	$(CC) $(CFLAGS) -fpic -c $< -o $@

out/debug.o: src/unic.c src/*.h include/unic.h
	mkdir -p out
	$(CC) -g -c $< -o $@

doc: unic.dox include/unic.h
	mkdir -p doc
	doxygen $<

install: out/libunic.so
	cp $< /usr/lib/
	cp include/unic.h /usr/include/

uninstall:
	rm -f /usr/lib/libunic.so /usr/include/unic.h

clean:
	rm -fr out doc
