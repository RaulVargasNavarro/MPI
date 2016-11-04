// Raul Vargas Navarro
// COMP 605, SDSU Spring 2014
// Created from the source from Pacheco's
// Needs 5 arguments:
// 
//  m = number of rows (clients)
//  n = number of columns (types of accounts)
//  datafile = File containing the m x n matrix with initial
//  balances
//  interest = file containing the n-sized vector of interests.
//  outputfile = file to write the results.
//
//  
/* parallel_mat_vect.c -- computes a parallel matrix-vector product.  Matrix
 *     is distributed by block rows.  Vectors are distributed by blocks.
 *
 * Input:
 *     m, n: order of matrix
 *     A, x: the matrix and the vector to be multiplied
 *
 * Output:
 *     y: the product vector
 *
 * Notes:  
 *     1.  Local storage for A, x, and y is statically allocated.
 *     2.  Number of processes (p) should evenly divide both m and n.
 *
 * See Chap 5, p. 78 & ff in PPMPI.
 */

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#define MAX_ORDER 1000

typedef float LOCAL_MATRIX_T[MAX_ORDER][MAX_ORDER];

int main(int argc, char* argv[]) {
    int             my_rank;
    int             p;
    LOCAL_MATRIX_T local_A; 
    float           global_x[MAX_ORDER];
    float           local_x[MAX_ORDER];
    float           local_y[MAX_ORDER];
    float           local_b[MAX_ORDER];
    float           local_u[MAX_ORDER];
    int             m, n, i;
    int             local_m, local_n;
    char *interest, *datafile,*outputfile; 
  double t1,t2;

    void Read_matrix(char* prompt, LOCAL_MATRIX_T local_A, int local_m, int n,
             int my_rank, int p);
    void Read_vector(char* prompt, float local_x[], int local_n, int my_rank,
             int p);
    void Parallel_matrix_vector_prod( LOCAL_MATRIX_T local_A, int m, 
             int n, float local_x[], float global_x[], float local_y[],
             int local_m, int local_n);
    void balance( LOCAL_MATRIX_T local_A, int m, 
             int n, float local_x[], float global_x[], float local_balance[],
             int local_m, int local_n);
    void update_balance( LOCAL_MATRIX_T local_A, int m, 
             int n, float local_x[], float global_x[], float local_balance[],float old_balance[], float local_u[],
             int local_m, int local_n);
    void Print_matrix(char* title, LOCAL_MATRIX_T local_A, int local_m,
             int n, int my_rank, int p);
    void Print_full_report(char* title, LOCAL_MATRIX_T local_A,float local_y[], float local_b[],float local_u[], int local_m,
             int n, int my_rank, int p);
    void Print_full_report_to_file(char* title, LOCAL_MATRIX_T local_A,float local_y[], float local_b[],float local_u[], int local_m,
             int n, int my_rank, int p);
    void Print_vector(char* title, float local_y[], int local_m, int my_rank,
             int p);
    void Print_report(char* title, float local_y[], float local_b[], float local_u[],int local_m, int my_rank,
             int p);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if (my_rank == 0) {
     if(argc ==6) //check for complete arguments, or else.
     {
       m=atoi(argv[1]);
       n=atoi(argv[2]);
     }
     else
     {
       printf("Usage: %s number_of_rows number_of_columns data_file interest_file output_file\n",argv[0]);
       exit(1);
     }
    }
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
//*****************************************************
// Names of the files:
// datafile is the accounts file
// interest is the vector file containing the interest rates
// outputfile is the name of the file where the 
// report will be stored.

    datafile=argv[3];
    interest=argv[4];
    outputfile=argv[5];
//***********************************************
    local_m = m/p;
    local_n = n/p;

    Read_matrix(datafile, local_A, local_m, n, my_rank, p);

    Read_vector(interest, local_x, local_n, my_rank, p);

    t1 = MPI_Wtime();
    Parallel_matrix_vector_prod(local_A, m, n, local_x, global_x, 
        local_y, local_m, local_n);
    
    t2 = MPI_Wtime();
   balance(local_A, m, n, local_x, global_x, 

       local_b, local_m, local_n);

    update_balance( local_A,m,n,local_x,global_x,local_b,local_y,local_u,local_m,local_n);


//    Print_full_report("We read", local_A, local_y,local_b,local_u,local_m, n, my_rank, p);
    Print_full_report_to_file(outputfile, local_A, local_y,local_b,local_u,local_m, n, my_rank, p);
    MPI_Finalize();
printf( "Elapsed time is %f\n", t2 - t1 );
return(0);

}  /* main */


/**********************************************************************/
void Read_matrix(
         char*           file_name   /* in  */, 
         LOCAL_MATRIX_T  local_A  /* out */, 
         int             local_m  /* in  */, 
         int             n        /* in  */,
         int             my_rank  /* in  */, 
         int             p        /* in  */) {

    int             i, j;
    LOCAL_MATRIX_T  temp;
    FILE *fp;
    fp=fopen(file_name,"r");
    /* Fill dummy entries in temp with zeroes */
    for (i = 0; i < p*local_m; i++)
        for (j = n; j < MAX_ORDER; j++)
            temp[i][j] = 0.0;

    if (my_rank == 0) {
        for (i = 0; i < p*local_m; i++) 
            for (j = 0; j < n; j++)
                fscanf(fp,"%f",&temp[i][j]);
    }
    fclose(fp);
    MPI_Scatter(temp, local_m*MAX_ORDER, MPI_FLOAT, local_A,
        local_m*MAX_ORDER, MPI_FLOAT, 0, MPI_COMM_WORLD);

}  /* Read_matrix */


/**********************************************************************/
void Read_vector(
         char*  file_name     /* in  */,
         float  local_x[]  /* out */, 
         int    local_n    /* in  */, 
         int    my_rank    /* in  */,
         int    p          /* in  */) {

    int   i;
    float temp[MAX_ORDER];
    FILE *fp;
    fp=fopen(file_name,"r");
    if (my_rank == 0) {
        for (i = 0; i < p*local_n; i++) 
            fscanf(fp,"%f", &temp[i]);
    }
    fclose(fp);
    MPI_Scatter(temp, local_n, MPI_FLOAT, local_x, local_n, MPI_FLOAT,
        0, MPI_COMM_WORLD);

}  /* Read_vector */


/**********************************************************************/
/* All arrays are allocated in calling program */
/* Note that argument m is unused              */
void Parallel_matrix_vector_prod(
         LOCAL_MATRIX_T  local_A     /* in  */,
         int             m           /* in  */,
         int             n           /* in  */,
         float           local_x[]   /* in  */,
         float           global_x[]  /* in  */,
         float           local_y[]   /* out */,
         int             local_m     /* in  */,
         int             local_n     /* in  */) {

    /* local_m = m/p, local_n = n/p */

    int i, j;

    MPI_Allgather(local_x, local_n, MPI_FLOAT,
                   global_x, local_n, MPI_FLOAT,
                   MPI_COMM_WORLD);
    for (i = 0; i < local_m; i++) {
        local_y[i] = 0.0;
        for (j = 0; j < n; j++)
            local_y[i] = local_y[i] +
                         local_A[i][j]*global_x[j];
    }
}  /* Parallel_matrix_vector_prod */


/**********************************************************************/
void Print_matrix(
         char*           title      /* in */, 
         LOCAL_MATRIX_T  local_A    /* in */, 
         int             local_m    /* in */, 
         int             n          /* in */,
         int             my_rank    /* in */,
         int             p          /* in */) {

    int   i, j;
    float temp[MAX_ORDER][MAX_ORDER];

    MPI_Gather(local_A, local_m*MAX_ORDER, MPI_FLOAT, temp, 
         local_m*MAX_ORDER, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("%s\n", title);
        for (i = 0; i < p*local_m; i++) {
            for (j = 0; j < n; j++)
                printf("%4.1f\t ", temp[i][j]);
            printf("\n");
        }
    } 
}  /* Print_matrix */


/**********************************************************************/
void Print_vector(
         char*  title      /* in */, 
         float  local_y[]  /* in */, 
         int    local_m    /* in */, 
         int    my_rank    /* in */,
         int    p          /* in */) {

    int   i;
    float temp[MAX_ORDER];

    MPI_Gather(local_y, local_m, MPI_FLOAT, temp, local_m, MPI_FLOAT,
        0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("%s\n", title);
        for (i = 0; i < p*local_m; i++)
            printf("%d %f \n", i+1,temp[i]);
        printf("\n");
    } 
}  /* Print_vector */

//***********************************************************
//
//  Print_report generates a minimum report
//  containing the old balance, interest gained
//  and new balance
//
//***********************************************************
void Print_report(
         char*  title      /* in */, 
         float  local_y[]  /* in */, 
         float  local_b[]  /* in */, 
         float  local_u[]  /* in */, 
         int    local_m    /* in */, 
         int    my_rank    /* in */,
         int    p          /* in */) {

    int   i;
    float temp[MAX_ORDER];
    float temp1[MAX_ORDER];
    float temp2[MAX_ORDER];

    MPI_Gather(local_y, local_m, MPI_FLOAT, temp, local_m, MPI_FLOAT,
        0, MPI_COMM_WORLD);
    MPI_Gather(local_b, local_m, MPI_FLOAT, temp1, local_m, MPI_FLOAT,
        0, MPI_COMM_WORLD);
    MPI_Gather(local_u, local_m, MPI_FLOAT, temp2, local_m, MPI_FLOAT,
        0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("%s\n", "Statement Report");
        printf("User ID\tOld Balance\tInterest Earned\tNew Balance\n");
        for (i = 0; i < p*local_m; i++)
            printf("%d\t %f \t%f\t%f\n", i+1,temp1[i],temp[i],temp2[i]);
        printf("\n");
    } 
}  /* Print_report */
/**********************************************************************/
/* All arrays are allocated in calling program */
/* Note that argument m is unused              */
//********************************************************
/* The function balance adds all the individual vales 
  in a row to get the total balance for that user */
//
// the output is local_y[]
//
//****************************************************

void balance(
         LOCAL_MATRIX_T  local_A     /* in  */,
         int             m           /* in  */,
         int             n           /* in  */,
         float           local_x[]   /* in  */,
         float           global_x[]  /* in  */,
         float           local_y[]   /* out */,
         int             local_m     /* in  */,
         int             local_n     /* in  */) {

    /* local_m = m/p, local_n = n/p */

    int i, j;

    MPI_Allgather(local_x, local_n, MPI_FLOAT,
                   global_x, local_n, MPI_FLOAT,
                   MPI_COMM_WORLD);
    for (i = 0; i < local_m; i++) {
        local_y[i] = 0.0;
        for (j = 0; j < n; j++)
            local_y[i] = local_y[i] +
                         local_A[i][j];
    }
}  /* balance */

//*************************************************
// 
//  update_balance adds tto the current balance 
//   the interest gained for every user.
//   The output is local_u.
//
//***********************************************
void update_balance(
         LOCAL_MATRIX_T  local_A     /* in  */,
         int             m           /* in  */,
         int             n           /* in  */,
         float           local_x[]   /* in  */,
         float           global_x[]  /* in  */,
         float           local_b[]  /* in  */,
         float           local_y[]   /* in */,
         float           local_u[]   /* out */,
         int             local_m     /* in  */,
         int             local_n     /* in  */) {

    /* local_m = m/p, local_n = n/p */

    int i, j;

    MPI_Allgather(local_x, local_n, MPI_FLOAT,
                   global_x, local_n, MPI_FLOAT,
                   MPI_COMM_WORLD);
    for (i = 0; i < local_m; i++) {
            local_u[i] = local_y[i] +
                         local_b[i];
    }
}  /* update_balance */

//*********************************************
//
// Print_full_report creates a report to stdout
void Print_full_report_to_file(
         char*           filename      /* in */, 
         LOCAL_MATRIX_T  local_A    /* in */, 
         float local_y[],
         float local_b[],
         float local_u[],
         int             local_m    /* in */, 
         int             n          /* in */,
         int             my_rank    /* in */,
         int             p          /* in */) {

    int   i, j;
    FILE *fp;
    float temp[MAX_ORDER][MAX_ORDER];

    float temp3[MAX_ORDER];
    float temp1[MAX_ORDER];
    float temp2[MAX_ORDER];
    MPI_Gather(local_A, local_m*MAX_ORDER, MPI_FLOAT, temp, 
         local_m*MAX_ORDER, MPI_FLOAT, 0, MPI_COMM_WORLD);

    MPI_Gather(local_y, local_m, MPI_FLOAT, temp3, local_m, MPI_FLOAT,
        0, MPI_COMM_WORLD);
    MPI_Gather(local_b, local_m, MPI_FLOAT, temp1, local_m, MPI_FLOAT,
        0, MPI_COMM_WORLD);
    MPI_Gather(local_u, local_m, MPI_FLOAT, temp2, local_m, MPI_FLOAT,
        0, MPI_COMM_WORLD);
    fp=fopen(filename,"w");
    if (my_rank == 0) {
        fprintf(fp,"User ID\t Checking\tCollege\tVacation\tCD\tIRA\tOld Balance\tInterest Earned\tNew Balance\n");
        fprintf(fp,"\n");
        for (i = 0; i < p*local_m; i++) {
            fprintf(fp,"%d\t", i+1);
            for (j = 0; j < n; j++)
                fprintf(fp,"%4.1f\t ", temp[i][j]);
            fprintf(fp,"%f\t%f\t%f\n",temp1[i],temp3[i],temp2[i]);
        }
    } 
   fclose(fp);
}  /* Print_full_report_to_file */
