#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "utils_v1.h"

int main(int argc, char* argv){
    int numCompte = atoi(argv[1]);
    int montant=atoi(argv[2]);
    int sem_id=sem_get(SEMKEY,1);
    int shm_id=sshmget(SHMKEY,sizeof(int),PERM);
    
        
    sem_down0(sem_id);
    Banque* livreDeCompte = sshmat(shm_id);
    if (checkNeg(montant)){
        //montant négatif => retrait 
        *livreDeCompte[numCompte-1].solde-=montant;
    }else{
        //montant positif => ajout
        *livreDeCompte[numCompte-1].solde+=montant;
    }
    sem_up0(sem_id);
    shmdt(solde);
}

