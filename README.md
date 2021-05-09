# unic
A unicode library for C. Supports unicode general categories, simple case mappings and utf-8.

## building
Run `make` to build the library binary to `out/libunic.so`.

## installing
Run `make install` to build the library and copy it to `/usr/lib/` as well as copying a header to `/usr/include`.

## linking and using
Compile with `-lunic` and `#include <unic.h>` to use the library.

If you want to statically link unic without installing it, you can use the `out/unic.o` object.

Run `make doc` to generate documentation to `doc`
