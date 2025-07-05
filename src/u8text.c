#include "u8text.h"
#include "unic.h"
#include <assert.h>
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
	return file->size.byteCount ? _fl_seekB(list, file, list->firstNonEmpty, list->pop) : _fl_seekB(list, file, 0, list->firstNonEmpty);
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

	// create markers
	size_t chr = 0;
	size_t markIx = 0;
	unsigned col = 0;
	unsigned line = 1;

	for(size_t off = 0; off < size && markIx < nMark;)
	{
		uchar_t c;
		off += u8ndec(bytes + off, size - off, &c);
		++chr;

		if(c == '\n')
		{
			++line;
			col = 0;
		}
		else
			++col;

		const size_t nextMark = (markIx + 1) * MARKER_FREQ;

		if(off >= nextMark)
		{
			m[markIx++] = (struct Location) {
				.characterIndex = chr,
				.charOff = off - nextMark,
				.column = col,
				.line = line
			};
		}
	}

	u8size_t siz = (u8size_t) {
		.bytesExact = true,
		.byteCount = size,
		.charsExact = true,
		.charCount = chr
	};

	// Magic L-expression to overwrite a const-marked field
	#define SET_FIELD(field) (*(typeof_unqual(field)*)&(field))

	file->udata = NULL;
	SET_FIELD(file->bytes) = bytes;
	SET_FIELD(file->size) = siz;
	SET_FIELD(file->lines) = line;
	SET_FIELD(file->cleanupCallback) = cleanup;

	#undef SET_FIELD

	return file;
}



