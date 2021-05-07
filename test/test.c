#include "utf8.h"
#include "u8string.h"
#include "util.h"

int main(void)
{
	#define TEST(m) puts("Testing " #m ".h..."); test_##m ()

	TEST(utf8);
	TEST(u8string);
	TEST(util);
	printf("Completed all tests!\n");
	
	#undef TEST
}