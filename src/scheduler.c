#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define HASH_SIZE   99
#define NO_HOSTS    3
#define RANGE_SIZE  10
#define MAX_PASS_SIZE   20

char abcSymbols[] = {'a','b','c','d','e','f'};
unsigned char noSymbols = 6;
typedef enum {REGISTER = 0, CRACK, FINISH, FOUND} command;


struct host{
    command comm;
    bool local;
    unsigned long ipAddr;  // load with inet_aton()
    bool free;
    unsigned int memory;
    unsigned char noCores;
    float cpuUsage;
    char *hash;
    char *range;
    char *password;
} ;

unsigned char totalHosts;
struct host *listOfHosts;
struct host * createListOfHosts(void);
int schedulePasswords(void);

int main(int argc, char **argv)
{

	listOfHosts = createListOfHosts();
	totalHosts++;
	schedulePasswords();	
	return 0;
}


struct host *createListOfHosts(void)
{
    struct host *hosts = NULL;
    size_t size = sizeof(struct host);
    unsigned char i;    

    hosts = (struct host *)malloc(size * NO_HOSTS);
    
    for (i = 0; i < NO_HOSTS; i++) {
        hosts[i].comm = -1; 
        hosts[i].local = false;
        hosts[i].ipAddr = 0;
        hosts[i].free = true;
        hosts[i].memory = 0;
        hosts[i].noCores = 0;
        hosts[i].cpuUsage = 0;
        hosts[i].hash = (char *)malloc(HASH_SIZE);
        hosts[i].range = (char *)malloc(RANGE_SIZE);
        hosts[i].password = (char *)malloc(MAX_PASS_SIZE);
    }   

    return hosts; 
}


int schedulePasswords(void )
{
	char *hash = "$6$b0TQ0PcQ$ygdSQw.jXAFjtl2MA7DDTul.v/XFviegDZ/kb81V4Re4Hs/1lTN    KpmrD6jqNqBPPDOe1A.hXqZ8VHSW7J6vMp0";
	size_t passSize = 0;
	size_t r = noSymbols / totalHosts;
	char range[4];
	char i;

	printf("schedulePasswords");	
	printf("r: %d  noSymbols: %d   totalHosts: %d", r, noSymbols, totalHosts);
	/* r = 6/3 = 2, so the first */
	printf("%d\n", r);	
	for (i = 0; i < totalHosts; i++) {
		sprintf(listOfHosts[i].range,"%c-%c", abcSymbols[i + r] + (r - 1));
		printf("Host[%d].%s", i, listOfHosts[i].range);
		strcpy(listOfHosts[i].hash, hash);
		printf("Host[%d].%s", i, listOfHosts[i].hash);
	}
	
	return;

}

