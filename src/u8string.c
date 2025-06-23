// u8string.h: Implements string functions for utf-8 strings
#include <stddef.h>
#include <stdbool.h>
#include "../include/unic.h"

size_t u8_strlen(const char *str)
{
	return u8z_strlen(str, NUL_TERMINATED);
}

size_t u8_strmap(const char *str, char *dst, uchar_t (*map_f)(uchar_t))
{
	return u8z_strmap(str, NUL_TERMINATED, dst, map_f);
}

size_t u8_strcpy(const char *str, char *dst)
{
	return u8z_strcpy(str, NUL_TERMINATED, dst);
}

const char *u8_strpos(const char *str, size_t pos)
{
	return u8z_strpos(str, NUL_TERMINATED, pos);
}

uchar_t u8_strat(const char *str, size_t pos)
{
	return u8z_strat(str, NUL_TERMINATED, pos);
}

const char *u8_strchr(const char *str, uchar_t chr)
{
	return u8z_strchr(str, NUL_TERMINATED, chr);
}

const char *u8_strrchr(const char *str, uchar_t chr)
{
	return u8z_strrchr(str, NUL_TERMINATED, chr);
}

const char *u8_strchrI(const char *str, uchar_t chr)
{
	return u8z_strchrI(str, NUL_TERMINATED, chr);
}

const char *u8_strrchrI(const char *str, uchar_t chr)
{
	return u8z_strrchrI(str, NUL_TERMINATED, chr);
}

const char *u8_strstr(const char *str, const char *needle)
{
	return u8z_strstr(str, NUL_TERMINATED, needle, NUL_TERMINATED);
}

const char *u8_strrstr(const char *str, const char *needle)
{
	return u8z_strrstr(str, NUL_TERMINATED, needle, NUL_TERMINATED);
}

const char *u8_strstrI(const char *str, const char *needle)
{
	return u8z_strstrI(str, NUL_TERMINATED, needle, NUL_TERMINATED);
}

const char *u8_strrstrI(const char *str, const char *needle)
{
	return u8z_strrstrI(str, NUL_TERMINATED, needle, NUL_TERMINATED);
}

bool u8_streq(const char *a, const char *b)
{
	return u8z_streq(a, NUL_TERMINATED, b, NUL_TERMINATED);
}

bool u8_strneq(const char *a, const char *b, size_t n)
{
	return u8z_streq(a, MAX_CHARS(n), b, MAX_CHARS(n));
}

bool u8_streqI(const char *a, const char *b)
{
	return u8z_streqI(a, NUL_TERMINATED, b, NUL_TERMINATED);
}

bool u8_strneqI(const char *a, const char *b, size_t n)
{
	return u8z_streqI(a, MAX_CHARS(n), b, MAX_CHARS(n));
}

bool u8_isnorm(const char *str)
{
	return u8z_isnorm(str, NUL_TERMINATED);
}

bool u8_isvalid(const char *str)
{
	return u8z_isvalid(str, NUL_TERMINATED);
}
