/*************************
   Raul Vargas
   COMP605
  Adapted from the examples in tuckoo
  from FORTRAN to C
*********/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

int main( int argc, char** argv )
{
    int my_rank, p;
    double **A, *X, *Y;
    int ni,nj;
    int i;
    int wide, my_start, my_end;
    double t1,t2,tstart,tend;
 
    void loop1(double **A, double *X, double *Y, int ni, int nj,int my_start,int my_end);
    void loop2(double **A, double *X, double *Y, int ni, int nj,int my_start,int my_end);
    void loop3(double **A, double *X, double *Y, int ni, int nj,int my_start,int my_end);
    void initialize(double **A, double *X, double *Y, int ni, int nj);
    void print_matrix(double **A,  int ni, int nj);
    void print_vector( double *X, int nj);
    void print_usage(char *name);
    void print_my_domain(int rank, int p, int ni, int nj);

    if (argc == 3)
    {
      ni=atoi(argv[1]);
      nj=atoi(argv[2]);
    }
    else
    {
      print_usage(argv[0]);
      return 1;
    }

/*********************
   Dynamic allocation of matrix and vectors
                       *******************/
    X=malloc(nj*sizeof(double));
    Y=malloc(nj*sizeof(double));
    A=malloc(ni*sizeof(X));
    for(i=0;i<ni;i++)
    {
      A[i]=malloc(nj*sizeof(double));
    }
// Initialize values for A, X and Y

    initialize(A,X,Y,ni,nj);

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &p );
    MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );

    wide=nj/p;  //Check for integer
    my_start=my_rank*wide;
    my_end=my_start+wide-1;

    printf( "process %d ",  my_rank );
    
//    printf("with domain from %d to %d\n", my_start, my_end);
    t1=MPI_Wtime();
    tstart=t1;
    loop1(A,X,Y,ni,nj,my_start,my_end);
    t2=MPI_Wtime();
    printf("Process %d time loop1: %lf\n",my_rank,t2-t1);
    
    t1=MPI_Wtime();
    loop2(A,X,Y,ni,nj,my_start,my_end);
    t2=MPI_Wtime();
    printf("Process %d time loop2: %lf\n",my_rank,t2-t1);
    
    t1=MPI_Wtime();
    loop3(A,X,Y,ni,nj,my_start,my_end);
    t2=MPI_Wtime();
    printf("Process %d time loop3: %lf\n",my_rank,t2-t1);
    tend=t2;
    
    printf("Process %d total time: %lf\n",my_rank,tstart-tend);
    MPI_Finalize();
    free (X);
    free (Y);
    free (A);
    return 0;
}
void initialize(double **A, double *X, double *Y, int ni, int nj)
{
  int i,j;

  for (j=0;j<nj;j++)
  {
     X[j]=sqrt(2.0);
     Y[j]=0; 
     for (i=0;i<ni;i++)
     {
       A[i][j]=sqrt(2.0);
     }
  } 
}   
void print_vector( double *X, int nj)
{
   int j;
   for (j=0;j<nj;j++)
   {
     printf("%lf ",X[j]);
   }
   printf("\n");
}
void print_matrix(double **A,  int ni, int nj)
{
   int i,j;
   for(i=0;i<ni;i++)
   {
     print_vector(A[i],nj);
   }
}
void loop1(double **A, double *X, double *Y, int ni, int nj, int my_start, int my_end)
{
   int i,j;
   for(i=0;i<ni;i++)
     for(j=my_start;j<=my_end;j++)
        Y[j]=A[i][j]*X[j];

}

void loop2(double **A, double *X, double *Y, int ni, int nj, int my_start, int my_end)
{
   int i,j;
   for(j=my_start;j<=my_end;j++)
      for(i=0;i<ni;i++)
        Y[j]=A[i][j]*X[j];

}
void loop3(double **A, double *X, double *Y, int ni, int nj, int my_start, int my_end)
{
   int i,j;
   for(j=my_start;j<=my_end;j++)
      for(i=0;i<ni;i++)
        Y[j]=A[i][j]*sqrt(X[j]);

}

void print_usage(char *myname)
{
   printf("\n Usage: %s rows colums \n",myname);
}

void print_my_domain(int rank, int p, int ni, int nj)
{
   int wide;
   int start;
   int end;

   wide=nj/p;
   start=rank*wide;
   end=start+wide-1;
   printf("%d %d\n", start, end);
}
