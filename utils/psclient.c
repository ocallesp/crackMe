/* $Id$ */

/*
 * build instructions
 *
 * gcc -o bclient bclient.c
 *
 * Usage:
 * ./bclient <serverport>
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#define MAXBUF 65536
typedef enum {REGISTER, CRACK, FINISH, FOUND} command;

int main(int argc, char*argv[])
{
  command comm = REGISTER;
  char *password = "$6$b0TQ0PcQ$ygdSQw.jXAFjtl2MA7DDTul.v/XFviegDZ/kb81V4Re4Hs/1lTNKpmrD6jqNqBPPDOe1A.hXqZ8VHSW7J6vMp0";
  char *range = "a-d";

  int sock, status, buflen, sinlen;
  char buffer[MAXBUF];
  struct sockaddr_in sock_in;
  int yes = 1;

  sinlen = sizeof(struct sockaddr_in);
  memset(&sock_in, 0, sinlen);
  buflen = MAXBUF;

  sock = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
  sock_in.sin_port = htons(0);
  sock_in.sin_family = PF_INET;

  status = bind(sock, (struct sockaddr *)&sock_in, sinlen);
  printf("Bind Status = %d\n", status);

  status = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(int) );
  printf("Setsockopt Status = %d\n", status);

  /* -1 = 255.255.255.255 this is a BROADCAST address,
     a local broadcast address could also be used.
     you can comput the local broadcat using NIC address and its NETMASK 
  */ 

  sock_in.sin_addr.s_addr=htonl(-1); /* send message to 255.255.255.255 */
  sock_in.sin_port = htons(atoi(argv[1])); /* port number */
  sock_in.sin_family = PF_INET;
  /* ipAddr free memory noCores cpuUsage hash range*/	
  sprintf(buffer, "%d %s %d %d %f %s %s ", REGISTER, "free", 8124, 4, 0.0032, password, range);
  buflen = strlen(buffer);
  status = sendto(sock, buffer, buflen, 0, (struct sockaddr *)&sock_in, sinlen);
  printf("sendto Status = %d\n", status);

  shutdown(sock, 2);
  close(sock);
}
