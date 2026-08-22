/* ucdb.h: Defines the unicode character database.
	Note that due to being over 33000 LOC, the file doesn't play nice with most IDEs. */
#pragma once
#include "unic.h"

/** The amount of unicode characters assigned under the UNIC_VERSION unicode standard */
#define UCDB_COUNT $count
/** The highest character whose UCDB index and codepoint are equal */
#define UCDB_DIRECT_MAX $max_direct

/** The type of a ucdb entry */
struct ucdb_entry
{
	uchar_t codepoint : UNIC_BIT;
	enum unic_gc class : UNIC_GC_BITS;
	/** @see ucdb_deltas */
	unsigned deltaIndex : $delta_ix_bits;
} __attribute__((packed));

/** Encodes the difference between a unicode character and its simple lower- and uppercase mappings */
struct ucdb_delta
{
	signed int uppercaseDelta : $ucd_bits;
	signed int lowercaseDelta : $lcd_bits;
} __attribute__((packed));

/** Gets the ucdb entry for the given character */
const struct ucdb_entry *ucdb_get(uchar_t u);

extern const struct ucdb_delta ucdb_deltas[];
