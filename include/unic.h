/* The include file for the unic library */
#ifndef UNIC_VERSION
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

/** The unicode version used to generate the library */
#define UNIC_VERSION 1500
/** The unicode version as a human readable string */
#define UNIC_VERSION_STRING "15.0.0"
/** The highest valid unicode character */
#define UNIC_MAX 0x10FFFD
/** The amount of bits needed to encode every valid unicode character */
#define UNIC_BIT 21
/** Type-correct EOF for unicode functions */
#define UEOF ((uchar_t)-1)
/** Over-encoded NUL terminator to include \0 in utf-8 encoded strings */
#define UNUL "\xC0\x80"
/** The maximum amount of bytes any UTF-8 encoded character can take up */
#define UTF8_MAX 4

/** A single unicode character */
typedef uint32_t uchar_t;

/** The amount of bits taken up by an enum unic_gc value */
#define UNIC_GC_BITS 6
/** The amount of bits taken up by the minor category of an enum unic_gc value */
#define UNIC_GC_SUB_BITS 3

/** A unicode general category */
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

// #region utf8.h

__nonnull((1))
/** Reads the next unicode character from the given utf-8 encoded steam.
 * Returns UEOF on reading EOF.
 * @param f The file stream. May not be NULL.
 * @returns The next unicode character in the stream
*/
extern uchar_t fgetu8(FILE *f);

__nonnull((2))
/** Writes the given unicode character to the given utf-8 encoded stream.
 * Always writes either nothing on failure or an entire character on success.
 * @param c A unicode character
 * @param f A file stream. May not be NULL.
 * @returns The amount of bytes written on success, or 0 on failure.
*/
extern size_t fputu8(uchar_t c, FILE *f);

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
 * A well-encoded NUL terminator is treated as a character of length 1.
 * @param str The utf-8 encoded buffer to read from. May not be NULL.
 * @param c The location to store the character in. May be NULL to only determine the length of the next character.
 * @returns The amount of bytes read. */
extern size_t u8dec(const char *str, uchar_t *c);

/** Writes the given unicode character to the buffer.
 * @param uc The unicode character
 * @param buf The buffer to write the character to. May be NULL to only determine its length.
 * @returns The amount of bytes written. */
extern size_t u8enc(uchar_t uc, char *buf);

// #endregion utf8.h

// #region util.h

/** Retrieves the unicode character class of the given character
 * @param c The character
 * @returns Its general category, or UCLASS_UNASSIGNED if it is invalid.
*/
extern enum unic_gc uchar_class(uchar_t c);

/** Determines if two unicode characters are alike/similar.
 * This means that some combination of the simple lower- or uppercase mapping produces the same character
 * i.e.: ∃f,g ∈ { lower, upper, id }: f(a) = g(b)
 * @param a A character
 * @param b Another character
 * @returns a is similar to b
*/
extern bool uchar_alike(uchar_t a, uchar_t b);

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

// #endregion util.h

// #region u8string.h

__nonnull((1))
/** Determines the amount of unicode characters in the given utf-8 string
 * Does not count the NUL terminator.
 * @param str A utf-8 string. May not be NULL.
 * @returns The amount of unicode characters in str.
*/
extern size_t u8_strlen(const char *str);

/** Determines the amount of bytes occupied by the first n characters in the given utf-8 string.
 * If the string contains fewer unicode characters, counts the entire string's length.
 * Doesn't count the final NUL terminator.
 * @param str A string. May be NULL only if n is 0.
 * @param n The maximum amount of unicode characters.
 * @returns The amount of bytes taken up by the first n characters.
*/
extern size_t u8_strnlen(const char *str, size_t n);

/** Determines the amount of unicode characters in the first c bytes of the given string.
 * If the string is less than c bytes long, returns the amount of unicode characters in the entire string.
 * @param str A string. May be NULL only is c is 0.
 * @param c The amount of bytes.
 * @returns The amount of unicode characters in the first c bytes.
*/
extern size_t u8_strclen(const char *str, size_t c);

__nonnull((1))
/** Copies the utf-8 encoded string str to dst.
 * Every character written to dst is guaranteed to be utf-8 normalized.
 * Over-encoded NUL characters are omitted from the output and don't terminate the input string.
 * If dst is NULL, no write operations are performed but the correct byte amount is returned.
 * @param str The utf-8 encoded source string. May not be NULL.
 * @param dst The destination buffer. May be NULL to determine required buffer size.
 * @returns The amount of bytes written to dst, including the NUL terminator.
*/
extern size_t u8_strcpy(const char *str, char *dst);

/** Copies at most n unicode characters from str to dst.
 * Every character written to dst is guaranteed to be utf-8 normalized.
 * Over-encoded NUL characters are omitted from the output and don't terminate the input string.
 * If dst is NULL, no write operations are performed but the correct byte amount is returned.
 * @param str The utf-8 encoded source string. May be NULL if n is 0.
 * @param dst The destination buffer. May be NULL to determine required buffer size.
 * @param n The amount of unicode characters to copy.
 * @returns The amount of bytes written to dst, including the NUL terminator.
*/
extern size_t u8_strncpy(const char *str, char *dst, size_t n);

/** Copies at most c bytes from str to dst.
 * Every character written to dst is guaranteed to be utf-8 normalized.
 * Over-encoded NUL characters are omitted from the output and don't terminate the input string.
 * If dst is NULL, no write operations are performed but the correct byte amount is returned.
 * @param str The utf-8 encoded source string. May be NULL if c is 0.
 * @param dst The destination buffer. May be NULL to determine required buffer size.
 * @param c The maximum amount of bytes to copy (note that due to character re-encoding this is NOT a limit on dst).
 * @returns The amount of bytes written to dst, including the NUL terminator.
*/
extern size_t u8_strccpy(const char *str, char *dst, size_t c);

__nonnull((1))
/** Finds the position of the unicode character with the given index in the given utf-8 encoded string.
 * The NUL terminator is considered to be inside the string bounds.
 * @param str The utf-8 encoded string. May be NULL if pos is 0.
 * @param pos The unicode character index.
 * @returns A pointer to the start of the given unicode character,
 *	or NULL if the index is outside the string bounds.
*/
extern const char *u8_strpos(const char *str, size_t pos);

/** Finds the character at the given index in the given utf-8 encoded string.
 * @param str The utf-8 encoded string. May be NULL is pos is 0.
 * @param pos The unicode character index.
 * @returns The character at that position,
 *	or 0 if the index is outside the string bounds.
*/
extern uchar_t u8_strat(const char *str, size_t pos);

__nonnull((1))
/** Finds the first occurrence of the given character in the given utf-8 encoded string.
 * Note that, for utf-8 normalized strings, strstr achieves the same and will be more efficient.
 * @param str The string. May not be NULL.
 * @param chr The character to find.
 * @returns A pointer to the start of the first occurrence of the given character,
 *	or NULL if the string doesn't contain the character.
*/
extern const char *u8_strchr(const char *str, uchar_t chr);

__nonnull((1))
/** Finds the first occurrence of the given character, or a case-insensitive variant of it, in the given utf-8 encoded string.
 * see uchar_alike() for specifics on case-insensitivity.
 * @param str The string. May not be NULL.
 * @param chr The character to find.
 * @returns A pointer to the start of the first occurrence of the given character or a case-insensitive variant of it,
 *	or NULL if the string doesn't contain the character.
*/
extern const char *u8_strchrI(const char *str, uchar_t chr);

__nonnull((1))
/** Finds the last occurrence of the given character in the given utf-8 encoded string.
 * @param str The string. May not be NULL.
 * @param chr The character to find
 * @returns A pointer to the start of the last occurrence of the given character,
 *	or NULL is the string doesn't contain the character.
*/
extern const char *u8_strrchr(const char *str, uchar_t chr);

__nonnull((1))
/** Finds the last occurrence of the given character, or a case-insensitive variant of it, in the given utf-8 encoded string.
 * see uchar_alike() for specifics on case-insensitivity.
 * @param str The string. May not be NULL.
 * @param chr The character to find
 * @returns A pointer to the start of the last occurrence of the given character,
 *	or NULL is the string doesn't contain the character.
*/
extern const char *u8_strrchrI(const char *str, uchar_t chr);

__nonnull((1,2))
/** Finds the first occurrence of the given substring in the given utf-8 encoded string.
 * Note that, for utf-8 normalized strings, strstr achieves the same and will be more efficient.
 * @param haystack The string to search in. May not be NULL.
 * @param needle The string to search for. May not be NULL.
 * @returns A pointer to the start of the first occurrence of the given substring,
 *	or NULL if the string doesn't contain the substring.
*/
extern const char *u8_strstr(const char *haystack, const char *needle);

__nonnull((1,2))
/** Finds the last occurrence of the given substring in the given utf-8 encoded string.
 * Note that, for utf-8 normalized strings, a loop based on strstr achieves the same and will be more efficient.
 * @param haystack The string to search in. May not be NULL.
 * @param needle The string to search for. May not be NULL.
 * @returns A pointer to the start of the last occurrence of the given substring,
 *	or NULL if the string doesn't contain the substring.
*/
extern const char *u8_strrstr(const char *haystack, const char *needle);

__nonnull((1,2))
/** Finds the first occurrence of a case-insensitive variation of the given substring in the given string.
 * See uchar_alike() for specifics on case-insensitivity.
 * @param haystack The string to search in. May not be NULL.
 * @param needle The string to search for. May not be NULL.
 * @returns A pointer to the start of the first occurrence of the given substring,
 *	or NULL is the string doesn't contain a variation of the substring.
*/
extern const char *u8_strstrI(const char *haystack, const char *needle);

__nonnull((1,2))
/** Finds the last occurrence of a case-insensitive variation of the given substring in the given string.
 * See uchar_alike() for specifics on case-insensitivity.
 * @param haystack The string to search in. May not be NULL.
 * @param needle The string to search for. May not be NULL.
 * @returns A pointer to the start of the first occurrence of the given substring,
 *	or NULL is the string doesn't contain a variation of the substring.
*/
extern const char *u8_strrstrI(const char *haystack, const char *needle);


__nonnull((1,2))
/** Determines if two utf-8 encoded strings contain the same characters.
 * Note that, for utf-8 normalized strings, strcmp achieves the same and is more efficient.
 * @param a A string. May not be NULL.
 * @param b Another string. May not be NULL.
 * @returns a and b contain the same characters.
*/
extern bool u8_streq(const char *a, const char *b);

__nonnull()
/** Determines if the first n characters of two utf-8 encoded strings are the same.
 * @param a A string. May not be NULL.
 * @param b Another string. May not be NULL.
 * @returns a and b contain the same first n characters.
*/
extern bool u8_strneq(const char *a, const char *b, size_t n);

__nonnull((1,2))
/** Determines if two utf-8 encoded strings contain the same characters.
 * Case-insensitive, see uchar_alike() for specifics.
 * @param a A string. May not be NULL.
 * @param b Another string. May not be NULL.
 * @returns a and b contain the same characters, ignoring case.
*/
extern bool u8_streqI(const char *a, const char *b);

__nonnull()
/** Determines if the first n characters of two utf-8 encoded strings are the same.
 * Case-insensitive, see uchar_alike() for specifics.
 * @param a A string. May not be NULL.
 * @param b Another string. May not be NULL.
 * @returns a and b contain the same first n characters, ignoring case.
*/
extern bool u8_strneqI(const char *a, const char *b, size_t n);

__nonnull((1))
/** Determines if the given utf-8 encoded string is normalized utf-8.
 * This means that the string is valid, and every character is encoded with its normal length.
 * @param str A string. May not be NULL.
 * @returns str is normalized utf-8.
*/
extern bool u8_isnorm(const char *str);

__nonnull((1))
/** Determines if the given utf-8 encoded string is valid utf-8.
 * This means that the every character in the string is assigned in the unicode standard.
 * Private use characters are considered valid.
 * Invalid encodings are allowed and are handled via normal fallbacks.
 * @param str A string. May not be NULL.
 * @returns str is valid utf-8.
*/
extern bool u8_isvalid(const char *str);

__nonnull((1,3))
/** Applies map_f to every character in the utf-8 encoded string and writes them to dst.
 * If map_f returns 0, no character is written to dst.
 * Every character written to dst is guaranteed to be utf-8 normalized.
 * Over-encoded NUL characters from the input are passed to map_f as 0, but the actual single-byte NUL terminator is not.
 * If dst is NULL, no write operations are performed but the correct byte amount is returned.
 * @param str The source string, may not be NULL.
 * @param dst The destination buffer, may be NULL.
 * @param map_f The function used to map characters, may not be NULL.
 * @returns The amount of bytes written to dst, including the NUL terminator.
*/
extern size_t u8_strmap(const char *str, char *dst, uchar_t (*map_f)(uchar_t));

__nonnull((4))
/** Applies map_f to, at most, the first n character in the utf-8 encoded string and writes them to dst.
 * If map_f returns 0, no character is written to dst.
 * Every character written to dst is guaranteed to be utf-8 normalized.
 * Over-encoded NUL characters from the input are passed to map_f as 0, but the actual single-byte NUL terminator is not.
 * If dst is NULL, no write operations are performed but the correct byte amount is returned.
 * @param str The source string, may be NULL if n is 0.
 * @param dst The destination buffer, may be NULL.
 * @param n The maximum amount of characters to read.
 * @param map_f The function used to map characters, may not be NULL.
 * @returns The amount of bytes written to dst, including the NUL terminator.
*/
extern size_t u8_strnmap(const char *str, char *dst, size_t n, uchar_t (*map_f)(uchar_t));

__nonnull((4))
/** Applies map_f to every character in the first c bytes of the utf-8 encoded string and writes them to buf.
 * If map_f returns 0, no character is written to buf.
 * Every character written to dst is guaranteed to be utf-8 normalized.
 * Over-encoded NUL characters from the input are passed to map_f as 0, but the actual single-byte NUL terminator is not.
 * If dst is NULL, no write operations are performed but the correct byte amount is returned.
 * @param str The source string, may be NULL if c is 0.
 * @param dst The destination buffer, may be NULL.
 * @param c The maximum amount of bytes to read. Note that this is not a limit on output length.
 * @param map_f The function used to map characters, may not be NULL.
 * @returns The amount of bytes written to dst, including the NUL terminator.
*/
extern size_t u8_strcmap(const char *str, char *dst, size_t c, uchar_t (*map_f)(uchar_t));

// #endregion u8string.h

#endif
