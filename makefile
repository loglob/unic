# cflags needed for every build
CFLAGS ?= $(shell cat compile_flags.txt)
# cflags needed only for release builds
OPT_CFLAGS ?= -O3 -march=native -mtune=native
OPT_CFLAGS += $(CFLAGS)

OBJECTS = $(patsubst src/%.c,out/%.o, $(wildcard src/*.c))
DEBUG_OBJECTS = $(patsubst %.o,%-debug.o, $(OBJECTS))
TEST_OBJECTS = $(patsubst %.c,%.so, $(wildcard test/*.c))

.PHONY: test install uninstall --

out/libunic.so: $(OBJECTS)
	cc $(OPT_CFLAGS) -shared $^ -o $@

out/libunic-debug.so: $(DEBUG_OBJECTS)
	cc $(CFLAGS) -g -fPIC -shared $^ -o $@

test/%.so: test/%.c test/*.h ccheck ccheck/*.h include/*
	cc $(CFLAGS) -g -fPIC -shared $< -o $@

test: ccheck/ccheck out/libunic.so -- ccheck/integer-provider.so $(TEST_OBJECTS)
	make -C testdata
	./$^

out/test: test/test.c $(DEBUG_OBJECTS) test/*.h
	mkdir -p out
	cc $(CFLAGS) -g $< $(DEBUG_OBJECTS) -o $@

out/%.o: src/%.c src/*.h include/*
	mkdir -p out
	cc $(OPT_CFLAGS) -fpic -c $< -o $@

out/%-debug.o: src/%.c src/*.h include/*
	mkdir -p out
	cc $(CFLAGS) -g -fPIC -c $< -o $@

doc: unic.dox include/unic.h
	mkdir -p doc
	doxygen $<

install: out/libunic.so
	cp $< /usr/lib/
	cp include/unic.h /usr/include/


ccheck/ccheck ccheck/integer-provider.so ccheck/interface.h:
	if [ ! -d ccheck ]; then git clone https://github.com/loglob/ccheck; fi
	make -C ccheck

uninstall:
	rm -f /usr/lib/libunic.so /usr/include/unic.h

clean:
	rm -fr out doc
