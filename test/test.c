#include "utf8.h"

char mul_apin[] = "\xF0\x92\x80\xAF" "\xF0\x92\x80\xB3";
char ueber[] = "Deutschland, Deutschland " "\xC3\xBC" "ber alles; " "\xC3\x0c" "ber alles in der Welt";


int main(int argc, char **argv)
{
	test_utf8();
	printf("Completed all tests!\n");
}