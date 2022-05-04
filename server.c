
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "config.h"
#include "utils_v1.h"

sig_atomic_t end = 1;
char ctrlC[100];

int killHandler(int sig){
    printf("ctrl C sur le serveur --> arret en cours...");
    swrite(FD_0, ctrlC, sizeof(ctrlC));
}

int initSocketServer(int port)
{
  int sockfd = ssocket();
  sbind(port, sockfd);
  slisten(sockfd, BACKLOG);
  return sockfd;
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        printf("usage : ./server [port]\n");
        _exit(2);
    }

    sigset_t set;
    ssigemptyset(&set);
    sigaddset(&set, SIGINT);
    ssigprocmask(SIG_BLOCK, &set, NULL);
    ssigaction(SIGINT, killHandler);

    int port = atoi(argv[1]);
    int sockfd = initSocketServer(port);
    printf("Le serveur tourne sur le port : %i \n", port);
    ssigprocmask(SIG_UNBLOCK, &set, NULL);

    struct CompteEnBanque compte;
    struct Banque banque;

    while (!end) {

        int newsockfd = saccept(sockfd);
        //printf("connection demandee :: %i", port);

        smalloc((sizeof(CompteEnBanque) * NBRCOMPTESENBANQUE)); //TODO
        
        int sem_id, shm_id;
        sshmget(SHMKEY, sizeof(CompteEnBanque)*10, 0);
        sem_get(SEMKEY, 2);
        sshmat(shm_id);

        for (int i = 0; i < NBRCOMPTESENBANQUE; i++)
        {
            //compte = banque[i]; TODO localiser 1 compte dans le tableau de comptes
            int noCompte = compte.noCompte;
            if(noCompte < 0 || noCompte > 1000) {
                printf("numero de compte invalide... réessayez\n");
                continue;
            }
            
        }
            
    }
    sclose(sockfd);
}
