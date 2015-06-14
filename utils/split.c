#include<string.h>
#include<stdio.h>


int main( int argc, char **argv){

	char data[] = "uno dos asdl;fjasldkjfasd a-d";

	split(data);

	return 0;
}


int split(char *data){

	const char delim = ' ';
	char *buffer;

	buffer = strtok( data, " ");

	while(buffer != NULL){

		printf("information: %s\n", buffer);
		buffer = strtok( NULL, " ");

	} 


}
