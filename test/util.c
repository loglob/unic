#include "common.h"
#include "unic.h"
#include <assert.h>

TEST(check_category, struct Codepoint, chr)
{
	assertIEq(chr.category, uchar_class(chr.codepoint));
}

TEST(test_util_uclass_is)
{
	assert(uclass_is(UCLASS_LETTER, UCLASS_CASED_LETTER));
	assert(uclass_is(UCLASS_LETTER, UCLASS_UPPERCASE_LETTER));
	assert(!uclass_is(UCLASS_LETTER, UCLASS_CURRENCY_SYMBOL));

	assert(uclass_is(UCLASS_CASED_LETTER, UCLASS_UPPERCASE_LETTER));
	assert(!uclass_is(UCLASS_CASED_LETTER, UCLASS_CURRENCY_SYMBOL));
	assert(!uclass_is(UCLASS_CASED_LETTER, UCLASS_OTHER_LETTER));

	assert(uclass_is(UCLASS_CURRENCY_SYMBOL, UCLASS_CURRENCY_SYMBOL));
	assert(!uclass_is(UCLASS_CURRENCY_SYMBOL, UCLASS_CLOSE_PUNCTUATION));		
}

TEST(test_uchar_upper, struct Codepoint, chr)
{
	assertCEq(chr.simpleUpper, uchar_upper(chr.codepoint));
}

TEST(test_uchar_lower, struct Codepoint, chr)
{
	assertCEq(chr.simpleLower, uchar_lower(chr.codepoint));
}

TEST(test_uchar_alike, struct Codepoint, chr)
{
	assertTrue(uchar_alike(chr.codepoint, chr.simpleLower));
	assertTrue(uchar_alike(chr.codepoint, chr.simpleUpper));
}
