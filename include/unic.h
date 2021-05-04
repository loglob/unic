#ifndef _UNIC_H
#define _UNIC_H
typedef uint32_t uchar_t;

/* Reads the next unicode character from the given utf-8 encoded stream.
	Returns EOF on reading EOF. */
uchar_t fgetu8(FILE *);
/* Writes the given unicode character to the given utf-8 encoded stream. */
void fputu8(uchar_t, FILE *);
/* Reads the next utf-8 encoded character from the given string.
	Returnes the amount of bytes read.
	The character is Stored in *c.
	c may be NULL to only determine the length of the next character. */
size_t u8dec(const char *str, uchar_t *c);
/* Writes the given unicode character to the buffer.
	Returnes the amount of bytes written.
	buf may be NULL to only determine the length of the given character. */
size_t u8enc(uchar_t uc, char *buf);

#endif