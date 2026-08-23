#include "common.h"
#include "unic.h"

/** Quells a GCC warning because the scratch buffers don't have NUL terminators */
#ifdef __has_attribute
	#if __has_attribute(nonstring)
		#define NONSTRING __attribute__((nonstring))
	#else
		#define NONSTRING
	#endif
#else
#define NONSTRING
#endif

/** Checks that u8_strncpy writes only the NUL terminator when n=0 */
TEST(zero_char_strcpy, str_t, str)
{
	NONSTRING
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
	NONSTRING
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

/** u8_prefix must accept all actual prefixes of a string */
TEST(u8_prefix_accept, str_t, str)
{
	u8size_t z = (u8size_t){ true, str.size, true, str.count };

	for(size_t c = 0; c <= str.count; ++c)
		assertTrue( u8z_prefix(str.bytes, EXACT_CHARS(c), str.bytes, z) );
}

TEST(u8_prefix_example)
{
	assertTrue( u8_prefix("aababa", "aabababbaabb") );
	assertTrue( !u8_prefix("aabababbaabb", "aababa") );
	assertTrue( u8_prefixI("aababa", "aabababbaabb") );
	assertTrue( !u8_prefixI("aabababbaabb", "aababa") );
	
	assertTrue( !u8_prefix("aababa", "ccdcdc") );
	assertTrue( !u8_prefix("ccdcdc", "aababa") );
	assertTrue( !u8_prefixI("aababa", "ccdcdc") );
	assertTrue( !u8_prefixI("ccdcdc", "aababa") );

	assertTrue( !u8_prefix("aababa", "AabABabbaabb") );
	assertTrue( u8_prefixI("aababa", "AabABabbaabb") );
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

	// well-encoded U+1FFFFF, outside assigned ranges
	const char *invalid = "foo" "\xF7\xBF\xBF\xBF" "bar";
	assert(u8_isnorm(invalid));
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

TEST(hash_is_pure, str_t, str)
{
	assertUEq(u8_hash(str.bytes), u8_hash(str.bytes));
}

TEST(hash_distinguishes_char_prefixes, str_t, str)
{
	for(size_t n = 0; n + 1 <= str.count; ++n)
		assertUNEq(u8z_hash(str.bytes, EXACT_CHARS(n)), u8z_hash(str.bytes, EXACT_CHARS(n + 1)));
}

TEST(hash_distinguishes_byte_prefixes, str_t, str)
{
	for(size_t n = 0; n + 1 <= str.size; ++n)
		assertUNEq(u8z_hash(str.bytes, EXACT_BYTES(n)), u8z_hash(str.bytes, EXACT_BYTES(n + 1)));
}

static uchar_t id(uchar_t chr)
{
	return chr;
}

TEST(hashF_under_identity, str_t, _str)
{
	const char *const str = _str.bytes;

	assertUEq(u8_hash(str), u8_hashF(str, id));

	for(size_t n = 0; n + 1 <= _str.count; ++n)
		assertUEq(u8z_hash(str, EXACT_CHARS(n)), u8z_hashF(str, EXACT_CHARS(n), id));

	for(size_t n = 0; n + 1 <= _str.size; ++n)
		assertUEq(u8z_hash(str, EXACT_BYTES(n)), u8z_hashF(str, EXACT_BYTES(n), id));
}

TEST(hashF_lower)
{
	const char *uppercase = "FooBar";
	const char *lowercase = "foobar";
	assertUEq(u8_hash(lowercase), u8_hashF(uppercase, uchar_lower));
}

TEST(hash_normalizes)
{
	const char normalized[] = "foobar";
	char overEncoded[sizeof(normalized)*2] = {};

	for(size_t i = 0; i < sizeof(normalized) - 1; ++i) 
		u8nenc(normalized[i], 2, overEncoded + 2*i);

	assertTrue(u8_streq(normalized, overEncoded));
	assertUEq(u8_hash(normalized), u8_hash(overEncoded));
}

TEST(hash_prefix)
{
	const char *a = "foobar";
	const char *b = "foosball";
	
	assertUEq(u8z_hash(a, EXACT_CHARS(1)), u8z_hash(b, EXACT_CHARS(1)));
	assertUEq(u8z_hash(a, EXACT_CHARS(2)), u8z_hash(b, EXACT_CHARS(2)));
	assertUEq(u8z_hash(a, EXACT_CHARS(3)), u8z_hash(b, EXACT_CHARS(3)));
	assertUNEq(u8z_hash(a, EXACT_CHARS(4)), u8z_hash(b, EXACT_CHARS(4)));
}

TEST(hashI_suffixes)
{
	const char *a = "foobar";
	const char *b = "FooBar";

	for(size_t i = 0; i <= 5; ++i)
		assertUEq(u8_hashI(a + i), u8_hashI(b + i), " i = %zu", i);
}

TEST(hashI_prefixes)
{
	const char *a = "foobar";
	const char *b = "FooBar";

	for(size_t i = 1; i <= 6; ++i)
	{
		assertUEq(u8z_hashI(a, EXACT_CHARS(i)), u8z_hashI(b, EXACT_CHARS(i)));
		assertUNEq(u8z_hash(a, EXACT_CHARS(i)), u8z_hash(b, EXACT_CHARS(i)));
	}
}

TEST(eqI_implies_hashI)
{
	const char *strings[] = {
		"foo",
		"FOO",
		"\u03D0", "\u0392", "\u03B2",
		NULL
	};

	for(const char **p = strings; *p; ++p)
	{
		for(const char **q = p; *q; ++q)
		{
			#define msg " p=\"%s\" q=\"%s\"", *p, *q
			if(u8_streq(*p, *q))
			{
				assertTrue(u8_streqI(*p, *q), msg);
				assertUEq(u8_hash(*p), u8_hash(*q), msg);
				assertUEq(u8_hashI(*p), u8_hashI(*q), msg);
			}
			else if(u8_streqI(*p, *q))
			{
				assertUNEq(u8_hash(*p), u8_hash(*q), msg);
				assertUEq(u8_hashI(*p), u8_hashI(*q), msg);
			}
		}
	}
}
