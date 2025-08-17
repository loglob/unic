#include "common.h"
#include "interface.h"
#include "u8text.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static inline u8file_t u8file(const char *content)
{
	size_t size = strlen(content);
	u8file_t f = u8txt_load(content, size, NULL);

	if(! f)
		testFailure("u8txt_load(): %s", strerror(errno));

	f->udata = (void*)content;

	return f;
}

static const char *A = "aaa";
static const char *B = "bbb";
static const char *C = "ccc";
static const char *D = "ddd";

PROVIDER(u8list_t, lists)

#define linkFile(list, file) _linkFile((list), #list, (file), #file)

static void _linkFile(u8list_t list, const char *_list, u8file_t file, const char *_file)
{
	errno = 0;
	int res = u8txt_link(list, file);

	if(res)
		testFailure("u8txt_link(%s, %s): %d (%s)", _list, _file, res, strerror(errno));
}

size_t lists(size_t cap, u8list_t buf[restrict static cap])
{
	const size_t N = 8;
	
	if(N > cap)
		return N;

	u8file_t a = u8file(A), b = u8file(B), c = u8file(C), d = u8file(D);

	for(size_t i = 0; i < N; ++i)
	{
		u8list_t ls = u8txt_create();

		if(! ls)
			testFailure("u8txt_create(): %s", strerror(errno));

		buf[i] = ls;
	}

	linkFile(buf[1], a);

	linkFile(buf[2], a);
	linkFile(buf[2], b);

	linkFile(buf[3], b);
	linkFile(buf[3], d);

	linkFile(buf[4], a);
	linkFile(buf[4], c);
	linkFile(buf[4], d);

	linkFile(buf[5], a);
	linkFile(buf[5], d);

	linkFile(buf[6], c);

	linkFile(buf[7], a);
	linkFile(buf[7], b);
	linkFile(buf[7], c);
	linkFile(buf[7], d);

	return N;
}

size_t format_u8list_t(char *const to, const size_t n, const u8list_t thing[restrict static 1])
{
	size_t w = 0;

	#define push(s) do { if(w + sizeof(s) > n) return w+sizeof(s); memcpy(to + w, s, sizeof(s)); w += sizeof(s) - 1; } while(0)

	push("[");

	size_t z = u8txt_count(*thing);
	
	for(size_t i = 0; i < z; ++i)
	{
		if(i)
			push(", ");
		else
			push(" ");
		
		u8file_t f = u8txt_access(*thing, i);

		assert(w < n);
		w += snprintf(to + w, n - w, "'%.3s%s' %p:%p", f->bytes, f->size.charCount > 3 ? "…" : "", f, f->bytes);
	}

	push(" ]");
	return w;
}

TEST(addToEmpty)
{
	u8list_t ls = u8txt_create();
	assertTrue(ls != NULL);

	u8file_t f = u8file("file");
	assertTrue(f != NULL);

	assertIEq(0, u8txt_link(ls, f));
	assertUEq(1, u8txt_count(ls));
	assertPEq(f, u8txt_access(ls, 0));

	assertIEq(1, u8txt_link(ls, f));
}


/** tests that linking the (reference) same file again does nothing */
TEST(linkIdempotent, u8list_t, list, uint16_t, i)
{
	size_t z0 = u8txt_count(list);

	if(i >= z0)
		return;

	u8file_t f = u8txt_access(list, i);
	assertTrue(f != NULL);

	assertIEq(1, u8txt_link(list, f));
	assertUEq(z0, u8txt_count(list));
}

/** tests that linking a different file with equivalent content does nothing */
TEST(linkContentIdempotent, u8list_t, list, uint16_t, i)
{
	size_t z0 = u8txt_count(list);

	if(i >= z0)
		return;

	u8file_t f = u8txt_access(list, i);
	assertTrue(f != NULL);

	u8file_t alias = u8txt_load(f->bytes, f->size.byteCount, NULL);

	assertIEq(1, u8txt_link(list, alias));
	assertUEq(z0, u8txt_count(list));
	assertPEq(f, u8txt_access(list, i));

	u8txt_free(alias);
}

// NOTE: these tests assert persistent ordering, which the interface doesn't explicitly guarantee
#define LIST_CAP 20

TEST(unlinkThenLink, u8list_t, list, uint16_t, i)
{
	size_t z0 = u8txt_count(list);

	if(i >= z0)
		return;

	u8file_t buf[LIST_CAP];
	assertTrue(z0 < LIST_CAP);

	// copy to buffer
	for(size_t j = 0; j < z0; ++j)
	{
		buf[j] = u8txt_access(list, j);
		assertTrue(buf[j] != NULL);
	}

	assertTrue(u8txt_unlink(list, buf[i]));
	assertUEq(z0 - 1, u8txt_count(list));

	assertIEq(0, u8txt_link(list, buf[i]));
	assertUEq(z0, u8txt_count(list));
	
	for(size_t j = 0; j < z0; ++j)
		assertPEq(buf[j], u8txt_access(list, j));
}

/** content that is never part of input file lists */
const char *NEW = "NEW";

TEST(fileIsNew, u8list_t, list)
{
	assertPEq(NULL, u8txt_fileof(list, NEW));
}

TEST(linkThenUnlink, u8list_t, list)
{
	size_t z0 = u8txt_count(list);
	u8file_t f = u8file(NEW);

	u8file_t buf[LIST_CAP];
	assertTrue(z0 < LIST_CAP);
	
	// copy to buffer
	for(size_t j = 0; j < z0; ++j)
	{
		buf[j] = u8txt_access(list, j);
		assertTrue(buf[j] != NULL);
	}

	assertIEq(0, u8txt_link(list, f));
	assertUEq(z0 + 1, u8txt_count(list));
	
	assertTrue(u8txt_unlink(list, f));
	assertUEq(z0, u8txt_count(list));
	
	// compare with saved buffer
	for(size_t j = 0; j < z0; ++j)
		assertPEq(buf[j], u8txt_access(list, j));
}

TEST(fileof, u8list_t, list, uint16_t, i)
{
	size_t z = u8txt_count(list);

	if(i >= z)
		return;

	u8file_t f = u8txt_access(list, i);
	assertTrue(f != NULL);
	
	assertPEq(f, u8txt_fileof(list, f->bytes));
	assertPEq(f, u8txt_fileof(list, f->bytes + 1));
	assertPEq(f, u8txt_fileof(list, f->bytes + 2));
	// NUL terminator not in reach
	assertPEq(NULL, u8txt_fileof(list, f->bytes + 3));
}
