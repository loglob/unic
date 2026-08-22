#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Usage: $0 [version]" 1>&2
    exit 1
fi

set -e

case "$(uname -s)-$(uname -m)" in
  Linux-x86_64)
    MARCH="-march=x86-64-v2"
    ;;
  Darwin-arm64)
    MARCH="-mcpu=apple-m1"
    ;;
  Linux-aarch64|Linux-arm64)
    MARCH="-march=armv8-a"
    ;;
  *)
    ;;
esac

export OPT_CFLAGS="-O3 $MARCH -mtune=generic -flto"
make -j4 build
make doc

# build package in a temp dir
DIR="$(mktemp -d)"

cp -r README.md LICENSE include doc/man "$DIR/"
mkdir "$DIR/lib"
cp out/libunic.so out/libunic.a "$DIR/lib/"
echo "$1" > "$DIR/version"

# this is ugly but tar is incredibly inflexible
(cd "$DIR"; tar -czf "unic-$1-$(uname -s)-$(uname -m)".tar.gz *)
mv "$DIR"/*.tar.gz .
rm -r "$DIR"
