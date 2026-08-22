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

const struct ucdb_delta ucdb_deltas[] = {
	$UCDB_DELTAS
};

const struct ucdb_entry ucdb[] = {
	$UCDB
};
