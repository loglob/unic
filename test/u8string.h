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

void test_u8string()
{
	test_u8string_emptyCopies();
	test_u8string_lenths();
	test_u8string_strpos();
}