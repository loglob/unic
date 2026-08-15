#!/usr/bin/bash

set -e

export OPT_CFLAGS="-O3 -march=x86-64-v2 -mtune=generic"
make codegen
make -j4 out/libunic.so out/libunic.a
make doc

tar czf "unic-$1-linux-amd64".tar.gz \
  include \
  --transform 's|^out/|lib/|' out/libunic.so out/libunic.a \
  --transform 's|^doc/||' doc/man
