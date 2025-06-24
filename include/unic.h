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

/** Specified the size of a string.*/
typedef struct
{
	/** If set, `maxBytes` is an exact byte count, and NUL terminators should be treated as regular single byte characters. */
	bool bytesExact : 1;
	/** A maximum amount of bytes the string's content spans */
	size_t maxBytes : 63;
	/** If set, `maxChars` is an exact character count, and NUL terminators should be treated as regular single byte characters. */
	bool charsExact : 1;
	/** A maximum amount of characters the string's content spans */
	size_t maxChars : 63;
} u8size_t;


/** A u8size that specifies only a maximum byte size */
#define MAX_BYTES(n) ((u8size_t){ false, (n), false, (SIZE_MAX >> 1) })
/** A u8size that specifies only a maximum character count */
#define MAX_CHARS(n) ((u8size_t){ false, (SIZE_MAX >> 1), false, (n) })
/** A u8size that specifies an exact byte count */
#define EXACT_BYTES(n) ((u8size_t){ true, (n), false, (SIZE_MAX >> 1) })
/** A u8size that specifies an exact character count */
#define EXACT_CHARS(n) ((u8size_t){ false, (SIZE_MAX >> 1), true, (n) })
/** A u8size that imposes no size limit, i.e. reads until a NUL byte. */
#define NUL_TERMINATED ((u8size_t){ false, (SIZE_MAX >> 1), false, (SIZE_MAX >> 1) })


// #region utf8.c

__nonnull((1))
/** Reads the next unicode character from the given utf-8 encoded steam.
	Returns UEOF on reading EOF.
	@param f The file stream. May not be NULL.
	@returns The next unicode character in the stream
*/
extern uchar_t fgetu8(FILE *f);

__nonnull((2))
/** Writes the given unicode character to the given utf-8 encoded stream.
	Always writes either nothing on failure or an entire character on success.
	@param c A unicode character
	@param f A file stream. May not be NULL.
	@returns The amount of bytes written on success, or 0 on failure.
*/
extern size_t fputu8(uchar_t c, FILE *f);

/** Like u8dec(), but reads the next utf-8 encoded character in the first n bytes of the given string.
	If n is 0, behaves as if it read NUL terminator and returns 0.
	@param str The utf-8 encoded buffer to read from. May be NULL if n is 0.
	@param n The maximum amount of bytes to read.
	@param c The location to store the character in. May be NULL to only determine the length of the next character.
	@returns The amount of bytes read.
*/
extern size_t u8ndec(const char *str, size_t n, uchar_t *c);

__nonnull((1))
/** Reads the next utf-8 encoded character from the given string.
	Note that reading and re-encoding a character may change its length due to improper encoding in source streams.
	A well-encoded NUL terminator is treated as a character of length 1.
	@param str The utf-8 encoded buffer to read from. May not be NULL.
	@param c The location to store the character in. May be NULL to only determine the length of the next character.
	@returns The amount of bytes read. */
extern size_t u8dec(const char *str, uchar_t *c);

/** Writes the given unicode character to the buffer.
	@param uc The unicode character
	@param buf The buffer to write the character to. May be NULL to only determine its length.
	@returns The amount of bytes written. */
extern size_t u8enc(uchar_t uc, char *buf);

// #endregion utf8.c

// #region util.c

/** Retrieves the unicode character class of the given character
	@param c The character
	@returns Its general category, or UCLASS_UNASSIGNED if it is invalid.
*/
extern enum unic_gc uchar_class(uchar_t c);

/** Determines if two unicode characters are alike/similar.
	This means that some combination of the simple lower- or uppercase mapping produces the same character
	i.e.: ∃f,g ∈ { lower, upper, id }: f(a) = g(b)
	@param a A character
	@param b Another character
	@returns a is similar to b
*/
extern bool uchar_alike(uchar_t a, uchar_t b);

/** Determines if two unicode character classes are the same or compatible
	@param general A general category, may be a major category
	@param specific A general category
	@returns Both classes are the same,
	or general is a major category which specific is a subcategory of.
*/
extern bool uclass_is(enum unic_gc general, enum unic_gc specific);

/** Determines if a unicode character is of a unicode class
	@param chr The character
	@param class The general category, may be a major category
	@returns The character is of the given class, or of the given major category.
*/
extern bool uchar_is(uchar_t chr, enum unic_gc class);

/** Determines if the given unicode character is whitespace
	@param c The character
	@returns c is in the SEPARATOR general category,
	or an ascii control character that is considered whitespace, as per libc's isspace()
*/
extern int u_isspace(uchar_t c);

/** Returns the simple lowercase mapping of the given character
	@param c The character
	@returns Its simple lowercase mapping, or the character itself if it doesn't exist.
*/
extern uchar_t uchar_lower(uchar_t c);

/** Returns the simple uppercase mapping of the given character
	@param c The character
	@returns Its simple uppercase mapping, or the character itself if it doesn't exist.
*/
extern uchar_t uchar_upper(uchar_t c);

// #endregion util.c

// #region u8string.c

__nonnull((1))
/** Determines the amount of unicode characters in the given NUL-terminated UTF-8 string.
	Does not count the final NUL terminator.
	
	@param str A NUL-terminated utf-8 string. May not be NULL.
	@returns The amount of unicode characters in str.
*/
extern size_t u8_strlen(const char *str);

__nonnull((1))
/** Copies the utf-8 encoded string str to dst.
	Every character written to dst is guaranteed to be utf-8 normalized.
	If `str` is already normalized, you should use `strcpy` instead.

	If dst is NULL, no write operations are performed but the correct byte amount is returned.

	@param str The NUL-terminated UTF-8 source string. May not be NULL.
	@param dst The destination buffer, may be NULL to just check the resulting size. The function behaves identical otherwise.
	@param cap Capacity of `dst` in bytes.
	@param nulTerminate If true, NUL characters written to `dst` are over-encoded as UNUL, and a closing NUL terminator is appended.
	@returns The size of the string written to `dst`. The `*exact` flags are set iff. the output was not truncated. 
			 If `nulTerminate` is set, the byte count includes the final NUL terminator, but the char does not.
*/
extern u8size_t u8_strcpy(const char *str, char *dst, size_t cap, bool nulTerminate);

__nonnull((1))
/** Looks up a character index in a UTF-8 encoded string.

	@param str The NUL-terminated UTF-8 string.
	@param pos The unicode character index.
	@returns A pointer to the start of the given unicode character,
		or NULL if the index is outside the string bounds.
*/
extern const char *u8_strpos(const char *str, size_t pos);

/** Finds the character at the given index in the given utf-8 encoded string.
	@param str The utf-8 encoded string. May be NULL is pos is 0.
	@param pos The unicode character index.
	@returns The character at that position,
	or 0 if the index is outside the string bounds.
*/
extern uchar_t u8_strat(const char *str, size_t pos);

__nonnull((1))
/** Finds the first occurrence of the given character in the given utf-8 encoded string.
	Note that, for utf-8 normalized strings, strstr achieves the same and will be more efficient.
	@param str The string. May not be NULL.
	@param chr The character to find.
	@returns A pointer to the start of the first occurrence of the given character,
	or NULL if the string doesn't contain the character.
*/
extern const char *u8_strchr(const char *str, uchar_t chr);

__nonnull((1))
/** Finds the first occurrence of the given character, or a case-insensitive variant of it, in the given utf-8 encoded string.
	see `uchar_alike()` for specifics on case-insensitivity.
	@param str The string. May not be NULL.
	@param chr The character to find.
	@returns A pointer to the start of the first occurrence of the given character or a case-insensitive variant of it,
	or NULL if the string doesn't contain the character.
*/
extern const char *u8_strchrI(const char *str, uchar_t chr);

__nonnull((1))
/** Finds the last occurrence of the given character in the given utf-8 encoded string.
	@param str The string. May not be NULL.
	@param chr The character to find
	@returns A pointer to the start of the last occurrence of the given character,
	or NULL is the string doesn't contain the character.
*/
extern const char *u8_strrchr(const char *str, uchar_t chr);

__nonnull((1))
/** Finds the last occurrence of the given character, or a case-insensitive variant of it, in the given utf-8 encoded string.
	see uchar_alike() for specifics on case-insensitivity.
	@param str The string. May not be NULL.
	@param chr The character to find
	@returns A pointer to the start of the last occurrence of the given character,
	or NULL is the string doesn't contain the character.
*/
extern const char *u8_strrchrI(const char *str, uchar_t chr);

__nonnull((1,2))
/** Finds the first occurrence of the given substring in the given utf-8 encoded string.
	Note that, for utf-8 normalized strings, strstr achieves the same and will be more efficient.
	@param haystack The string to search in. May not be NULL.
	@param needle The string to search for. May not be NULL.
	@returns A pointer to the start of the first occurrence of the given substring,
	or NULL if the string doesn't contain the substring.
*/
extern const char *u8_strstr(const char *haystack, const char *needle);

__nonnull((1,2))
/** Finds the last occurrence of the given substring in the given utf-8 encoded string.
	Note that, for utf-8 normalized strings, a loop based on strstr achieves the same and will be more efficient.
	@param haystack The string to search in. May not be NULL.
	@param needle The string to search for. May not be NULL.
	@returns A pointer to the start of the last occurrence of the given substring,
	or NULL if the string doesn't contain the substring.
*/
extern const char *u8_strrstr(const char *haystack, const char *needle);

__nonnull((1,2))
/** Finds the first occurrence of a case-insensitive variation of the given substring in the given string.
	See uchar_alike() for specifics on case-insensitivity.
	@param haystack The string to search in. May not be NULL.
	@param needle The string to search for. May not be NULL.
	@returns A pointer to the start of the first occurrence of the given substring,
	or NULL is the string doesn't contain a variation of the substring.
*/
extern const char *u8_strstrI(const char *haystack, const char *needle);

__nonnull((1,2))
/** Finds the last occurrence of a case-insensitive variation of the given substring in the given string.
	See uchar_alike() for specifics on case-insensitivity.
	@param haystack The string to search in. May not be NULL.
	@param needle The string to search for. May not be NULL.
	@returns A pointer to the start of the first occurrence of the given substring,
	or NULL is the string doesn't contain a variation of the substring.
*/
extern const char *u8_strrstrI(const char *haystack, const char *needle);

__nonnull((1,2))
/** Determines if two utf-8 encoded strings contain the same characters.
	Note that, for utf-8 normalized strings, strcmp achieves the same and is more efficient.
	@param a A string. May not be NULL.
	@param b Another string. May not be NULL.
	@returns a and b contain the same characters.
*/
extern bool u8_streq(const char *a, const char *b);

/** Determines if the first `n` characters of two strings are equal. */
extern bool u8_strneq(const char *a, const char *b, size_t n);

__nonnull((1,2))
/** Determines if two utf-8 encoded strings contain the same characters.
	Case-insensitive, see uchar_alike() for specifics.
	@param a A NUL-terminated string. May not be NULL.
	@param b Another string. May not be NULL.
	@returns a and b contain the same characters, ignoring case.
*/
extern bool u8_streqI(const char *a, const char *b);

/** Determines if the first `n` characters of two strings are equal, ignoring case. */
extern bool u8_strneqI(const char *a, const char *b, size_t n);

__nonnull((1))
/** Determines if the given utf-8 encoded string is normalized utf-8.
	This means that every character is encoded with its normal length.
	Specifically NUL may be encoded as 2 bytes.

	On normalized strings, most operations don't need to be UTF-8 aware and can be covered by `<string.h>`.
	@param str A NUL-terminated string. May not be NULL.
	@returns str is normalized utf-8.
*/
extern bool u8_isnorm(const char *str);

__nonnull((1))
/** Determines if the given utf-8 encoded string is valid utf-8.
	This means that the every character in the string is assigned in the unicode standard.
	Private use characters are considered valid.
	Invalid encodings are allowed and are handled via normal fallbacks.
	@param str A string. May not be NULL.
	@returns str is valid utf-8.
*/
extern bool u8_isvalid(const char *str);

__nonnull((1,5))
/** Applies map_f to every character in the utf-8 encoded string and writes them to dst.

	Every character written to dst is guaranteed to be utf-8 normalized.
	If dst is NULL, no write operations are performed but the correct byte amount is returned.

	Non-respected NULs in `str` are passed to `map_f`.
	NULs returned by map_f are encoded either as UNUL (if `nulTerminate` is set) or a NUL byte otherwise.

	@param str The source string, may not be NULL.
	@param dst The destination buffer, may be NULL to just check the resulting size. The function behaves identical otherwise.
	@param cap Capacity of `dst` in bytes.
	@param nulTerminate If true, content written to `dst` is NUL terminated, meaning that NUL characters are over-encoded as UNUL, and a closing NUL terminator is appended.
	@param map_f The function used to map characters, may not be NULL.
	@returns The size of the string written to `dst`. The `*exact` flags are set iff. the output was not truncated.
			 If `nulTerminate` is set, the byte count includes the final NUL terminator, but the char does not.
*/
extern u8size_t u8_strmap(const char *str, char *dst, size_t cap, bool nulTerminate, uchar_t (*map_f)(uchar_t));

// #endregion u8string.c

// #region u8sized.c

/** Determines the exact size of a string. Doesn't count a respected NUL terminator.

	@param str A UTF-8 string, possibly NUL-terminated, depending on `kind`
	@returns A size spec with both exact flags set
*/
extern u8size_t u8z_strsize(const char *str, u8size_t size);

/** Variant of `u8_strlen()` on a sized prefix */
extern size_t u8z_strlen(const char *str, u8size_t size);
/** Variant of `u8_strcpy()` on a sized prefix */
extern u8size_t u8z_strcpy(const char *str, u8size_t size, char *dst, size_t cap, bool nulTerminated);
/** Variant of `u8_strpos()` within a sized prefix */
extern const char *u8z_strpos(const char *str, u8size_t size, size_t pos);
/** Variant of `u8_strat()` within a sized prefix */
extern uchar_t u8z_strat(const char *str, u8size_t size, size_t pos);

/** Variant of `u8_strchr()` within a sized prefix */
extern const char *u8z_strchr(const char *str, u8size_t size, uchar_t chr);

/** Variant of `u8_strchrI()` within a sized prefix */
extern const char *u8z_strchrI(const char *str, u8size_t size, uchar_t chr);

/** Variant of `u8_strrchr()` within a sized prefix */
extern const char *u8z_strrchr(const char *str, u8size_t size, uchar_t chr);

/** Variant of `u8_strrchrI()` within a sized prefix */
extern const char *u8z_strrchrI(const char *str, u8size_t size, uchar_t chr);

/** Variant of `u8_strstr()` over sized prefixes */
extern const char *u8z_strstr(const char *haystack, u8size_t n, const char *needle, u8size_t m);

/** Variant of `u8_strrstr()` over sized prefixes */
extern const char *u8z_strrstr(const char *haystack, u8size_t n, const char *needle, u8size_t m);

/** Variant of `u8_strstrI()` over sized prefixes */
extern const char *u8z_strstrI(const char *haystack, u8size_t n, const char *needle, u8size_t m);

/** Variant of `u8_strrstrI()` over sized prefixes */
extern const char *u8z_strrstrI(const char *haystack, u8size_t n, const char *needle, u8size_t m);

/** Variant of `u8_streq()` over sized prefixes */
extern bool u8z_streq(const char *a, u8size_t n, const char *b, u8size_t m);

/** Variant of `u8_streqI()` over sized prefixes */
extern bool u8z_streqI(const char *a, u8size_t n, const char *b, u8size_t m);

/** Variant of `u8_isnorm()` on a sized prefix */
extern bool u8z_isnorm(const char *str, u8size_t size);

/** Variant of `u8_isvalid()` on a sized prefix */
extern bool u8z_isvalid(const char *str, u8size_t size);

/** Variant of `u8_strmap()` on a sized prefix */
__nonnull((6))
extern u8size_t u8z_strmap(const char *str, u8size_t size, char *dst, size_t cap, bool nulTerminate, uchar_t (*map_f)(uchar_t));

// #endregion u8sized.c
#endif
