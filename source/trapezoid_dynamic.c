#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define SEND 1
#define STOP 0

float f(float x);

int main(int argc, char** argv) {
  int rank, size;
  double a, b;  /* default left and right endpoints of the interval */
  int n;        /* total number of trapezoids */
  double h;        /* height of the trapezoids */
  double param[2]; /* array containing starting end point and height for each individual trapezoid */
  double one_result = 0.0;  /* area of each individual trapezoid */
  double local_result = 0.0;
  double result = 0.0;     /* Total integral */
  int source;    /* Process sending the partial integral  */
  int dest = 0;  /* All messages go to 0      */
  int tag = 0;
  int i,count,partial_count;
  MPI_Status  status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0){
    a = atof(argv[1]);
    b = atof(argv[2]);
    n = atoi(argv[3]);
    h = (b-a)/n;
    count = 0;
    for (i = 1; i < size; i++){
      param[0] = a + count * h;
      param[1] = h;
      MPI_Send(param,2,MPI_DOUBLE,i,SEND,MPI_COMM_WORLD);
      count = count + 1;
    }
  } else {
    MPI_Recv(param,2,MPI_DOUBLE,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
  }

  if (rank != 0){
    partial_count = 0;
    do {
      partial_count += 1;
      one_result = param[1] * (f(param[0]) +  f(param[0] + param[1])) / 2;
      local_result += one_result;
      MPI_Send(&one_result,1,MPI_DOUBLE,0,SEND,MPI_COMM_WORLD);
      MPI_Recv(param,2,MPI_DOUBLE,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
    } while(status.MPI_TAG != STOP);
    printf("Process %d calculate %d trapezoids to a local area of %lf \n", rank, partial_count, local_result);
  } else {
    do {
      MPI_Recv(&local_result,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
      result = result + local_result;
      param[0] = a + count * h;
      param[1] = h;
      MPI_Send(param,2,MPI_DOUBLE,status.MPI_SOURCE,SEND,MPI_COMM_WORLD);
      count = count + 1;
    } while (count < n);

    for (i = 0; i < (size - 1); i++){
      MPI_Recv(&local_result,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
      result = result + local_result;
    }

    for (i = 1; i < size; i++){
      MPI_Send(param,2,MPI_DOUBLE,i,STOP,MPI_COMM_WORLD);
    }
    printf("The integral of f(x) from %lf to %lf using %d processses is %lf\n", a, b, size, result);
  }

  MPI_Finalize();
}

float f(float x) {
    return ( x*x );
}
