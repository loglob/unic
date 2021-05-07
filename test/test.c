#include "utf8.h"
#include "u8string.h"

int main(void)
{
	#define TEST(m) puts("Testing " #m ".h..."); test_##m ()

	TEST(utf8);
	TEST(u8string);
	printf("Completed all tests!\n");
	
	#undef TEST
}