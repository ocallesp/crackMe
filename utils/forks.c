#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int mult(void);


static int idata = 111;

int main( int argc, char *argv[])
{
	int istack = 222;	/* allocated in stack segment */
	pid_t childPid;
	int status;
	int r;

	switch ( childPid = fork() ) {
	case -1:
		printf("Cannot create process\n");
		exit(-1);
	
	case 0:
		/* This is the child */
		exit(printNumbers(5));
		

	default:
		/* This is the parent */
		printf("Parent %d\n", getpid());
		sleep(3); /* Give child a chance to execute */
		break;
	}

	/* Both parent and child come here */
	if ( (r = wait(&status)) == -1 )
		printf("ERROR: could not return from child\n");
		
	printf( "return value from printNumbers = %d\n", status);
	printf( "return value from printNumbers = %d\n", r);

	exit(EXIT_SUCCESS);

}


int printNumbers( unsigned int x)
{
	unsigned int i,y = 0;

	printf("Child %d\n", getpid());

	for ( i = 0; i < x; i++){
		printf("%d\n",i);
		sleep(1);
	}

	y = i*10;
	printf("y = %d\n", y);
	return y;
}
