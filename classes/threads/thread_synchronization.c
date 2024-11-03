#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* 
    the threads library provides three synchronization
    mechanisms:
    mutexes --> Mutual exclusion lock - block access to variables
    by other threads. this enforces exclusive access by a thread
    to a variable or set of variables
    joins - make a thread wait till others are complete
    condition variables - data type pthread_cond_t
*/

/* 
    Mutexes
    Mutexes are used to prevent data inconsistencies due to race
    conditions. A race condition often occurs when two or more
    threads need to perform operations on the same memory area,
    but the results of computations depends on the orders in which
    these operations are performed.
    Mutexes are used for serializing shared resources. Anytime a
    global resource is accessed by more the one thread the resource
    should have a Mutex associated with it. One can apply a mutex
    to protect a segment of memory ("critical region") from other 
    threads. Mutexes can be applied only to threads in a single 
    process and do not work between processes as do semaphores
*/

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void *functionC()
{
    pthread_mutex_lock( &mutex1);
    counter++;
    printf("Counter value: %d\n",counter);
    pthread_mutex_unlock(&mutex1);
}

/*
    Join

    A join is performed when one wants to wait for a thread to 
    finish. A thread calling routine may launch multiple threads 
    then wait for them to finish to get the results. One wait for 
    the completion of the threads with a join. 
*/

#define NTHREADS 10
void *thread_function(void *dummyPtr)
{
   printf("Thread number %ld\n", pthread_self());
   pthread_mutex_lock( &mutex1 );
   counter++;
   pthread_mutex_unlock( &mutex1 );
}

/*
    Condition variables 

    A condition variable is a variable of type pthread_cond_t 
    and is used with the appropriate functions for waiting and 
    later, process continuation. The condition variable mechanism 
    allows threads to suspend execution and relinquish the 
    processor until some condition is true. A condition variable 
    must always be associated with a mutex to avoid a race 
    condition created by one thread preparing to wait and another 
    thread which may signal the condition before the first thread 
    actually waits on it resulting in a deadlock. The thread will 
    be perpetually waiting for a signal that is never sent. Any 
    mutex can be used, there is no explicit link between the mutex 
    and the condition variable. 

     Functions used in conjunction with the condition variable:

    Creating/Destroying:
        pthread_cond_init
        pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
        pthread_cond_destroy
    Waiting on condition:
        pthread_cond_wait
        pthread_cond_timedwait - place limit on how long it 
        will block.
    Waking thread based on condition:
        pthread_cond_signal
        pthread_cond_broadcast - wake up all threads blocked 
        by the specified condition variable.

*/

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;


int  count = 0;
#define COUNT_DONE  10
#define COUNT_HALT1  3
#define COUNT_HALT2  6

void *functionCount1()
{
   for(;;)
   {
      pthread_mutex_lock( &condition_mutex );
      while( count >= COUNT_HALT1 && count <= COUNT_HALT2 )
      {
         pthread_cond_wait( &condition_cond, &condition_mutex );
      }
      pthread_mutex_unlock( &condition_mutex );

      pthread_mutex_lock( &count_mutex );
      count++;
      printf("Counter value functionCount1: %d\n",count);
      pthread_mutex_unlock( &count_mutex );

      if(count >= COUNT_DONE) return(NULL);
    }
}

void *functionCount2()
{
    for(;;)
    {
       pthread_mutex_lock( &condition_mutex );
       if( count < COUNT_HALT1 || count > COUNT_HALT2 )
       {
          pthread_cond_signal( &condition_cond );
       }
       pthread_mutex_unlock( &condition_mutex );

       pthread_mutex_lock( &count_mutex );
       count++;
       printf("Counter value functionCount2: %d\n",count);
       pthread_mutex_unlock( &count_mutex );

       if(count >= COUNT_DONE) return(NULL);
    }

}

int main()
{
    /* 
    MUTEX
    int rc1, rc2;
    pthread_t thread1, thread2;

    if( (rc1=pthread_create( &thread1, NULL, &functionC, NULL)) )
    {
        printf("Thread creation failed: %d\n", rc1);
    }

    if( (rc2=pthread_create( &thread2, NULL, &functionC, NULL)) )
    {
        printf("Thread creation failed: %d\n", rc2);
    }



    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);  */

    /* 
    JOIN 
    pthread_t thread_id[NTHREADS];
   int i, j;

   for(i=0; i < NTHREADS; i++)
   {
      pthread_create( &thread_id[i], NULL, thread_function, NULL );
   }

   for(j=0; j < NTHREADS; j++)
   {
      pthread_join( thread_id[j], NULL); 
   }
                                                

   printf("Final counter value: %d\n", counter);
   */

  pthread_t thread1, thread2;

   pthread_create( &thread1, NULL, &functionCount1, NULL);
   pthread_create( &thread2, NULL, &functionCount2, NULL);
   pthread_join( thread1, NULL);
   pthread_join( thread2, NULL);

    return 0;
}