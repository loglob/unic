// u8string.h: Implements string functions for utf-8 strings
#pragma once
#include <stddef.h>
#include <stdbool.h>
#include "utf8.h"
#include "util.h"

size_t u8_strlen(const char *str)
{
	uchar_t c;
	size_t len = 0;

	for (len = 0; str[len];)
		len += u8dec(str, NULL);
	
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
	uchar_t c;
	size_t curlen;
	size_t curind = 0;

	while(curlen = u8dec(str, &c), c)
	{
		if(curind == pos)
			return str;

		str += curlen;
		curind++;
	}

	return NULL;
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

const char *u8_strchr(const char *str, uchar_t chr)
{
	uchar_t c;
	size_t curlen;

	while(curlen = u8dec(str, &c), c)
	{
		if(c == chr)
			return str;

		str += curlen;
	}

	return NULL;
}

const char *u8_strrchr(const char *str, uchar_t chr)
{
	const char *loc = NULL;
	uchar_t c;
	size_t curlen;

	while(curlen = u8dec(str, &c), c)
	{
		if(c == chr)
			loc = str;

		str += curlen;
	}

	return loc;
}

const char *u8_strstr(const char *haystack, const char *needle)
{
	size_t n = u8_strlen(needle);

	SCANFUNC(u8_strneq(haystack + i, needle, n))
}

const char *u8_strrstr(const char *haystack, const char *needle)
{
	size_t n = u8_strlen(needle);

	RSCANFUNC(u8_strneq(haystack + i, needle, n))
}

const char *u8_strstrI(const char *haystack, const char *needle)
{
	size_t n = u8_strlen(needle);

	SCANFUNC(u8_strneqI(haystack + i, needle, n))
}

const char *u8_strrstrI(const char *haystack, const char *needle)
{
	size_t n = u8_strlen(needle);

	RSCANFUNC(u8_strneqI(haystack + i, needle, n))
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
	uchar_t c;
	size_t curlen;

	while(str += (curlen = u8dec(str, &c)), c)
	{
		if(curlen != u8enc(c, NULL))
			return false;
		if(uchar_class(c) == UCLASS_UNASSIGNED)
			return false;
	}

	return true;
}

bool u8_isvalid(const char *str)
{
	uchar_t c;
	size_t curlen;

	while(str += (curlen = u8dec(str, &c)), c)
	{
		if(curlen < u8enc(c, NULL))
			return false;
		if(uchar_class(c) == UCLASS_UNASSIGNED)
			return false;
	}

	return true;
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
