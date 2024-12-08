#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
/*#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h> */
int main()
{

    pid_t myPid = getpid();

    int tempo;
    srand((unsigned)time(NULL));
    int x = rand() % 10;
    if (x == 0)
        tempo = 15;
    else if (x > 0 && x <= 3)
        tempo = 5;
    else
        tempo = 1;
    printf("+++++++++++++++++++++\n");
    printf("+++ My pid is %d.\n", myPid);
    printf("+++ My time is %d.\n", tempo);
    printf("+++++++++++++++++++++\n");
    FILE *demanda = fopen("demanda.txt", "w+");
    fprintf(demanda, "%d", tempo);
    fclose(demanda);
    raise(SIGSTOP);
    /* int tempo;
    srand((unsigned)time(NULL));
    int x = rand() % 10;
    if (x == 0)
        tempo = 15;
    else if (x > 0 && x <= 3)
        tempo = 5;
    else
        tempo = 1;

    printf("-- My time is %d.\n", tempo); */

    /* FILE *demanda = fopen("demanda.txt", "w+");
    fprintf(demanda, "%d", tempo);
    fclose(demanda);
     raise(SIGSTOP);
     sem_t *sem = sem_open("/sem_atend", O_RDWR);
     if (sem != SEM_FAILED)
         sem_wait(sem);
     usleep(tempo);
     if (sem != SEM_FAILED)
         sem_post(sem); */
    return 0;
}