#include <sys/wait.h> /* waitpid */
#include <signal.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h> //for threading , link with lpthread
#include <time.h>
#include <stdarg.h>

#include "passcrackd.h"


#define MAXBUF 	256
extern char passwordToCrack[ 128 ];
static const char *LOG_FILE = "/tmp/ps.log";
static const char *CONFIG_FILE = "/tmp/ps.conf";


bool readConfigFile(const char *configFilename);
void logClose(void);
void logOpen(const char *logFilename);
void logMessage(const char *format, ...);


static volatile sig_atomic_t hupReceived = 0; /* Set nonzero on receipt of SIGHUP */
static void sighupHandler(int sig)
{
	hupReceived = 1;
}

// MANAGE USER INTERFACE COMMANDS
static void sigUIHandler(int sig)
{
	// READ QUEUE
	// SAVE COMMAND
}

extern struct host *listOfHosts;

int main(int argc, char **argv)
{
	const int SLEEP_TIME = 15; 	/* Time to sleep between messages */
	int count = 0;				/* Number of completed SLEEP_TIME intervals */
	int unslept = 0;				/* Time remaining in sleep interval */
	struct sigaction sa;
	int status;
	int serverSocket;
	pthread_t thread_id;
	bool crack = false;

	// Set signal handlers
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = sighupHandler;
	if (sigaction(SIGHUP, &sa, NULL) == -1)
		errExit("sigaction");
	
	// Detach from terminal or parent process
	if (becomeDaemon(1) == -1)
		errExit("becomeDaemon");

	
	/* Set log file for debugging */	
	logOpen(LOG_FILE);
	readConfigFile(CONFIG_FILE);

	// CREATE STRUCTURE/LIST TO MANAGE HOSTS
	if ((status = createListOfHosts()) == 0) {
		logMessage("creatListOfHosts() failed");
		errExit("createListOfHosts");
	}

	// GET LOCAL COMPUTER RESOURCES
	logMessage("get local resources");
	if ((status = getResources()) != OK) {
		logMessage("getResources() failed");
		errExit("getResources");	
	}
	
	// SEND BROADCAST (UDP)
	logMessage("broadcast sent");
	if ((status = sendBroadcast()) != OK) {
		logMessage("sendBroadcast() failed");
		errExit("sendBroadcast");
	}

	// FORK A THREAD TO WAIT FOR UDP PACKETS
	if (pthread_create(&thread_id, NULL, createUDPServer, (void *)0) < 0) {
		logMessage("pthread_create() failed");
		errExit("pthread_create");
	}

	logMessage("server socket created");

	unslept =  SLEEP_TIME;


	for ( ; ; ) {

		unslept = sleep(unslept);	/* Return > 0 if interrupted */

		if (hupReceived) {	// If we got SIGHUP 
			logClose();
			logOpen(LOG_FILE);
			logMessage("hupReceived");
			crack = readConfigFile(CONFIG_FILE);
			if (crack)
				logMessage("crack");
			else
				logMessage("hash not found");
			hupReceived = 0;
		}
	
		if (crack) {
			logMessage("password to crack: %s", passwordToCrack);
			schedulePasswords();
			crack = false;
		}

		if (unslept == 0) {	// On completed interval 	
			count ++;
			logMessage("Main: %d", count);
			unslept = SLEEP_TIME;	 //Reset interval 	
	
		}
	}

	logClose();
	return 0;
}

