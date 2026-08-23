/* ucdb.h: Defines the unicode character database.
	Note that due to being over 33000 LOC, the file doesn't play nice with most IDEs. */
#include <stdint.h>
#include <stdlib.h>
#include "ucdb.h"

extern const struct ucdb_entry ucdb[];

const struct ucdb_entry *ucdb_get(uchar_t u)
{
	if(u <= UCDB_DIRECT_MAX)
		return ucdb + u;
	if(u > UNIC_MAX)
		return NULL;

	size_t l = UCDB_DIRECT_MAX + 1, r = UCDB_COUNT - 1;

	while(l <= r)
	{
		size_t m = (l + r) / 2;
		if(ucdb[m].codepoint < u)
			l = m + 1;
		else if(ucdb[m].codepoint > u)
			r = m - 1;
		else
			return ucdb + m;
	}

	return NULL;
}

uchar_t uchar_canonical(uchar_t c)
{
	/*
		we want 
		```
			canonical(c) := min { canonical(x) | alike(c,x) }
		```
		which is equivalent to
		```
			canonical(c) = min ({ canonical(x) | x ∈ { c, lower(c), upper(c) } } ∪ {c})
		```
		For almost all `c`, `lower(upper(c)) == c` and `lower(lower(c)) == lower(c)` (resp. `upper`) hold, which means only one iteration is required, i.e.
		```
			canonical(c) ≈ min { c, lower(c), upper(c) }
		```
		Other cases are hard baked during code gen
	*/
	switch(c)
	{
		// pre-bake some special cases
		$CANONICAL_SPECIAL

		default: {
			// default: min { c, lower(c), upper(c) }
			const struct ucdb_entry *e = ucdb_get(c);
			
			if(e == NULL || e->deltaIndex == 0)
				return c;

			struct ucdb_delta d = ucdb_deltas[e->deltaIndex];
			int o = d.lowercaseDelta < d.uppercaseDelta ? d.lowercaseDelta : d.uppercaseDelta;
			return c + (o < 0 ? o : 0);
		}
	}
}

const struct ucdb_delta ucdb_deltas[] = {
	$UCDB_DELTAS
};

const struct ucdb_entry ucdb[] = {
	$UCDB
};
