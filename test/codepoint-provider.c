// provider for unicode codepoints
#include "common.h"
#include "unic.h"

PROVIDER((struct, Codepoint), alphanum)
PROVIDER((struct, Codepoint), misc)
PROVIDER((struct, Codepoint), private_use)
PROVIDER((struct, Codepoint), unassigned)

size_t format_struct_Codepoint(char *to, size_t n,const struct Codepoint thing[restrict static 1])
{
	return snprintf(to, n, "U+%04X", thing->codepoint);
}

size_t alphanum(size_t cap, struct Codepoint buf[restrict static cap])
{
	const size_t N = 26+26+10;
	if(cap >= N)
	{
		size_t i = 0;

		for(char c = 'a'; c <= 'z'; ++c)
			buf[i++] = (struct Codepoint){ c, c, c + ('A' - 'a'), UCLASS_LOWERCASE_LETTER };
		for(char c = 'A'; c <= 'Z'; ++c)
			buf[i++] = (struct Codepoint){ c, c + ('a' - 'A'), c, UCLASS_UPPERCASE_LETTER };
		for(char c = '0'; c <= '9'; ++c)
			buf[i++] = (struct Codepoint){ c, c, c, UCLASS_DECIMAL_NUMBER };
	}

	return N;
}

size_t misc(size_t cap, struct Codepoint buf[restrict static cap])
{
	const size_t N = 12;
	if(cap < N)
		return N;

	buf[0] = (struct Codepoint){ // ü
		0xFC, 0xFC, 0xDC, UCLASS_LOWERCASE_LETTER 
	};
	buf[1] = (struct Codepoint){ // Ü
		0xDC, 0xFC, 0xDC, UCLASS_UPPERCASE_LETTER 
	};
	buf[2] = (struct Codepoint){ // ä
		0xE4, 0xE4, 0xC4, UCLASS_LOWERCASE_LETTER 
	};
	buf[3] = (struct Codepoint){ // Ä
		0xC4, 0xE4, 0xC4, UCLASS_UPPERCASE_LETTER 
	};
	buf[4] = (struct Codepoint){ // ö
		0xF6, 0xF6, 0xD6, UCLASS_LOWERCASE_LETTER 
	};
	buf[5] = (struct Codepoint){ // Ö
		0xD6, 0xF6, 0xD6, UCLASS_UPPERCASE_LETTER 
	};
	buf[6] = (struct Codepoint){ // ß
		0xDF, 0xDF, 0xDF, UCLASS_LOWERCASE_LETTER 
	};
	buf[7] = (struct Codepoint){ // €
		0x20AC, 0x20AC, 0x20AC, UCLASS_CURRENCY_SYMBOL
	};
	buf[8] = (struct Codepoint){ // İ
		0x0130, 'i', 0x0130, UCLASS_UPPERCASE_LETTER
	};
	buf[9] = (struct Codepoint){ // ı
		0x0131, 0x0131, 'I', UCLASS_LOWERCASE_LETTER
	};
	buf[10] = (struct Codepoint){ // MUL cuneiform sign
		0x1202F, 0x1202F, 0x1202F, UCLASS_OTHER_LETTER
	};
	buf[11] = (struct Codepoint){ // APIN cuneiform sign
		0x12033, 0x12033, 0x12033, UCLASS_OTHER_LETTER
	};
	
	return N;
}

size_t private_use(size_t cap, struct Codepoint buf[restrict static cap])
{
	size_t n;

	for(n = 0; n < cap/2; ++n)
	{
		uchar_t chr = (rand() & 0x1FFFF) + 0xF0000;

		buf[n] = (struct Codepoint){
			chr, chr, chr,
			UCLASS_PRIVATE_USE
		};
	}

	return n;
}

size_t unassigned(size_t cap, struct Codepoint buf[restrict static cap])
{
	size_t n;

	for(n = 0; n < cap; ++n)
	{
		unsigned r = rand();
		uchar_t u = r >> 16;

		uchar_t chr = (( (u % (0xD-3)) + 3 ) << 16) | (r & 0xFFFF);

		buf[n] = (struct Codepoint){
			chr, chr, chr,
			UCLASS_UNASSIGNED
		};
	}

	return n;
}
