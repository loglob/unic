#include "u8text.h"
#include "unic.h"
#include <asm-generic/errno-base.h>
#include <assert.h>
#include <stdlib.h>

#if _POSIX_SOURCE >= 200112L
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/mman.h>
#endif

/** Number of bytes between each marker */
#define MARKER_FREQ 2048
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
	size_t pop, cap;
	struct TextFile **files;
};



int txt_cmp(struct TextFile *a, struct TextFile *b)
{
	if(a->size.byteCount == 0 && b->size.byteCount == 0)
		return 
}

/** 
	@param lo (Inclusive) low index strictly smaller than `file`
	@param hi (Exclusive) high index strictly greater than `file`
	@returns The first index >= `file` (or the capacity) 
*/
struct TextFile *ls_seek(struct ListNode *node, const char *ptr)
{
	if(!node || node->from < (size_t)ptr || node->to > (size_t)ptr)
		return NULL;

	if(node->isLeaf)
		return node->leaf;

	return ls_seek(node->inner.left, ptr) ?: ls_
}

int u8txt_link(struct FileList *list, struct TextFile *file)
{
	if(list->lowest >= list->cap)
	{ // empty
		assert(list->cap > 0);
		size_t ix0 = list->cap / 4;

		list->files[ix0] = file;
		return 0;
	}

	size_t ix = ls_seek(list->files, file, list->lowest, list->highest + 1);

	if(ix <= list->highest && txt_cmp(list->files[ix], file) == 0)
		return 1;

	if(ix > 0 && !list->files[ix - 1])
	{ // insert
		list->files[ix - 1] = file;
		return 0;
	}

}

bool u8txt_unlink(struct FileList *list, struct TextFile *file)
{

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



