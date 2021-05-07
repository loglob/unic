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
	eq_p(u8_strpos(mul_apin, 0), mul_apin, mul_apin);
	eq_p(u8_strpos(mul_apin, 1), mul_apin + 4, mul_apin);
	eq_p(u8_strpos(mul_apin, 2), mul_apin + 8, mul_apin);
	eq_p(u8_strpos(mul_apin, 3), NULL, mul_apin);
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
	#define eq(a,b) eq_p(a,b,deLied)

	eq(u8_strchr(deLied, 't'), strchr(deLied, 't'));
	eq(u8_strrchr(deLied, 't'), strrchr(deLied, 't'));

	eq(u8_strchr(deLied, 0x00FC), deLied + 25);
	eq(u8_strrchr(deLied, 0x00FC), deLied + 25);
	eq(u8_strchr(deLied, 0x00DC), deLied + 38);
	eq(u8_strrchr(deLied, 0x00DC), deLied + 38);

	eq(u8_strchrI(deLied, 0x00FC), u8_strchr(deLied, 0x00FC));
	eq(u8_strrchrI(deLied, 0x00FC), u8_strchr(deLied, 0x00DC));

	eq(u8_strchrI(deLied, 0x00DC), u8_strchrI(deLied, 0x00FC));
	eq(u8_strrchrI(deLied, 0x00DC), u8_strrchrI(deLied, 0x00FC));

	#undef eq
}

void test_u8string_strstr()
{
	#define eq(a,b) eq_p(a,b,deLied)

	eq(u8_strstr(deLied, "über"), u8_strchr(deLied, 0x00FC));
	eq(u8_strrstr(deLied, "über"), u8_strchr(deLied, 0x00FC));

	eq(u8_strstr(deLied, "Über"), u8_strchr(deLied, 0x00DC));
	eq(u8_strrstr(deLied, "Über"), u8_strchr(deLied, 0x00DC));

	eq(u8_strstrI(deLied, "über"), u8_strstr(deLied, "über"));
	eq(u8_strrstrI(deLied, "über"), u8_strstr(deLied, "Über"));

	#undef eq
}

void test_u8string()
{
	test_u8string_emptyCopies();
	test_u8string_lenths();
	test_u8string_strpos();
	test_u8string_strat();
	test_u8string_strchr();
	test_u8string_strstr();
}