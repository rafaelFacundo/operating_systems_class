#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* 
    Threads are basically a execution line of the our program, or 
    in other words a execution line of our process (that is the 
    running program) 

    threads operations include thread creation, termination,
    synchronization(joins, blocking), scheduling, data 
    management and process interaction

    a thread does not maintain a list of created threads, nor 
    does it know the thread that created it

    all threads within a process share the same address space

    threads in same process share
        process instructions
        most data
        open files(descriptors)
        signals and signal handlers
        current working directory
        user and group id
    each thread has a unique
        thread id
        set of registers, stack pointer
        stack for local variables, return addresses
        signal mask
        priority
        return value: errno
    pthread function return "0" if OK
*/

void *print_message_function(void *ptr)
{
    char *message;
    message = (char *)ptr;
    printf("%s \n", message);
}


int main()
{
    pthread_t thread1, thread2;
    char *message1 = "Thread 1";
    char *message2 = "Thread 2";
    int iret1, iret2;

    iret1 = pthread_create(&thread1, NULL, print_message_function, (void *) message1);
    iret2 = pthread_create(&thread2, NULL, print_message_function, (void *) message2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Thread 1 returns: %d\n", iret1);
    printf("Thread 2 returns: %d\n", iret2);

    return 0;
}

/* 
    In this example the same function is used in each thread.
    The arguments are different. The function need not to be the
    same.

    Threads terminate by explicitly calling "pthread_exit", by
    letting the function return, or by a cal to the function
    exit which will terminate the process including any threads

    function call: pthread_create
    int pthread_create(pthread_t * thread, 
                    const pthread_attr_t * attr,
                    void * (*start_routine)(void *), 
                    void *arg);

    thread --> returns the thread id 
    attr --> Set to NULL if default thread attributes are used. 
    (else define members of the struct pthread_attr_t defined in 
    bits/pthreadtypes.h) Attributes include:
        detached state (joinable? Default: PTHREAD_CREATE_JOINABLE.
        Other option: PTHREAD_CREATE_DETACHED)
        scheduling policy (real-time? PTHREAD_INHERIT_SCHED,
        PTHREAD_EXPLICIT_SCHED,SCHED_OTHER)
        scheduling parameter
        inheritsched attribute (Default: PTHREAD_EXPLICIT_SCHED 
        Inherit from parent thread: PTHREAD_INHERIT_SCHED)
        scope (Kernel threads: PTHREAD_SCOPE_SYSTEM User
        threads: PTHREAD_SCOPE_PROCESS Pick one or the other not 
        both.)
        guard size
        stack address (See unistd.h and bits/posix_opt.h 
        _POSIX_THREAD_ATTR_STACKADDR)
        stack size (default minimum PTHREAD_STACK_SIZE set in 
        pthread.h), 
    void * (*start_routine) - pointer to the function to be 
    threaded. Function has a single argument: pointer to void. 
    *arg - pointer to argument of function. To pass multiple 
    arguments, send a pointer to a structure.

    

*/