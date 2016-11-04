#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void ReadMatrix(FILE *fp,double **A,int m,int n)
{
  int i,j;
  for(i=0;i<m;i++)
  {
    for(j=0;j<n;j++)
    {
      fscanf(fp,"%lf",&A[i][j]);
    }
  }
}

void WriteMatrix(FILE *fp,double **A,int m,int n)
{
  int i,j;
  for(i=0;i<m;i++)
  {
    for(j=0;j<n;j++)
    {
      fprintf(fp,"%lf ",A[i][j]);
    }
    fprintf(fp,"\n");
  }
}
int main(int argc, char *argv[])
{

  int i,j,k;
  FILE *fpa,*fpb,*fpc;
  int m,n;
  char *file_A,*file_B,*file_C;
  double **A, **B, **C;

  if (argc < 6)
  {
    printf("%s usage : nrows ncolums file_name_Matrix_A file_name_Matrix_B file_name_Matrix_C \n",argv[0]);
    return(1);
  }
  m=atoi(argv[1]);
  n=atoi(argv[2]);

  file_A=argv[3];
  file_B=argv[4];
  file_C=argv[5];

  fpa=fopen(file_A,"r");
  fpb=fopen(file_B,"r");
  fpc=fopen(file_C,"w");

  if ( fpa == NULL  || fpb == NULL || fpc == NULL)
  {
    printf("problem opening file(s)....\n");
    fclose(fpa);
    fclose(fpb);
    fclose(fpc);
    return(1);
  }
  A=malloc(m*sizeof(double *));
  B=malloc(m*sizeof(double *));
  C=malloc(m*sizeof(double *));

  if (A==NULL || B==NULL || C== NULL)
  {
    printf("problem with malloc...\n");
    free(A);
    free(B);
    free(C);
    fclose(fpa);
    fclose(fpb);
    fclose(fpc);
    return(1);
  }
  for(i=0;i<m;i++)
  {
    A[i]=malloc(n*sizeof(double));
    B[i]=malloc(n*sizeof(double));
    C[i]=malloc(n*sizeof(double));
  }
  ReadMatrix(fpa,A,m,n);
  ReadMatrix(fpb,B,m,n);
  clock_t tic =clock();
  for (i=0;i<m;i++)
  {
    for(j=0;j<n;j++)
    {
      C[i][j]=0.0;
      for(k=0;k<m;k++)
      {
        C[i][j]=C[i][j]+A[i][k]*B[k][j];
      }
    }
  }
  clock_t toc =clock();
  printf("Time in seconds : %lf\n",(double)(toc-tic)/(double)CLOCKS_PER_SEC);
  WriteMatrix(fpc,C,m,n);
  fclose(fpa);
  fclose(fpb);
  fclose(fpc);
  return (0);
}

