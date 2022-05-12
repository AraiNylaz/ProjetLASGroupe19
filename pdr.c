
#include "config.h"
#include "utils_v1.h"

#define LIMITE -1500

/**
 * @brief programme permettant de simuler des depots et retraits
 * 
 * @param argc prend 2 arguments
 * @param argv le compte de destination et le montant à deposer ou retirer
 */
int main(int argc, char *argv[]){ 
    if(argc != 3) {
        printf("usage : ./pdr [no compte destination] [montant]\n");
        _exit(2);
    }
    int numCompte = atoi(argv[1]);
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
    if (livreDeCompte[numCompte - 1].solde + montant >= LIMITE){
        sem_down0(sem_id);
         //TODO ajouter une linite max
         //printf("passe ici ! %i\n", montant);
        livreDeCompte[numCompte - 1].solde  += montant;
        sem_up0(sem_id);
    }else{
        printf("Solde insuffisant\n");
    }
    printf("solde restant : %i\n", livreDeCompte[numCompte - 1].solde);

    sshmdt(livreDeCompte);

}

