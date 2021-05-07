#include "common.h"

// tests if str_cpy() produces empty strings for n = 0 or c = 0.
void test_u8string_emptyCopies()
{
	char buf[10] = "/////////";

	eq_i(u8_strncpy(NULL, NULL, 0), 1);
	eq_i(u8_strncpy(NULL, buf, 0), 1);
	eq_s(buf, "");

	*buf = '_';

	eq_i(u8_strccpy(NULL, NULL, 0), 1);
	eq_i(u8_strccpy(NULL, buf, 0), 1);
	eq_s(buf, "");
}

// tests the u8_strlen variants
void test_u8string_lenths()
{
	eq_i(u8_strlen(mul_apin), 2);
	eq_i(u8_strlen(deLied), sizeof(deLied) - 3);

	eq_i(u8_strnlen(mul_apin, 1), 4);
	eq_i(u8_strnlen(deLied, 11), 11);

	eq_i(u8_strclen(mul_apin, 4), 1);
	eq_i(u8_strclen(mul_apin, 6), 3);
	eq_i(u8_strclen(deLied, 11), 11);
}

void test_u8string_strpos()
{
	eq_i(u8_strpos(mul_apin, 0), mul_apin);
	eq_i(u8_strpos(mul_apin, 1), mul_apin + 4);
	eq_i(u8_strpos(mul_apin, 2), mul_apin + 8);
	eq_i(u8_strpos(mul_apin, 3), NULL);
}

void test_u8string_strat()
{
	uchar_t mul_apin_chrs[3];

	u8dec(mul_apin, mul_apin_chrs + 0);
	u8dec(mul_apin + 4, mul_apin_chrs + 1);
	u8dec(mul_apin + 8, mul_apin_chrs + 2);

	eq_i(u8_strat(mul_apin, 0), mul_apin_chrs[0]);
	eq_i(u8_strat(mul_apin, 1), mul_apin_chrs[1]);
	eq_i(u8_strat(mul_apin, 2), mul_apin_chrs[2]);
	eq_i(u8_strat(mul_apin, 3), mul_apin_chrs[2]);
}

void test_u8string_strchr()
{
	eq_i(u8_strchr(deLied, 't'), strchr(deLied, 't'));
	eq_i(u8_strrchr(deLied, 't'), strrchr(deLied, 't'));

	eq_i(u8_strchr(deLied, 0x00FC), deLied + 25);
	eq_i(u8_strrchr(deLied, 0x00FC), deLied + 25);

	eq_i(u8_strchrI(deLied, 0x00FC), u8_strchr(deLied, 0x00FC));
	eq_i(u8_strrchrI(deLied, 0x00FC), deLied + 37);

	eq_i(u8_strchrI(deLied, 0x00DC), u8_strchrI(deLied, 0x00FC));
	eq_i(u8_strrchrI(deLied, 0x00DC), u8_strrchrI(deLied, 0x00FC));
}

void test_u8string()
{
	test_u8string_emptyCopies();
	test_u8string_lenths();
	test_u8string_strpos();
}