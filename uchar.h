/* Defines the uchar_t datatype as well as multiple preprocessor macros for use with it and
	the current version of the unicode standard */
#ifndef _UNIC_UCHAR_H
#define _UNIC_UCHAR_H
#include <stdint.h>

/* The unicode version used to generate the library */
#define UNIC_VERSION "12.0.0"
/* The amount of unicode characters assigned under the UNIC_VERSION unicode standard */
#define UCHAR_COUNT 32840
/* The highest valid unicode character */
#define UCHAR_MAX 0x10FFFD
/* The amount of bits needed to encode every valid unicode character */
#define UCHAR_BIT 21
/* The amount of unbroken assigned unicode characters in the unicode character database */
#define UCHAR_IN_LINE 889
/* Used to avoid Wsign-compare warnings without manual casting */
#define UCHAR_EOF ((uchar_t)EOF)

typedef uint32_t uchar_t;

#endif