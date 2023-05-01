#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 1000
#define SUBARRAY_SIZE 50
#define INFINITY 999999999

clock_t start, end;
double cpu_time_used;
double time4Send = 0;
double time4process = 0;

// mpicc FW_t2.c -o FW_t2
// mpirun -np 3 ./FW_t2

struct dataPoint
{
  int x;
  int y;
};


int getPos(int** array,struct dataPoint pos){
  return array[pos.x][pos.y];
}

int setPos(int** array,struct dataPoint pos, int val){
  array[pos.x][pos.y] = val;
}

void log_value(float val, int val2)
{
    FILE *fp;

    // Open the file for appending
    fp = fopen("log.txt", "a");

    // Check if the file was opened successfully
    if (fp == NULL) {
        printf("Error: could not open file\n");
        return;
    }

    // Write the value to the end of the file
    fprintf(fp, "%f %d\n", val,val2);

    // Close the file
    fclose(fp);
}

int main(int argc, char** argv) {
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0){
    
    int avgLayer = 5;
    int numStack = 0;

    struct dataPoint* stack = (struct dataPoint*) malloc(ARRAY_SIZE*ARRAY_SIZE * sizeof(struct dataPoint));
    int** array = NULL;
    array = (int**) malloc(ARRAY_SIZE * sizeof(int*));
    for (int i = 0; i < ARRAY_SIZE; i++) {
      array[i] = (int*) malloc(ARRAY_SIZE * sizeof(int));
      for (int j = 0; j < ARRAY_SIZE; j++) {
        if (rand()%ARRAY_SIZE < avgLayer){
          array[i][j] = rand()%100;
          stack[numStack].x = i;
          stack[numStack].y = j;
          numStack += 1;
        }
        else{
          array[i][j] = INFINITY;
        }
      }
    }
    for(int k=0; k<ARRAY_SIZE; k++){
      start = clock();

      int currentStack = numStack;
      for (int i = 0; i < currentStack-1; i++) {
        for (int j = i+1; j < currentStack; j++) {
          if (getPos(array,stack[i]) == INFINITY || getPos(array,stack[j]) == INFINITY){
            continue;
          }
          int matchCoordX = -1;
          int matchCoordY = -1;
          if (stack[i].x == k && stack[j].y == k){
            matchCoordX = stack[j].x;
            matchCoordY = stack[i].y;
          }
          if (stack[i].y == k && stack[j].x == k){
            matchCoordX = stack[i].x;
            matchCoordY = stack[j].y;
          }

          if (matchCoordX == -1){
            continue;
          }

          if (array[matchCoordX][matchCoordY] > getPos(array,stack[i]) + getPos(array,stack[j])){
            if (array[matchCoordX][matchCoordY] == INFINITY){
              stack[numStack].x = matchCoordX;
              stack[numStack].y = matchCoordY;
              numStack += 1;
            }
            array[matchCoordX][matchCoordY] = getPos(array,stack[i]) + getPos(array,stack[j]);
          }
        }
      }

      end = clock();
      cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
      printf("loop %d has stack %d take time %f \n",k,numStack,cpu_time_used);
      log_value(cpu_time_used,numStack);
    }

    MPI_Request request;
    MPI_Status status;
    
    free(array);



    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time measure in core %d : %f \n",rank,cpu_time_used);
    printf("Time use for process in core %d : %f \n",rank,time4process);
    printf("Time use for send core %d : %f \n",rank,time4Send);
    
  }


  MPI_Finalize();
  return 0;
}
