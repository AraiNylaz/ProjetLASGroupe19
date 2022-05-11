#ifndef _CONFIG_H_
#define _CONFIG_H_


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/ipc.h>

#define SERVER_IP		"127.0.0.1"  /* localhost */

#define NBRCOMPTESENBANQUE 1000
#define BACKLOG 10
#define FD_0 0 //read
#define FD_1 1 //write
#define FD_ERROR 2 //error
#define PERM 0666
#define SHMKEY 1234
#define SEMKEY 1234

typedef struct CompteEnBanque{
int noCompte;
int solde;
}CompteEnBanque;

struct Banque{
 int comptes[NBRCOMPTESENBANQUE];
} Banque;



typedef struct ResponseClient{
  int montant;
  int noCompteSource;
  int noCompteDestination;
} ResponseClient;

typedef struct Transfer {
    int type;
    ResponseClient response;
} Transfer;

typedef struct ResponseServer{
  int code;
  int solde;
} ResponseServer;


#endif

