#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <u8text.h>
#include "common.h"
#include "interface.h"
#include "unic.h"
#include <fcntl.h>
#include <unistd.h>

/** Example of simple ASCII text */
typedef u8file_t Bible;
/** Contains Latin-1 (i.e. 2-byte) sequences */
typedef u8file_t Grundgesetz;

/** Opens a file via mmaping */
static u8file_t mmapFile(const char *path)
{
	int fd = open(path, O_RDONLY);

	if(fd == -1)
		testFailure("Could not open '%s': %s\n", path, strerror(errno));

	u8file_t f = u8txt_open(fd);

	if(! f)
		testFailure("Internal error: %s\n", strerror(errno));

	close(fd);
	return f;
}

/** Opens a file via reading into dynamic array */
static u8file_t readFile(const char *path)
{
	int fd = open(path, O_RDONLY);

	if(fd == -1)
		testFailure("Could not open '%s': %s\n", path, strerror(errno));

	char *buffer = NULL;
	size_t size = 0;
	const size_t grain = 4096;
	ssize_t delta;

	do
	{
		buffer = realloc(buffer, size + grain);

		if(! buffer)
			testFailure("Malloc failure");

		delta = read(fd, buffer + size, grain);

		if(delta < 0)
			testFailure("read(): %s", strerror(errno));

		size += delta;
	} while(delta == grain);

	u8file_t f = u8txt_load(buffer, size, u8txt_cleanup_free);

	if(! f)
		testFailure("Internal error: %s\n", strerror(errno));

	return f;
}

extern void u8lenc(uchar_t, size_t l, char buf[restrict static l]);

/** Loads a file with randomly over-encoded characters */
static u8file_t jumbleFile(const char *path)
{
	FILE *f = fopen(path, "rb");

	if(! f)
		testFailure("fopen(): %s", strerror(errno));

	uchar_t chr;
	char *buffer = NULL;
	size_t size = 0, cap = 0;
	const size_t grain = 4096;
	
	while(chr = fgetu8(f), chr != UEOF)
	{
		if(cap - size < UTF8_MAX)
		{
			cap += grain;
			buffer = realloc(buffer, cap);

			if(! buffer)
				testFailure("Malloc failure");
		}

		size_t l = u8enc(chr, NULL);

		if(l != UTF8_MAX)
			l += rand() % (1 + UTF8_MAX - l);
		
		assertTrue(l <= UTF8_MAX);
		u8lenc(chr, l, buffer + size);
		size += l;
	}

	fclose(f);
	u8file_t uf = u8txt_load(buffer, size, u8txt_cleanup_free);

	if(! uf)
		testFailure("Internal error: %s", strerror(errno));

	return uf;
}

#define MAKE_PROVIDERS(type, file) \
	PROVIDER(type, mmap##type) \
	PROVIDER(type, read##type) \
	PROVIDER(type, jumbled##type) \
	size_t mmap##type  (size_t n, type buf[restrict static n]) { if(n) buf[0] = mmapFile(file);  return 1; } \
	size_t read##type  (size_t n, type buf[restrict static n]) { if(n) buf[0] = readFile(file);  return 1; } \
	size_t jumbled##type  (size_t n, type buf[restrict static n]) { if(n) buf[0] = jumbleFile(file);  return 1; } \
	size_t format_##type (char *to, size_t n, const type thing[restrict static 1]) { (void)to; (void)n; (void)thing; return 0; } \
	TEST(sanityCheck_##type, type, x) { u8size_t size = u8z_strsize(x->bytes, EXACT_BYTES(x->size.byteCount)); assertUEq(size.charCount, x->size.charCount); }

MAKE_PROVIDERS(Bible, "testdata/kjv.txt")
MAKE_PROVIDERS(Grundgesetz, "testdata/gg.txt")

TEST(bible_knownSize, Bible, f)
{
	assertUEq(4602959, f->size.charCount);
	assertUEq(31105, f->lines);
	assertTrue(f->size.byteCount >= 4606957); // original encoding size
	assertTrue(f->size.byteCount <= 4602959 * UTF8_MAX);
}

TEST(gg_knownSize, Grundgesetz, f)
{
	assertUEq(179180, f->size.charCount);
	assertUEq(3715, f->lines);
	assertTrue(f->size.byteCount >= 181906); // original encoding size
	assertTrue(f->size.byteCount <= 179180 * UTF8_MAX);
}
