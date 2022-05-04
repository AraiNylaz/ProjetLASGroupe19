#ifndef _CONFIG_H_
#define _CONFIG_H_

#define SERVER_PORT 	80
#define SERVER_IP		"127.0.0.1"  /* localhost */
#define NBRCOMPTESENBANQUE 1000


struct CompteEnBanque{
  int solde;
  int noCompte;
} CompteEnBanque;
struct Banque{
 struct CompteEnBanque comptes[NBRCOMPTESENBANQUE];
} Banque;


#endif

