/* Defines the uchar_t datatype as well as multiple preprocessor macros for use with it and
	the current version of the unicode standard */
#ifndef _UNIC_UCHAR_H
#define _UNIC_UCHAR_H
#include <stdint.h>

/* The unicode version used to generate the library */
#define UNIC_VERSION 1300
/* The unicode version as a human readable string */
#define UNIC_VERSION_STRING "13.0.0"
/* The highest valid unicode character */
#define UNIC_MAX 0x10FFFD
/* The amount of bits needed to encode every valid unicode character */
#define UNIC_BIT 21

typedef uint32_t uchar_t;

#endif
