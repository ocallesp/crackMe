# distributed-password-cracker

This program will be used to crack password in distributed environment.

I will add more information.

$ make

in one computer execute
$ ./daemon_ps

after few seconds 
in another computer execute
$ ./daemon_ps

the first computer will receive a broadcast from the second,third,etc.
the first computer will save the ipaddrs

then in the first computer execute
$ pgrep daemon_ps
$ kill -1 pid

it will calculate and distribute work.
password to crack is static :( I will change this.



