/*********************************
/ Raul Vargas Navarro
/ trapezoid.c
/ losely based on https://computing.llnl.gov/tutorials/pthreads/#Mutexes
/  Args: number of random numbres, number of bins, number of threads
/ Number of random should be divisible by number of threads
**************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define X0 5
#define XF 6

int N;
pthread_mutex_t lock;
int number_per_thread;

struct slice{
  double start;
  double end;
  double value;
} ;

struct slice *my_slice;

double f( double x)
{
  return 2*x;
}
void *trapezoid(void *threadid)
{
  long tid;
  tid = (long)threadid;
  int i,j;
  double x0,xf,x,value;
  double h;
  x0=my_slice[tid].start;
  xf=my_slice[tid].end;
  h=(xf-x0)/number_per_thread;
  for(x=x0;x<=xf;x+=h)
  {
     value= h*(f(x)+f(x+h))/2.0;
        my_slice[tid].value=my_slice[tid].value+value;
   }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
   pthread_t *threads;
   pthread_attr_t attr;
   int  NUM_THREADS;
   int number_per_threads;
   int rc;
   long t;
   void *status;
   int i; 
   N=atoi(argv[1]);
   NUM_THREADS=atoi(argv[2]); 
  double test;
   threads=malloc(NUM_THREADS*sizeof(pthread_t));
   my_slice=malloc(NUM_THREADS*sizeof(struct slice));
   if (my_slice == NULL)
      printf("Problem with Malloc\n");
   number_per_thread=N/NUM_THREADS;
 
   printf("Conditions: N: %d, , Number of threads: %d, Number per thread: %d\n", N,NUM_THREADS,number_per_thread);

   for (i=0;i<NUM_THREADS;i++)
   {
     my_slice[i].start=X0+(double)i*(XF-X0)/(double)N*number_per_thread;
     my_slice[i].end=X0+(double)(i+1)*(XF-X0)/(double)N*number_per_thread;
     my_slice[i].value=0;
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
     rc = pthread_create(&threads[t], &attr, trapezoid, (void *)t);
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
   for (i=0;i<NUM_THREADS;i++)
   {
//     printf("bin (%lf,%lf) = %lf\n",my_slice[i].start,my_slice[i].end,my_slice[i].value);
     test=test+my_slice[i].value;
   }
   printf("Time = %lf \n",(double)(toc-tic)/(double)CLOCKS_PER_SEC);
   printf("Integral value = %lf \n",test);
   pthread_mutex_destroy(&lock);
   pthread_exit(NULL);
  free (threads);
  free (my_slice);
}
