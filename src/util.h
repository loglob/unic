/* util.h: Provides functions for working with unicode characters */
#pragma once
#include "uchar.h"
#include "ucdb.h"
#include <ctype.h>

/* Retrieves the unicode character class of the given character */
enum unic_gc uchar_class(uchar_t c)
{
	const struct ucdb_entry *e = ucdb_get(c);
	return e ? e->class : UCLASS_UNASSIGNED;
}

bool uclass_is(enum unic_gc general, enum unic_gc specific)
{
	return general == specific
		|| (general & ((1 << UNIC_GC_SUB_BITS) - 1) == 0 && (general >> UNIC_GC_SUB_BITS == specific >> UNIC_GC_SUB_BITS));
}

bool uchar_is(uchar_t chr, enum unic_gc class)
{
	return uclass_is(class, uchar_class(chr));
}

/* Determines if the given unicode character is whitespace */
int u_isspace(uchar_t c)
{
	return (c < 0x80 && isspace(c)) || uchar_is(c, UCLASS_SEPARATOR);
}

/* Returns the simple lowercase mapping of the given character */
uchar_t uchar_lower(uchar_t c)
{
	const struct ucdb_entry *e = ucdb_get(c);
	return e ? c + e->lowercaseDelta : c;
}

/* Returns the simple uppercase mapping of the given character */
uchar_t uchar_upper(uchar_t c)
{
	const struct ucdb_entry *e = ucdb_get(c);
	return e ? c + e->uppercaseDelta : c;
}
