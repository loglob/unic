#pragma once
#include <stdlib.h>
#include <string.h>
#include "../include/unic.h"

static void _eq_i(long got, long want, const char *got_expr, const char *want_expr, const char *file, long line, const char *func)
{
	if(got != want)
	{
		fprintf(stderr, "test: %s: %lu: %s: Assertion `%s == %s` failed; Got %lu, expected %lu\n", file, line, func, got_expr, want_expr, got, want);
		exit(EXIT_FAILURE);
	}
}

static void _eq_s(const char *got, const char *want, const char *got_expr, const char *want_expr, const char *file, long line, const char *func)
{
	if((got == NULL || want == NULL) ? (got != want) : strcmp(got, want))
	{
		fprintf(stderr, "test: %s: %lu: %s: Assertion `%s == %s` failed; Got '%s', expected '%s'\n", file, line, func, got_expr, want_expr, got ? got : "(nil)", want ? want : "(nil)");
		exit(EXIT_FAILURE);
	}
}

#define _eq(s, ...) _eq##s (__VA_ARGS__)

#define eq_i(got, want) _eq(_i, (long)(got), (long)(want), #got, #want, __FILE__, __LINE__, __func__)
#define eq_s(got, want) _eq(_s, (got), (want), #got, #want, __FILE__, __LINE__, __func__)

const char mul_apin[] = "\xF0\x92\x80\xAF" "\xF0\x92\x80\xB3";
const char deLied[] =
	"Deutschland, Deutschland " "\xC3\xBC" "ber alles,\n"
	"\xC3\x9C" "ber alles in der Welt";
