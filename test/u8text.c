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
	} while((size_t)delta == grain);

	u8file_t f = u8txt_load(buffer, size, u8txt_cleanup_free);

	if(! f)
		testFailure("Internal error: %s\n", strerror(errno));

	return f;
}

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
		u8nenc(chr, l, buffer + size);
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
	size_t format_##type (char *to, size_t n, const type thing[restrict static 1]) { (void)to; (void)n; (void)thing; return 0; }

MAKE_PROVIDERS(Bible, "testdata/kjv.txt")
MAKE_PROVIDERS(Grundgesetz, "testdata/gg.txt")

/** Runs a u8file_t test on both text files. Expects a following function body.
	@param test Name of the test
	@param arg Name of the u8file_t argument to accept
*/
#define TEST_ALL(test, arg) \
	static void _##test(u8file_t arg); \
	TEST(Bible_##test, Bible, arg) { _##test(arg); } \
	TEST(GG_##test, Grundgesetz, arg) { _##test(arg); } \
	static void _##test(u8file_t arg)

TEST_ALL(strsize, f)
{
	u8size_t sizeFromBytes = u8z_strsize(f->bytes, EXACT_BYTES(f->size.byteCount));
	assertUEq(f->size.charCount, sizeFromBytes.charCount);

	u8size_t sizeFromChars = u8z_strsize(f->bytes, EXACT_CHARS(f->size.charCount));
	assertUEq(f->size.byteCount, sizeFromChars.byteCount);
}

TEST_ALL(isvalid, f)
{
	assertTrue( u8z_isvalid(f->bytes, f->size) );
}

static const size_t BIBLE_CHARS = 4602959;
static const size_t BIBLE_NORM_BYTES = 4606957;

TEST(Bible_knownSize, Bible, f)
{
	assertUEq(BIBLE_CHARS, f->size.charCount);
	assertUEq(31105, f->lines);
	assertTrue(f->size.byteCount >= BIBLE_NORM_BYTES, " but got %zu", (size_t)f->size.byteCount); // original encoding size
	assertTrue(f->size.byteCount <= BIBLE_CHARS * UTF8_MAX, " but got %zu", (size_t)f->size.byteCount);
}

static const size_t GG_CHARS = 179180;
static const size_t GG_NORM_BYTES = 181906;

TEST(GG_knownSize, Grundgesetz, f)
{
	assertUEq(GG_CHARS, f->size.charCount);
	assertUEq(3715, f->lines);
	assertTrue(f->size.byteCount >= GG_NORM_BYTES); // original encoding size
	assertTrue(f->size.byteCount <= GG_CHARS * UTF8_MAX);
}

__nonnull((1,2))
static inline void assertPrefix(const char *prefix, const char *string)
{
	assertTrue(u8_prefix(prefix, string), " but got: '%.10s…'", string);
}

TEST(GG_unLoc, Grundgesetz, f)
{
	size_t ix;
	const char *hit = u8txt_unLoc(f, 28, 22, &ix);
	assertTrue(hit != NULL);
	assertPrefix("Rheinland-Pfalz", hit);
	assertUEq(1060, ix);
}

TEST(GG_line, Grundgesetz, f)
{
	u8size_t size;
	const char *line = u8txt_line(f, 40, &size);

	assertTrue(line != NULL);
	assertTrue(size.bytesExact);
	assertTrue(size.charsExact);
	assertPrefix("(1) Die Würde des Menschen ist unantastbar", line);
	assertUEq(65, size.charCount);

	if(f->size.byteCount == GG_NORM_BYTES)
		assertUEq(66, size.byteCount);
}

TEST(GG_text_search, Grundgesetz, f)
{
	const char needle[] = "Artikel 100";
	const char *hit = u8z_strstr(f->bytes, f->size, needle, NUL_TERMINATED);

	assertTrue(hit != NULL);
	assertPrefix(needle, hit);

	// resolve location
	u8loc_t loc;
	assertIEq(0, u8txt_loc(f, hit, &loc));
	assertUEq(2236, loc.line);
	assertUEq(1, loc.column);
	assertUEq(105779, loc.characterIndex);
	assertUEq(0, loc.charOff);
}

TEST(GG_chr, Grundgesetz, f)
{
	u8loc_t loc;
	const size_t ix = 51289;
	const char *hit = u8txt_chr(f, ix, &loc);

	assertTrue(hit != NULL);
	assertPrefix("Artikel 59", hit);
	assertUEq(ix, loc.characterIndex);
	assertUEq(1115, loc.line);
	assertUEq(1, loc.column);
	assertUEq(0, loc.charOff);
}

TEST(GG_loc_round_trip, Grundgesetz, f)
{
	size_t ix;
	const char *pos = u8txt_unLoc(f, 3031, 1, &ix);
	assertTrue(pos != NULL);
	assertUEq(146988, ix);
	assertPrefix("XI. ÜBERGANGS- UND SCHLUSSBESTIMMUNGEN", pos);

	u8loc_t loc;
	assertUEq(0, u8txt_loc(f, pos, &loc));
	assertIEq(0, loc.charOff);
	assertUEq(146988, loc.characterIndex);
	assertUEq(3031, loc.line);
	assertUEq(1, loc.column);
}

TEST(Bible_text_search, Bible, f)
{
	const char *caseSens = u8z_strstr(f->bytes, f->size, "Seek ye the LORD", NUL_TERMINATED);
	const char *caseInsens = u8z_strstrI(f->bytes, f->size, "seek ye the lord", NUL_TERMINATED);

	assertPEq(caseSens, caseInsens);
	assertTrue(caseSens != NULL);

	u8loc_t loc;
	assertIEq(0, u8txt_loc(f, caseSens, &loc));

	assertUEq(0, loc.charOff);
	assertUEq(2781136, loc.characterIndex);
	assertUEq(18749, loc.line);
	assertUEq(13, loc.column);

	u8size_t lineSize;
	const char *line = u8txt_line(f, loc.line, &lineSize);
	assertTrue(line != NULL);
	assertPrefix("Isaiah 55:6", line);
	assertTrue(lineSize.bytesExact);
	assertTrue(lineSize.charsExact);
	assertUEq(87, lineSize.charCount);

	if(f->size.byteCount == BIBLE_NORM_BYTES)
		assertUEq(87, lineSize.byteCount);
}

TEST(Bible_location_search, Bible, f)
{
	size_t ix;
	const char *str = u8txt_unLoc(f, 17774, 31, &ix);

	assertTrue(str != NULL);
	assertPrefix("seraphims", str);
	assertUEq(2621402, ix);
}

TEST(Bible_chr, Bible, f)
{
	u8loc_t loc;
	const size_t ix = 2463663 + 1;
	const char *hit = u8txt_chr(f, ix, &loc);

	assertTrue(hit != NULL);
	assertPrefix("find the knowledge of God.", hit);

	assertUEq(0, loc.charOff);
	assertUEq(ix, loc.characterIndex);
	assertUEq(16441, loc.line);
	assertUEq(67, loc.column);
}

/** Check for the proper byte order mark */
TEST(Bible_BOM, Bible, f)
{
	uchar_t bom;
	size_t l = u8dec(f->bytes, &bom);

	if(f->size.byteCount == BIBLE_NORM_BYTES)
		assertUEq(3, l);

	assertUEq(0xFEFF, bom); // Zero-width nonbreaking space

	assertPrefix("KJV", f->bytes + l);
}

TEST(mmapFree)
{
	u8file_t f = mmapFile("testdata/kjv.txt");
	u8txt_free(f);
}

TEST(mallocFree)
{
	u8file_t f = readFile("testdata/kjv.txt");
	u8txt_free(f);
}

TEST(nullFree)
{
	const char data[] = "THis is meaningless example text.";
	u8file_t f = u8txt_load(data, sizeof(data) - 1, NULL);
	u8txt_free(f);
}