// provider for strings to test decoding routines
#include <stddef.h>
#include <stdio.h>
#include "interface.h"
#include "string.h"
#include "common.h"

// proper UTF-8 encoding of ü
#define ue "\xC3\xBC"

PROVIDER(str_t, fixed_strings)

size_t format_str_t(char *to, size_t n, const str_t thing[restrict static 1])
{
	unsigned w = snprintf(to, n, "'%s' (", thing->bytes);

	for(size_t i = 0; i < thing->size; ++i)
		w += snprintf(to + w, w < n ? n - w : 0, "%02hhd", thing->bytes[i]);

	w += snprintf(to + w, w < n ? n - w : 0, ")");

	return w;
}

static const char deLied[] = "Bl"ue"h im Glanze dieses Gl"ue"ckes, Bl"ue"he, deutsches Vaterland!";
static const uchar_t deLiedChr[] = {
	'B','l',0xFC,'h',' ','i','m',' ','G','l','a','n','z','e',' ','d','i','e','s','e','s',' ','G','l',0xFC,'c','k','e','s',',',' ',
	'B','l',0xFC,'h','e',',',' ','d','e','u','t','s','c','h','e','s',' ','V','a','t','e','r','l','a','n','d','!'
};

static str_t strings[] = {
	{
		"",
		(uchar_t[]){  },
		0, 0
	},
	{
		deLied, deLiedChr,
		sizeof(deLied) - 1, sizeof(deLiedChr)/sizeof(uchar_t)
	},
	{
		"\xF0\x92\x80\xAF" "\xF0\x92\x80\xB3", (uchar_t[2]){ 0x1202F, 0x12033 },
		8, 2
	},
	{
		"/////////", (uchar_t[]){ '/','/','/','/','/','/','/','/','/' },
		9, 9
	}
};


size_t fixed_strings(size_t cap, str_t buf[restrict static cap])
{
	const size_t N = sizeof(strings)/sizeof(str_t);
	if(cap >= N)
		memcpy(buf, strings, sizeof(strings));

	return N;
}
