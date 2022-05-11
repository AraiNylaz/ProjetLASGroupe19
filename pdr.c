#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "utils_v1.h"

int main(int argc, char *argv[]){
    if(argc != 3) {
        printf("usage : ./pdr [no compte destination] [montant]\n");
        _exit(2);
    }
    int numCompte = atoi(argv[1]);
    if(numCompte < 0 || numCompte >= 1000) _exit(3);
    int montant = atoi(argv[2]);
    int sem_id = sem_get(SEMKEY,1);
    int shm_id = sshmget(SHMKEY,NBRCOMPTESENBANQUE*sizeof(CompteEnBanque), 0 | PERM);
    struct CompteEnBanque*  livreDeCompte;
    if((livreDeCompte = (CompteEnBanque*)malloc((sizeof(CompteEnBanque) * NBRCOMPTESENBANQUE))) == NULL){
            perror("erreur Malloc tableau de comptes");
            _exit(3);
    }
    livreDeCompte = sshmat(shm_id);
    livreDeCompte[numCompte - 1].noCompte = numCompte;
    for (int  i = 0; i < 3; i++)
    {
        printf("livreComte %i :: %i\n", i, livreDeCompte[i].solde);
        printf("no compte %i :: %i\n", i, livreDeCompte[i].noCompte);
    }
    sem_down0(sem_id);
    //printf("passe ici ! %i\n", montant);
    livreDeCompte[numCompte - 1].solde  += montant;
    sem_up0(sem_id);

    sshmdt(livreDeCompte);

}

