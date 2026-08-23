#include "common.h"
#include "unic.h"
#include "ucdb.h"

extern const struct ucdb_entry ucdb[];

TEST(alikeImpliesCanonical) 
{
	for(size_t i = 0; i < UCDB_COUNT; ++i)
	{
		const struct ucdb_entry ae = ucdb[i];
		
		if(! ae.deltaIndex)
			continue;

		uchar_t a = ae.codepoint;

		for(size_t j = 0; j < UCDB_COUNT; ++j)
		{
			const struct ucdb_entry be = ucdb[j];

			if(!be.deltaIndex)
				continue;

			uchar_t b = be.codepoint;

			if(uchar_alike(a, b))
				assertCEq(uchar_canonical(a), uchar_canonical(b), " Canonical maps of U+%04X and U+%04X differ", a, b);
		}
	}
}
