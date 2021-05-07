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

static void _pr_relp(const char *p, const char *base, const char *base_expr)
{
	if(p == NULL || p < base - 0x100 || p > base + strlen(base) + 0x100)
		fprintf(stderr, "%p", p);
	else if(p < base || p > base + strlen(base))
		fprintf(stderr, "%s%+zd", base_expr, p - base);
	else
		fprintf(stderr, "%s%+zd(%s)", base_expr, p - base, p);
}

static void _eq_p(const char *got, const char *want, const char *base, const char *got_expr, const char *want_expr, const char *base_expr, const char *file, long line, const char *func)
{
	if(got != want)
	{
		fprintf(stderr, "test: %s: %lu: %s: Assertion `%s == %s` failed; Got ", file, line, func, got_expr, want_expr);
		_pr_relp(got, base, base_expr);
		fprintf(stderr, ", expected ");
		_pr_relp(want, base, base_expr);
		fputc('\n', stderr);

		exit(EXIT_FAILURE);
	}
}

#define _eq(s, ...) _eq##s (__VA_ARGS__)

#define eq_i(got, want) _eq(_i, (long)(got), (long)(want), #got, #want, __FILE__, __LINE__, __func__)
#define eq_p(got, want, base) _eq(_p, (got), (want), (base), #got, #want, #base, __FILE__, __LINE__, __func__)
#define eq_s(got, want) _eq(_s, (got), (want), #got, #want, __FILE__, __LINE__, __func__)

const char mul_apin[] = "\xF0\x92\x80\xAF" "\xF0\x92\x80\xB3";
const char deLied[] =
	"Deutschland, Deutschland " "\xC3\xBC" "ber alles; " "\xC3\x9C" "ber alles in der Welt";
