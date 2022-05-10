
#include "config.h"
#include "utils_v1.h"

char ctrlC[100];

void ctrlCHandler(int sig){

    printf("CTRL+C sur le serveur :: arret en cours...");
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

    struct ResponseClient responseClient;
    int montantResponse;
    int numeroCompteSource;
    int numeroCompteDestination;
    struct CompteEnBanque *tabCompte; //tableau de comptes
    //TODO faire en sorte que tous les comptes soient enregistrés en memoire...

    //tant que ! ctrlC --> run
    while (true) {

        //accepte une instance de client
        int newsockfd = saccept(sockfd);
        //va cherer les valeurs envoyee par le client
        sread(newsockfd, &responseClient, sizeof(responseClient));
        numeroCompteSource = responseClient.noCompteSource;
        montantResponse = responseClient.montant;
        numeroCompteDestination = responseClient.noCompteDestination;
        printf("response %i + %i + %i\n", responseClient.noCompteDestination, responseClient.noCompteSource, responseClient.montant);

        // allocation des 1000 comptes de la banque     
        if((tabCompte = (CompteEnBanque*)malloc((sizeof(CompteEnBanque) * NBRCOMPTESENBANQUE))) == NULL){
           perror("erreur Malloc tableau de comptes");
           _exit(3);
        }
              
        //sread(newsockfd, tabCompte, sizeof(ResponseClient));
        
        //get semaphores et sharedmemory + attach
        shm_id = sshmget(SHMKEY, NBRCOMPTESENBANQUE*sizeof(CompteEnBanque), 0);
        sem_id = sem_get(SEMKEY, 1);
        tabCompte = sshmat(shm_id);


        for (int i = 0; i < 10; i++)
            {
                printf("malloc1 == %i + %i\n", tabCompte[i].noCompte, tabCompte[i].solde);
            }   
            
        //for (int i = 0; i < NBRCOMPTESENBANQUE; i++)
        //{
            // si id du compte pas ok
            if(numeroCompteSource < 0 || numeroCompteSource > 1000) {
                char* messageErreur = "numero de compte invalide... reessayez";
                printf("%s\n", messageErreur);
                //envoi au client
                swrite(newsockfd, messageErreur, sizeof(messageErreur));
                continue;
            }
            //prends la main
            sem_down0(sem_id);
            //si solde insiffisant
            if(tabCompte[responseClient.noCompteSource].solde - montantResponse < 0){
                char* messageErreur = "Solde insuffisant sur le compte";
                printf("%s\n", messageErreur);
                //envoi au client
                swrite(newsockfd, messageErreur, sizeof(messageErreur));
                continue;
            }
            printf("virement de %i € vers le compte %i de la part du compte %i.\n", montantResponse, numeroCompteDestination, numeroCompteSource);
            //else modifications des montants
            tabCompte[numeroCompteSource].solde += montantResponse;
            tabCompte[numeroCompteDestination].solde -= montantResponse;

            printf("balance du compte source : %i\n", tabCompte[numeroCompteSource].solde);
            printf("balance du compte destination : %i\n", tabCompte[numeroCompteDestination].solde);
            char* responseServer = "ok";
            //envoi au client
            swrite(newsockfd, responseServer, sizeof(responseServer));
            for (int i = 0; i < 10; i++)
            {
                printf("malloc2 == %i + %i\n", tabCompte[i].noCompte, tabCompte[i].solde);
            }   
            
            //redonne la main
            sem_up0(sem_id);

        //}
            
    }

    //fermeture propre    
    sem_delete(sem_id);
    sshmdelete(sem_id);
    sclose(sockfd);
    _exit(0);
}
