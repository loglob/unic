// u8string.h: Implements string functions for utf-8 strings
#pragma once
#include <stddef.h>
#include <stdbool.h>
#include "utf8.h"
#include "util.h"

size_t u8_strlen(const char *str)
{
	size_t len = 0;

	for (len = 0; str[len];)
		len += u8dec(str + len, NULL);
	
	return len;
}

size_t u8_strnlen(const char *str, size_t n)
{
	size_t len = 0;

	for (size_t i = 0; i < n && str[len]; i++)
		len += u8dec(str + len, NULL);
	
	return len;
}

size_t u8_strclen(const char *str, size_t lim)
{
	size_t c = 0;

	for (size_t i = 0; i < lim && str[i];)
	{
		i += u8ndec(str + i, lim - i, NULL);
		c++;
	}
	
	return c;
}


size_t u8_strmap(const char *str, char *dst, uchar_t (*map_f)(uchar_t))
{
	size_t r = 0, w = 0;

	while(str[r])
	{
		uchar_t c;
		r += u8dec(str + r, &c);
		c = map_f(c);
		w += u8enc(c, dst ? dst + w : NULL);
	}

	if(dst)
		dst[w] = 0;

	return w + 1;
}

size_t u8_strnmap(const char *str, char *dst, size_t n, uchar_t (*map_f)(uchar_t))
{
	size_t r = 0, w = 0;

	for (size_t i = 0; i < n && str[r]; i++)
	{
		uchar_t c;
		r += u8dec(str + r, &c);
		c = map_f(c);
		w += u8enc(c, dst ? dst + w : NULL);
	}

	if(dst)
		dst[w] = 0;

	return w + 1;	
}

size_t u8_strcmap(const char *str, char *dst, size_t lim, uchar_t (*map_f)(uchar_t))
{
	size_t r = 0, w = 0;

	while(r < lim && str[r])
	{
		uchar_t c;
		r += u8ndec(str + r, lim - r, &c);
		c = map_f(c);
		w += u8enc(c, dst ? dst + w : NULL);
	}

	if(dst)
		dst[w] = 0;

	return w + 1;	
}


// used with the strmap functions
static uchar_t uchar_id(uchar_t x)
{
	return x;
}

size_t u8_strcpy(const char *str, char *dst)
{
	return u8_strmap(str, dst, uchar_id);
}

size_t u8_strncpy(const char *str, char *dst, size_t n)
{
	return u8_strnmap(str, dst, n, uchar_id);
}

size_t u8_strccpy(const char *str, char *dst, size_t c)
{
	return u8_strcmap(str, dst, c, uchar_id);
}


const char *u8_strpos(const char *str, size_t pos)
{
	size_t r = 0;
	size_t i;

	for (i = 0; i < pos && str[r]; i++)
		r += u8dec(str + i, NULL);
	
	if(i < pos)
		return NULL;
	
	return str + r;
}

uchar_t u8_strat(const char *str, size_t pos)
{
	const char *s = u8_strpos(str, pos);

	if(s)
	{
		uchar_t ret;

		u8dec(s, &ret);

		return ret;
	}
	else
		return 0;
}

/** Expands to an iteration over every character in the string
 * @param str The string to iterate over
 * @param ic The character index
 * @param i The byte index
 * @param c The current character
 * @param l Its encoded length
*/
#define SCAN(...) { \
	size_t ic = 0; \
	for(size_t i = 0; str[i]; ic++) \
	{ \
		uchar_t c; \
		const size_t l = u8dec(str + i, &c); \
		{ __VA_ARGS__ } \
	i += l; \
	} \
}

#define SCANFUNC(cond) { \
		SCAN({\
			if(cond) \
				return str + i; \
			i += l; \
		}) \
	return NULL; \
}

#define RSCANFUNC(cond) { \
		const char *ret = NULL; \
		SCAN({\
			if(cond) \
				ret = str + i; \
			i += l; \
		}) \
	return ret; \
}

const char *u8_strchr(const char *str, uchar_t chr)
	SCANFUNC(c == chr)

const char *u8_strrchr(const char *str, uchar_t chr)
	RSCANFUNC(c == chr)

const char *u8_strchrI(const char *str, uchar_t chr)
	SCANFUNC(uchar_alike(c, chr))

const char *u8_strrchrI(const char *str, uchar_t chr)
	RSCANFUNC(uchar_alike(c, chr))


const char *u8_strstr(const char *str, const char *needle)
{
	size_t n = u8_strlen(needle);

	SCANFUNC(u8_strneq(str + i, needle, n))
}

const char *u8_strrstr(const char *str, const char *needle)
{
	size_t n = u8_strlen(needle);

	RSCANFUNC(u8_strneq(str + i, needle, n))
}

const char *u8_strstrI(const char *str, const char *needle)
{
	size_t n = u8_strlen(needle);

	SCANFUNC(u8_strneqI(str + i, needle, n))
}

const char *u8_strrstrI(const char *str, const char *needle)
{
	size_t n = u8_strlen(needle);

	RSCANFUNC(u8_strneqI(str + i, needle, n))
}


bool u8_streq(const char *a, const char *b)
{
	uchar_t ac, bc;

	while(a += u8dec(a, &ac), b += u8dec(b, &bc), ac == bc)
	{
		if(!ac)
			return true;
	}

	return false;
}

bool u8_strneq(const char *a, const char *b, size_t n)
{
	uchar_t ac, bc;
	size_t i = 0;

	if(n == 0)
		return true;

	while(a += u8dec(a, &ac), b += u8dec(b, &bc), ac == bc)
	{
		if(++i >= n || !ac)
			return true;
	}

	return false;
}

bool u8_streqI(const char *a, const char *b)
{
	uchar_t ac, bc;

	while(a += u8dec(a, &ac), b += u8dec(b, &bc), uchar_alike(ac, bc))
	{
		if(!ac)
			return true;
	}

	return false;
}

bool u8_strneqI(const char *a, const char *b, size_t n)
{
	uchar_t ac, bc;
	size_t i = 0;

	if(n == 0)
		return true;

	while(a += u8dec(a, &ac), b += u8dec(b, &bc), uchar_alike(ac, bc))
	{
		if(++i >= n || !ac)
			return true;
	}

	return false;
}


bool u8_isnorm(const char *str)
{
	for (size_t i = 0; str[i];)
	{
		uchar_t c;
		size_t l = u8dec(str + i, &c);
		i += l;

		if(uchar_is(c, UCLASS_UNASSIGNED) || l != u8enc(c, NULL))
			return false;
	}
	
	return true;
}

bool u8_isvalid(const char *str)
{
	for (size_t i = 0; str[i];)
	{
		uchar_t c;
		i += u8dec(str + i, &c);

		if(uchar_is(c, UCLASS_UNASSIGNED))
			return false;
	}
	
	return true;
}

#undef SCAN
#undef SCANFUNC
#undef RSCANFUNC