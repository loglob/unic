/* utf8.h: Allows for working with utf-8 encoded streams or buffers. */
#ifndef _UNIC_UTF8_H
#define _UNIC_UTF8_H
#include "uchar.h"
#include <stdio.h>

/* Reads the next unicode character from the given utf-8 encoded stream.
	Returns EOF on reading EOF. */
uchar_t fgetu8(FILE *);
/* Writes the given unicode character to the given utf-8 encoded stream. */
void fputu8(uchar_t, FILE *);
/* Gets the amount of bytes needed to encode the given unicode character in utf-8 */
size_t u8len(uchar_t);
/* Reads the next utf-8 encoded character from the given string.
	The amount of bytes read is stored in *l */
uchar_t u8dec(const char *str, size_t *l);
/* Writes the unicode character to the buffer. */
void u8enc(uchar_t uc, char *buf);

inline size_t u8len(uchar_t c)
{
	return (c > 0xFFFF)
		? 4
		: (c > 0x7FF)
			? 3
			: (c > 0x7F)
				? 2
				: 1;
}

static inline uchar_t _w1252_fallback(unsigned char c)
{
	#define MAP(w, u) case w: return u;

	switch(c)
	{
		MAP(0x80, 0x20AC)
		
		MAP(0x82, 0x201A)
		MAP(0x83, 0x0192)
		MAP(0x84, 0x201E)
		MAP(0x85, 0x2026)
		MAP(0x86, 0x2020)
		MAP(0x87, 0x2021)
		MAP(0x88, 0x02C6)
		MAP(0x89, 0x2030)
		MAP(0x8A, 0x0160)
		MAP(0x8B, 0x2039)
		MAP(0x8C, 0x0152)

		MAP(0x8E, 0x017D)
		
		
		MAP(0x91, 0x2018)
		MAP(0x92, 0x2019)
		MAP(0x93, 0x201C)
		MAP(0x94, 0x201D)
		MAP(0x95, 0x2022)
		MAP(0x96, 0x2013)
		MAP(0x97, 0x2014)
		MAP(0x98, 0x02DC)
		MAP(0x99, 0x2122)
		MAP(0x9A, 0x0161)
		MAP(0x9B, 0x203A)
		MAP(0x9C, 0x0153)
		
		MAP(0x9E, 0x017E)
		MAP(0x9F, 0x0178)

		default:
			return c;
	}

	#undef MAP
}

/* Count the amount of leading ones in i */
static inline unsigned int _cl1(int i)
{
	int c;

	for(c = 0; i & 0x80; i = i << 1)
		c++;
	
	return c;
}

uchar_t u8dec(const char *str, size_t *l)
{
	*l = 1;
	int cl = _cl1(str[0]);
	
	if(cl < 2 || cl > 4)
		return _w1252_fallback(*str);
	
	uchar_t v = str[0] & (0xFF >> cl);

	for(int i = 1; i < cl; i++)
	{
		if((str[i] & 0xC0) != 0x80)
			return _w1252_fallback(*str);
	
		v = v << 6;
		v |= str[i] & (0x3F);
	}

	*l = cl;
	return v;
}

void u8enc(uchar_t uc, char *buf)
{
	size_t l = u8len(uc);
	// avoid the mess of bitwise manipulation
	if(l == 1)
	{
		buf[0] = uc;
		return;
	}

	buf[0] = ((uc >> (6*(l - 1))) & (0xFF >> l)) | (0xFF00 >> l);

	for(size_t i = 1; i < l; i++)
		buf[i] = 0x80 | (0x3F & (uc >> (6 * (l - i - 1))));
}

uchar_t fgetu8(FILE *f)
{
	char b[4];
	int c0 = fgetc(f);
	
	if(c0 == EOF)
		return EOF;
	
	b[0] = c0;
	unsigned int l = _cl1(c0);
	uchar_t uc = c0 & (0xFF >> l);
	
	if(l < 2 || l > 4)
		return _w1252_fallback(b[0]);

	for(size_t i = 1; i < l; i++)
	{
		int curr = fgetc(f);
		b[i] = curr;

		if(curr == EOF)
			goto on_eof;

		if((curr & 0xC0) != 0x80)
			goto bad_char;

		uc = uc << 6;
		uc |= curr & 0x3F;

		continue;

		bad_char:
		ungetc(curr, f);
		on_eof:
		for(size_t j = 1; j < i; j++)
			ungetc(b[j], f);
		
		return _w1252_fallback(b[0]);
	}
	
	return uc;
}

void fputu8(uchar_t c, FILE *f)
{
	char buf[4];
	u8enc(c, buf);
	size_t l = u8len(c);

	for(size_t i = 0; i < l; i++)
		fputc(buf[i], f);
}

#endif