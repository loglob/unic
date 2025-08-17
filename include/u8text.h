// u8text: Implements a sliceable text representation for big files
#ifndef UNIC_U8TEXT
#define UNIC_U8TEXT
#include <stddef.h>
#include "unic.h"

/** An unordered collection of files for backtracking pointers */
typedef struct FileList *u8list_t;

/** Information about a byte location */
typedef struct {
	/** 1-based line index (i.e. 1 + # of preceding \n characters) */
	unsigned line;
	/** 1-based character index into the current line. */
	unsigned column;
	/** 0-based character index of the next character after the pointer.
		@see charOff
	*/
	size_t characterIndex : 62;
	/** Amount of bytes the pointer was offset from the start of a character.
		i.e. number of preceding bytes that were part of the same character
		i.e. index of this byte in the current character
		Zero when the pointer was character-aligned.
	 */
	unsigned charOff : 2;
} u8loc_t;

struct TextFile;

/** A handle to a UTF-8 text file */
typedef struct TextFile *u8file_t;

/** Callback function invoked to free the content of a text file before freeing. NULL indicates noop. */
typedef void (*cleanup_f)(u8file_t file);

struct TextFile
{
	/** Arbitrary userdata tagged onto this file.
		Entirely application controlled, may be overwritten freely.
		Initialized to NULL on creation.
	*/
	void *udata;
	/** Size of `bytes`. Guaranteed to be exact. DO NOT MODIFY. */
	const u8size_t size;
	/** Total number of lines in the file */
	const unsigned lines;
	/** Points to the raw file contents. DO NOT MODIFY. */
	const char *const bytes;
	/** The callback passed to `u8txt_load()` */
	const cleanup_f cleanupCallback;

	/** Private implementation data */
	const char _opaque[];
};

//#region File List interface

/** Allocates a new file list
	@returns An empty file list
	@returns NULL and sets errno on malloc failure
*/
extern u8list_t u8txt_create();

/** Destroys a file list
	@param files A file list. Noop if NULL.
	@param freeAll If true, also free every contained file
*/
extern void u8txt_destroy(u8list_t files, bool freeAll);

/** Looks up the file containing a character location 
	@returns That text file
	@returns NULL if `chr` is not contained in `files`
*/
extern u8file_t u8txt_fileof(u8list_t files, const char *chr);

/** Appends a file to a file list
	@returns 0 on success
	@returns 1 if the file is already part of the list
	@returns -1 and sets errno on internal malloc error
*/
extern int u8txt_link(u8list_t list, u8file_t file);

/** Removes a file from a list
	@returns true on success
	@returns false if the file isn't part of a list
*/
extern bool u8txt_unlink(u8list_t list, u8file_t file);

/** Accesses an element in a file list by index.
	@note The index order is not specified, and elements may change position after each call to `u8txt_link` or `u8txt_unlink`
	@note Accessing an out-of-bounds index is undefined behaviour
	@param ix An index between 0 and `u8txt_count(list)`
*/
extern u8file_t u8txt_access(u8list_t list, size_t ix);

/** @returns Number of files in `list` */
extern size_t u8txt_count(u8list_t list);

//#endregion

//#region file init/free

#if _POSIX_SOURCE >= 200112L
/** Opens a text file from a file descriptor.
	@param fd A file descriptor. Regular files must be positioned at the start of the file.
	@returns A file containing the entire file referenced by `fd`, or NULL on error
*/
extern u8file_t u8txt_open(int fd);

/** Prefab for passing a mmap()ed buffer to `u8txt_load()`. 
	Note that `size` must be exactly what was passed to mmap().
 */
extern void u8txt_cleanup_munmap(u8file_t file);
#endif

/** Opens a buffer as a text file.
	@param bytes A buffer containing UTF-8 data.
			May contain plain NULs, which are treated as valid characters.
	@param size The number of bytes in `bytes`
	@param cleanup Callback invoked when the text file is closed.
			Receives the return value of the call, which contains the arguments verbatim.
			NULL may be passed to skip cleanup.
	@returns The allocated text file
	@returns `NULL` and sets `errno` on failure. Does NOT clean up the passed buffer.
*/
extern u8file_t u8txt_load(const char *bytes, size_t size, cleanup_f cleanup);

/** Prefab for passing a malloc()ed buffer to `u8txt_load()` */
extern void u8txt_cleanup_free(u8file_t file);


/** Frees a file and its content.
	@note Does NOT touch any file list it's in, may lead to use-after-free
 */
extern void u8txt_free(u8file_t file);

//#endregion

/** Looks up the location of a character in a file
	@param file The file handle that contains `chr`
	@param chr A pointer to the start of the character to locate
	@param out_loc If not NULL, overwritten with the location of `chr`
	@returns 0 and overwrites `out_loc` if the location is found within `file`
	@returns A negative number if the location precedes the first byte of the file or the file is empty.
	@returns A positive number if the location follows after the last byte of the file
*/
extern int u8txt_loc(u8file_t file, const char *chr, u8loc_t *out_loc);

//#region Content Lookup

/** Retrieves a single line of the file
	@note To look up a single character in the line, prefer `u8txt_unLoc`
	@param line A 1-based line index
	@param out_size If not NULL, overwritten with the size of the returned string
	@returns The line with index `line`, or NULL if the line index os out of bounds
*/
extern const char *u8txt_line(u8file_t file, unsigned line, u8size_t *out_size);

/** Looks up a character by its index
	@param out_loc Unless NULL, overwritten with the complete location of that character
	@returns A pointer to the first byte of the character at `index`
	@returns NULL if `index` was out of bounds
*/
extern const char *u8txt_chr(u8file_t file, size_t index, u8loc_t *out_loc);

/** Looks up a line/col location
	@param line A 1-based line index
	@param col A 1-based column index within that line.
				The final newline character is not considered to be within line bounds.
	@param out_charIndex Unless NULL, overwritten with the equivalent total character offset of the result on success.
	@returns A pointer to the first byte of the character at `index`
	@returns NULL if the location was out of bounds
*/
extern const char *u8txt_unLoc(u8file_t file, unsigned line, unsigned col, size_t *out_charIndex);

//#endregion
#endif