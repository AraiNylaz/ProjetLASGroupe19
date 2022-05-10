#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils_v1.h"
#include "config.h"

#define MAXVIREMENTRECURRENT 100
#define MAXCHAR 100
#define HEARTBEAT -1
#define VIREMENT 0

void virementRecurrentHandler(void* pipefd);
void minuterieHandler(void* pipefd);
void quit(pid_t PIDVirementReccurent, pid_t PIDMinuterie);
void envoyerVirement(ResponseClient response);

char* ADRESSE;
int SERVER_PORT;
int NUMERO_COMPTE;
int DELAIS;


int main(int argc, char *arg[]) {
    ADRESSE = arg[1];
    SERVER_PORT = atoi(arg[2]);
    NUMERO_COMPTE = atoi(arg[3]);
    DELAIS = atoi(arg[4]);

    int pipefd[2];
    spipe(pipefd);
    pid_t PIDVirementReccurent = fork_and_run1(virementRecurrentHandler, pipefd);
    sclose(pipefd[0]);

    pid_t PIDMinuterie = fork_and_run1(minuterieHandler, pipefd);

    int numeroCompteDestinataire = 1;
    while(true){
        char val[MAXCHAR];
        int ret = sread(0, val, MAXCHAR);
        val[ret-1] = '\0';

        char prefix = val[0];

        
        char compte[4];
        char montant[MAXCHAR];
        int indexVal = 2;
        int indexCompte = 0;
        int indexMontant = 0;

        while(val[indexVal] != ' '){
            compte[indexCompte] = val[indexVal];
            indexVal++;
            indexCompte++;
        }
        compte[indexCompte] = '\0';
        indexVal++;

        while(val[indexVal] != '\0'){
            montant[indexMontant] = val[indexVal];
            indexVal++;
            indexMontant++;
        }
        montant[indexMontant] = '\0';
        
        ResponseClient virement;
        virement.montant = atoi(montant);
        virement.noCompteSource = NUMERO_COMPTE;
        virement.noCompteDestination = atoi(compte);

        Transfer transfert;
        transfert.type = VIREMENT;
        transfert.response = virement;

        if(prefix == 'q') {
            //quit and free ressources
            quit(PIDVirementReccurent, PIDMinuterie);
        }else if(prefix == '+'){
            //virement
            printf("%s\n", "virement");
            envoyerVirement(virement);
        }else if(prefix == '*'){
            //virement récurrent
            printf("%s\n", "virement récurent");
            swrite(pipefd[1], &transfert, sizeof(Transfer));
            numeroCompteDestinataire++;

        }
    }

}

void virementRecurrentHandler(void* pipefd){
    int *pipe = pipefd;
    sclose(pipe[1]);
    //int val;
    Transfer buffer;
    ResponseClient tabCompte[MAXVIREMENTRECURRENT];
    int index = 0;

    while(true){
        
        sread(pipe[0], &buffer, sizeof(Transfer));
        //sread(pipe[0], &val, sizeof(int));
        
        if(buffer.type != HEARTBEAT){
            tabCompte[index] = buffer.response;
            index++;
            printf("%s %i\n", "compte ajouté : ", tabCompte[index-1].noCompteDestination);
        } else {
            for (size_t i = 0; i < index; i++){
                printf("%i\n", tabCompte[i].noCompteDestination);
                //virement
                envoyerVirement(tabCompte[i]);
            }
        }
    }
}

void minuterieHandler(void* pipefd){
    int *pipe = pipefd;
    int delaisMinuterie = DELAIS;
    ResponseClient temp = {0, 0, 0};
    Transfer heartbeat = {HEARTBEAT, temp};
    while(true){
        sleep(delaisMinuterie);
        swrite(pipe[1], &heartbeat, sizeof(Transfer));
    }
}

void quit(pid_t PIDVirementReccurent, pid_t PIDMinuterie){
    printf("%s\n","fin du client");
    skill(PIDMinuterie, SIGKILL);
    skill(PIDVirementReccurent, SIGKILL);
    _exit(0);
}

void envoyerVirement(ResponseClient virement){
    int sockfd = ssocket();
    sconnect(ADRESSE, SERVER_PORT, sockfd);
    ResponseClient data = virement;
    swrite(sockfd, &data, sizeof(ResponseClient));

    char response[90];
    sread(sockfd, &response, 90);
    if(!(response[2] == '\0' && response[0] == 'o' && response[1]=='k')){
        printf("%s\n", response);
    }

}