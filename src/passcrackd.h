#ifndef PASSCRACK_DAEMON_H		/* Prevent double inclusion */
#define PASSCRACK_DAEMON_H

#define PS_NO_CHDIR				01	/* Don't chdir */
#define PS_NO_CLOSE_FILES 		02	/* Don't close all open files */
#define PS_NO_REOPEN_STD_FDS	04	/* Don't reopen stdin, stdout, and stderr to /dev/null */

#define PS_NO_UMASK0			010 /* Don't do a mask(0) */

#define PS_MAX_CLOSE			8192 /* Maximum files descriptors to close if sysconf(_SC_OPEN_MAX) is interminate */

#define OK 		0
#define FAILURE 1
#define MAX_PROCESSES	4
#include <stdbool.h>
#include <unistd.h>

int becomeDaemon(int flags);
int getResources(void);
void * createUDPServer(void *);
int sendBroadcast(void);
int sendBroadcastFound(void);
int createListOfHosts(void);
void updateListOfHosts( int handle );
void schedulePasswords(void);
void sendMessage( unsigned long, char *);
int parseData(char *, char *);

typedef enum {REGISTER=0, CRACK, FINISH, FOUND} command;

struct host{
	command comm;
	bool local;
	unsigned long ipAddr;  // load with inet_aton()
	bool free;
	unsigned int memory;
	unsigned char noCores;
	float cpuUsage;
	char hash[128];
	char range[128];
	char password[128];
} ;

//struct host *listOfHosts;

unsigned char noProcesses;
pid_t childPid[ MAX_PROCESSES ];

#endif
