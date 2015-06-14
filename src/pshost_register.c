


struct table {
	unsigned int 	hostId;
	struct in_addr	addr;	/* Ip address */
	char passwordBase[30]; /* If you have a guess, then put here */
	char passwordRange[10];	/* Range of vowels to start trying */
	unsigned char resources; /* If 0 do not assign more tasks (lower the best)*/
	unsigned char priority; /* High will be attended first */
};


int getAvailableHosts() {


	
}





addHost(unsigned int hostId) {



}



removeHost(unsigned int hostId) {




}

addTaskToHost(unsigned int hostId, 
