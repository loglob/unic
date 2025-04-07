#pragma once
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "unic.h"
#include "interface.h"

/** A string with some extra information */
typedef struct {
	/** Actual bytes */
	const char *bytes;
	/** unicode codepoints */
	const uchar_t *chars;
	/** number of bytes/characters */
	size_t size, count;
} str_t;

struct Codepoint {
	uchar_t codepoint;
	uchar_t simpleLower, simpleUpper;
	enum unic_gc category;
};

struct Pipe {
	int fds[2];
	FILE *out;
	FILE *in;
};

#define GEN_EQ(type, eq, pr, f, want, want_str, got, got_str, fmt, ...) do { \
	type __w = (want); \
	type __g = (got); \
	if(!(eq)) \
		testFailure( "Assertion Failure: Expected %s to equal %s, expected " f ", but got " f "." fmt , got_str, want_str, pr(__w), pr(__g) ,##__VA_ARGS__ ); \
} while(0)

#define GEN_NEQ(type, eq, pr, f, bad, bad_str, got, got_str, fmt, ...) do { \
	type __b = (bad); \
	type __g = (got); \
	if((eq)) \
		testFailure( "Assertion Failure: Expected %s not to equal %s, but got " f "." fmt , got_str, bad_str, pr(__g) ,##__VA_ARGS__ ); \
} while(0)

#define assertIEq(want, got, ...) GEN_EQ(intmax_t, __w == __g, , "%ld", want, #want, got, #got, "" __VA_ARGS__)
#define assertUEq(want, got, ...) GEN_EQ(uintmax_t, __w == __g, , "%lu", want, #want, got, #got, "" __VA_ARGS__)
#define assertSEq(want, got, ...) GEN_EQ(const char*, (strcmp(__w,__g) == 0),, "'%s'", want, #want, got, #got, "" __VA_ARGS__)
#define assertCEq(want, got, ...) GEN_EQ(uchar_t, __w == __g,  , "U+%04X", want, #want, got, #got, "" __VA_ARGS__)
#define assertPEq(want, got, ...) GEN_EQ(const void*, __w == __g,  , "%p", want, #want, got, #got, "" __VA_ARGS__)

#define assertUNEq(bad, got, ...) GEN_NEQ(uintmax_t, __b == __g,  , "%lu", bad, #bad, got, #got, "" __VA_ARGS__)


// _eq(_s, (got), (want), #got, #want, __FILE__, __LINE__, __func__)
