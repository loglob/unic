#include "common.h"
#include "unic.h"
#include <libexplain/fwrite.h>

TEST(u8enc_size_correct, struct Codepoint, chr)
{
	char buf[10];
	size_t z = u8enc(chr.codepoint, buf);

	assertTrue(z <= UTF8_MAX);
	assertTrue(z > 0);
	assertIEq(z, u8enc(chr.codepoint, NULL));
}

/** Ensures the data written by enc() is in the expected format */
TEST(u8enc_writes_data, struct Codepoint, chr)
{
	const char padding = 0xFF;
	char buf[10];

	for(size_t i = 0; i < sizeof(buf); ++i)
		buf[i] = padding;

	size_t encSize = u8enc(chr.codepoint, buf);

	for(size_t i = 0; i < encSize; ++i)
		assertUNEq(padding, buf[i]);
	// no NULL terminator
	for(size_t i = encSize; i < sizeof(buf); ++i)
		assertUEq(padding, buf[i]);
}

TEST(u8enc_round_trip, struct Codepoint, chr)
{
	char buf[10];
	size_t encSize = u8enc(chr.codepoint, buf);

	uchar_t decoded;
	size_t decSize = u8dec(buf, &decoded);

	assertUEq(encSize, decSize);
	assertCEq(chr.codepoint, decoded);
}

TEST(fputu8_round_trip, struct Pipe, echo, struct Codepoint, chr)
{
	size_t written = fputu8(chr.codepoint, echo.in);
	assertUEq(u8enc(chr.codepoint, NULL), written);

	fflush(echo.in);
	uchar_t read = fgetu8(echo.out);

	assertCEq(chr.codepoint, read);
}

/** Checks that over-encoded strings are read properly */
TEST(u8dec_overencode)
{
	char *data[4][4] = {
		{
			// $
			"\x24",
			"\xC0\xA4",
			"\xE0\x80\xA4",
			"\xF0\x80\x80\xA4",
		},
		{
			// ¢
			NULL,
			"\xC2\xA2",
			"\xE0\x82\xA2",
			"\xF0\x80\x82\xA2"
		},
		{
			// €
			NULL,
			NULL,
			"\xE2\x82\xAC",
			"\xF0\x82\x82\xAC",
		},
		{
			// 𐍈
			NULL,
			NULL,
			NULL,
			"\xF0\x90\x8D\x88"
		}
	};
	uchar_t codepoints[] = {
		0x0024,
		0x00A2,
		0x20AC,
		0x10348
	};

	for (int i = 0; i < 4; i++)
	{
		for (int j = i; j < 4; j++)
		{
			const char *str = data[i][j];

			uchar_t chr;
			assertUEq(j+1, u8dec(str, &chr));
			assertCEq(codepoints[i], chr);

			if(i == j)
			{ // normalized encoding
				assertTrue( u8_isnorm(str) );
				char buf[UTF8_MAX + 1] = {};

				assertUEq(j+1, u8enc(chr, buf));
				assertSEq(str, buf);
			}
		}
	}
}

TEST(u8dec_underencode)
{
	char *data[] = {
		"\xC0",
		"\xE0",
		"\xF0",
		"\xE0\x8F",
		"\xF0\x8F",
		"\xF0\x8F\x8F",
	};

	for (size_t i = 0; i < sizeof(data) / sizeof(*data); i++)
	{
		for (size_t j = 0; data[i][j]; j++)
			assertIEq(1, u8dec(&data[i][j], NULL));
	}

	for (size_t n = 0; n <= 4; n++)
	{
		// 4-byte encoded \0
		const char *d = "\xF0\x80\x80\x80";

		uchar_t chr;
		size_t len = u8ndec(d, n, &chr);

		if(n == 4)
		{
			assertUEq(0, chr);
			assertUEq(4, len);
		}
		else
			assertUEq(1, len);
	}
}
