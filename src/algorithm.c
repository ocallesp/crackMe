#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <crypt.h>
#include <string.h>

#define DELAY		1


char *generatedPass = NULL;

extern char passwordToCrack[ 128 ];
//aaabd
//char *passwordToCrack = "$6$b0TQ0PcQ$x9.PZeRaLWDGBtu0WSIZmjzqBFaeTcZk/ex5/UGp6Jyy/Q1H/81cQkOj/hKwBSYnN09UwgI6cDZOjS7idQS1n0";
//"$6$b0TQ0PcQ$ygdSQw.jXAFjtl2MA7DDTul.v/XFviegDZ/kb81V4Re4Hs/1lTNKpmrD6jqNqBPPDOe1A.hXqZ8VHSW7J6vMp0";

int found(const char letter, const char symbols[], unsigned char noSymbols);
int passGen( const char [], unsigned char, const char [],unsigned char, unsigned char);

/*
int main( int argc, char **argv){

	int result = 0;
	unsigned char noSymbols = 0;
	const char symbols[] = {'a','b','c','d'};
	const char range[] = {'b'};
	unsigned char noRange = 0;

	noSymbols = sizeof(symbols)/sizeof(const char);
	noRange = sizeof(range)/sizeof(unsigned char);

	result = passGen(symbols, noSymbols, range, noRange, 5);

	return 0;
}
*/

/* Generate all possible passwords
 *
 * example
 * symbols = 'a' 'b' 'c' 
 * password size of 4
 * starting with 'a'
 * level 0                      a
 * level 1       a              b                c
 * level 2   a   b   c	    a   b   c		 a   b   c
 * level 3  abc abc abc    abc abc abc      abc abc abc
 *
 * */
int passGen(const char symbols[], unsigned char noSymbols, const char range[], unsigned char noRange, unsigned char passSize)
{

	char *buffer = NULL;
	unsigned char nLevel = 0, cLevel = 0;
	unsigned char h,i,j;
	unsigned char *conts = NULL;

	logMessage("passGen");

	buffer = (unsigned char *)malloc(passSize +1);	
	conts = (unsigned char *)malloc(passSize);
	for (h = 0; h < passSize; h++) {
		buffer[ h ] = '\0';
		conts[ h ] = 0;
	} 	

	logMessage("number of symbols: %d\n", noSymbols);
	logMessage("hash to crack: %s", passwordToCrack);
	logMessage("max password size: %d", passSize);
	
	
	do {	
		/* Check if symbols are in the blacklist */
		if (cLevel == 0 && !found(symbols[conts[cLevel]], range, noRange)) {	
			if (conts[cLevel] < (noSymbols-1))
				conts[ cLevel ]++;
			else
				return 0;

			continue;
		}
			
		/* If we are in level 0, so lets modify only the first character
 		   of the generated password */
		buffer[cLevel] = symbols[conts[cLevel]];
		buffer[cLevel + 1] = '\0';	
		/* PUT CRYPT FUNCTION AND COMPARE HERE */
		generatedPass = crypt(buffer, "$6$b0TQ0PcQ$");
		if (!strcmp( generatedPass, passwordToCrack )) {
			logMessage("PASSWORD FOUND!!\n");
			logMessage("pasword: %s\n", buffer);
			return 2;
		}

		logMessage("password: %s\n", buffer);
	
		/* The counter of a level will tell which symbol to choose */
		nLevel = cLevel + 1;
		for (i = 1; i < noSymbols; i++) {
			buffer[nLevel] = symbols[i];
			buffer[nLevel + 1] = '\0';		
			/* PUT CRYPT FUNCTION AND COMPARE HERE */	
			generatedPass = crypt(buffer, "$6$b0TQ0PcQ$");
			if (!strcmp(generatedPass, passwordToCrack)) {
				logMessage("PASSWORD FOUND!!\n");
				logMessage("pasword: %s\n", buffer);
				return 2;
			}

			logMessage("password: %s\n", buffer);
			sleep(DELAY);
		}
	
		/* Each time the counter of the next level gets the maximum value, 
 		   then the current counter is incremented by 1 and the next level 
		   is set to 0 again */	
		if (nLevel < (passSize-1)) {
			buffer[nLevel] = symbols[conts[nLevel]];	
			cLevel++;		
		} else {
			for (j = (passSize-2); j >= 0; j--) { 
				if (conts[j] < (noSymbols-1)) {
					conts[j]++;
					break;
				} else {
					if (j == 0)
						return 0;
					conts[j] = 0;
					cLevel--;
				}
			}
		}

		/* If we are creating passwords for the last symbol, we are just done */
		if (conts[0] == noSymbols)
			break; 	
						
	} while (1);

	return 1;
}


int found(const char letter, const char symbols[], unsigned char noSymbols)
{
	unsigned char j = 0;

	for (j = 0; j < noSymbols; j++)
		if (letter == symbols[j])
			return 1;

	return 0;
}
