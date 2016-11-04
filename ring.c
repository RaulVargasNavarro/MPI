 /*MPI ring message passing program
 * takes a single command line option: the maximum message
 * size in number of bytes
 * the program converts the number of bytes you specify
 * into numbers of doubles based on the byte size of a
 * double on that system. Then it starts with a message
 * of one double and scales by 2 until it reaches that
 * number, spitting out timing all along the way
 */

#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>

/* if you want a larger number of runs to be averaged
#define ITERATIONS 1000
** together, increase INTERATIONS */
#define WARMUP 8

int main(int argc, char **argv)
{

   int i, j, rank, size, tag=96,bytesize, dblsize;
   int max_msg, min_msg, packetsize;
   int iterations;

   double *mess;
   double tend, tstart, tadd, bandwidth;
   MPI_Status status;

   MPI_Init(&argc, &argv);

   MPI_Comm_size(MPI_COMM_WORLD,&size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   /* get the message size from the command line */
   if(rank == 0)
   {
      printf("argcnt= %d\n",argc);
      dblsize = sizeof(double);

      if( argc >= 2 )
         max_msg = atoi(argv[1]);
      else
         max_msg = 4096;

      if( argc >= 3 )
         min_msg = atoi(argv[2]);
      else
         min_msg = 0;

     if( argc >= 4 )
         iterations = atoi(argv[3]);
      else
         iterations = 10;

     printf("ring size is %i nodes\n", size);
      printf("max message specified= %i\n", max_msg);
      printf("min message specified= %i\n", min_msg);
      printf("iterations =           %i\n", iterations);
      bytesize = max_msg;
      printf("double size is %i bytes\n", dblsize);
      max_msg = max_msg/dblsize;
      if(max_msg <= 0) max_msg = 1;
      printf("#of doubles being sent is %i\n", max_msg);

      printf("PacketLength\tBandwidth\tPacketTime\n");
      printf(" (MBytes)   \t (B/sec) \t(sec)\n");
      printf("------------ -------------- --------------\n");
   }

   /* pass out the size to the kids */
   MPI_Bcast(&max_msg, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&min_msg, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);

   /* make the room for the largest sized message */
   mess = (double*)malloc(max_msg * (sizeof(double)));
   if(mess == NULL)
   {
      printf("malloc prob, exiting\n");
      MPI_Finalize();
   }

   /* warmup lap */
   for(packetsize = 0; packetsize < WARMUP; packetsize++)
   {
      /* head node special case */
      if(rank == 0)
      {
         MPI_Send(mess, max_msg, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD);
         MPI_Recv(mess, max_msg, MPI_DOUBLE, size-1,tag,
         MPI_COMM_WORLD, &status);
      }
      /* general case */
      if((rank != 0) && (rank != (size-1)))
      {
         MPI_Recv(mess, max_msg, MPI_DOUBLE, rank-1,tag,
         MPI_COMM_WORLD, &status);
         MPI_Send(mess, max_msg, MPI_DOUBLE, rank +1,tag,
         MPI_COMM_WORLD);
      }
      /* end node case */
      if(rank == size-1)
      {
         MPI_Recv(mess, max_msg, MPI_DOUBLE, rank-1,tag,
         MPI_COMM_WORLD, &status);
         MPI_Send(mess, max_msg, MPI_DOUBLE, 0,tag, MPI_COMM_WORLD);
      }
   }
   /* end warmup lap */
   /*
   if(rank == 0)
   printf("warmup lap done\n");
   */

   /* real timed stuff now */
   for(packetsize = min_msg; packetsize <= max_msg; packetsize*=2)
   {
      if(rank == 0)
         printf("Starting packetsize: %i\n",packetsize);
      /* init timing variables */
      tadd = 0.0;
      tend = 0.0;
      tstart = 0.0;

      for(j = 0; j < iterations; j++)
      {
         MPI_Barrier(MPI_COMM_WORLD);
         if(rank == 0)
         {
            tstart = MPI_Wtime(); /* timing call */
            MPI_Send(mess, packetsize, MPI_DOUBLE, 1, tag,
            MPI_COMM_WORLD);
            MPI_Recv(mess, packetsize, MPI_DOUBLE, size-1,tag,
            MPI_COMM_WORLD, &status);

            tend = MPI_Wtime();
            tadd += (tend - tstart);
         if( j%20 == 0 )
            printf("deltaT[%i]= %i\n",j,tend-tstart);
         }
         /* general case */
         if((rank != 0) && (rank != (size-1)))
         {
            MPI_Recv(mess, packetsize, MPI_DOUBLE, rank-1,tag,
            MPI_COMM_WORLD, &status);
            MPI_Send(mess, packetsize, MPI_DOUBLE, rank +1,tag,
            MPI_COMM_WORLD);
         }
         /* end node case */
         if(rank == size-1)
         {
            MPI_Recv(mess, packetsize, MPI_DOUBLE, rank-1,tag,
            MPI_COMM_WORLD, &status);
            MPI_Send(mess, packetsize, MPI_DOUBLE, 0,tag,
            MPI_COMM_WORLD);
         }

      }
      /* calc and print out the results */
      if(rank == 0)
      {
         bandwidth = ((size * packetsize *dblsize)/
                                (tadd/(double)iterations));
         printf("RESULTS: %16.12lf \t%20.8lf \t%16.14lf \n",
                 (double)(packetsize * dblsize)/1048576.0,
                 bandwidth,
                 tadd/(double)iterations);
      }
      /* to make it possible to do a 0 size message */
      if (packetsize == 0) packetsize = 1;

  }
  /* end real timed stuff */

   if( rank == 0 ) printf("\nRing Test Complete\n\n");
  MPI_Finalize();
  exit(0);


} /* end ring.c */
