
#include "config.h"
#include "utils_v1.h"

/**
 * @brief permet de créer, detruire ou réserver la memoire partagee et semaphore 
 * 
 * @param argc prend 1 argument
 * @param argv l'option possible : 1, 2 ou 3 
 */
int main (int argc , char *argv[]){
    if(argc != 2) {
        printf("usage : ./maint [option]\n");
        _exit(2);
    }
    int type= atoi(argv[1]) ;
    int shm_id;
    int sem_id;
    switch(type)
    {
    case  1:
    //crée la mémoire partagée
        sshmget(SHMKEY, NBRCOMPTESENBANQUE*sizeof(CompteEnBanque), IPC_CREAT | PERM);
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
        sem_id = sem_get(SEMKEY,1);
        shm_id = sshmget(SHMKEY,NBRCOMPTESENBANQUE*sizeof(int),0|PERM);
        int* livreDeCompte = sshmat(shm_id);
        sem_down0(sem_id);
        sleep(atoi(argv[2]));
        sem_up0(sem_id);
        sshmdt(livreDeCompte);
        break;
    default:
        printf("type non valide \n");
        break;
    }
}

