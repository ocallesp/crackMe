#include <sys/stat.h>
#include <sys/types.h> /* pid_t */
#include <sys/wait.h> /* waitpid */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "passcrackd.h"
#include <string.h>

#define MAXBUF 65536

unsigned char totalHosts = 1;
struct host listOfHosts[10];

char passwordToCrack[128];

int becomeDaemon(int flags)
{
	int maxfd, fd;
	
	switch (fork()) {
	case -1:
		fprintf(stderr, "ERROR: Unable to fork #1 to child process, exiting.\n"); 
		return -1;

	case 0: 
		break;			/* Child falls through... */
	default:
		 _exit(EXIT_SUCCESS); /* while parent terminates */
	}

	if (setsid() == -1) {/* Become leader of new session */
		fprintf(stderr, "ERROR: setsid() failed, exiting.\n");
		return -1;
	}

	switch (fork()) {	/* Ensure we are not session leader */
	case -1: 
		fprintf(stderr, "ERROR: Unable to fork #2 to child process, exiting.\n");
		return -1;

	case 0: 
		break;
	default:
		_exit(EXIT_SUCCESS);
	}

  	// Reaching this point means we are the child and that we will soon
  	//   // be disconnected from the parent that will terminate.

	if (!(flags & PS_NO_UMASK0))
		umask(0);	/* Clear file mode creation mask */

	if(!(flags & PS_NO_CHDIR))
		chdir("/");	/* Change to root directory */

	if(!(flags & PS_NO_CLOSE_FILES)) { /* Close all open files */
		maxfd = sysconf(_SC_OPEN_MAX);
		if (maxfd == -1)			/* Limit is indteterminate */
			maxfd = PS_MAX_CLOSE;	/* so take a guess */
	
		for (fd = 0; fd < maxfd; fd++)
			close(fd);
	}

	if (!(flags & PS_NO_REOPEN_STD_FDS)) {
		close(STDIN_FILENO);	/* Reopen standard fd's to /dev/null */

		fd = open("/dev/null", O_RDWR);

		if (fd != STDIN_FILENO)	/* 'fd' should be 0 */
			return -1;
		if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
			return -1;
		if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
			return -1;
	}


	return 0;
}


int getResources(void)
{
    	long double a[4], b[4], loadavg;
	FILE *fp;
    	char dump[50];
	int cores = 0;
	int coresConf = 0;
	int avPhyMem = 0;

    	fp = fopen("/proc/stat","r");
    	fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &a[0],&a[1],&a[2],&a[3]);
    	fclose(fp);
    	sleep(1);

    	fp = fopen("/proc/stat","r");
    	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
    	fclose(fp);

    	loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));

	coresConf = sysconf(_SC_NPROCESSORS_CONF);

	/* Index 0 will always be localhost */
	listOfHosts[0].local = true;
	listOfHosts[0].ipAddr = 0;
	listOfHosts[0].free = true;
	listOfHosts[0].memory = avPhyMem = sysconf(_SC_AVPHYS_PAGES);
	listOfHosts[0].noCores = sysconf(_SC_NPROCESSORS_ONLN);
	listOfHosts[0].cpuUsage = loadavg;

	logMessage("# of cores: %d", listOfHosts[0].noCores);
	logMessage("# of cores configures: %d", coresConf);
	logMessage("available memory: %d", listOfHosts[0].memory);
	logMessage("cpu usage: %f", listOfHosts[0].cpuUsage);

    return(0);
}

void * createUDPServer(void *arg)
{
  	int sock, status, buflen;
  	unsigned sinlen;
  	char buffer[MAXBUF];
  	struct sockaddr_in sock_in;
  	int yes = 1;
  	char addstr[128];

  	sinlen = sizeof(struct sockaddr_in);
  	memset(&sock_in, 0, sinlen);

  	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  	sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
  	sock_in.sin_port = htons(2222);
  	sock_in.sin_family = PF_INET;

  	status = bind(sock, (struct sockaddr *)&sock_in, sinlen);
  	logMessage("Bind Status = %d", status);

  	status = getsockname(sock, (struct sockaddr *)&sock_in, &sinlen);
  	logMessage("Sock port %d", htons(sock_in.sin_port));

  	buflen = MAXBUF;
  	memset(buffer, 0, buflen);

  	for ( ; ; ) {
     		status = recvfrom(sock, buffer, buflen, 0, (struct sockaddr *)&sock_in, &sinlen);
      		logMessage("recvfrom status = %d", status);
      		if (inet_ntop(sock_in.sin_family, &sock_in.sin_addr, addstr, sizeof(addstr))) 
          		logMessage("info request received from %s", addstr);
      		
		logMessage("message: %s", buffer);
      		parseData(addstr, buffer);
  	}

  	shutdown(sock, 2);
  	close(sock);
  	return 0;
}

int sendBroadcast(void)
{
  int sock, status, buflen, sinlen;
  char buffer[MAXBUF];
  struct sockaddr_in sock_in;
  int yes = 1;

  sinlen = sizeof(struct sockaddr_in);
  memset(&sock_in, 0, sinlen);
  buflen = MAXBUF;

  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
  sock_in.sin_port = htons(0);
  sock_in.sin_family = PF_INET;

  status = bind(sock, (struct sockaddr *)&sock_in, sinlen);
  logMessage("bind() status = %d", status);

  status = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(int));
  logMessage("setsockopt() status = %d", status);

  /* -1 = 255.255.255.255 this is a BROADCAST address,
 *      a local broadcast address could also be used.
 *           you can comput the local broadcat using NIC address and its NETMASK 
 *             */ 

  sock_in.sin_addr.s_addr = htonl(-1); /* send message to 255.255.255.255 */
  sock_in.sin_port = htons(atoi("2222")); /* port number */
  sock_in.sin_family = PF_INET;

  sprintf(buffer, "%d %s %d %d %f", REGISTER, "free", listOfHosts[0].memory, listOfHosts[0].noCores, listOfHosts[0].cpuUsage);
  buflen = strlen(buffer);
  status = sendto(sock, buffer, buflen, 0, (struct sockaddr *)&sock_in, sinlen);
  logMessage("sendto() status = %d", status);

  shutdown(sock, 2);
  close(sock);
}

void sendMessage(unsigned long ipAddr, char *msj) 
{
  	size_t sinlen, buflen;
  	int sock;
  	int status;
  	struct sockaddr_in servaddr;

  	logMessage("sendMessage");

  	sinlen = sizeof(struct sockaddr_in);
  	memset(&servaddr, 0, sinlen);

  	servaddr.sin_family = AF_INET;
  	servaddr.sin_port = htons(2222);
  	servaddr.sin_addr.s_addr = ipAddr;

  	sock = socket(AF_INET, SOCK_DGRAM, 0);

  	buflen = strlen(msj);
  	status = sendto(sock, msj, buflen, 0, (struct sockaddr *)&servaddr, sinlen);
  	logMessage("sendto() status = %d", status);

  	shutdown(sock, 2);
  	close(sock);
  	return ;
}

int sendBroadcastFound(void) {
  	int sock, status, buflen, sinlen;
  	char buffer[MAXBUF];
  	struct sockaddr_in sock_in;
  	int yes = 1;

  	sinlen = sizeof(struct sockaddr_in);
  	memset(&sock_in, 0, sinlen);
  	buflen = MAXBUF;

  	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  	sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
  	sock_in.sin_port = htons(0);
  	sock_in.sin_family = PF_INET;

  	status = bind(sock, (struct sockaddr *)&sock_in, sinlen);
  	logMessage("bind() status = %d", status);

  	status = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(int));
  	logMessage("setsockopt() status = %d", status);

  	/* -1 = 255.255.255.255 this is a BROADCAST address,
 	*      a local broadcast address could also be used.
 	*           you can comput the local broadcat using NIC address and its NETMASK 
 	*/
 	sock_in.sin_addr.s_addr = htonl(-1); /* send message to 255.255.255.255 */
  	sock_in.sin_port = htons(atoi("2222")); /* port number */
  	sock_in.sin_family = PF_INET;

  	sprintf(buffer, "found");
  	buflen = strlen(buffer);
  	status = sendto(sock, buffer, buflen, 0, (struct sockaddr *)&sock_in, sinlen);
  	logMessage("sendto() status = %d", status);

  	shutdown(sock, 2);
  	close(sock);
}

#define HASH_SIZE	128
#define NO_HOSTS	10
#define RANGE_SIZE	128
#define MAX_PASS_SIZE	20 
int createListOfHosts(void){
//	struct host *hosts = NULL;
	size_t size = sizeof(struct host);
	unsigned char i;	
	logMessage("createListOfHosts");

	if (listOfHosts == NULL)
		return 0;
	
	for (i = 0; i <	NO_HOSTS; i++) {
		listOfHosts[i].comm = -1;
		listOfHosts[i].local = false;
		listOfHosts[i].ipAddr = 0;
		listOfHosts[i].free = true;
		listOfHosts[i].memory = 0;
		listOfHosts[i].noCores = 0;
		listOfHosts[i].cpuUsage = 0;
		listOfHosts[i].hash[ 10 ] = '\0';
		listOfHosts[i].range[ 10 ] = '\0';
		listOfHosts[i].password[ 10 ] = '\0';
	}

	return 1; 
}




char abcSymbols[] = {'a','b','c','d','e','f'};
unsigned char noSymbols;

void schedulePasswords(void)
{
	char buffer[128];
	char tmp[128];
	char hash[] = "$6$b0TQ0PcQ$x9.PZeRaLWDGBtu0WSIZmjzqBFaeTcZk/ex5/UGp6Jyy/Q1H/81cQkOj/hKwBSYnN09UwgI6cDZOjS7idQS1n0";
	size_t passSize = 0;	
	size_t r ;
	char range[4];
	char i, j;
	char noRange;

	logMessage("schedulePasswords");	
	/* r = 6/3 = 2, so the first */
	noSymbols = sizeof(abcSymbols) / sizeof(char);
	logMessage("noSymbols: %d  totalHosts: %d", noSymbols, totalHosts);
	r = noSymbols / totalHosts;
		

	for (i = 0; i < totalHosts; i++) {
		sprintf(tmp,"%c-%c", abcSymbols[r * i], abcSymbols[r * i] + (r - 1));

		strcpy(listOfHosts[i].range, tmp);	
		logMessage("Host[%d].ipAddr: %s", i, listOfHosts[i].range);
		strcpy(listOfHosts[ i ].hash, hash );
		strcpy(passwordToCrack, hash);
		logMessage("Host[%d].hash: %s", i, listOfHosts[i].hash);
		if (i != 0) {
  			sprintf(buffer, "%d %s %s", CRACK, listOfHosts[i].hash, listOfHosts[i].range);
			logMessage("message to distribute: %s", buffer);
			logMessage("remote host");
			sendMessage(listOfHosts[i].ipAddr, buffer);
		} else {		
			logMessage("local host");
			switch (childPid[noProcesses] =  fork()) {
        		case -1: 
            			logMessage("Cannot create process");
            			break;
        		case 0:
				noRange = tmp[2] - tmp[0] + 1;
				for (j = 0; j < noRange; j++)
					listOfHosts[i].range[j] = tmp[0] + j;
				listOfHosts[i].range[j] = '\0';
            			/* This is the child */
            			logMessage("local range: %s", listOfHosts[i].range);	
            			exit(passGen(abcSymbols, noSymbols, listOfHosts[i].range, noRange, 5));
            			break;
        		default:
            			/* This is the parent */
            			logMessage("Parent %d", getpid());
            			break; 
			} 
        	}
	}
return ;	
}


int parseData(char *ipAddr, char *data)
{		
	struct host remote;
	char *tmp;
	int j;
	unsigned char noRange;
	char range[20];

	tmp =  strtok(data, " ");
	remote.comm = atoi(tmp);
	
	remote.local = false;
		
	switch (remote.comm) {
	case REGISTER:
		logMessage("register");
	
		inet_pton(AF_INET, ipAddr, &(listOfHosts[totalHosts].ipAddr));
		logMessage("remote.ipAddr: %d", remote.ipAddr); 

		tmp =  strtok(NULL, " ");
		if (!strcmp(tmp, "free"))
			listOfHosts[totalHosts].free = true;
		else
			listOfHosts[totalHosts].free = false;

		logMessage("remote.free: %d", listOfHosts[totalHosts].free);

		tmp = strtok(NULL, " ");
		listOfHosts[totalHosts].memory = atoi(tmp);
		logMessage("remote.memory: %d", listOfHosts[totalHosts].memory);

		tmp = strtok(NULL, " ");
		listOfHosts[totalHosts].noCores = atoi(tmp);
		logMessage("remote.noCores: %d", listOfHosts[totalHosts].noCores);

		tmp = strtok(NULL, " ");
		listOfHosts[totalHosts].cpuUsage = atof(tmp);
		logMessage("remote.cpuUsage: %f", listOfHosts[totalHosts].cpuUsage);

		totalHosts++;		
		logMessage("total hosts: %d", totalHosts);

		break;
	case CRACK:
		logMessage("crack");

		tmp = strtok(NULL, " ");
		strcpy(passwordToCrack, tmp);
		logMessage("remote.hash: %s", remote.hash);

		strcpy(passwordToCrack, remote.hash);

		tmp = strtok(NULL, " ");
		strcpy(remote.range, tmp);
		logMessage("remote.range: %s", remote.range);

		/* CREATE PROCESS */
		switch (childPid[noProcesses] =  fork()) {
		case -1:
			logMessage("Cannot create process");
			break;
		case 0:
			noRange = remote.range[2] - remote.range[0] + 1;
			for (j = 0; j < noRange; j++)
				range[j] = remote.range[0] + j;
			range[j] = '\0';
			/* This is the child */
			noSymbols = sizeof(abcSymbols) / sizeof(unsigned char);
			noRange = strlen(listOfHosts[0].range);
			exit(passGen(abcSymbols, noSymbols, range, noRange, 5));
			break;
		default:
			/* This is the parent */
			logMessage("Parent %d", getpid());
			break;	
		}

		break;

	case FINISH:
		logMessage("finish");
		/* Give me more work */
		break;

	case FOUND:
		logMessage("found");
		sendBroadcastFound();
		/* stop all processes */
		//tmp = strtok( data, " ");
		//strcpy(remote.password, tmp);
		//logMessage("remote.password: %s", remote.password);
		break;

	default:
		logMessage("command not found");
		break;
	}
	
	return 0;
}
