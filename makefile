CC = xargs -t -a compile_flags.txt -- cc
CFLAGS ?= -O3 -march=native
OBJECTS = $(patsubst src/%.c,out/%.o, $(wildcard src/*.c))
DEBUG_OBJECTS = $(patsubst %.o,%-debug.o, $(OBJECTS))

.PHONY: test unic install uninstall

out/libunic.so: $(OBJECTS)
	cc -shared $^ -o $@

test: out/test
	./$<

out/test: test/test.c $(DEBUG_OBJECTS) test/*.h
	mkdir -p out
	$(CC) -g $< $(DEBUG_OBJECTS) -o $@ -lexplain

out/%.o: src/%.c src/*.h include/*
	mkdir -p out
	$(CC) $(CFLAGS) -fpic -c $< -o $@

out/%-debug.o: src/%.c src/*.h include/*
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
