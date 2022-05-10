
#Makefile groupe 19

CC= gcc
CCFLAGS= -D_DEFAULT_SOURCE -D_XOPEN_SOURCE -D_BSD_SOURCE -std=c11 -pedantic -Wvla -Wall -Werror -Wno-unused-variable

ALL=  pdr server client maint

all: $(ALL)

pdr: pdr.o utils_v1.o
	cc $(CFLAGS) -o pdr pdr.o utils_v1.o

pdr.o: pdr.c config.h utils_v1.h
	cc $(CFLAGS) -c pdr.c

server: server.o utils_v1.o
	$(CC) $(CCFLAGS) -o server server.o utils_v1.o

server.o: server.c config.h utils_v1.h
	$(CC) $(CCFLAGS) -c server.c

client: client.o utils_v1.o
	$(CC) $(CCFLAGS) -o client client.o utils_v1.o
	
client.o: client.c config.h utils_v1.h
	$(CC) $(CCFLAGS) -c client.c

maint: maint.o utils_v1.o
	cc $(CFLAGS) -o maint maint.o utils_v1.o

maint.o: maint.c config.h utils_v1.h
	cc $(CFLAGS) -c maint.c

utils_v1.o: utils_v1.c utils_v1.h
	$(CC) $(CCFLAGS) -c utils_v1.c

clean :
	rm -rf *.o
	rm -rf $(ALL)
	ipcrm -a

