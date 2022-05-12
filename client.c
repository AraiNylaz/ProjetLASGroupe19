
#include "utils_v1.h"
#include "config.h"

#define MAXVIREMENTRECURRENT 100
#define MAXCHAR 100
#define HEARTBEAT -1
#define VIREMENT 0

<<<<<<< HEAD
/*
PRE : pipefd != NULL
POST : crée le handle des virements récurents
ce dernier envoye une série de virement inscrit au
préalable au serveur à chaque fois qu'il read
un heartbeat 
*/ : 
void virementRecurrentHandler(void* pipefd);

/*
PRE : pipefd != NULL
POST : write un heartbeat dans un pipe
tout les x temps en fonction du DELAIS
*/ 
void minuterieHandler(void* pipefd);


/*
POST : tue les programmes enfants conrespondant aux 2 pids
fournis et exit le programme avec un code 0
*/ 
void quit(pid_t PIDVirementReccurent, pid_t PIDMinuterie);

/*
PRE : response != NULL
POST : envoye une structure ResponseClient au serveur et 
printf si print est à true ou non si print est à false
*/ 
=======
/**
 * @brief 
 * 
 * @param pipefd 
 */
void virementRecurrentHandler(void* pipefd);

/**
 * @brief 
 * 
 * @param pipefd 
 */
void minuterieHandler(void* pipefd);

/**
 * @brief 
 * 
 * @param PIDVirementReccurent 
 * @param PIDMinuterie 
 */
void quit(pid_t PIDVirementReccurent, pid_t PIDMinuterie);

/**
 * @brief 
 * 
 * @param response 
 * @param print 
 */
>>>>>>> 77080138db6fface87f23cafc0e80d3c2584c556
void envoyerVirement(ResponseClient response, bool print);

char* adresse;
int portServeur;
int numeroCompte;
int delais;

/**
 * @brief 
 * 
 * @param argc 
 * @param arg 
 * @return int 
 */
int main(int argc, char *arg[]) {
    if(argc != 5) {
        printf("usage : ./client [IP address] [port] [no compte destination] [delais]\n");
        _exit(2);
    }
    adresse = arg[1];
    portServeur = atoi(arg[2]);
    numeroCompte = atoi(arg[3]);
    delais = atoi(arg[4]);

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
        virement.noCompteSource = numeroCompte;
        virement.noCompteDestination = atoi(compte);

        Transfer transfert;
        transfert.type = VIREMENT;
        transfert.response = virement;

        if(prefix == 'q') {
            //quit and free ressources
            quit(PIDVirementReccurent, PIDMinuterie);
        }else if(prefix == '+'){
            //virement
            envoyerVirement(virement, true);
        }else if(prefix == '*'){
            //virement récurrent
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
        } else {
            for (size_t i = 0; i < index; i++){
                envoyerVirement(tabCompte[i], false);
            }
        }
    }
}

void minuterieHandler(void* pipefd){
    int *pipe = pipefd;
    int delaisMinuterie = delais;
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

void envoyerVirement(ResponseClient virement, bool print){
    int sockfd = ssocket();
    sconnect(adresse, portServeur, sockfd);
    ResponseClient data = virement;
    swrite(sockfd, &data, sizeof(ResponseClient));

    ResponseServer response;
    sread(sockfd, &response, sizeof(ResponseServer));
    if(print) {
        if(response.code == NOCOMPTEINVALIDE) {
            printf(STRINGCOMPTEINVALIDE);
        } else if(response.code == SOLDEINSUFFISANT) {
            printf(STRINGSOLDEINSUFFISANT);
        } else if (response.code == CODEOK) {
            printf("montant restant sur votre compte : %i\n", response.solde);
        }
    }
    
    sclose(sockfd);

}