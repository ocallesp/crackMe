all: crackMe

crackMe: daemon_ps.c passcrackd.c pslogs.c algorithm.c
	$(CC) -ggdb -lcrypt -lpthread algorithm.c daemon_ps.c pslogs.c passcrackd.c error_functions.c -o crackMe

clean:
	-rm crackMe

.PHONY: all clean
