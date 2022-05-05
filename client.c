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
        int ret = sread(0, val, sizeof(val));
        val[ret-1] = '\0';

        char prefix = val[0];

        ResponseClient virement;
        virement.montant = 0;
        virement.noCompteSource = NUMERO_COMPTE;
        virement.noCompteDestination = numeroCompteDestinataire;

        Transfer transfert;
        transfert.type = VIREMENT;
        transfert.response = virement;

        if(prefix == 'q') {
            //quit and free ressources
            quit(PIDVirementReccurent, PIDMinuterie);
        }else if(prefix == '+'){
            //virement
            printf("%s\n", "virement");
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
                //virement
                printf("%i\n", tabCompte[i].noCompteDestination);
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
    exit(0);
}