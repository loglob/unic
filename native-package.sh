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

tar czf "unic-$1-$(uname -s)-$(uname -m)".tar.gz \
  README.md \
  LICENSE \
  include \
  --transform 's|^doc/||' doc/man \
  --transform 's|^out/|lib/|' out/libunic.so out/libunic.a \
