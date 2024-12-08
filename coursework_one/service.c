#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <signal.h>

sem_t *sem_atend, *sem_block;

// attendant thread function
void *attendant(void *args)
{
}

// front desk thread struct argument

typedef struct
{
    pid_t clientPid;
    time_t createdAt;
    int timeDemand;
    int priority;
} clientProcessQueueInfo;

typedef struct
{
    int numberOfClientsToBeCreated;
    int patienceOfEachClient;
    clientProcessQueueInfo *clientQueue;
} frontdeskArguments;

// front desk thread function
void *frontdesk(void *args)
{
    srand(time(NULL));
    printf("++ FRONT DESK THREAD.\n");
    frontdeskArguments *structArgs = (frontdeskArguments *)args;
    printf("++ Number of clients to be created = %d.\n", structArgs->numberOfClientsToBeCreated);
    printf("++ Patience of each client         = %d.\n", structArgs->patienceOfEachClient);
    pid_t iPid;
    int keepRunningWhile = 1;
    int i = 0;
    char *argv2[] = {"client2", NULL};
    char *path = "./client2";
    char *env[] = {NULL};
    FILE *clientsDemandFile;
    int clientDemandTime;
    // clientProcessQueueInfo *clientQueue = (clientProcessQueueInfo *)malloc(structArgs->numberOfClientsToBeCreated * sizeof(clientProcessQueueInfo));
    if (structArgs->clientQueue == NULL)
    {
        printf("++ ERROR WITH THE POINTER OF THE CLIENT QUEUE.\n");
        printf("++ EXITING THE PROGRAM...\n");
        exit(1);
    }
    printf("++ Going to create the semaphores.\n");
    sem_unlink("/sem_atend");
    sem_unlink("/sem_block");
    sem_atend = sem_open("/sem_atend", O_CREAT, 0644, 1);
    sem_block = sem_open("/sem_block", O_CREAT, 0644, 1);
    if (sem_atend == SEM_FAILED || sem_block == SEM_FAILED)
    {
        printf("++ SOMETHING WENT WRONG WHEN CREATING SOME OF THE SEMAPHORES.\n");
        printf("++ Trying to unlink the semaphores.\n");
        sem_unlink("/sem_atend");
        sem_unlink("/sem_block");
        printf("++ Exiting the program...\n");
        exit(1);
    }
    while (keepRunningWhile == 1 && i < structArgs->numberOfClientsToBeCreated)
    {
        printf("The value of i is %d.\n", i);
        iPid = fork();
        if (iPid == 0)
        {
            // execve(path, argv2, env);
            if (execve(path, argv2, env) == -1)
            {
                perror("== Error in creating client2");
                exit(1);
            }
        }

        if (iPid == -1)
        {
            perror("== Fail to create the fork fork.\n");
            printf("== Exiting the program...\n");
            exit(1);
        }
        structArgs->clientQueue[i].clientPid = iPid;
        structArgs->clientQueue[i].createdAt = time(NULL);
        structArgs->clientQueue[i].priority = (rand() % 2) ? 1 : 0;
        int status;
        waitpid(iPid, &status, WUNTRACED);
        if (WIFSTOPPED(status))
        {
            clientsDemandFile = fopen("demanda.txt", "r");
            fscanf(clientsDemandFile, "%d\n", &clientDemandTime);
            structArgs->clientQueue[i].timeDemand = clientDemandTime;
            fclose(clientsDemandFile);
            kill(iPid, SIGCONT);
        }
        i += 1;
    }

    // free(clientQueue);
}

int main(int argc, char *argv[])
{
    printf("== %d arguments received.\n", argc);
    if (argc != 3)
    {
        printf("== NUMBER OF ARGUMENTS IS INCORRECT.\n");
        printf("== THE NUMBER OF ARGUMENTS EXPECTED IS 3.\n");
        printf("==  = 1. THE NAME OF THE PROGRAM (service - it's already correct).\n");
        printf("==  = 2. THE NUMBER OF CLIENTS TO BE CREATED.\n");
        printf("==  = 3. THE PATIENCE OF EACH CLIENT.\n");
        printf("== PLEASE TRY AGAIN.\n");
        exit(1);
    }
    int numberOfClientsToBeCreated = atoi(argv[1]);
    int patienceOfEachClient = atoi(argv[2]);
    printf("== Number of clients to be created = %d \n", numberOfClientsToBeCreated);
    printf("== Patience of each client         = %d \n", patienceOfEachClient);
    printf("== GOIN TO CREATE THE FRONT DESK THREAD.\n");
    clientProcessQueueInfo *clientQueue = (clientProcessQueueInfo *)malloc(numberOfClientsToBeCreated * sizeof(clientProcessQueueInfo));

    // creating the front desk thread
    pthread_t frontDeskThread;
    // preparing the front desk thread arguments
    frontdeskArguments *frontDeskThreadArguments = malloc(sizeof(frontdeskArguments));
    frontDeskThreadArguments->numberOfClientsToBeCreated = numberOfClientsToBeCreated;
    frontDeskThreadArguments->patienceOfEachClient = patienceOfEachClient;
    frontDeskThreadArguments->clientQueue = clientQueue;
    // verifying if there is some error when try to create the front desk thread
    if (pthread_create(&frontDeskThread, NULL, frontdesk, (void *)frontDeskThreadArguments) != 0)
    {
        printf("== SOMETHING WENT WRONG IN CREATION OF FRONT DESK THREAD.\n");
        printf("== PLEASE TRYING AGAIN LATER.\n");
        printf("== EXITING THE PROGRAM...\n");
        exit(1);
    }

    pthread_join(frontDeskThread, NULL);

    for (int i = 0; i < numberOfClientsToBeCreated; ++i)
    {
        printf("== Child of position %d.\n", i);
        printf("== = Have the PID of %d.\n", clientQueue[i].clientPid);
        printf("== = Created at %ld.\n", clientQueue[i].createdAt);
        printf("== = Client demand %d.\n", clientQueue[i].timeDemand);
        printf("== = Priority is %d.\n", clientQueue[i].priority);
        printf("== ++\n");
    }

    // calling the front desk thread
    pthread_join(frontDeskThread, NULL);
    free(frontDeskThreadArguments);
    sem_unlink("/sem_atend");
    sem_unlink("/sem_block");
    free(clientQueue);
    return 0;
}