// Raul Vargas Navarro
// Modified ping-pong.c
// Based on the code provided for COMP605
// SDSU, Spring 2014
//
/* ping_pong.c -- two-process ping-pong -- send from 0 to 1 and send back
 *     from 1 to 0
 *
 * Input: minimun size, maximum size, increment
 * Output: time elapsed for each ping-pong
 *
 *
 * See Chap 12, pp. 267 & ff. in PPMPI.
 */

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

main(int argc, char* argv[]) {
    int    p;
    int    my_rank;
    double  *x;
    int    size;
    int    pass;
    MPI_Status  status;
    int         i;
    double      wtime_overhead;
    double      start, finish;
    double      raw_time;
    MPI_Comm    comm;
    int min,max;
    int increment; 
    int rep;
    if (argc <4)
    {
      printf("Usage: %s min max incr \n",argv[0]);
      exit(1);
    }
    else
    {
      min=atoi(argv[1]);
      max=atoi(argv[2]);
      increment=atoi(argv[3]);
    }

    printf("***************************double************************\n");

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);
    x=malloc((max-min)*sizeof(double));
    if(x ==NULL)
    { 
      printf("Problem allocating memory!\n");
      exit(1);
    }
    wtime_overhead = 0.0;
    for (i = 0; i < 100; i++) {
        start = MPI_Wtime();
        finish = MPI_Wtime();
        wtime_overhead = wtime_overhead + (start - finish);
    }
    wtime_overhead = wtime_overhead/100.0;

    if (my_rank == 0) {
        for ( size=min;size<=max;size =size+ increment)
        {
                MPI_Barrier(comm);
                start = MPI_Wtime();
                MPI_Send(x, size, MPI_DOUBLE, 1, 0, comm);
                MPI_Recv(x, size, MPI_DOUBLE, 1, 0, comm,
                    &status);
                finish = MPI_Wtime();
                raw_time = finish - start - wtime_overhead;
                printf("%f\n", raw_time);
        }
    } else { /* my_rank == 1 */
        for (  size = min;size<=max;size =size+ increment) {
		MPI_Barrier(comm); 
                MPI_Recv(x, size, MPI_DOUBLE, 0, 0, comm,
		    &status); 
                MPI_Send(x, size, MPI_DOUBLE, 0, 0, comm);
        } 
    }

    free(x);
    MPI_Finalize();
}  /* main */
