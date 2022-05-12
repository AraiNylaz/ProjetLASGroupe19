
#include "config.h"
#include "utils_v1.h"

char ctrlC[10];

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

    struct ResponseClient reponseClient;
    int montantResponse;
    int numeroCompteSource;
    int numeroCompteDestination;
    struct CompteEnBanque *tabCompte; //tableau de comptes
    struct ResponseServer reponseServeur;

    //tant que ! ctrlC --> run
    while (true) {

        //accepte une instance de client
        int newsockfd = saccept(sockfd);
        //va chercher les valeurs envoyees par le client
        printf("nouvelle connexion client\n");
        sread(newsockfd, &reponseClient, sizeof(reponseClient));
        numeroCompteSource = reponseClient.noCompteSource - 1;
        montantResponse = reponseClient.montant;
        numeroCompteDestination = reponseClient.noCompteDestination - 1;
        printf("response %i + %i + %i\n", numeroCompteDestination, numeroCompteSource, montantResponse);

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

        // si id du compte pas ok
        if(numeroCompteSource < 0 || numeroCompteSource > 1000) {
            reponseServeur.solde = 0;
            reponseServeur.code = NOCOMPTEINVALIDE;
            swrite(newsockfd, &reponseServeur, sizeof(reponseServeur));
            continue;
        }            
        //si solde insiffisant
        printf("solde compte --> %i\n", tabCompte[reponseClient.noCompteSource].solde);
        if(tabCompte[numeroCompteSource].solde - montantResponse < 0){
            reponseServeur.solde = 0;
            reponseServeur.code = SOLDEINSUFFISANT;
            swrite(newsockfd, &reponseServeur, sizeof(reponseServeur));
            continue;
        }
        printf("virement de %i € vers le compte %i de la part du compte %i.\n", montantResponse, numeroCompteDestination, numeroCompteSource);
        
        sem_down0(sem_id);
        //else modifications des montants
        tabCompte[numeroCompteSource].solde -= montantResponse;
        tabCompte[numeroCompteDestination].solde += montantResponse;

        printf("balance du compte source %i : %i\n", tabCompte[numeroCompteSource], tabCompte[numeroCompteSource].solde);
        printf("balance du compte destination %i : %i\n", tabCompte[numeroCompteSource], tabCompte[numeroCompteDestination].solde);
        
        reponseServeur.solde = tabCompte[numeroCompteSource].solde;
        reponseServeur.code = CODEOK;
        //envoi au client
        swrite(newsockfd, &reponseServeur, sizeof(reponseServeur));

        //redonne la main
        sem_up0(sem_id);
        
    }

    //fermeture propre    
    sem_delete(sem_id);
    sshmdelete(sem_id);
    sclose(sockfd);
    _exit(0);
}
