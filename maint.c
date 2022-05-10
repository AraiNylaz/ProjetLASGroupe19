#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include "config.h"
#include "utils_v1.h"

#define PERM 0666



int main (int argc , char *argv[]){
    int type= atoi(argv[1]) ;
    int shm_id;
    int sem_id;
    switch(type)
    {
    case  1:
    //crée la mémoire partagée
        sshmget(SHMKEY, NBRCOMPTESENBANQUE*sizeof(int), IPC_CREAT | PERM);
        sem_create(SEMKEY,1,PERM,1);
        printf("la mémoire partagée et les sémaphores ont bien été crées \n");
        
        break;
    case  2:
    //détruit la mémoire partagée
        shm_id = sshmget(SHMKEY, NBRCOMPTESENBANQUE*sizeof(int), 0 | PERM);
        
        sem_id = sem_get(SEMKEY, 1);
        sshmdelete(shm_id);
        //sem_id=sem_get()
        sem_delete(sem_id);
        printf("la mémoire partagée et les sémaphores  ont bien été détruites \n ");
        break;
    case  3:
    //réserve la mémoire partagée
        sem_down0(sem_id);
        sleep(atoi(argv[2]));
        sem_up0(sem_id);
        break;
    default:
        printf("type non valide \n");
        break;
    }
}

