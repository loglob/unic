#pragma once
#include "unic.h"

/* The amount of unicode characters assigned under the UNIC_VERSION unicode standard */
#define UCDB_COUNT 34924
/* The highest character with its UCDB index and codepoint are equal */
#define UCDB_DIRECT_MAX 0x0377

/* The type of a ucdb entry */
struct ucdb_entry
{
	uchar_t codepoint : UNIC_BIT;
	enum unic_gc class : UNIC_GC_BITS;

	signed int uppercaseDelta : 17;
	signed int lowercaseDelta : 17;
} __attribute__((packed));

/* The unicode character database */
extern const struct ucdb_entry ucdb[];

const struct ucdb_entry *ucdb_get(uchar_t u);