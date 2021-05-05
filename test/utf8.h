#include "common.h"

void test_utf8_overencoding()
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
			"\xC2\xA2",
			"\xE0\x82\xA2"
			"\xF0\x80\x82\xA2"
		},
		{
			// €
			"\xE2\x82\xAC",
			"\xF0\x82\x82\xAC",
		},
		{
			// 𐍈
			"\xF0\x90\x8D\x88"
		}
	};
	uchar_t codepoints[] = {
		0x0024,
		0x00A2,
		0x20AC,
		0x10348
	};

	// test overencoding
	for (int i = 0; i < 4; i++)
	{
		for (int j = i; j < 4; j++)
		{
			uchar_t chr;
			size_t len = u8dec(data[i][j], &chr);

			eq_i(chr, codepoints[i]);
			eq_i(len, j + 1);
		}

		char buf[5] = {};
		size_t len = u8enc(codepoints[i], buf);

		eq_i(len, i + 1);
		eq_s(buf, data[i][i]);
	}
}

void test_utf8()
{
	test_utf8_overencoding();
}