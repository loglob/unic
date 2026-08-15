/* ucdb.h: Defines the unicode character database.
	Note that due to being over 33000 LOC, the file doesn't play nice with most IDEs. */
#pragma once
#include "unic.h"

/** The amount of unicode characters assigned under the UNIC_VERSION unicode standard */
#define UCDB_COUNT $count
/** The highest character with its UCDB index and codepoint are equal */
#define UCDB_DIRECT_MAX $max_direct

/** The type of a ucdb entry */
struct ucdb_entry
{
	uchar_t codepoint : UNIC_BIT;
	enum unic_gc class : UNIC_GC_BITS;

	signed int uppercaseDelta : $ucd_bits;
	signed int lowercaseDelta : $lcd_bits;
} __attribute__((packed));

/** The unicode character database */
extern const struct ucdb_entry ucdb[];

/** Gets the ucdb entry for the given character */
const struct ucdb_entry *ucdb_get(uchar_t u);