/* 
 * ring code source: https://gist.github.com/perdacherMartin
 *
 * Modified by M.Thomas, Feb 2014 to use variable number of
 *    repetitions, renamed iters
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
 
#define MASTER 0
#define REPETITIONS 10
#define T_mpisendtype MPI_DOUBLE
 
typedef double T_arraytype;
 
void InitArray(T_arraytype array[], long size, T_arraytype value );
void Sum(T_arraytype arrayA[], T_arraytype arrayB[], long size);
 
int main(int argc, char *argv[]){
	int myrank, nprocs, leftid, rightid, val, tmp,i;
	long n;
	//double times[REPETITIONS];
	double* times;
	double time,slowest;
	int iters;
  	MPI_Status recv_status;
  	MPI_Init(&argc, &argv);
  	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);  
  	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  
	if ( argc != 3 ){
    	printf("Usage: %s  n iters \n\n",argv[0]);
		return 1;
    }
 
	n = atol(argv[1]);
	iters = atol(argv[2]);

        times = (double*) malloc(iters*sizeof(double));
 
	T_arraytype a[n];
	T_arraytype b[n];
	T_arraytype recv[n];
	InitArray(a,n,myrank);
	
	// for ( i = 0 ; i < REPETITIONS ; ++i ){
	for ( i = 0 ; i < iters ; ++i ){
		MPI_Barrier(MPI_COMM_WORLD);
		
		InitArray(b,n,0);
		
		time = MPI_Wtime();
		
		leftid = myrank - 1 < 0 ? nprocs - 1 : myrank -1 ;
		rightid = myrank + 1 >= nprocs ? 0 : myrank +1 ;
 
		// send to the left and receive from the righ
		MPI_Sendrecv(&a,n,T_mpisendtype,leftid,0,&recv,n,T_mpisendtype,rightid,0,MPI_COMM_WORLD, &recv_status);
		Sum(recv, b, n);
 
		// send to the right and receive from the left
		MPI_Sendrecv(&a,n,T_mpisendtype,rightid,0,&recv,n,T_mpisendtype,leftid,0,MPI_COMM_WORLD, &recv_status);
		Sum(recv, b, n);
		Sum(a, b, n);
		
		time = MPI_Wtime() - time;
		MPI_Barrier(MPI_COMM_WORLD); 
		// take the slowest time of all processes
		MPI_Reduce(&time, &slowest, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);
		
		if (myrank == MASTER) {
	    	times[i]=slowest;
	   	}
	}
	
	if ( myrank == MASTER ){
		double min = times[0];
		// take the best run of the REPITITIONS-runs
	   	//for (int i = 1; i < REPETITIONS ; i++) {
	   	for (i = 1; i < iters ; i++) {
			min = ( times[i] < min ) ? times[i] : min;
			printf( "np %d time %d is %f \n", nprocs,i,times[i]);
	   	}
	   	
//		printf("%ld :%f\n", n, min);
	}
 
  	MPI_Finalize();		
	return 0;
}
 
 
void InitArray(T_arraytype array[], long size, T_arraytype value ){
	long i;
	
	// initialisation of the array
	for ( i=0l ; i < size ; ++i ){
		array[i] = value;
	}
}
 
// elementwise sum of A + B and store it in B
void Sum(T_arraytype arrayA[], T_arraytype arrayB[], long size){
	long i;
	
	// calculate elmentwise sum
	for ( i=0l ; i < size ; ++i ){
		arrayB[i] = arrayB[i] + arrayA[i];
	}
}
