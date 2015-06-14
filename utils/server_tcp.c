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

#include "time.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define LISTENQ 5
#define MAXLINE 255
#define SA struct sockaddr_in

// function main begins program execution
int main( int argc, char *argv[])
{

	int listenfd, connfd;
	char buffer[ MAXLINE ];
	struct sockaddr_in servaddr;
	time_t ticks;

	printf("server v1.0\n");

	// create TCP socket
	listenfd = socket( AF_INET, SOCK_STREAM, 0 );
	
	// specify server's IP address and port
	bzero( &servaddr, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
	servaddr.sin_port = htons(2222);
	// convert ASCII command line argument into the proper format
	bind( listenfd, (SA *) &servaddr, sizeof( servaddr ) );

	listen( listenfd, LISTENQ );

	for ( ; ; ) {
		printf("aceptar conexion\n");		
		connfd = accept( listenfd, (SA *) NULL, NULL );

		ticks = time( NULL );
		snprintf( buffer, sizeof( buffer ), "%.24s\r\n", ctime( &ticks ) );
		write( connfd, buffer, strlen( buffer ) );

		close( connfd );
	}

	return 0;
}
