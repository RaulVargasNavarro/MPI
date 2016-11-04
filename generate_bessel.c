#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

int main(int argc, char *argv[])
{
  int i,j,k;
  int n;
  FILE *fpa, *fpb;
  double *x, *y;
  double r;
  double x_start,y_start,x_end,y_end;
  double x_step,y_step;
  double f,g;
  char *filename1, *filename2;
  double **A,**B;
   if(argc < 7)
  {
    printf("Usage %s: rank s f g file_name_Matrix_1 file_name_Matrix_2\n",argv[0]);
    return(1);
  }
  n=atoi(argv[1]);
  x_start=atof(argv[2]);
  f=atof(argv[3]);
  g=atof(argv[4]);
  filename1=argv[5];
  filename2=argv[6];

  x_end=-x_start;
  y_start=x_start;
  y_end=-y_start;

  x=malloc(n*sizeof(double));
  y=malloc(n*sizeof(double));
  A=malloc(n*sizeof(double*));
  B=malloc(n*sizeof(double*));
  if(x ==NULL || y == NULL || A==NULL || B==NULL)
  {
    printf("Problem with malloc...\n");
    return(1);
  }
  for(i=0;i<n;i++)
  {
    A[i]=malloc(n*sizeof(double));
    B[i]=malloc(n*sizeof(double));
  }
  fpa=fopen(filename1,"w");
  fpb=fopen(filename2,"w");
  if(fpa ==NULL || fpb== NULL)
  {
    printf("problem opening output files...\n");
    return(1);
  }
  x[0]=x_start;
  y[0]=y_start;
  x[n-1]=x_end;
  y[n-1]=y_end;

  x_step=(x_end-x_start)/(double)(n-1);
  y_step=(y_end-y_start)/(double)(n-1);

// Initialize the grid values into arrays x and y
  for(i=1;i<(n-1);i++)
  {
    x[i]=x[i-1]+x_step;
    y[i]=y[i-1]+y_step;
  }
  
  for(i=0;i<n;i++)
  {
    for(j=0;j<n;j++)
    {
      r=sqrt(x[i]*x[i]+y[j]*y[j]);
      for(k=0;k<n;k++)
      {
        A[i][k]=f*cos(r)*cos(r);
        B[k][j]=exp(-g*r);
      }
    }
  }
  for(i=0;i<n;i++)
  {
    for(j=0;j<n;j++)
    {
      fprintf(fpa,"%lf ",A[i][j]);
      fprintf(fpb,"%lf ",B[i][j]);
    }
    fprintf(fpa,"\n");
    fprintf(fpb,"\n");
  }
  free(x);
  free(y);
  fclose(fpa);
  fclose(fpb);
  return(0);
}
