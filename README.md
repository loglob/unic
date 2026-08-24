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
UNIC_VERSION ?= v1.1.0

UNIC_DIR = .unic/$(UNIC_VERSION)
UNIC_OBJ = $(UNIC_DIR)/lib/libunic.a
UNIC_INCLUDE = $(UNIC_DIR)/include/unic.h $(UNIC_DIR)/include/u8text.h
CFLAGS += -I$(UNIC_DIR)/include
LDFLAGS += $(UNIC_OBJ)

$(UNIC_OBJ): $(UNIC_DIR)
	make -C $< lib/libunic.a
$(UNIC_INCLUDE): $(UNIC_DIR)
.unic/v%:
	if [ ! -d $@ ]; then \
		mkdir -p .unic; \
		V=`basename $@`; \
		wget -nv https://github.com/loglob/unic/releases/download/$$V/unic-$$V-source.tar.gz -O $@.tar.gz && \
		mkdir -p $@ && \
		tar -xzf $@.tar.gz -C $@ ; \
	fi
```

### Static Linking via Binary Package
The equivalent setup for a binary package would be:
```make
UNIC_VERSION ?= v1.1.0

UNIC_DIR = .unic/$(UNIC_VERSION)
UNIC_OBJ = $(UNIC_DIR)/lib/libunic.a
UNIC_INCLUDE = $(UNIC_DIR)/include/unic.h $(UNIC_DIR)/include/u8text.h
CFLAGS += -I$(UNIC_DIR)/include
LDFLAGS += $(UNIC_OBJ)

$(UNIC_OBJ) $(UNIC_INCLUDE): $(UNIC_DIR)
.unic/v%:
	if [ ! -d $@ ]; then \
		mkdir -p .unic; \
		V=`basename $@`; \
		wget -nv https://github.com/loglob/unic/releases/download/$$V/unic-$$V-$(shell uname -s)-$(shell uname -m).tar.gz -O $@.tar.gz && \
		mkdir -p $@ && \
		tar -xzf $@.tar.gz -C $@ ; \
	fi
```
