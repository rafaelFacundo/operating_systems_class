#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

int main()
{
    sem_t *sem = sem_open("/sem_atend", O_RDWR);
    return 0;
}