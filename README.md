# unic
A unicode library for C. Supports unicode general categories, simple case mappings and utf-8.

Also implements a large text type for O(1) mapping between byte offsets, character indices, and line/column positions in UTF-8 text.

## Packages
From version 1.0.2 onwards, Unic is distributed via Github releases.
Binary packages are available for Linux on arm64/x86_64, and for macOS on Apple Silicon.
A generic source package is available for any platform.

### Static Linking via Source
To use the source package via `make`, you can use a rule like this:
```make
UNIC_VERSION=v1.0.2

unic/lib/libunic.a: unic
	make -C $< lib/libunic.a
unic/lib/libunic.so: unic
	make -C $< lib/libunic.so
unic/include/unic.h unic/include/u8text.h: unic
.PHONY: unic # must run in case UNIC_VERSION changes
unic:
	if [ ! -d unic ] || [ `cat unic/version` != "$(UNIC_VERSION)" ]; then \
		wget -nv https://github.com/loglob/unic/releases/download/$(UNIC_VERSION)/unic-$(UNIC_VERSION)-source.tar.gz -O unic.tar.gz && \
		mkdir -p unic && \
		tar -xzf unic.tar.gz -C unic ; \
	fi
```

### Static Linking via Binary Package
The equivalent setup for a binary package would be:
```make
UNIC_VERSION=v1.0.2

unic/lib/libunic.so unic/lib/libunic.a unic/include/unic.h unic/include/u8text.h: unic
.PHONY: unic # must run in case UNIC_VERSION changes
unic:
	if [ ! -d unic ] || [ `cat unic/version` != "$(UNIC_VERSION)" ]; then \
		wget -nv https://github.com/loglob/unic/releases/download/$(UNIC_VERSION)/unic-$(UNIC_VERSION)-$(shell uname -s)-$(shell uname -m).tar.gz -O unic.tar.gz && \
		mkdir -p unic && \
		tar -xzf unic.tar.gz -C unic ; \
	fi
```
