#include "common.h"
#include "unic.h"

/** Checks that u8_strncpy writes only the NUL terminator when n=0 */
TEST(zero_char_strcpy, str_t, str)
{
	char scratch[10] = "aaaaaaaaaa";

	assertIEq(1, u8z_strcpy(str.bytes, MAX_CHARS(0), NULL, 1024, true).byteCount);
	assertIEq(1, u8z_strcpy(str.bytes, MAX_CHARS(0), scratch, sizeof(scratch), true).byteCount);
	assertTrue(*scratch == 0);
	
	for(size_t i = 1; i < sizeof(scratch); ++i)
		assertCEq('a', scratch[i], "Canary at index %zu overwritten", i);
}

/** Checks that u8_strccpy writes only the NUL terminator when c=0 */
TEST(zero_byte_strcpy, str_t, str)
{
	char scratch[10] = "aaaaaaaaaa";

	assertIEq(1, u8z_strcpy(str.bytes, MAX_BYTES(0), NULL, 1024, true).byteCount);
	assertIEq(1, u8z_strcpy(str.bytes, MAX_BYTES(0), scratch, sizeof(scratch), true).byteCount);
	assertTrue(*scratch == 0);
	
	for(size_t i = 1; i < sizeof(scratch); ++i)
		assertCEq('a', scratch[i], "Canary at index %zu overwritten", i);
}

/** Ensures that u8*_strlen() on the entire string is correct */
TEST(strlen_correct, str_t, str)
{
	assertIEq(str.count, u8_strlen(str.bytes));
	assertIEq(str.count, u8z_strlen(str.bytes, MAX_BYTES(str.size)));
	assertIEq(str.count, u8z_strlen(str.bytes, MAX_CHARS(str.count)));
}

/** Ensures that u8z_strsize() on the entire string is correct */
TEST(strsize_correct, str_t, str)
{
	assertIEq(str.count, u8z_strsize(str.bytes, NUL_TERMINATED).charCount);
	assertIEq(str.size, u8z_strsize(str.bytes, NUL_TERMINATED).byteCount);

	assertIEq(str.count, u8z_strsize(str.bytes, MAX_BYTES(str.size)).charCount);
	assertIEq(str.size, u8z_strsize(str.bytes, MAX_BYTES(str.size)).byteCount);

	assertIEq(str.count, u8z_strsize(str.bytes, MAX_CHARS(str.count)).charCount);
	assertIEq(str.size, u8z_strsize(str.bytes, MAX_CHARS(str.count)).byteCount);
}

/** Ensures that indexing is correct for valid and excess indices */
TEST(indexing_correct, str_t, str, uint16_t, i)
{
	if(i < str.count)
		assertCEq(str.chars[i], u8_strat(str.bytes, i));
	else
		assertIEq(0, u8_strat(str.bytes, i));
}

/** u8_strchr must be identical to ascii strchr for ascii characters */
TEST(ascii_strchr, str_t, str, char, ascii)
{
	assertPEq(strchr(str.bytes, ascii), u8_strchr(str.bytes, ascii));
	assertPEq(strrchr(str.bytes, ascii), u8_strrchr(str.bytes, ascii));
}

TEST(streq_reflexive, str_t, str)
{
	assertTrue(u8_streq(str.bytes, str.bytes));
	assertTrue(u8_streqI(str.bytes, str.bytes));
}

TEST(streq_nontrivial, str_t, a, str_t, b)
{
	if(a.bytes == b.bytes)
		return;

	assertTrue(! u8_streq(a.bytes, b.bytes));
	assertTrue(! u8_streqI(a.bytes, b.bytes));
}

TEST(streq_overencode)
{
	const char *normal = "foo" UNUL "bar";
	const char *overenc = "\xC1\xA6" "oo" UNUL "bar";
	
	assert(u8_streq(normal, overenc));
	assert(u8_streq(overenc, normal));
	assert(!u8_streq(normal, "foo"));
	assert(!u8_streq(overenc, "foo"));
	assert(!u8_streq(normal, "foobar"));
	assert(!u8_streq(overenc, "foobar"));

	assert(u8_strneq("foo", normal, 3));
	assert(u8_strneq(normal, "foo", 3));
	assert(!u8_strneq("foo", normal, 4));
	assert(!u8_strneq(normal, "foo", 4));
}

TEST(all_isnorm, str_t, str)
{
	assertTrue(u8_isvalid(str.bytes));
	assertTrue(u8_isnorm(str.bytes));
}

TEST(isnorm_example)
{
	const char *properNUL = "foo" UNUL "bar";
	const char *improperNUL = "foo" "\xE0\x80\x80" "bar";
	
	assert(u8_isnorm(properNUL));
	assert(u8_isvalid(properNUL));
	assert(!u8_isnorm(improperNUL));
	assert(u8_isvalid(improperNUL));

	const char *invalid = "foo" "\xF7\xBF\xBF\xBF" "bar";
	assert(!u8_isnorm(invalid));
	assert(!u8_isvalid(invalid));
}

TEST(u8_ststr_example)
{
	const char strophe1[] = "Deutschland, Deutschland " "\xC3\xBC" "ber alles; " "\xC3\x9C" "ber alles in der Welt";

	assertPEq(u8_strchr(strophe1, 0x00FC), u8_strstr(strophe1, "über"));
	assertPEq(u8_strrchr(strophe1, 0x00FC), u8_strrstr(strophe1, "über"));

	assertPEq(u8_strchr(strophe1, 0x00DC), u8_strstr(strophe1, "Über"));
	assertPEq(u8_strchr(strophe1, 0x00DC), u8_strrstr(strophe1, "Über"));

	assertPEq(u8_strchr(strophe1, 0xFC), u8_strrchr(strophe1, 0xFC));
	assertPEq(u8_strchr(strophe1, 0xDC), u8_strrchr(strophe1, 0xDC));
	

	assertPEq(u8_strchr(strophe1, 0xFC), u8_strchrI(strophe1, 0xFC));
	assertPEq(u8_strchr(strophe1, 0xFC), u8_strchrI(strophe1, 0xDC));
	assertPEq(u8_strchr(strophe1, 0xDC), u8_strrchrI(strophe1, 0xFC));
	assertPEq(u8_strrchr(strophe1, 0xDC), u8_strrchrI(strophe1, 0xDC));

	assertPEq(u8_strstrI(strophe1, "über"), u8_strstr(strophe1, "über"));
	assertPEq(u8_strrstrI(strophe1, "über"), u8_strstr(strophe1, "Über"));
}

TEST(partial_chars, struct Codepoint, chr)
{
	char buf[UTF8_MAX];
	size_t z = u8enc(chr.codepoint, buf);

	for(size_t off = 0; off < z; ++off)
	{
		for(size_t n = 1; off + n <= z && n < z; ++n)
			assertUEq(1, u8ndec(buf + off, n, NULL));
	}
}
