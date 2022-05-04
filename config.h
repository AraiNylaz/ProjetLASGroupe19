#ifndef _CONFIG_H_
#define _CONFIG_H_

#define SERVER_PORT 	80
#define SERVER_IP		"127.0.0.1"  /* localhost */
#define NBRCOMPTESENBANQUE 1000
#define BACKLOG 5 //TODO vérifier si utile...
#define FD_0 0
#define FD_1 1
#define FD_ERROR 2
#define PERM 0666
#define SHMKEY 1234
#define SEMKEY 1234


struct CompteEnBanque{
  int solde;
  int noCompte;
} CompteEnBanque;
struct Banque{
 int comptes[NBRCOMPTESENBANQUE];
} Banque;


#endif

