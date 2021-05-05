/* util.h: Provides functions for working with unicode characters */
#pragma once
#include "uchar.h"
#include "ucdb.h"
#include <ctype.h>

enum unic_gc uchar_class(uchar_t c)
{
	// check for private use characters
	if((c >= 0xE000 && c <= 0xF8FF) || (c >= 0xF0000 && c <= 0xFFFFD) || (c >= 0x100000 && c <= 0x10FFFD))
		return UCLASS_PRIVATE_USE;

	const struct ucdb_entry *e = ucdb_get(c);
	return e ? e->class : UCLASS_UNASSIGNED;
}

bool uchar_alike(uchar_t a, uchar_t b)
{
	if(a == b)
		return true;

	const struct ucdb_entry *ea = ucdb_get(a), *eb = ucdb_get(b);
	
	if(!ea || !eb)
		return false;

	const uchar_t ua = a + ea->uppercaseDelta;
	const uchar_t ub = b + eb->uppercaseDelta;
	const uchar_t la = a + ea->lowercaseDelta;
	const uchar_t lb = b + eb->lowercaseDelta;

	// im fairly certain that no 2 characters actually match the u_ = l_ rules, they are mostly for completeness
	return ua == lb || ua == b || ua == ub || a == ub
		|| la == ub || la == b || la == lb || a == lb;
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

int u_isspace(uchar_t c)
{
	return (c < 0x80 && isspace(c)) || uchar_is(c, UCLASS_SEPARATOR);
}

uchar_t uchar_lower(uchar_t c)
{
	const struct ucdb_entry *e = ucdb_get(c);
	return e ? c + e->lowercaseDelta : c;
}

uchar_t uchar_upper(uchar_t c)
{
	const struct ucdb_entry *e = ucdb_get(c);
	return e ? c + e->uppercaseDelta : c;
}
