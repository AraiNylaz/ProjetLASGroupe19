#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils_v1.h"

#define MAXVIREMENTRECURRENT 100
#define MAXCHAR 100
#define HEARTBEAT 7777

void virementRecurrentHandler(void* pipefd);
void minuterieHandler(void* pipefd, void* delais);
void quit();

char* ADRESSE;
int SERVER_PORT;
int NUMERO_COMPTE;
int DELAIS;
pid_t PIDMINUTERIE;
pid_t PIDVIREMENTRECCURENT;


int main(int argc, char *arg[]) {
    ADRESSE = arg[1];
    SERVER_PORT = atoi(arg[2]);
    NUMERO_COMPTE = atoi(arg[3]);
    DELAIS = atoi(arg[4]);

    int pipefd[2];
    spipe(pipefd);
    PIDVIREMENTRECCURENT = fork_and_run1(virementRecurrentHandler, pipefd);
    sclose(pipefd[0]);

    void *pointerDelais = &DELAIS;
    PIDMINUTERIE = fork_and_run2(minuterieHandler, pointerDelais, pipefd);
    int numeroCompte = 1;
    while(true){
        char val[MAXCHAR];
        int ret = sread(0, val, sizeof(val));
        val[ret-1] = '\0';

        char prefix = val[0];

        if(prefix == 'q') {
            //quit and free ressources
            quit();
        }else if(prefix == '+'){
            //virement
            printf("%s\n", "virement");
        }else if(prefix == '*'){
            //virement récurrent
            printf("%s\n", "virement récurent");
            swrite(pipefd[1], &numeroCompte, sizeof(int));
            numeroCompte++;

        }
    }

}

void virementRecurrentHandler(void* pipefd){
    int *pipe = pipefd;
    sclose(pipe[1]);
    int val;

    int tabCompte[MAXVIREMENTRECURRENT];
    int index = 0;

    while(true){
        sread(pipe[0], &val, sizeof(int));
        if(val != HEARTBEAT){
            tabCompte[index] = val;
            index++;
            printf("%s %i\n", "compte ajouté : ", tabCompte[index-1]);
            printf("%s", "test");
        } else {
            for (size_t i = 0; i < index; i++){
                //virement
                printf("%i", tabCompte[i]);
                printf("%s", "test");
            }
        }
    }
}

void minuterieHandler(void* pipefd, void* delais){
    int *pipe = pipefd;
    int delaisMinuterie = *((int *) delais);
    int val = HEARTBEAT;
    while(true){
        sleep(delaisMinuterie);
        swrite(pipe[1], &val, sizeof(int));
    }
}

void quit(){
    printf("%s\n","fin du client");
    skill(PIDMINUTERIE, SIGKILL);
    skill(PIDVIREMENTRECCURENT, SIGKILL);
    exit(0);
}