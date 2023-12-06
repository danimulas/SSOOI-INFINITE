#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

//#include <unistd.h>
//#include <signal.h>


pid_t H1 = -1, H2 = -1, N2 = -1, H3 = -1, N3 = -1, H4 = -1;
sigset_t maskPadre;
sigset_t maskTestigo;
int pidDisparo;
int temp = 1;
int grandchild = -1;
int parentpid = 0;
struct sigaction manejadoraT;
struct sigaction manejadoraA;
struct sigaction manejadoraerror;
struct sigaction terminateHandler;
int contador = 0;
void error(){
    temp = -1;
    if(H1 != -1){
        if(kill(H1, SIGINT) == -1){
            perror("Error al eliminar (H1)");
            exit(-1);
        }
    }
    if(H2 != -1){
        if(kill(H2, SIGTERM) == -1){
            perror("Error al eliminar (H2)");
            exit(-1);
        }
    }
    if(H3 != -1){
        if(kill(H3, SIGTERM) == -1){
            perror("Error al eliminar (H3)");
            exit(-1);
        }
    }
    if(H4 != -1){
        if(kill(H4, SIGINT) == -1){
            perror("Error al eliminar (H4)");
            exit(-1);
        }
    }
    while(wait(NULL) > 0);
    if(kill(parentpid, SIGINT) == -1){
        perror("Error al eliminar al proceso principal");
        exit(-1);
    }
}
void codHijo(){
    while(temp) sigsuspend(&maskTestigo);
}
void mascarabloqueo() {
    if (sigfillset(&maskPadre) == -1) {
        perror("Error en sigfillset");
        exit(-1);
    }
    if (sigdelset(&maskPadre, SIGINT) == -1) {
        perror("Error en sigdelset");
        exit(-1);
    }
    if(sigdelset(&maskPadre, SIGTERM) == -1){
        perror("Error en sigdelset");
        exit(-1);
    }
}

void mascaratestigo() {
    if (sigfillset(&maskTestigo) == -1) {
        perror("Error en sigfillset");
        exit(-1);
    }
    if (sigdelset(&maskTestigo, SIGUSR1) == -1 || sigdelset(&maskTestigo, SIGINT) == -1 ||
        sigdelset(&maskTestigo, SIGALRM) == -1 || sigdelset(&maskTestigo, SIGTERM) == -1 ||
        sigdelset(&maskTestigo, SIGKILL) == -1 || sigdelset(&maskTestigo, SIGUSR2) == -1) {
        perror("Error en sigdelset");
        exit(-1);
    }
}

void funcionTestigo() {
    if (kill(pidDisparo, SIGUSR1) == -1) {
        perror("Error en kill");
        kill(parentpid, SIGUSR2);
    }
}

void funcionAlarma() {
    temp = -1;
    system("clear");
    printf("        @@@@@@@@@@@@@@@@                            @@@@@@@@@@@@@@@@@         \n");
    printf("     @@@@@@@@@@@@@@@@@@@@@@@                    @@@@@@@@@@@@@@@@@@@@@@@@,     \n");
    printf("   @@@@@@@@@(       @@@@@@@@@@@.             @@@@@@@@@@@         @@@@@@@@@.   \n");
    printf(" .@@@@@@@                @@@@@@@@@        @@@@@@@@@@                @@@@@@@@  \n");
    printf(" @@@@@@                     @@@@@@@@@   @@@@@@@@@                     @@@@@@@ \n");
    printf("@@@@@@                         @@@@@@@@@@@@@@@                        .@@@@@@ \n");
    printf("@@@@@@                           .@@@@@@@@@@                           @@@@@@ \n");
    printf("@@@@@@                              %d                              @@@@@@ \n", contador / 2);
    printf("@@@@@@                           @@@@@@@@@@@                           @@@@@@ \n");
    printf("@@@@@@@                        @@@@@@@@@@@@@@@@                       @@@@@@@ \n");
    printf(" @@@@@@@                    @@@@@@@@@    @@@@@@@@@                   @@@@@@@  \n");
    printf("  @@@@@@@@               @@@@@@@@@.        @@@@@@@@@@              @@@@@@@@   \n");
    printf("    .@@@@@@@@@@*  .&@@@@@@@@@@@&              @@@@@@@@@@@@@&#&@@@@@@@@@@@     \n");
    printf("       @@@@@@@@@@@@@@@@@@@@@                     &@@@@@@@@@@@@@@@@@@@@@       \n");
    printf("           @@@@@@@@@@@@                                @@@@@@@@@@@&            \n");

    if (kill(H1, SIGINT) == -1) {
        perror("Error en kill (H1)");
    }
        
    if (kill(H4, SIGINT) == -1) {
        perror("Error en kill (H4)");
    }

    if (kill(H2, SIGTERM) == -1) {
        perror("Error en kill (H2)");
    }

    if (kill(H3, SIGTERM) == -1) {
        perror("Error en kill (H3)");
    }
    waitpid(H1, NULL, 0);
    waitpid(H2, NULL, 0);
    waitpid(H3, NULL, 0);
    waitpid(H4, NULL, 0);

    exit(0);
}

void alarmHandler() {
    manejadoraA.sa_handler = &funcionAlarma;
    manejadoraA.sa_flags = SA_RESTART;
    if (sigaction(SIGALRM, &manejadoraA, NULL) == -1) {
        perror("Error en sigaction (SIGALRM)");
        exit(-1);
    }
}

void TestigoHandler() {
    manejadoraT.sa_handler = &funcionTestigo;
    manejadoraT.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR1, &manejadoraT, NULL) == -1) {
        perror("Error en sigaction (SIGUSR1)");
        exit(-1);
    }
}
void errorHandler(){
    manejadoraerror.sa_handler = &error;
    manejadoraerror.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR2, &manejadoraerror, NULL) == -1) {
        perror("Error en sigaction (SIGINT)");
        exit(-1);
    }
}
void terminar() {
    if(grandchild != -1){
        if(kill(grandchild, SIGINT) == -1){
        perror("Error en kill (grandchild)");
        kill(parentpid, SIGUSR2);
        }
        wait(NULL);
    }
        kill(getpid(), SIGINT);
}

void fterminateHandler() {
    terminateHandler.sa_handler = &terminar;
    terminateHandler.sa_flags = SA_RESTART;
    if (sigaction(SIGTERM, &terminateHandler, NULL) == -1) {
        perror("Error en sigaction (SIGTERM)");
        exit(-1);
    }
}

int main() {
    mascarabloqueo();
    mascaratestigo();
    TestigoHandler();
    alarmHandler();
    errorHandler();
    fterminateHandler();
    if (sigprocmask(SIG_SETMASK, &maskPadre, NULL) == -1) {
        perror("Error en sigprocmask");
        exit(-1);
    }
    parentpid = getpid();
    (H1 = fork()) && (H4 = fork()) && (H2 = fork()) && (H3 = fork()); // Creates 4 children

    if (H1 == 0) {
        /* Child H1 code goes here */
        pidDisparo = getppid();
        codHijo();
    } else if (H4 == 0) {
        /* Child H4 code goes here */
        pidDisparo = getppid();
        codHijo();
    } else if (H2 == 0) {
        N2 = fork();
        if (N2 == 0) {
            /* Grandchild N2 code goes here */
            pidDisparo = H1;
            codHijo();
        } else if (N2 > 0) {
            /* Child H2 code goes here */
            pidDisparo = N2;
            grandchild = N2;
            codHijo();
        } else if (N2 == -1) {
            perror("Error en fork (N2)");
            kill(parentpid, SIGUSR2);
        }
    } else if (H3 == 0) {
        N3 = fork();
        if (N3 == 0) {
            /* Grandchild N3 code goes here */
            pidDisparo = H4;
            codHijo();
        } else if (N3 > 0) {
            /* Child H3 code goes here */
            pidDisparo = N3;
            grandchild = N3;
            codHijo();
        } else if (N3 == -1) {
            perror("Error en fork (N3)");
            kill(parentpid, SIGUSR2);
        }
    } else if (H1 == -1 || H2 == -1 || H3 == -1 || H4 == -1) {
        perror("Error en algun fork (H)");
        kill(parentpid, SIGUSR2);
    } else {
        /* Parent code goes here */
        if(alarm(25) == -1){
            perror("Error en alarm");
            kill(parentpid, SIGUSR2);
        }
        pidDisparo = H2;
        if (kill(pidDisparo, SIGUSR1) == -1) {
            perror("Error en kill (H2)");
            kill(parentpid, SIGUSR2);
        }
        while (temp) {
            if (contador % 2 == 0) { //si es par
                pidDisparo = H3;
            } else {
                pidDisparo = H2;
            }
            sigsuspend(&maskTestigo);
            contador++;
        }
    }
}
