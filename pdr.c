#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "utils_v1.h"

int main(int argc, char *argv[]){
    int numCompte = atoi(argv[1]);
    int montant= atoi(argv[2]);
    printf("%d",montant);
    int sem_id=sem_get(SEMKEY,1);
    int shm_id=sshmget(SHMKEY,NBRCOMPTESENBANQUE*sizeof(int),0| PERM);
    struct Banque* livreDeCompte = sshmat(shm_id);
    sem_down0(sem_id);
   
   
    (*livreDeCompte).comptes[numCompte]+=montant;
     printf("%d",livreDeCompte->comptes[numCompte]);
    
    sem_up0(sem_id);
    sshmdt(livreDeCompte);

}

