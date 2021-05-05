#include <assert.h>
#include "../include/unic.h"

#define ue "\xc3\xbc"
#define UE "\xc3\x9c"
#define ae "\xc3\xa4"

char normalSample[] =
"Deutschland, Deutschland "ue"ber alles,\n"
UE"ber alles in der Welt,\n"
"Wenn es stets zu Schutz und Trutze\n"
"Br"ue"derlich zusammenh"ae"lt.";


void test_u8string()
{
	printf("Running tests for u8string...\n");
	
	assert(u8_strlen(normalSample) == sizeof(normalSample) - 5 /* 4 umlauts and 1 NUL terminator */);
}

int main(int argc, char **argv)
{
	test_u8string();
	printf("Completed all tests!\n");
}