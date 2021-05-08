#include "common.h"
#include <assert.h>

void test_util_private_use_planes()
{
	eq_i(uchar_class(0xE000), UCLASS_PRIVATE_USE);
	eq_i(uchar_class(0xF000), UCLASS_PRIVATE_USE);
	eq_i(uchar_class(0xF8FF), UCLASS_PRIVATE_USE);

	eq_i(uchar_class(0xF0000), UCLASS_PRIVATE_USE);
	eq_i(uchar_class(0xFF000), UCLASS_PRIVATE_USE);
	eq_i(uchar_class(0xFFFFD), UCLASS_PRIVATE_USE);

	eq_i(uchar_class(0x100000), UCLASS_PRIVATE_USE);
	eq_i(uchar_class(0x100FFF), UCLASS_PRIVATE_USE);
	eq_i(uchar_class(0x10FFFD), UCLASS_PRIVATE_USE);

	eq_i(uchar_class(0xEEEEE), UCLASS_UNASSIGNED);
}

void test_util_uchar_class()
{
	eq_i(uchar_class('0'), UCLASS_DECIMAL_NUMBER);
	eq_i(uchar_class('9'), UCLASS_DECIMAL_NUMBER);
	eq_i(uchar_class('a'), UCLASS_LOWERCASE_LETTER);
	eq_i(uchar_class('A'), UCLASS_UPPERCASE_LETTER);
}

void test_util_uclass_is()
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

void test_util_uchar_upper()
{
	uchar_t data[][2] = {
		{ '5', '5' },
		{ 'i', 'I' },
		{ 0x0131 /*ı*/, 'I' },
		{ 0x00FC /*ü*/, 0x00DC /*Ü*/ },
		{ 0x20AC /*€*/, 0x20AC /*€*/ }
	};

	for (size_t i = 0; i < sizeof(data)/sizeof(*data); i++)
		eq_i(uchar_upper(data[i][0]), data[i][1]);
}

void test_util_uchar_lower()
{
	uchar_t data[][2] = {
		{ '5', '5' },
		{ 'I', 'i' },
		{ 0x0130 /*İ*/, 'i' },
		{ 0x00DC /*Ü*/, 0x00FC /*ü*/ },
		{ 0x20AC /*€*/, 0x20AC /*€*/ }
	};

	for (size_t i = 0; i < sizeof(data)/sizeof(*data); i++)
		eq_i(uchar_lower(data[i][0]), data[i][1]);
}

void test_util()
{
	test_util_private_use_planes();
	test_util_uchar_class();
	test_util_uclass_is();
	test_util_uchar_lower();
	test_util_uchar_upper();
}