#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RADIUS 1

long long MAX_LOOP;
long long counter=0;
long long trials_per_thread;
pthread_mutex_t lock;

void *Calculate_pi(void *threadid)
{
   long tid;
   tid = (long)threadid;
   double x,y;
   int i;
   srand(time(NULL));

   for(i=0;i<trials_per_thread;i++)
   {
     x= ((double)rand())/RAND_MAX;
     y= ((double)rand())/RAND_MAX;
//     printf("%ld %lf %lf %lf %lld\n", tid,x,y,x*x+y*y,counter);
     if( (x*x+y*y) <= RADIUS*RADIUS)
     { 
       pthread_mutex_lock(&lock);
       counter=counter+1;
       pthread_mutex_unlock(&lock);
     }
   }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
   pthread_t *threads;
   pthread_attr_t attr;
   int rc;
   long t;
   MAX_LOOP=atoll(argv[1]);
   long long NUM_THREADS;
   NUM_THREADS=atoll(argv[2]); 
   void *status;
   threads=malloc(NUM_THREADS*sizeof(pthread_t));
   clock_t tic,toc;

   if (threads == NULL)
   {
     printf("Problem with malloc...\n");
     return 1;
   }

  if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   trials_per_thread=MAX_LOOP/NUM_THREADS;
//   trials_per_thread=MAX_LOOP;

   printf("Conditions:\n%lld threads, %lld trials, %lld trials per thread\n",NUM_THREADS,MAX_LOOP,trials_per_thread);

   tic=clock();
   for(t=0;t<NUM_THREADS;t++)
   {
     rc = pthread_create(&threads[t], &attr, Calculate_pi, (void *)t);
     if (rc)
     {
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
     }
   }
   pthread_attr_destroy(&attr);
          /* Wait on the other threads */
	for(t=0; t<NUM_THREADS; t++)
        {
	  pthread_join(threads[t], &status);
	}
    toc=clock();
    printf("Time in seconds : %lf\n",(double)(toc-tic)/(double)CLOCKS_PER_SEC);
    printf("Counter is %lld so pi = %lf \n", counter, 4.0*((double)counter)/((double)MAX_LOOP));
   pthread_mutex_destroy(&lock);
   /* Last thing that main() should do */
   pthread_exit(NULL);
  free (threads);
}
