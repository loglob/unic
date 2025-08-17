#include "u8text.h"
#include "unic.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#if _POSIX_SOURCE >= 200112L
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/mman.h>
#endif

/** Number of bytes between each marker */
#define MARKER_FREQ 2048

/** Allocation grain for `FileList` dynamic array */
#define FILE_LIST_GRAIN 16

#ifdef __has_builtin
	#if __has_builtin(__builtin_unreachable)
		#define UNREACHABLE __builtin_unreachable()
	#else
		#define UNREACHABLE
	#endif
#else
	#define UNUNREACHABLE
#endif

//#region File List Interface
/** Simple ordered dynamic array */
struct FileList 
{
	/** Actual number of entries populated in `files` */
	size_t pop;
	/** Total capacity of `files` */
	size_t cap;
	/** Index of first non-empty string in `files` */
	size_t firstNonEmpty;
	u8file_t *files;
};

u8list_t u8txt_create()
{
	return calloc(1, sizeof(struct FileList));
}

void u8txt_destroy(u8list_t files, bool freeAll)
{
	if(! files)
		return;

	if(freeAll) for(size_t i = 0; i < files->pop; ++i)
		u8txt_free(files->files[i]);

	free(files->files);
	free(files);
}

/** Generic binary search over indices
	@param lo INCLUSIVE lower bound
	@param hi INCLUSIVE upper bound
	@param list Clojure context for `ord`
	@param target closure context for `ord`
	@param ord monotone ordering function. 
			Returns a negative if `ix < target`, positive if `ix > target` and 0 if `ix` is an exact match
	@returns The index (>= 0) of an exact match
	@returns `-(x + 1)` where `x` is the smallest index > the target, or `x = hi+1` if all elements were smaller
*/
static inline ssize_t _bseek(size_t lo, size_t hi, void *list, void *target, int (*ord)(size_t ix, void *list, void *target))
{
	size_t supremum = hi + 1;

	while(lo <= hi)
	{
		size_t diff = hi - lo;
		size_t mid = lo + diff/2 + diff%2;
		int cmp = ord(mid, list, target);

		if(cmp < 0)
			lo = mid + 1;
		else if(cmp > 0)
		{
			supremum = mid;
			hi = mid - 1;
		}
		else
			return mid;
		if(diff == 0)
			break;
	}
	
	return -(supremum + 1);
}

/** Compares two files to sort file lists. Considers all overlapping files to be equal.
	Empty files precede non-empty files but are only considered equal if the files are pointer-equal.
	@returns -1 if a < b
	@returns  0 if a == b
	@returns +1 if a > b
*/
static int _txt_cmp(u8file_t a, u8file_t b)
{	
	// empty files come before all non-empty files
	if(a->size.byteCount == 0 && b->size.byteCount == 0)
	{ // ordering doesn't really matter, only has to be total
		return ((size_t)a < (size_t)b) ? -1 : 
				((size_t)a > (size_t)b) ? +1 : 0;
	}
	else if(a->size.byteCount == 0)
		return -1;
	else if(b->size.byteCount == 0)
		return +1;

	const char *aHi = a->bytes + a->size.byteCount;
	const char *bHi = b->bytes + b->size.byteCount;

	return (aHi <= b->bytes) ? -1 :
			(a->bytes >= bHi) ? +1 : 0;
}

struct _list_ord_file_ctx
{
	u8list_t list;
	u8file_t file;
};

/** ord function suited for `_bseek()` */
static int _list_ord_file(size_t ix, void *_list, void *_file)
{
	u8list_t list = _list;
	u8file_t file = _file;

	u8file_t entry = list->files[ix];
	return _txt_cmp(entry, file);
}

/** Prefab for `_bseek()`ing for a file 
	@return See `_bseek()`
*/
static ssize_t _fl_seek(u8list_t list, u8file_t file)
{
	return file->size.byteCount 
		? (list->pop ? _bseek(list->firstNonEmpty, list->pop - 1, list, file, _list_ord_file) : 0)
		: (list->firstNonEmpty ? _bseek(0, list->firstNonEmpty - 1, list, file, _list_ord_file) : 0);
}

int u8txt_link(u8list_t list, u8file_t file)
{
	// assert capacity
	if(list->pop == list->cap)
	{
		u8file_t *newBuf = realloc(list->files, sizeof(u8file_t) * (list->cap + FILE_LIST_GRAIN));

		if(! newBuf)
			return -1;

		list->files = newBuf;
		list->cap += FILE_LIST_GRAIN;
	}

	ssize_t ix = _fl_seek(list, file);

	if(ix >= 0)
		return +1;

	if(file->size.byteCount == 0)
		++list->firstNonEmpty;

	ix = -(ix + 1);
	memmove(list->files + ix + 1, list->files + ix, sizeof(u8file_t) * ( list->pop - ix ));
	list->files[ix] = file;
	++list->pop;

	return 0;
}

bool u8txt_unlink(u8list_t list, u8file_t file)
{
	ssize_t ix = _fl_seek(list, file);

	if(ix < 0)
		return false;

	if(file->size.byteCount == 0)
		--list->firstNonEmpty;
	
	// patch hole
	memmove(list->files + ix, list->files + ix + 1, list->pop - ix - 1);
	--list->pop;

	if(list->cap - list->pop >= 2*FILE_LIST_GRAIN)
	{ // shrink buffer
		u8file_t *newBuf = realloc(list->files, (list->cap - FILE_LIST_GRAIN)*sizeof(u8file_t));

		if(newBuf)
		{
			list->cap -= FILE_LIST_GRAIN;
			list->files = newBuf;
		}
	}

	return true;
}

u8file_t u8txt_fileof(u8list_t list, const char *chr)
{
	if(! list->pop)
		return NULL; // avoid underflow

	size_t lo = list->firstNonEmpty;
	size_t hi = list->pop - 1; // inclusive
	
	while(lo <= hi)
	{
		size_t mid = lo + (hi - lo)/2;
		int c = u8txt_loc(list->files[mid], chr, NULL);

		if(c < 0)
			lo = mid + 1;
		else if(c > 0)
			hi = mid - 1;
		else
			return list->files[mid];
	}

	return NULL;

__nonnull((1))
u8file_t u8txt_access(u8list_t list, size_t ix)
{
	return list->files[ix];
}

__nonnull((1))
size_t u8txt_count(u8list_t list)
{
	return list->pop;
}
//#endregion

static const u8loc_t initialLocation = {
	.characterIndex = 0,
	.charOff = 0,
	.column = 0,
	.line = 1
};

//#region Helper Functions

static inline const u8loc_t *_getMarkers(u8file_t file)
{
	return (u8loc_t*)&file->_opaque;
}

static inline u8loc_t _getMarker(const u8loc_t *array, size_t ix)
{
	return ix ? array[ix - 1] : initialLocation;
}

static inline u8loc_t _loc_incr(u8loc_t l0, uchar_t chr)
{
	++l0.characterIndex;

	if(chr == '\n')
	{
		++l0.line;
		l0.column = 1;
	}
	else
		++l0.column;

	return l0;
}

/** Computes a location after moving further along the string
	@param l0 The initial location of str[0]
	@param n Number of bytes to consume
	@param size Actual total size of `str`. Must be `>= n` (relevant for interrupted characters)
	@returns THe location of l0[n]
*/
static u8loc_t _loc_move(u8loc_t l0, const char *str, size_t n, size_t size)
{
	assert(n <= size);

	// correct for character offset
	if(l0.charOff)
	{
		str -= l0.charOff; // must be bound-safe by construction of markers
		n += l0.charOff;
		size += l0.charOff;
		l0.charOff = 0;
	}

	for(size_t i = 0;;)
	{
		uchar_t c;
		size_t l = u8ndec(str + i, size - i, &c);

		i += l;
		u8loc_t nextLoc = _loc_incr(l0, c);

		if(n < i)
		{
			l0.charOff = n - (i - l);
			return l0;
		}
		else if(n == i)
			return nextLoc;

		l0 = nextLoc;
	}
}
//#endregion

//#region file init/free

#if _POSIX_SOURCE >= 200112L
/** Size of a huge page, 2 MiB */
static const size_t hugePageSize = 2*1024*1024;

u8file_t u8txt_open(int fd)
{
	struct stat st;
	if(fstat(fd, &st))
	// fstat failing is a bad sign (prob. EBADF)
		return NULL;
	
	if(! S_ISREG(st.st_mode))
		goto fallback;

	if(st.st_size == 0)
		return u8txt_load(NULL, 0, NULL);

	// attempt mmap()
	void *const mapping = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);

	if(mapping == MAP_FAILED)
	{
		switch(errno)
		{
			case ENODEV: /* "The underlying filesystem of the specified file does not support memory mapping." */
				goto fallback;
	
			default:
				return NULL;
		}
	}

	{
		u8file_t file = u8txt_load(mapping, st.st_size, u8txt_cleanup_munmap);

		if(! file)
			munmap(mapping, st.st_size);

		return file;
	}




	fallback:;
	char *buf = NULL;
	size_t size = 0;

	// simple copy into buffer
	for(;;)
	{
		// biggest page size below current length, hope malloc is smart enough to mmap for us
		size_t delta = size >= hugePageSize ? hugePageSize : 4096;

		if(size > SIZE_MAX - delta)
		{ // just in case some very weird 32/64 bit shenanigans happens
			free(buf);
			errno = EOVERFLOW;
			return NULL;
		}

		void *nbuf = realloc(buf, size + delta);

		if(! nbuf)
		{
			free(buf);
			return NULL;
		}

		buf = nbuf;
		ssize_t n = read(fd, buf + size, delta);

		if(n < 0)
		{
			free(buf);
			return NULL;
		}

		size += n;

		if((size_t)n < delta)
		{
			nbuf = realloc(buf, size);

			if(nbuf)
				buf = nbuf;

			u8file_t file = u8txt_load(buf, size, u8txt_cleanup_free);

			if(! file)
				free(buf);

			return file;
		}
	}
}
#endif

u8file_t u8txt_load(const char *bytes, size_t size, cleanup_f cleanup)
{
	size_t nMark = size / MARKER_FREQ;
	u8file_t file = malloc(sizeof(struct TextFile) + nMark * sizeof(u8loc_t));

	if(! file)
		return NULL;

	u8loc_t *m = (u8loc_t*)&file->_opaque;
	u8loc_t cur = initialLocation;

	// create markers
	for(size_t i = 0; i < nMark; ++i)
	{
		size_t o = i * MARKER_FREQ;
		m[i] = cur = _loc_move(cur, bytes + o, MARKER_FREQ, size - o);
	}

	size_t leftover = size - nMark*MARKER_FREQ;
	u8loc_t last = _loc_move(cur, bytes + nMark*MARKER_FREQ, leftover, leftover);

	u8size_t siz = (u8size_t) {
		.bytesExact = true,
		.byteCount = size,
		.charsExact = true,
		.charCount = last.characterIndex
	};

	// Magic L-expression to overwrite a const-marked field
	#define SET_FIELD(field) (*(typeof_unqual(field)*)&(field))

	file->udata = NULL;
	SET_FIELD(file->bytes) = bytes;
	SET_FIELD(file->size) = siz;
	SET_FIELD(file->lines) = last.line;
	SET_FIELD(file->cleanupCallback) = cleanup;

	#undef SET_FIELD

	return file;
}

void u8txt_cleanup_free(u8file_t file)
{
	free((char*)file->bytes);
}

void u8txt_cleanup_munmap(u8file_t file)
{
	munmap( (char*)file->bytes, file->size.byteCount );
}

void u8txt_free(u8file_t file)
{
	if(file->cleanupCallback)
		file->cleanupCallback(file);
	
	free(file);
}
//#endregion


int u8txt_loc(u8file_t file, const char *chr, u8loc_t *out_loc)
{
	if(file->size.byteCount == 0)
		return -1;
	if((size_t)chr < (size_t)file->bytes)
		return -1;
	if((size_t)chr >= (size_t)file->bytes + file->size.byteCount)
		return +1;

	if(! out_loc)
		return 0;

	size_t off = chr - file->bytes;
	// look up marker
	size_t mi = off / MARKER_FREQ;
	u8loc_t marker = _getMarker(_getMarkers(file), mi);
	size_t mOff = mi * MARKER_FREQ;

	*out_loc = _loc_move(marker, file->bytes + mOff, off - mOff, file->size.byteCount - mOff);

	return 0;
}

//#region Content Lookup
/** Shorthand for decoding a single unicode character from a text file.
	@see `u8dec()`
	@param chr Points to the start of the target character
	@param out_chr Unless NULL, overwritten with the decoded character
	@returns The encoded size of the target character
*/
inline static size_t u8txt_dec(u8file_t file, const char *chr, uchar_t *out_chr)
{
	return u8ndec(chr, file->size.byteCount - (chr - file->bytes), out_chr);
}

const char *u8txt_line(u8file_t file, unsigned line, u8size_t *out_size)
{
	if(line <= 0 || line > file->lines)
		return NULL;

	const char *start;
	size_t startIndex; // character index of `start`
	
	if(line == 1)
	{
		start = file->bytes;
		startIndex = 0;
	}
	else
	{
		start = u8txt_unLoc(file, line, 1, &startIndex);
		assert(start);
	}

	if(out_size)
	{
		if(line == file->lines)
		{ // is final line
			*out_size = (u8size_t) {
				.byteCount = file->size.byteCount - (start - file->bytes),
				.bytesExact = true,
				.charCount = file->size.charCount - startIndex,
				.charsExact = true
			};
		}
		else
		{
			size_t endIndex;
			const char *end = u8txt_unLoc(file, line + 1, 1, &endIndex);
			assert(end);

			*out_size = (u8size_t) {
				.byteCount = end - start,
				.bytesExact = true,
				.charCount = endIndex - startIndex,
				.charsExact = true
			};
		}
	}

	return start;
}

/** Prefab for `_bseek`
	@param _markers actually `const u8loc_t*`
	@param _target actually `size_t` (without indirection)
*/
static inline int _ord_loc_ix(size_t ix, void *_markers, void *_target)
{
	const u8loc_t *markers = _markers;
	size_t target = (size_t)_target;
	size_t got = _getMarker(markers, ix).characterIndex;
	static const size_t MIN_CHARS_PER_MARKER = MARKER_FREQ/UTF8_MAX;

	if(got < target)
		return -1;
	else if(target - got <= MIN_CHARS_PER_MARKER)
		return 0;
	else // got + X > target
		return +1;
}

const char *u8txt_chr(u8file_t file, size_t index, u8loc_t *out_loc)
{
	if(index >= file->size.charCount)
		return NULL;

	// possible range of byte offsets corresponding to `chrIx`
	size_t bMin = index, bMax = (index * UTF8_MAX);

	if(bMax >= file->size.byteCount)
		bMax = file->size.byteCount - 1;

	bMin /= MARKER_FREQ;
	bMax /= MARKER_FREQ;
	
	const u8loc_t *m = _getMarkers(file);
	ssize_t i = _bseek(bMin, bMax, (void*)m, (void*)index, _ord_loc_ix);

	// correct index to reference last marker <= target
	if(i < 0)
	{
		i = -i - 1;
		assert(i > 0);
		--i;
	}

	u8loc_t loc = _getMarker(m, i);
	const char *p = file->bytes + i*MARKER_FREQ;

	if(loc.charOff)
	{
		p -= loc.charOff;
		loc.charOff = 0;
	}

	for(;;)
	{
		uchar_t c;
		size_t l = u8txt_dec(file, p, &c);
		assert(l != 0); // we've already checked the file size

		if(loc.characterIndex == index)
		{
			if(out_loc)
				*out_loc = loc;

			return p;
		}

		assert(loc.characterIndex < index); // otherwise we've skipped an index?!		
		p += l;
		loc = _loc_incr(loc, c);
	}
}

/** Prefab for `_bseek` that accepts a u8file_t, and a pair of ints */
static inline int _ord_loc_pos(size_t ix, void *_file, void *_pos)
{
	u8file_t file = _file;
	const unsigned *pos = _pos;
	u8loc_t loc = _getMarker(_getMarkers(file), ix);

	if(loc.line < pos[0])
		return -1;
	if(loc.line > pos[0])
		return +1;

	if(loc.column > pos[1])
		return +1;
	else 
		return -1;
}

const char *u8txt_unLoc(u8file_t file, unsigned line, unsigned col, size_t *out_charIndex)
{
	if(line <= 0 || line > file->lines)
		return NULL;

	unsigned want[2] = { line, col };
	const ssize_t negIx =  _bseek(0, file->size.byteCount / MARKER_FREQ, file, want, _ord_loc_pos);
	assert(negIx < 0); // _ord_loc_pos never returns 0
	const size_t next = -negIx - 1; // first marker after target
	assert(next > 0); // can't be initial position
	const size_t mIx = next - 1;
	
	u8loc_t loc = _getMarker(_getMarkers(file), mIx); // last marker before target
	const char *p = file->bytes + mIx*MARKER_FREQ;

	if(loc.charOff)
	{
		p -= loc.charOff;
		loc.charOff = 0;
	}

	while(loc.line <= line)
	{
		uchar_t c;
		size_t l = u8txt_dec(file, p, &c);

		if(l == 0)
			break;

		if(loc.line == line && loc.column == col)
		{
			if(out_charIndex)
				*out_charIndex = loc.characterIndex;

			return p;
		}

		loc = _loc_incr(loc, c);
		p += l;
	}

	return NULL;
}

//#endregion
