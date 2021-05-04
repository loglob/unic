#ifndef _UNIC_H
#define _UNIC_H
#include <stdint.h>

/* The unicode version used to generate the library */
#define UNIC_VERSION 1300
/* The unicode version as a human readable string */
#define UNIC_VERSION_STRING "13.0.0"
/* The highest valid unicode character */
#define UNIC_MAX 0x10FFFD
/* The amount of bits needed to encode every valid unicode character */
#define UNIC_BIT 21

#define UEOF ((uchar_t)-1)

typedef uint32_t uchar_t;

/** Reads the next unicode character from the given utf-8 encoded steam.
 * Returns UEOF on reading EOF.
 * @param f The file stream
 * @returns The next unicode character in the stream
*/
extern uchar_t fgetu8(FILE *f);

/** Writes the given unicode character to the given utf-8 encoded stream.
 * @param c A unicode character
 * @param f A file stream
*/
extern void fputu8(uchar_t c, FILE *f);

/** Reads the next utf-8 encoded character from the given string.
 * Note that reading and re-encoding a character may change its length due to improper encoding in source streams.
 * @param str The utf-8 encoded buffer to read from.
 * @param c The location to store the character in. May be NULL to only determine the length of the next character.
 * @returns The amount of bytes read. */
size_t u8dec(const char *str, uchar_t *c);

/** Writes the given unicode character to the buffer.
 * @param uc The unicode character
 * @param buf The buffer to write the character to. May be NULL to only determine its length.
 * @returns The amount of bytes written. */
size_t u8enc(uchar_t uc, char *buf);

#endif