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
	struct TextFile **files;
};

struct FileList *u8txt_create()
{
	return calloc(1, sizeof(struct FileList));
}

void u8txt_destroy(struct FileList *files, bool freeAll)
{
	if(! files)
		return;

	if(freeAll) for(size_t i = 0; i < files->pop; ++i)
		u8txt_free(files->files[i]);

	free(files->files);
	free(files);
}

/** Compares two files to sort file lists. Considers all overlapping files to be equal.
	Empty files precede non-empty files but are only considered equal if the files are pointer-equal.
	@returns -1 if a < b
	@returns  0 if a == b
	@returns +1 if a > b
*/
int txt_cmp(struct TextFile *a, struct TextFile *b)
{	
	// empty files come before all non-empty files
	if(a->size.byteCount == 0 && b->size.byteCount == 0)
	{ // ordering doesn't matter semantically
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

/** body of _fl_seek binary search
	@param hi Exclusive
	@returns The proper index at which to insert `file`
	@returns `-(index + 1)` if the item is already present at `index`
*/
static ssize_t _fl_seekB(struct FileList *list, struct TextFile *file, size_t lo, size_t hi)
{
	if(lo >= hi)
		return lo;

	size_t mid = lo + (hi - lo)/2;
	int c = txt_cmp(list->files[mid], file);

	return (c < 0) /* [mid] < file */ ? _fl_seekB(list, file, mid + 1, hi)
		: (c > 0) /* [mid] > file */ ? _fl_seekB(list, file, lo, mid)
		: /* [mid] == file */ -(mid + 1);
}

/** Seeks a list for a file. */
static ssize_t _fl_seek(struct FileList *list, struct TextFile *file)
{
	return file->size.byteCount
			? _fl_seekB(list, file, list->firstNonEmpty, list->pop) 
			: _fl_seekB(list, file, 0, list->firstNonEmpty);
}

static ssize_t _fl_seek_ptr(struct FileList *list, const char *ptr, size_t lo, size_t hi)
{
	if(lo >= hi)
		return -(lo + 1);

	size_t mid = lo + (hi - lo)/2;
	int c = u8txt_loc(list->files[mid], ptr, NULL);

	return (c < 0) /* [mid] < ptr */ ? _fl_seek_ptr(list, ptr, mid + 1, hi)
		: (c > 0) /* [mid] > ptr */ ? _fl_seek_ptr(list, ptr, lo, mid)
		: /* [mid] == file */ -(mid + 1);
}

int u8txt_link(struct FileList *list, struct TextFile *file)
{
	// assert capacity
	if(list->pop == list->cap)
	{
		struct TextFile **newBuf = realloc(list->files, sizeof(struct TextFile*) * (list->cap + FILE_LIST_GRAIN));

		if(! newBuf)
			return -1;

		list->files = newBuf;
		list->cap += FILE_LIST_GRAIN;
	}

	ssize_t ix = _fl_seek(list, file);

	if(ix < 0)
		return +1;

	if(file->size.byteCount == 0)
		++list->firstNonEmpty;
	
	memmove(list->files + ix + 1, list->files + ix, sizeof(struct TextFile*) * ( list->pop - ix ));
	list->files[ix] = file;

	return 0;
}

bool u8txt_unlink(struct FileList *list, struct TextFile *file)
{
	ssize_t si = _fl_seek(list, file);

	if(si >= 0)
		return false;

	size_t ix = -si - 1;

	if(ix + 1 == list->firstNonEmpty)
		// we've removed the last empty file
		--list->firstNonEmpty;


	// patch hole
	memmove(list->files + ix, list->files + ix + 1, list->pop - ix - 1);

	if(list->cap - list->pop >= 2*FILE_LIST_GRAIN)
	{ // shrink buffer
		struct TextFile **newBuf = realloc(list->files, (list->cap - FILE_LIST_GRAIN)*sizeof(struct TextFile*));

		if(newBuf)
		{
			list->cap -= FILE_LIST_GRAIN;
			list->files = newBuf;
		}
	}

	return true;
}

const struct TextFile *u8txt_fileof(struct FileList *list, const char *chr)
{
	ssize_t ix = _fl_seek_ptr(list, chr, list->firstNonEmpty, list->pop);

	return ix < 0 ? NULL : list->files[ix];
}
//#endregion

static const struct Location initialLocation = {
	.characterIndex = 0,
	.charOff = 0,
	.column = 0,
	.line = 1
};

/** Computes a location after moving further along the string
	@param l0 The initial location of str[0]
	@param n Number of bytes to consume
	@param size Actual total size of `str`. Must be `>= n` (relevant for interrupted characters)
	@returns THe location of l0[n]
*/
static struct Location _loc_move(struct Location l0, const char *str, size_t n, size_t size)
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
		++l0.characterIndex;

		if(c == '\n')
		{
			++l0.line;
			l0.column = 0;
		}
		else
			++l0.column;

		if(n <= i)
		{
			l0.charOff = (n < i) ? n - (i - l) : 0;
			return l0;
		}
	}
}

//#region file init/free

#if _POSIX_SOURCE >= 200112L
/** Size of a huge page, 2 MiB */
static const size_t hugePageSize = 2*1024*1024;

struct TextFile *u8txt_open(int fd)
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
		struct TextFile *file = u8txt_load(mapping, st.st_size, u8txt_cleanup_munmap);

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

			struct TextFile *file = u8txt_load(buf, size, u8txt_cleanup_free);

			if(! file)
				free(buf);

			return file;
		}
	}
}
#endif

struct TextFile *u8txt_load(const char *bytes, size_t size, cleanup_f cleanup)
{
	size_t nMark = size / MARKER_FREQ;
	struct TextFile *file = malloc(sizeof(struct TextFile) + nMark * sizeof(struct Location));

	if(! file)
		return NULL;

	struct Location *m = (struct Location*)&file->_opaque;
	struct Location cur = initialLocation;

	// create markers
	for(size_t i = 0; i < nMark; ++i)
	{
		size_t o = i * MARKER_FREQ;
		m[i] = cur = _loc_move(cur, bytes + o, MARKER_FREQ, size - o);
	}

	size_t leftover = size - nMark*MARKER_FREQ;
	struct Location last = _loc_move(cur, bytes + nMark*MARKER_FREQ, leftover, leftover);

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

void u8txt_cleanup_free(struct TextFile *file)
{
	free((char*)file->bytes);
}

void u8txt_cleanup_munmap(struct TextFile *file)
{
	munmap( (char*)file->bytes, file->size.byteCount );
}
//#endregion


int u8txt_loc(struct TextFile *file, const char *chr, struct Location *out_loc)
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
	struct Location marker = mi ? ((struct Location*)(file->_opaque))[mi - 1] : initialLocation;
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
inline static size_t u8txt_dec(struct TextFile *file, const char *chr, uchar_t *out_chr)
{
	return u8ndec(chr, file->size.byteCount - (chr - file->bytes), out_chr);
}

const char *u8txt_line(struct TextFile *file, unsigned line, u8size_t *out_size)
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
		// seek for starting \n, then skip (in case line is empty)
		start = u8txt_unLoc(file, line, 0, &startIndex);
		assert(start);
		start += u8txt_dec(file, start, NULL);
		++startIndex;
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
			const char *end = u8txt_unLoc(file, line + 1, 0, &endIndex);

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

/** Binary search for the last marker before a character index
	@param hi inclusive 
 */
static size_t _seek_chr(const struct Location *markers, size_t chrIx, size_t lo, size_t hi)
{
	if(lo >= hi)
		return lo;

	size_t mid = lo + (hi - lo)/2;
	struct Location midM = mid ? markers[mid] : initialLocation;

	if(midM.characterIndex > chrIx) // [0] guaranteed to be 0
		return _seek_chr(markers, chrIx, lo, mid - 1);
	// mid <= chrIx
	else if(midM.characterIndex + MARKER_FREQ/UTF8_MAX >= chrIx)
		return mid; // stop early if chrIx definitely in pivot range (i.e. ==)
	else
		return _seek_chr(markers, chrIx, mid, hi);
}

const char *u8txt_chr(struct TextFile *file, size_t index)
{
	if(index >= file->size.charCount)
		return NULL;

	// possible range of byte offsets corresponding to `chrIx`
	size_t bMin = index, bMax = index * UTF8_MAX;
	
	struct Location *m = (struct Location*)file->_opaque;
	size_t i = _seek_chr(m, index, bMin/MARKER_FREQ, bMax/MARKER_FREQ);

	struct Location prec = i ? m[i - 1] : initialLocation;
	size_t off = i*MARKER_FREQ;

	if(prec.charOff)
		off -= prec.charOff;

	return u8z_strpos(file->bytes + off, EXACT_BYTES(file->size.byteCount - off), index - prec.characterIndex);
}


//#endregion
