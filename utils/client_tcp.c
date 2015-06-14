// This program is part of test to crack passwords
// 
// You can send 1 or more passwords to crack at the same time
// and the work is distributed almost equal
//
// Every computer will have a list of the other computers
// and the resources available. For example. Disk space, ram 
// and processors so it will help to decide how much work that
// computer will receive.
// How many threads should run in a computer per password ?
//
// I need to heuristic to distribute the password to crack.
// 
// If a computer finishes, then will send a message to the others
// to tell he found the password or it finishes its part of the work.
// When the other computer receive a message that the password
// was found, the rest of the computer will stop cracking that 
// specific password.
//
// What happen if suddenly one computer hangs or disconnect from
// the networkd ? 
//

#include "stdio.h"	// implements printf(), fputs()
#include "unistd.h"	// implements read()
#include "string.h"	// implements bzero()
#include <sys/types.h>	// has data types for sockets
#include <sys/socket.h>	// implements socket(), connect()
#include <arpa/inet.h>	// implements inet_pton


#define MAXLINE 255
#define SA struct sockaddr_in

// function main begins program execution
int main( int argc, char *argv[])
{

	int sockfd, n;
	char recvline[ MAXLINE + 1];
	struct sockaddr_in servaddr;

	if (argc != 2)
	{
		printf( "usage : a.out <IPaddress>\n" );
		return 1;
	}
	
	// create TCP socket
	printf( "create socket\n" );
	if ( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		printf( "ERROR: cannot create socket\n" );
		return 1;
	}

	printf( "initialize ip address %s and port 13\n", argv[1] );
	// specify server's IP address and port
	bzero( &servaddr, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(2222);
	// convert ASCII command line argument into the proper format
	if ( inet_pton( AF_INET, argv[1], &servaddr.sin_addr) <= 0 )
	{
		printf( "ERROR: not in presentation format error for %s\n", argv[1] );
		return 1;
	}


	// open socket for conexion
	printf( "open conexion\n" );
	if ( connect( sockfd, (const SA *) &servaddr, sizeof( servaddr ) ) < 0 )
	{	
		printf( "ERROR: connect error\n" );
		return 1;
	}

	printf( "read from socket\n" );
	while ( (n = read( sockfd, recvline, MAXLINE ) ) > 0 ) {
		recvline[ n ] = 0;
		if ( fputs( recvline, stdout ) == EOF )
		{
			printf( "ERROR: fputs error\n" );
			return 1;
		}
	}

	if (n < 0)
	{
		printf( "ERROR: cannot read from sockfd\n" );
		return 1;
	}

	exit(0);

}
