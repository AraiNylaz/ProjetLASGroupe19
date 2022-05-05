
#include "config.h"
#include "utils_v1.h"

char ctrlC[100];

void ctrlCHandler(int sig){

    printf("ctrl C sur le serveur --> arret en cours...");
    swrite(FD_0, ctrlC, sizeof(ctrlC));

}

int initSocketServer(int port){

  int sockfd = ssocket();
  sbind(port, sockfd);
  slisten(sockfd, BACKLOG);
  return sockfd;

}

int main(int argc, char **argv){
    if(argc != 2) {
        printf("usage : ./server [port]\n");
        _exit(2);
    }
    //prepare les signaux et les ajoutent au masque
    sigset_t set;
    ssigemptyset(&set);
    sigaddset(&set, SIGINT);
    ssigprocmask(SIG_BLOCK, &set, NULL);
    ssigaction(SIGINT, ctrlCHandler);

    int port = atoi(argv[1]);

    // connexion port - sockets
    int sockfd = initSocketServer(port);
    printf("Le serveur tourne sur le port : %i , en attente de connexions...\n", port);
    
    ssigprocmask(SIG_UNBLOCK, &set, NULL);

    int sem_id, shm_id;
    int montantArgument;
    struct ResponseClient responseClient;
    int numeroCompteSource;
    int numeroCompteDestination;
    struct CompteEnBanque compte[NBRCOMPTESENBANQUE]; 
    //TODO faire en sorte que tous les comptes soient enregistrés en memoire...

    //tant que ! ctrlC --> run
    while (true) {

        //accepte une instance de client
        int newsockfd = saccept(sockfd);
        //va cherhcer les valeurs envoyee par le client
        ssize_t returnValue = sread(newsockfd, &responseClient, sizeof(responseClient));
        numeroCompteSource = responseClient.noCompteSource;
        montantArgument = responseClient.montant;
        numeroCompteDestination = responseClient.noCompteDestination;

        // allocation des 1000 comptes de la banque
        smalloc((sizeof(CompteEnBanque) * NBRCOMPTESENBANQUE)); //TODO
        
        //get semaphores et sharedmemory + attach
        shm_id = sshmget(SHMKEY, sizeof(ResponseClient), 0);
        sem_id = sem_get(SEMKEY, 2);
        sshmat(shm_id);

        for (int i = 0; i < NBRCOMPTESENBANQUE; i++)
        {
            //compte = banque[i]; TODO localiser 1 compte dans le tableau de comptes
            // si id du compte pas ok
            if(numeroCompteSource < 0 || numeroCompteSource > 1000) {
                printf("numero de compte invalide... réessayez\n");
                continue;
            }
            //prends la main
            sem_down0(sem_id);
            //si solde insiffisant
            if(compte[responseClient.noCompteDestination].solde - montantArgument < 0){
                printf("Solde insuffisant sur le compte\n");
                continue;
            }
            //else modifications des montants
            compte[numeroCompteSource].solde += montantArgument;
            compte[numeroCompteDestination].solde -= montantArgument;
            //redonne la main
            sem_up0(sem_id);

        }
            
    }

    //fermeture propre    
    sem_delete(sem_id);
    sshmdelete(sem_id);
    sclose(sockfd);
    _exit(0);
}
