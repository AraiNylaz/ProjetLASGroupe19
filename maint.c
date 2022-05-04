#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include "config.h"
#include "utils_v1.h"

#define PERM 0666



int main (int agrc , char* argv){
    int type = argv[1] ;
    int shm_id;
    int sem_id;
    switch(type)
    {
    case type == 1:
    //crée la mémoire partagée
        printf("type=1");
        shm_id = sshmget(SHMKEY, Banque*sizeof(int*), IPC_CREAT | PERM);
        sem_id = sem_create(SEMKEY,1,PERM,1);
        printf("%d et %d ont bien été crées",shm_id,sem_id);
        break;
    case type == 2:
    //détruit la mémoire partagée
        printf("type=2");
        sshmdelete(shm_id);
        sem_delete(sem_id);
        printf("%d et %d ont bien été détruites",shm_id,sem_id);
        break;
    case type == 3:
    //réserve la mémoire partagée
        printf("type=3");
        sem_down0(sem_id);
        sleep(argv[2]);
        sem_up0(sem_id);
        break;
    default:
        break;
       
    }
}

