
#Makefile groupe 19

CC= gcc
CCFLAGS= -D_DEFAULT_SOURCE -D_XOPEN_SOURCE -D_BSD_SOURCE -std=c11 -pedantic -Wvla -Wall -Werror
ALL=  server


server: server.o utils_v1.o
	$(CC) $(CCFLAGS) -o server server.o utils_v1.o

server.o: server.c config.h utils_v1.h
	$(CC) $(CCFLAGS) -c server.c


utils_v1.o: utils_v1.c utils_v1.h
	$(CC) $(CCFLAGS) -c utils_v1.c

clean :
	rm -rf *.o
	rm -rf $(ALL)
	ipcrm -a