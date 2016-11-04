/*
 * File:     mpi_calc_pi.c
 *
 * Purpose:  Estimate pi using a monte carlo method
 *  
 * Compile:  mpicc -g -Wall -o mpi_calc_pi 
 *                 mpi_calc_pi.c
 * Run, using MPICH:      
 * 	     mpiexec -n <comm_sz> ./mpi_calc_pi
 * Run using batchscript - see batch.calcpi
 *
 * Input:    Number of "dart tosses"
 * Output:   Estimate of pi.  If DEBUG flag is set, results of processes'
 *           tosses.
 *
 * Created by Mary Thomas (mthomas@mail.sdsu.edu)
 *       Feb, 2014
 *       Based on solutions from Pachecho, 2011
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void Get_input(long long int* ndarts, int argc, char* argv[],
      int my_rank, MPI_Comm comm);
void Usage( int  argc, char prog_name[]);
long long int Est_pi(long long ndarts, 
      int my_rank);

/*-------------------------------------------------------------------*/
int main(int argc, char* argv[]) {

   long long int ndarts;
   long long int loc_ndarts;
   long long int hits;
   long long int loc_hits;
   
   double pi_estimate;
   int my_rank, comm_sz;
   MPI_Comm comm;
   double tend,tstart;
   
   MPI_Init(NULL, NULL);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &comm_sz);
   MPI_Comm_rank(comm, &my_rank);
   tstart=MPI_Wtime();   
   Get_input(&ndarts, argc, argv, my_rank, comm);
   loc_ndarts = ndarts/comm_sz;

#  ifdef DEBUG
   printf("Proc %d > number of tosses = %lld, local number = %lld\n",
         my_rank, ndarts, loc_ndarts);
#  endif

   loc_hits = Est_pi(loc_ndarts, my_rank);
   
   MPI_Reduce(&loc_hits, &hits, 1, 
         MPI_LONG_LONG, MPI_SUM, 0, comm);
   
   if ( my_rank == 0 ){
      pi_estimate = 4*hits/((double)ndarts);
      printf("pi estimate = %f\n", pi_estimate);
   }
   tend=MPI_Wtime();
   MPI_Finalize();
   printf("Elapsed time: %f\n",tend-tstart);
   return 0;
}  /* main */


/*-------------------------------------------------------------------*/
void Get_input(
      long long int* ndarts  /* out */,
      int argc                         /* in  */, 
      char* argv[]                     /* in  */,
      int my_rank                      /* in  */, 
      MPI_Comm comm                    /* in  */) {
     
   if (my_rank == 0) {
      if (argc != 2) Usage(argc, argv[0]);
      *ndarts = strtoll(argv[1], NULL, 10);
   }
   
   MPI_Bcast(ndarts, 1, MPI_LONG_LONG, 0, comm);
}  /* Get_input */

/*---------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message showing how to run program and quit
 * In arg:    prog_name:  the name of the program from the command line
 */
void Usage(
      int  argc        /* in  */,
      char prog_name[] /* in */) {

   fprintf(stderr, "usage: %s ", prog_name);
   fprintf(stderr, "<ndarts>\n");
   fprintf(stderr, "arg count = %d \n", argc);
   exit(0);
}  /* Usage */

/*-------------------------------------------------------------------*/
long long int Est_pi(long long loc_ndarts, int my_rank) {
   long long int i;
   double x,y;
   double distance_squared;
   long long int hits = 0;
      
   srandom(my_rank+1);
   for ( i=0 ; i< loc_ndarts ; i++) {
      x =  2*random()/((double)RAND_MAX) - 1.0;
      y =  2*random()/((double)RAND_MAX) - 1.0;
      
      distance_squared = x*x + y*y;
#     ifdef DEBUG
      printf("Proc %d > x = %f, y = %f, dist squared = %f\n",
            my_rank, x, y, distance_squared);
#     endif
      if (distance_squared <= 1) {
         hits++;
      }
   }
   
   return hits;
}  /* Est_pi */

