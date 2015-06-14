#include<pthread.h> //for threading , link with lpthread
#include<stdio.h>
#include<time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#define MAXBUF	65536
void * createUDPServer(void *arg);

int main( int argc, char **argv){	
	
	pthread_t thread_id;
	int arguments = 10;

   if( pthread_create( &thread_id , NULL , createUDPServer, (void*) &arguments) < 0){
       perror("could not create thread");
       return 1;
   }
    

	for(;;){
		printf("waiting for a conexion\n");
		sleep(1);
	}     

	return 0;
}



void * createUDPServer(void *arg){
  int sock, status, buflen;
  unsigned sinlen;
  char buffer[MAXBUF];
  struct sockaddr_in sock_in;
  int yes = 1;
  char addstr[128];

  sinlen = sizeof(struct sockaddr_in);
  memset(&sock_in, 0, sinlen);

  sock = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
  sock_in.sin_port = htons(2222);
  sock_in.sin_family = PF_INET;

  status = bind(sock, (struct sockaddr *)&sock_in, sinlen);
  printf("Bind Status = %d\n", status);

  status = getsockname(sock, (struct sockaddr *)&sock_in, &sinlen);
  printf("Sock port %d\n",htons(sock_in.sin_port));

  buflen = MAXBUF;
  memset(buffer, 0, buflen);
  for(;;){
      status = recvfrom(sock, buffer, buflen, 0, (struct sockaddr *)&sock_in, &sinlen);
      printf("sendto Status = %d\n", status);
      if (inet_ntop(sock_in.sin_family, &sock_in.sin_addr, addstr, sizeof(addstr))){
          printf("info request received from %s\n", addstr);
      }
  }

  shutdown(sock, 2);
  close(sock);
  return 0;
}
