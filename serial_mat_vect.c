/* serial_mat_vect.c -- computes a matrix-vector product on a single processor.
 *
 * Input:
 *     m, n: order of matrix
 *     A, x:  the matrix and the vector to be multiplied
 *
 * Output:
 *     y: the product vector
 *
 * Note:  A, x, and y are statically allocated.
 *
 * See Chap 5, p. 78 & ff in PPMPI.
 */
#include <stdio.h>

#define MAX_ORDER 100

typedef float MATRIX_T[MAX_ORDER][MAX_ORDER];

main() {
    MATRIX_T  A; 
    float     x[MAX_ORDER];
    float     y[MAX_ORDER];
    int       m, n;

    void Read_matrix(char* prompt, MATRIX_T A, int m, int n);
    void Read_vector(char* prompt, float v[], int n);
    void Serial_matrix_vector_prod(MATRIX_T A, int m, int n,
              float x[], float y[]);
    void Print_vector(float y[], int n);
    
    printf("Enter the order of the matrix (m x n)\n");
    scanf("%d %d", &m, &n);
    Read_matrix("the matrix", A, m, n);
    /* bug in line: Read_vector("the vector", x, m); */
    Read_vector("the vector", x, n);
    Serial_matrix_vector_prod(A, m, n, x, y);
    /*  bug in line: Print_vector(y, n);  */
    Print_vector(y, m);
}  /* main */
   

/*****************************************************************/
void Read_matrix(
         char*     prompt  /* in  */,
         MATRIX_T  A       /* out */,
         int       m       /* in  */,
         int       n       /* in  */) {
    int i, j;

    printf("Enter %s\n", prompt);
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            scanf("%f", &A[i][j]);
}  /* Read_matrix */


/*****************************************************************/
void Read_vector(
         char*  prompt  /* in  */,
         float  v[]     /* out */,
         int    n       /* in  */) {
    int i;

    printf("Enter %s\n", prompt);
    for (i = 0; i < n; i++)
        scanf("%f", &v[i]);
}  /* Read_vector */


/*****************************************************************/
void Serial_matrix_vector_prod(
        MATRIX_T  A    /* in  */,
        int       m    /* in  */,
        int       n    /* in  */,
        float     x[]  /* in  */,
        float     y[]  /* out */) {

    int k, j;

    for (k = 0; k < m; k++) {
        y[k] = 0.0;
        for (j = 0; j < n; j++)
            y[k] = y[k] + A[k][j]*x[j];
    }
}  /* Serial_matrix_vector_prod */


/*****************************************************************/
void Print_vector(
         float  y[]  /* in */,
         int    n    /* in */) {
    int i;

    printf("Result is \n");
    for (i = 0; i < n; i++)
        printf("%4.1f ", y[i]);
    printf("\n");
}  /* Print_vector */
