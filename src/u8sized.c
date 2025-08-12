#include "unic.h"
#include <stdint.h>

#define HAS_NEXT(byteIx, charIx, size, str) \
	( (byteIx) < (size).byteCount && (charIx) < (size).charCount && ((size).bytesExact || (size).charsExact || str[byteIx]) )

/** Expands to an iteration over every character in the string
	@param str The string to iterate over
	@param size The size of `str`
	@param __VA_ARGS__ A statement of the loop body.
		Received the variables `bytes` and `chars` giving the respective indices,
		`c` giving the current character, and `l` giving that char's size
*/
#define SCAN(str, size, ...) \
{ \
	const char *const _s = (str); \
	const u8size_t _z = (size); \
	for(size_t byteIx = 0, charIx = 0; HAS_NEXT(byteIx, charIx, _z, _s); ++charIx) \
	{ \
		uchar_t c; \
		const size_t l = u8ndec(_s + byteIx, _z.byteCount - byteIx, &c); \
		{ __VA_ARGS__ } \
		byteIx += l; \
	} \
}

/** Scans over two strings simultaneously
	Sets `c1` and `c2` to the current characters, and `l1` and `l2` to their lengths.
	The end of strings is observable as a single char with `l* = 0`.
	Once one such character is observed, iteration ends afterwards.
*/
#define BISCAN(str1, size1, str2, size2, ...) \
{ \
	const char *const _s1 = (str1), *const _s2 = (str2); \
	const u8size_t _z1 = (size1), _z2 = (size2); \
	for(size_t byteIx1 = 0, charIx1 = 0, byteIx2 = 0, charIx2 = 0;; ++charIx1, ++charIx2) \
	{ \
		uchar_t c1, c2; \
		const size_t l1 = HAS_NEXT(byteIx1, charIx1, _z1, _s1) \
			? u8ndec(_s1 + byteIx1, _z1.byteCount - byteIx1, &c1) \
			: (c1 = 0); \
		const size_t l2 = HAS_NEXT(byteIx2, charIx2, _z2, _s2) \
			? u8ndec(_s2 + byteIx2, _z2.byteCount - byteIx2, &c2) \
			: (c2 = 0); \
		{ __VA_ARGS__ } \
		byteIx1 += l1; \
		byteIx2 += l2; \
		if(l1 == 0 || l2 == 0) break; \
	} \
}

u8size_t u8z_min(u8size_t a, u8size_t b)
{
	return (u8size_t) {
		a.bytesExact || b.bytesExact,
		(a.byteCount < b.byteCount) ? a.byteCount : b.byteCount,
		a.charsExact || b.charsExact,
		(a.charCount < b.charCount) ? a.charCount : b.charCount
	};
}

u8size_t u8z_offset(u8size_t z, size_t byteOffset, size_t charOffset)
{
	return (u8size_t) {
		z.bytesExact,
		byteOffset > z.byteCount ? 0 : z.byteCount - byteOffset,
		z.charsExact,
		charOffset > z.charCount ? 0 : z.charCount - charOffset
	};

}

/** @returns Whether every string limited by `a` is always longer than any string limited by `b`. */
static inline bool triviallyGreater(u8size_t a, u8size_t b)
{
	// a NUL terminated (and thus may be arbitrarily short)
	if(!(a.bytesExact || a.charsExact))
		return false;
	// ∃ over b, ∀ over a
	return (a.byteCount > b.byteCount          && a.charCount > b.byteCount)
		|| (a.byteCount > UTF8_MAX*b.charCount && a.charCount > b.charCount);
}

u8size_t u8z_strsize(const char *str, u8size_t size)
{
	if(size.charsExact && size.bytesExact)
		return size;

	size_t bytes = 0, chars = 0;

	SCAN(str, size, {
		++chars;
		bytes += l;
	})

	return (u8size_t){ true, bytes, true, chars };
}

size_t u8z_strlen(const char *str, u8size_t size)
{
	if(size.charsExact)
		return size.charCount;

	size_t chars = 0;

	SCAN(str, size, {
		++chars;
	})

	return chars;	
}

// used with the strmap functions
static uchar_t uchar_id(uchar_t x)
{
	return x;
}

u8size_t u8z_strcpy(const char *str, u8size_t size, char *dst, size_t cap, bool nulTerminate)
{
	return u8z_strmap(str, size, dst, cap, nulTerminate, uchar_id);
}

uchar_t u8z_strat(const char *str, u8size_t size, size_t pos)
{
	SCAN(str, size, {
		if(charIx == pos)
			return c;
	})

	return 0;	
}

/** Generates a loop over an entire string searching for a single location
	@param str String to iterate
	@param size u8size_t of `str`
	@param cond acceptance condition
	@param stopCond reject condition. Checked BEFORE cond.
*/
#define SCANFUNC(str, size, cond, stopCond) { \
	SCAN(str, size, { \
		if(stopCond) \
			break; \
		if(cond) \
			return str + byteIx; \
	}) \
	return NULL; \
}

/** Generates a loop over an entire string searching for a single location
	@param str String to iterate
	@param size u8size_t of `str`
	@param cond acceptance condition
	@param stopCond early stop condition, returning last match. Checked BEFORE cond.
*/
#define R_SCANFUNC(str, size, cond, stopCond) { \
	const char *ret = NULL; \
	SCAN(str, size, { \
		if(stopCond) \
			break; \
		if(cond) \
			ret = str + byteIx; \
	}) \
	return ret; \
}

const char *u8z_strpos(const char *str, u8size_t size, size_t pos)
	SCANFUNC(str, size, charIx == pos, false)

const char *u8z_strchr(const char *str, u8size_t size, uchar_t chr)
	SCANFUNC(str, size, c == chr, false)

const char *u8z_strchrI(const char *str, u8size_t size, uchar_t chr)
	SCANFUNC(str, size, uchar_alike(c, chr), false)

const char *u8z_strrchr(const char *str, u8size_t size, uchar_t chr)
	R_SCANFUNC(str, size, c == chr, false)

const char *u8z_strrchrI(const char *str, u8size_t size, uchar_t chr)
	R_SCANFUNC(str, size, uchar_alike(c, chr), false)


const char *u8z_strstr(const char *haystack, u8size_t n, const char *needle, u8size_t m)
{
	const size_t len = u8z_strlen(needle, m);
	// make length available to streq()
	m.charsExact = true;
	m.charCount = len;

	if(triviallyGreater(m, n))
		return NULL;

	SCANFUNC(haystack, n, u8z_streq(
		haystack + byteIx, u8z_min(u8z_offset(n, byteIx, charIx), m),
		needle, m
	), triviallyGreater(m, u8z_offset(n, byteIx, charIx)))
}

const char *u8z_strrstr(const char *haystack, u8size_t n, const char *needle, u8size_t m)
{
	const size_t len = u8z_strlen(needle, m);
	// make length available to streq()
	m.charsExact = true;
	m.charCount = len;

	if(triviallyGreater(m, n))
		return NULL;

	R_SCANFUNC(haystack, n, u8z_streq(
		haystack + byteIx, u8z_min(u8z_offset(n, byteIx, charIx), m),
		needle, m
	), triviallyGreater(m, u8z_offset(n, byteIx, charIx)))
}

const char *u8z_strstrI(const char *haystack, u8size_t n, const char *needle, u8size_t m)
{
	const size_t len = u8z_strlen(needle, m);
	// make length available to streq()
	m.charsExact = true;
	m.charCount = len;

	if(triviallyGreater(m, n))
		return NULL;

	SCANFUNC(haystack, n, u8z_streqI(
		haystack + byteIx, u8z_min(u8z_offset(n, byteIx, charIx), m),
		needle, m
	), triviallyGreater(m, u8z_offset(n, byteIx, charIx)))
}

const char *u8z_strrstrI(const char *haystack, u8size_t n, const char *needle, u8size_t m)
{
	const size_t len = u8z_strlen(needle, m);
	// make length available to streq()
	m.charsExact = true;
	m.charCount = len;

	if(triviallyGreater(m, n))
		return NULL;

	R_SCANFUNC(haystack, n, u8z_streqI(
		haystack + byteIx, u8z_min(u8z_offset(n, byteIx, charIx), m),
		needle, m
	), triviallyGreater(m, u8z_offset(n, byteIx, charIx)))
}

bool u8z_streq(const char *a, u8size_t n, const char *b, u8size_t m)
{
	if(triviallyGreater(n, m) || triviallyGreater(m, n))
		return false;

	BISCAN(a, n, b, m, {
		if(c1 != c2  || (l1 > 0) != (l2 > 0))
			return false;
	})

	return true;
}

bool u8z_streqI(const char *a, u8size_t n, const char *b, u8size_t m)
{
	if(triviallyGreater(n, m) || triviallyGreater(m, n))
		return false;

	BISCAN(a, n, b, m, {
		if(!uchar_alike(c1, c2) || (l1 > 0) != (l2 > 0))
			return false;
	})

	return true;
}

bool u8z_prefix(const char *prefix, u8size_t n, const char *full, u8size_t m)
{
	if(triviallyGreater(n, m))
		return false;

	BISCAN(prefix, n, full, m, {
		if(l1 == 0) // prefix ended
			return true;
		if(c1 != c2)
			return false;
	})

	return false;
}

bool u8z_prefixI(const char *prefix, u8size_t n, const char *full, u8size_t m)
{
	if(triviallyGreater(n, m))
		return false;

	BISCAN(prefix, n, full, m, {
		if(l1 == 0) // prefix ended
			return true;
		if(! uchar_alike(c1, c2))
			return false;
	})

	return false;
}

bool u8z_isnorm(const char *str, u8size_t size)
{
	return u8z_chknorm(str, size).bytesExact;
}

u8size_t u8z_chknorm(const char *str, u8size_t size)
{
	size_t totalBytes = 0, totalChars = 0;

	SCAN(str, size, {
		if(!(c == 0 && l == 2) && l != u8enc(c, NULL))
			return (u8size_t){ .bytesExact = false, .byteCount = byteIx, .charsExact = false, .charCount = charIx };

		totalBytes += l;
		++totalChars;
	})

	return (u8size_t){ .bytesExact = true, .byteCount = totalBytes, .charsExact = true, .charCount = totalChars };
}

bool u8z_isvalid(const char *str, u8size_t size)
{
	return u8z_chkvalid(str, size).bytesExact;
}

u8size_t u8z_chkvalid(const char *str, u8size_t size)
{
	size_t totalBytes = 0, totalChars = 0;

	SCAN(str, size, {
		if(uchar_class(c) == UCLASS_UNASSIGNED)
			return (u8size_t){ .bytesExact = false, .byteCount = byteIx, .charsExact = false, .charCount = charIx };
	})

	return (u8size_t){ .bytesExact = true, .byteCount = totalBytes, .charsExact = true, .charCount = totalChars };
}

u8size_t u8z_strmap(const char *str, u8size_t size, char *dst, size_t cap, bool nulTerminate, uchar_t (*map_f)(uchar_t))
{
	size_t bytes = 0;
	size_t chars = 0;
	bool truncated = false;

	SCAN(str, size, {
		const uchar_t y = map_f(c);
		char next[UTF8_MAX];
		size_t nl;

		if(!y && nulTerminate)
		{
			next[0] = UNUL[0];
			next[1] = UNUL[1];
			nl = 2;
		}
		else
			nl = u8enc(y, next);
		
		if(bytes + nl + !!nulTerminate > cap)
		{
			truncated = true;
			break;
		}

		if(dst) for(size_t i = 0; i < nl; ++i)
			dst[bytes + i] = next[i];

		bytes += nl;
		++chars;
	})

	if(nulTerminate)
	{
		if(cap == 0)
			truncated = true;
		else
		{
			if(dst)
				dst[bytes] = 0;
			++bytes;
		} 
	}

	return (u8size_t){ .bytesExact = !truncated, .byteCount = bytes, .charsExact = !truncated, .charCount = chars };
}
