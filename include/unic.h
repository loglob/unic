/* Defines the uchar_t datatype as well as multiple preprocessor macros for use with it and
	the current version of the unicode standard */
#ifndef UNIC_VERSION
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* The unicode version used to generate the library */
#define UNIC_VERSION 1300
/* The unicode version as a human readable string */
#define UNIC_VERSION_STRING "13.0.0"
/* The highest valid unicode character */
#define UNIC_MAX 0x10FFFD
/* The amount of bits needed to encode every valid unicode character */
#define UNIC_BIT 21

#define UEOF ((uchar_t)-1)

typedef uint32_t uchar_t;

/* The amount of bits taken up by an enum unic_gc value */
#define UNIC_GC_BITS 6
/* The amount of bits taken up by the minor category of an enum unic_gc value */
#define UNIC_GC_SUB_BITS 3

/* A unicode general category */
enum unic_gc
{
	UCLASS_OTHER = 0 << UNIC_GC_SUB_BITS,
	/** Alias for UCLASS_OTHER */
	UCLASS_C = UCLASS_OTHER,
	UCLASS_CONTROL,
	/** Alias for UCLASS_CONTROL */
	UCLASS_CC = UCLASS_CONTROL,
	UCLASS_FORMAT,
	/** Alias for UCLASS_FORMAT */
	UCLASS_CF = UCLASS_FORMAT,
	UCLASS_UNASSIGNED,
	/** Alias for UCLASS_UNASSIGNED */
	UCLASS_CN = UCLASS_UNASSIGNED,
	UCLASS_PRIVATE_USE,
	/** Alias for UCLASS_PRIVATE_USE */
	UCLASS_CO = UCLASS_PRIVATE_USE,
	UCLASS_SURROGATE,
	/** Alias for UCLASS_SURROGATE */
	UCLASS_CS = UCLASS_SURROGATE,
	UCLASS_LETTER = 1 << UNIC_GC_SUB_BITS,
	/** Alias for UCLASS_LETTER */
	UCLASS_L = UCLASS_LETTER,
	UCLASS_CASED_LETTER,
	/** Alias for UCLASS_CASED_LETTER */
	UCLASS_LC = UCLASS_CASED_LETTER,
	UCLASS_LOWERCASE_LETTER,
	/** Alias for UCLASS_LOWERCASE_LETTER */
	UCLASS_LL = UCLASS_LOWERCASE_LETTER,
	UCLASS_MODIFIER_LETTER,
	/** Alias for UCLASS_MODIFIER_LETTER */
	UCLASS_LM = UCLASS_MODIFIER_LETTER,
	UCLASS_OTHER_LETTER,
	/** Alias for UCLASS_OTHER_LETTER */
	UCLASS_LO = UCLASS_OTHER_LETTER,
	UCLASS_TITLECASE_LETTER,
	/** Alias for UCLASS_TITLECASE_LETTER */
	UCLASS_LT = UCLASS_TITLECASE_LETTER,
	UCLASS_UPPERCASE_LETTER,
	/** Alias for UCLASS_UPPERCASE_LETTER */
	UCLASS_LU = UCLASS_UPPERCASE_LETTER,
	UCLASS_MARK = 2 << UNIC_GC_SUB_BITS,
	/** Alias for UCLASS_MARK */
	UCLASS_M = UCLASS_MARK,
	UCLASS_SPACING_MARK,
	/** Alias for UCLASS_SPACING_MARK */
	UCLASS_MC = UCLASS_SPACING_MARK,
	UCLASS_ENCLOSING_MARK,
	/** Alias for UCLASS_ENCLOSING_MARK */
	UCLASS_ME = UCLASS_ENCLOSING_MARK,
	UCLASS_NONSPACING_MARK,
	/** Alias for UCLASS_NONSPACING_MARK */
	UCLASS_MN = UCLASS_NONSPACING_MARK,
	UCLASS_NUMBER = 3 << UNIC_GC_SUB_BITS,
	/** Alias for UCLASS_NUMBER */
	UCLASS_N = UCLASS_NUMBER,
	UCLASS_DECIMAL_NUMBER,
	/** Alias for UCLASS_DECIMAL_NUMBER */
	UCLASS_ND = UCLASS_DECIMAL_NUMBER,
	UCLASS_LETTER_NUMBER,
	/** Alias for UCLASS_LETTER_NUMBER */
	UCLASS_NL = UCLASS_LETTER_NUMBER,
	UCLASS_OTHER_NUMBER,
	/** Alias for UCLASS_OTHER_NUMBER */
	UCLASS_NO = UCLASS_OTHER_NUMBER,
	UCLASS_PUNCTUATION = 4 << UNIC_GC_SUB_BITS,
	/** Alias for UCLASS_PUNCTUATION */
	UCLASS_P = UCLASS_PUNCTUATION,
	UCLASS_CONNECTOR_PUNCTUATION,
	/** Alias for UCLASS_CONNECTOR_PUNCTUATION */
	UCLASS_PC = UCLASS_CONNECTOR_PUNCTUATION,
	UCLASS_DASH_PUNCTUATION,
	/** Alias for UCLASS_DASH_PUNCTUATION */
	UCLASS_PD = UCLASS_DASH_PUNCTUATION,
	UCLASS_CLOSE_PUNCTUATION,
	/** Alias for UCLASS_CLOSE_PUNCTUATION */
	UCLASS_PE = UCLASS_CLOSE_PUNCTUATION,
	UCLASS_FINAL_PUNCTUATION,
	/** Alias for UCLASS_FINAL_PUNCTUATION */
	UCLASS_PF = UCLASS_FINAL_PUNCTUATION,
	UCLASS_INITIAL_PUNCTUATION,
	/** Alias for UCLASS_INITIAL_PUNCTUATION */
	UCLASS_PI = UCLASS_INITIAL_PUNCTUATION,
	UCLASS_OTHER_PUNCTUATION,
	/** Alias for UCLASS_OTHER_PUNCTUATION */
	UCLASS_PO = UCLASS_OTHER_PUNCTUATION,
	UCLASS_OPEN_PUNCTUATION,
	/** Alias for UCLASS_OPEN_PUNCTUATION */
	UCLASS_PS = UCLASS_OPEN_PUNCTUATION,
	UCLASS_SYMBOL = 5 << UNIC_GC_SUB_BITS,
	/** Alias for UCLASS_SYMBOL */
	UCLASS_S = UCLASS_SYMBOL,
	UCLASS_CURRENCY_SYMBOL,
	/** Alias for UCLASS_CURRENCY_SYMBOL */
	UCLASS_SC = UCLASS_CURRENCY_SYMBOL,
	UCLASS_MODIFIER_SYMBOL,
	/** Alias for UCLASS_MODIFIER_SYMBOL */
	UCLASS_SK = UCLASS_MODIFIER_SYMBOL,
	UCLASS_MATH_SYMBOL,
	/** Alias for UCLASS_MATH_SYMBOL */
	UCLASS_SM = UCLASS_MATH_SYMBOL,
	UCLASS_OTHER_SYMBOL,
	/** Alias for UCLASS_OTHER_SYMBOL */
	UCLASS_SO = UCLASS_OTHER_SYMBOL,
	UCLASS_SEPARATOR = 6 << UNIC_GC_SUB_BITS,
	/** Alias for UCLASS_SEPARATOR */
	UCLASS_Z = UCLASS_SEPARATOR,
	UCLASS_LINE_SEPARATOR,
	/** Alias for UCLASS_LINE_SEPARATOR */
	UCLASS_ZL = UCLASS_LINE_SEPARATOR,
	UCLASS_PARAGRAPH_SEPARATOR,
	/** Alias for UCLASS_PARAGRAPH_SEPARATOR */
	UCLASS_ZP = UCLASS_PARAGRAPH_SEPARATOR,
	UCLASS_SPACE_SEPARATOR,
	/** Alias for UCLASS_SPACE_SEPARATOR */
	UCLASS_ZS = UCLASS_SPACE_SEPARATOR,

};


#pragma region utf8.h

__nonnull((1))
/** Reads the next unicode character from the given utf-8 encoded steam.
 * Returns UEOF on reading EOF.
 * @param f The file stream. May not be NULL.
 * @returns The next unicode character in the stream
*/
extern uchar_t fgetu8(FILE *f);

__nonnull((2))
/** Writes the given unicode character to the given utf-8 encoded stream.
 * @param c A unicode character
 * @param f A file stream. May not be NULL.
*/
extern void fputu8(uchar_t c, FILE *f);

/** Like u8dec(), but reads the next utf-8 encoded character in the first n bytes of the given string.
 * If n is 0, behaves as if it read NUL terminator and returns 0.
 * @param str The utf-8 encoded buffer to read from. May be NULL if n is 0.
 * @param n The maximum amount of bytes to read.
 * @param c The location to store the character in. May be NULL to only determine the length of the next character.
 * @returns The amount of bytes read.
 */
extern size_t u8ndec(const char *str, size_t n, uchar_t *c);

__nonnull((1))
/** Reads the next utf-8 encoded character from the given string.
 * Note that reading and re-encoding a character may change its length due to improper encoding in source streams.
 * A well-encoded NUL terminater is treated as a character of length 1. 
 * @param str The utf-8 encoded buffer to read from. May not be NULL.
 * @param c The location to store the character in. May be NULL to only determine the length of the next character.
 * @returns The amount of bytes read. */
extern size_t u8dec(const char *str, uchar_t *c);

/** Writes the given unicode character to the buffer.
 * @param uc The unicode character
 * @param buf The buffer to write the character to. May be NULL to only determine its length.
 * @returns The amount of bytes written. */
extern size_t u8enc(uchar_t uc, char *buf);

#pragma endregion

#pragma region util.h

/** Retrieves the unicode character class of the given character
 * @param c The character
 * @returns Its general category, or UCLASS_UNASSIGNED if it is invalid.
*/
extern enum unic_gc uchar_class(uchar_t c);

/** Determines if two unicode character classes are the same or compatible
 * @param general A general category, may be a major category
 * @param specific A general category
 * @returns Both classes are the same,
 *	or general is a major category which specific is a subcategory of.
*/
extern bool uclass_is(enum unic_gc general, enum unic_gc specific);

/** Determines if a unicode character is of a unicode class
 * @param chr The character
 * @param class The general category, may be a major category
 * @returns The character is of the given class, or of the given major category.
*/
extern bool uchar_is(uchar_t chr, enum unic_gc class);

/** Determines if the given unicode character is whitespace
 * @param c The character
 * @returns c is in the SEPARATOR general category,
 *	or an ascii control character that is considered whitespace, as per libc's isspace() 
*/
extern int u_isspace(uchar_t c);

/** Returns the simple lowercase mapping of the given character
 * @param c The character
 * @returns Its simple lowercase mapping, or the character itself if it doesn't exist.
*/
extern uchar_t uchar_lower(uchar_t c);

/** Returns the simple uppercase mapping of the given character
 * @param c The character
 * @returns Its simple uppercase mapping, or the character itself if it doesn't exist.
*/
extern uchar_t uchar_upper(uchar_t c);

#pragma endregion

#endif
