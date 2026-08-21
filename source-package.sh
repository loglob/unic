#!/bin/bash
set -e

if [ $# -ne 1 ]
then
    echo "Usage: $0 [version]" 1>&2
    exit 1
fi

make codegen

# build package in a temp dir 
DIR="$(mktemp -d)"

cp -r README.md LICENSE src include "$DIR/"
cp src-gen/* "$DIR/src/"

# copy over compile flags, sans include flags
FLAGS="$(grep -v '^-I' compile_flags.txt | tr '\n' ' ')"

# generate a minimal distributable makefile
cat > "$DIR/makefile" << EOF
OBJECTS = \$(patsubst src/%.c, build/%.o, \$(wildcard src/*.c))

lib/libunic.a: \$(OBJECTS)
	mkdir -p lib
	ar rcs \$@ $^

lib/libunic.so: \$(OBJECTS)
	mkdir -p lib
	\$(CC) -flto -shared \$^ -o \$@ 

build/%.o: src/%.c src/*.h include/*
	mkdir -p build
	\$(CC) $FLAGS -O3 -march=native -mtune=native -flto -Iinclude -c $< -o \$@
EOF

tar -C "$DIR" -czf "unic-$1-source".tar.gz --transform 's|^.||' .
rm -r "$DIR"
