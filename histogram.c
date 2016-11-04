/*********************************
/ Raul Vargas Navarro
/ histogram.c
/ losely based on https://computing.llnl.gov/tutorials/pthreads/#Mutexes
/  Args: number of random numbres, number of bins, number of threads
/ Number of random should be divisible by number of threads
**************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RANGE 100

int NUM_BINS;
pthread_mutex_t lock;
long long rand_per_thread;

struct bin{
  int lower;
  int upper;
  long long value;
} ;

struct bin *my_bin;

void *histogram(void *threadid)
{
  long tid;
  tid = (long)threadid;
  int i,j;
  int x;
  srand(time(NULL));  
  for(i=0;i<rand_per_thread;i++)
  {
    x=rand()%(RANGE);
    for (j=0;j<NUM_BINS;j++)
    {
      if(x >=my_bin[j].lower && x< my_bin[j].upper)
      { 
        pthread_mutex_lock(&lock);
        my_bin[j].value=my_bin[j].value+1;
        pthread_mutex_unlock(&lock);
        break;
      }
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
   pthread_t *threads;
   pthread_attr_t attr;
   long long NUM_THREADS;
   int rc;
   long t;
   void *status;
   int i; 
   int bin_width;
   long long test;
long long NUM_RAND;
   NUM_RAND=atoll(argv[1]);
   NUM_BINS=atoi(argv[2]);
   NUM_THREADS=atoll(argv[3]); 

   threads=malloc(NUM_THREADS*sizeof(pthread_t));
   my_bin=malloc(NUM_BINS*sizeof(struct bin));
   if (my_bin == NULL)
      printf("Problem with Malloc\n");
   bin_width=RANGE/NUM_BINS;
   rand_per_thread=NUM_RAND/NUM_THREADS;
      
   printf("Conditions: Number of randoms: %lld, Number of bins: %d, Number of threads: %lld, Range: %d, Bin Width: %d, Rand per thread: %lld\n", NUM_RAND,NUM_BINS,NUM_THREADS,RANGE,bin_width, rand_per_thread);

   for (i=0;i<NUM_BINS;i++)
   {
     my_bin[i].lower=i*bin_width;
     my_bin[i].upper=(i+1)*bin_width;
     my_bin[i].value=0;
   }
   

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


   tic=clock();
   for(t=0;t<NUM_THREADS;t++)
   {
     rc = pthread_create(&threads[t], &attr, histogram, (void *)t);
     if (rc)
     {
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
     }
   }
   pthread_attr_destroy(&attr);
   for(t=0; t<NUM_THREADS; t++)
   {
     pthread_join(threads[t], &status);
   }
   toc=clock();
   test=0;
   for (i=0;i<NUM_BINS;i++)
   {
     printf("bin (%d,%d) = %lld\n",my_bin[i].lower,my_bin[i].upper,my_bin[i].value);
     test=test+my_bin[i].value;
   }
   printf("Total random number processed = %lld \n",test);
   printf("Time = %lf \n",(double)(toc-tic)/(double)CLOCKS_PER_SEC);
   pthread_mutex_destroy(&lock);
   pthread_exit(NULL);
  free (threads);
  free (my_bin);
}
