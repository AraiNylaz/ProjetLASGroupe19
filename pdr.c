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
    int montant= atoi(argv[2]);
    int sem_id=sem_get(SEMKEY,1);
    int shm_id=sshmget(SHMKEY,NBRCOMPTESENBANQUE*sizeof(CompteEnBanque), 0 | PERM);
    struct CompteEnBanque* livreDeCompte = sshmat(shm_id);
    printf("livreComte :: %i\n", livreDeCompte->solde);
    if(numCompte < 0 || numCompte >= 1000) _exit(3);
    livreDeCompte->noCompte = numCompte;
    printf("no compte :: %i\n", livreDeCompte->noCompte);
    sem_down0(sem_id);
    livreDeCompte->solde += montant;
    sem_up0(sem_id);
    sshmdt(livreDeCompte);

}

