// u8string.h: Implements string functions for utf-8 strings
#pragma once
#include <stddef.h>
#include <stdbool.h>
#include "utf8.h"
#include "util.h"

/* Determines the amount of unicode characters in the given utf-8 string */
size_t u8_strlen(const char *str);
/* Determines the amount of bytes occupied by the first n characters in the given utf-8 string. */
size_t u8_strnlen(const char *str, size_t n);
/* Determines the amount of complete unicode characters in the first c bytes of the given string. */
size_t u8_strclen(const char *str, size_t c);

/* Copies the utf-8 encoded string str to dst.
	Every character written to dst is guaranteed to be utf-8 normalized.
	Returns the amount of bytes written to dst, including the null terminator.
	If dst is NULL, no write operations are performed but the correct byte amount is returned. */
size_t u8_strcpy(const char *str, char *dst);
/* Copies at most n unicode characters from str to dst.
	Every character written to dst is guaranteed to be utf-8 normalized.
	Returns the amount of bytes written to dst, including the null terminator.
	If dst is NULL, no write operations are performed but the correct byte amount is returned. */
size_t u8_strncpy(const char *str, char *dst, size_t n);
/* Copies at most c bytes from str to dst.
	Every character written to dst is guaranteed to be utf-8 normalized.
	Returns the amount of bytes written to dst, including the null terminator.
	If dst is NULL, no write operations are performed but the correct byte amount is returned. */
size_t u8_strccpy(const char *str, char *dst, size_t c);

/* Finds the character with the given index in the given utf-8 encoded string. */
const char *u8_strpos(const char *str, size_t pos);
/* Finds the character at the given index in the given utf-8 encoded string. Returns 0 if the index is outside string bounds. */
uchar_t u8_strat(const char *str, size_t pos);
/* Finds the first occurence of the given character in the given utf-8 encoded string. */
const char *u8_strchr(const char *str, uchar_t chr);
/* Finds the first occurence of the given character in the given utf-8 encoded string. */
const char *u8_strrchr(const char *str, uchar_t chr);
/* Finds the first occurence of the given string in the given utf-8 encoded string. */
const char *u8_strstr(const char *haystack, const char *needle);
/* Finds the first occurence of the given string in the given utf-8 encoded string. Case-insensitive. */
const char *u8_strstrI(const char *haystack, const char *needle);

/* Determines if two utf-8 encoded strings contain the same characters. */
bool u8_streq(const char *a, const char *b);
/* Determines if two utf-8 encoded strings contain the same characters. Case-insensitive. */
bool u8_streqI(const char *a, const char *b);
/* Determines if the utf-8 encoded string str ends with the utf-8 encoded string end. */
bool u8_strend(const char *str, const char *end);
/* Determines if the utf-8 encoded string str ends with the utf-8 encoded string end. Case-insensitive. */
bool u8_strendI(const char *str, const char *end);
/* Determines if the utf-8 encoded string str starts with the utf-8 encoded string start. */
bool u8_strstart(const char *str, const char *start);
/* Determines if the utf-8 encoded string str starts with the utf-8 encoded string start. Case-insensitive. */
bool u8_strstartI(const char *str, const char *start);

/* Determines if the given utf-8 encoded string is normalized utf-8. */
bool u8_isnorm(const char *str);
/* Determines if the given utf-8 encoded string is valid utf-8. */
bool u8_isvalid(const char *str);

/* Applies map_f to every character in the utf-8 encoded string and writes them to buf.
	If map_f returns 0, no character is written to buf. 
	Every character written to dst is guaranteed to be utf-8 normalized.
	Returns the amount of bytes written to dst, including the null terminator.
	If dst is NULL, no write operations are performed but the correct byte amount is returned. */
size_t u8_strmap(const char *str, char *dst, uchar_t (*map_f)(uchar_t));
/* Applies map_f to, at most, the first n character in the utf-8 encoded string and writes them to buf.
	If map_f returns 0, no character is written to buf.
	Every character written to dst is guaranteed to be utf-8 normalized.
	Returns the amount of bytes written to dst, including the null terminator.
	If dst is NULL, no write operations are performed but the correct byte amount is returned. */
size_t u8_strnmap(const char *str, char *dst, size_t n, uchar_t (*map_f)(uchar_t));
/* Applies map_f to every character in the utf-8 encoded string and writes them to buf.
	If map_f returns 0, no character is written to buf.
	Writes at most c characters.
	Every character written to dst is guaranteed to be utf-8 normalized.
	Returns the amount of bytes written to dst, including the null terminator.
	If dst is NULL, no write operations are performed but the correct byte amount is returned. */
size_t u8_strnmap(const char *str, char *dst, size_t c, uchar_t (*map_f)(uchar_t));

size_t u8_strlen(const char *str)
{
	uchar_t c;
	size_t len = 0;

	while(str += u8dec(str, &c), c)
		len++;

	return len;
}

size_t u8_strnlen(const char *str, size_t n)
{
	uchar_t c;
	size_t curlen;
	size_t len = 0;
	size_t clen = 0;

	while(curlen = u8dec(str, &c), c && (len++ < n))
	{
		clen += curlen;
		str += curlen;
	}
	
	return clen;
}

size_t u8_strclen(const char *str, size_t lim)
{
	uchar_t c;
	size_t curlen;
	size_t r = 0;
	size_t len = 0;

	while(curlen = u8dec(str, &c), c && ((r += curlen) < lim))
		len++;
	
	return len;
}

size_t u8_strcpy(const char *str, char *dst)
{
	uchar_t c;
	size_t w = 1;

	while(str += u8dec(str, &c), c)
	{
		size_t curlen = u8enc(c, dst);

		if(dst)
			dst += curlen;

		w += curlen;
	}

	if(dst)
		*dst = 0;

	return w;
}

size_t u8_strncpy(const char *str, char *dst, size_t n)
{
	uchar_t c;
	size_t w = 1;
	size_t len = 0;

	while(str += u8dec(str, &c), c && len++ < n)
	{
		size_t curlen = u8enc(c, dst);

		if(dst)
			dst += curlen;

		w += curlen;
	}

	if(dst)
		*dst = 0;

	return w;
}

size_t u8_strccpy(const char *str, char *dst, size_t c)
{
	uchar_t chr;
	size_t w = 1;

	while(str += u8dec(str, &chr), chr)
	{
		size_t curlen = u8enc(chr, NULL);

		if(w + curlen > c)
			break;
		if(dst)
			dst += u8enc(chr, dst);
		
		w += curlen;
	}

	if(dst)
		*dst = 0;
	
	return w;
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
	uchar_t c;
	size_t curlen;
	uchar_t n0;
	u8dec(needle, &n0);

	while(curlen = u8dec(haystack, &c), c)
	{
		if(c == n0 && u8_strstart(haystack, needle))
			return haystack;
		
		haystack += curlen;
	}
}

const char *u8_strstrI(const char *haystack, const char *needle)
{
	uchar_t c;
	size_t curlen;
	uchar_t n0;
	u8dec(needle, &n0);
	n0 = uchar_lower(n0);

	while(curlen = u8dec(haystack, &c), c)
	{
		if(uchar_lower(c) == n0 && u8_strstartI(haystack, needle))
			return haystack;
		
		haystack += curlen;
	}
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

bool u8_streqI(const char *a, const char *b)
{
	uchar_t ac, bc;

	while(a += u8dec(a, &ac), b += u8dec(b, &bc), uchar_lower(ac) == uchar_lower(bc))
	{
		if(!ac)
			return true;
	}

	return false;
}

bool u8_strstart(const char *str, const char *start)
{
	uchar_t ac, bc;

	while(str += u8dec(str, &ac), start += u8dec(start, &bc), bc)
	{
		if(ac != bc)
			return false;
	}

	return true;
}

bool u8_strstartI(const char *str, const char *start)
{
	uchar_t ac, bc;

	while(str += u8dec(str, &ac), start += u8dec(start, &bc), bc)
	{
		if(uchar_lower(ac) != uchar_lower(bc))
			return false;
	}

	return true;
}

bool u8_strend(const char *str, const char *end)
{
	size_t slen = u8_strlen(str);
	size_t elen = u8_strlen(end);

	if(elen > slen)
		return false;

	return u8_streq(u8_strpos(str, slen - elen), end);
}

bool u8_strendI(const char *str, const char *end)
{
	size_t slen = u8_strlen(str);
	size_t elen = u8_strlen(end);

	if(elen > slen)
		return false;

	return u8_streqI(u8_strpos(str, slen - elen), end);
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
	uchar_t c;
	size_t w = 1;

	while(str += u8dec(str, &c), c)
	{
		c = map_f(c);
	
		size_t curlen = u8enc(c, dst);

		if(dst)
			dst += curlen;

		w += curlen;
	}

	if(dst)
		*dst = 0;

	return w;
}

size_t u8_strnmap(const char *str, char *dst, size_t n, uchar_t (*map_f)(uchar_t))
{
	uchar_t c;
	size_t w = 1;
	size_t len = 0;

	while(str += u8dec(str, &c), c && len++ < n)
	{
		c = map_f(c);

		size_t curlen = u8enc(c, dst);

		if(dst)
			dst += curlen;

		w += curlen;
	}

	if(dst)
		*dst = 0;

	return w;
}

size_t u8_strcmap(const char *str, char *dst, size_t c, uchar_t (*map_f)(uchar_t))
{
	uchar_t chr;
	size_t w = 1;

	while(str += u8dec(str, &chr), chr)
	{
		c = map_f(c);

		size_t curlen = u8enc(chr, NULL);

		if(w + curlen > c)
			break;
		if(dst)
			dst += u8enc(chr, dst);
		
		w += curlen;
	}

	if(dst)
		*dst = 0;
	
	return w;
}

#endif