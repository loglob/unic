# unic
A unicode library for C. Supports unicode general categories, simple case mappings and utf-8.
When compiled into a binary, the embedded unicode character database takes up about 321 KiB.

## how to build
Running ```make precompile``` will precompile the headers to speed up compile time when including them (this only works with gcc).

Running ```make install``` will copy the headers into */usr/include*, allowing you to import them with ```#include <unic/*.h>```.

The created files can be removed by running ```make uninstall```.
