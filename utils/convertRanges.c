#include <string.h>
#include <stdio.h>



int main( int argc, char **argv){
	int i;
	size_t noRange;
	char range[4] = "a-c";
	char buffer[20];

	noRange = range[2] - range[0] + 1;
	
	printf("noRange: %d\n", noRange);


	for(i = 0; i < noRange; i++){
		buffer[i] = range[0] + i;
	}
	buffer[i] = '\0';

	printf("buffer: %s\n", buffer);

	return 0;
}
