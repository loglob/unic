/* util.h: Provides functions for working with unicode characters */
#pragma once
#include "uchar.h"
#include "ucdb.h"
#include <ctype.h>

/* Retrieves the unicode character class of the given character */
unic_class_t uchar_class(uchar_t c)
{
	const ucdb_entry_t *e = ucdb_get(c);
	return e ? e->class : UCLASS_UNASSIGNED;
}

/* Determines if the given unicode character is whitespace */
int u_isspace(uchar_t c)
{
	return (uchar_class(c) & UCCLASS_SEPARATOR)
		|| (c < 0x80 && isspace(c));
}

#define CC(a,b) a##b
#define MAPFUNC(n) uchar_t CC(uchar_, n)(uchar_t c) \
{\
	const ucdb_entry_t *e = ucdb_get(c);\
	return (e && e->CC(n, caseMap)) ? e->CC(n, caseMap) : c;\
}

/* Returns the simple lowercase mapping of the given character */
MAPFUNC(lower)
/* Returns the simple uppercase mapping of the given character */
MAPFUNC(upper)

#undef MAPFUNC
#undef CC

/* Returns the simple titlecase mapping of the given character */
uchar_t uchar_title(uchar_t c)
{
	if(c < TCMIN || c > TCMAX)
		return uchar_upper(c);

	size_t l = 0, r = sizeof(ucdb_tcmap) / sizeof(ucdb_tcmap[0]);

	while(l <= r)
	{
		size_t m = (l + r) / 2;
		if(ucdb_tcmap[m].c < c)
			l = m + 1;
		else if(ucdb_tcmap[m].c > c)
			r = m - 1;
		else
			return ucdb_tcmap[m].tc;
	}

	return uchar_upper(c);
}
