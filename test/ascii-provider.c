#include <stddef.h>
#include <stdio.h>
#include "interface.h"
#include "string.h"
#include "common.h"

PROVIDER(char, alphanum)

size_t format_char(char *to, size_t n, const char thing[restrict static 1])
{
	return snprintf(to, n, "%c", *thing);
}

size_t alphanum(size_t cap, char buf[restrict static cap])
{
	size_t N = 26+26+10;
	if(cap >= N)
	{
		size_t i = 0;

		for(char c = 'a'; c <= 'z'; ++c)
			buf[i++] = c;
		for(char c = 'A'; c <= 'Z'; ++c)
			buf[i++] = c;
		for(char c = '0'; c <= '9'; ++c)
			buf[i++] = c;
	}

	return N;
}
