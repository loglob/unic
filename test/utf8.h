#include "common.h"
#include <libexplain/pipe.h>
#include <libexplain/fdopen.h>
#include <libexplain/fwrite.h>
#include <libexplain/fread.h>

static struct {
	int fds[2];
	FILE *write;
	FILE *read;
} echo;

void test_utf8_u8dec(const char *str, uchar_t chr, size_t len)
{
	uchar_t gotChr;
	size_t gotLen = u8dec(str, &gotChr);

	eq_i(gotChr, chr);
	eq_i(gotLen, len);
}

void test_utf8_fgetu8(const char *str, uchar_t chr, size_t len)
{
	explain_fwrite_or_die((void*)str, 1, len, echo.write);
	fflush(echo.write);

	uchar_t gotChr = fgetu8(echo.read);
	
	eq_i(gotChr, chr);
}

void test_utf8_u8enc(uchar_t chr, const char *str, size_t len)
{
	char buf[5] = {};
	size_t gotLen = u8enc(chr, buf);

	eq_i(gotLen, len);
	eq_s(buf, str);
}

void test_utf8_fputu8(uchar_t chr, const char *str, size_t len)
{
	char buf[5] = {};

	size_t gotLen = fputu8(chr, echo.write);
	fflush(echo.write);

	size_t gotAnything = explain_fread_or_die(buf, len, 1, echo.read);

	eq_i(gotAnything, true);
	eq_i(gotLen, len);
	eq_s(buf, str);
}


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

	// test string overencoding
	for (int i = 0; i < 4; i++)
	{
		for (int j = i; j < 4; j++)
		{
			test_utf8_u8dec(data[i][j], codepoints[i], j + 1);
			test_utf8_fgetu8(data[i][j], codepoints[i], j + 1);
		}

		test_utf8_u8enc(codepoints[i], data[i][i], i + 1);
		test_utf8_fputu8(codepoints[i], data[i][i], i + 1);
	}
}


void test_utf8_underencoding()
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
		{
			size_t l = u8dec(&data[i][j], NULL);
			eq_i(l, 1);
		}
	}
}

void test_utf8()
{
	explain_pipe_or_die(echo.fds);
	echo.read = explain_fdopen_or_die(echo.fds[0], "r");
	echo.write = explain_fdopen_or_die(echo.fds[1], "w");

	test_utf8_overencoding();
	test_utf8_underencoding();

	fclose(echo.write);

	eq_i(fgetu8(echo.read), UEOF);

	fclose(echo.read);
}