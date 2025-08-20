#include "common.h"
#include "interface.h"
#include "unic.h"

void checkIterationEqual(str_t str, u8size_t size)
{
	size_t i = 0;
	size_t bc = 0;

	U8Z_FOREACH(ctx, str.bytes, size)
	{
		assertIEq(i, ctx.chrIx);
		assertIEq(bc, ctx.byteIx);
		assertCEq(str.chars[i++], ctx.chr);
		bc += ctx.l;
	}

	assertIEq(i, str.count);
	assertIEq(bc, str.size);
}

TEST(completeIteration, str_t, str)
{
	checkIterationEqual(str, NUL_TERMINATED);
	checkIterationEqual(str, EXACT_BYTES(str.size));
	checkIterationEqual(str, EXACT_CHARS(str.count));
	u8size_t z = { true, str.size, true, str.count };
	checkIterationEqual(str, z);
}

TEST(prefixIteration, str_t, str, uint16_t, n)
{
	n %= str.count + 1;
	u8size_t z = EXACT_CHARS(n);
	size_t i = 0;

	U8Z_FOREACH(ctx, str.bytes, z)
	{
		assert(i < str.count);
		assertIEq(i, ctx.chrIx);
		assertCEq(str.chars[i++], ctx.chr);
	}

	assertIEq(n, i);
}
