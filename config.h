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
#define CODEOK 0
#define FD_0 0
#define NOCOMPTEINVALIDE 1
#define SOLDEINSUFFISANT 2
#define STRINGCOMPTEINVALIDE "compte invalide\n"
#define STRINGSOLDEINSUFFISANT "solde insuffisant\n"
#define PERM 0666
#define SHMKEY 1234
#define SEMKEY 1234

typedef struct CompteEnBanque{
int noCompte;
int solde;
}CompteEnBanque;

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

