/* 
 * Compile with
 * gcc -lcrypt crack_pass.c -o crack_pass
*/
#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <unistd.h>
#include <stdio.h>
#include <crypt.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>


int main( int argc, char **argv)
{
	char *hashGenerated;
	/* aaabd */
	char *password = "$6$b0TQ0PcQ$x9.PZeRaLWDGBtu0WSIZmjzqBFaeTcZk/ex5/UGp6Jyy/Q1H/81cQkOj/hKwBSYnN09UwgI6cDZOjS7idQS1n0";
	size_t max_size = 15;

	printf("size of hash: %d\n", strlen(password)+1);
	printf("password: %s\n", argv[1]);

	hashGenerated = crypt( argv[1], "$6$b0TQ0PcQ$");
	printf("hash generated: %s\n", hashGenerated);
	if(!strcmp( hashGenerated, password ))
		printf("FOUND!!!\n"); 
	else
		printf("NOT FOUND\n");

	return 0;
}

