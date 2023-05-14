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
    int numData = 0;

    struct dataPoint* data = (struct dataPoint*) malloc(ARRAY_SIZE*ARRAY_SIZE * sizeof(struct dataPoint));
    int** array = NULL;
    array = (int**) malloc(ARRAY_SIZE * sizeof(int*));
    for (int i = 0; i < ARRAY_SIZE; i++) {
      array[i] = (int*) malloc(ARRAY_SIZE * sizeof(int));
      for (int j = 0; j < ARRAY_SIZE; j++) {
        if (rand()%ARRAY_SIZE < avgLayer){
          array[i][j] = rand()%100;
          data[numData].x = i;
          data[numData].y = j;
          numData += 1;
        }
        else{
          array[i][j] = INFINITY;
        }
      }
    }
    for(int k=0; k<ARRAY_SIZE; k++){
      

      int currentStack = numData;
      int meaning = 0;

      struct dataPoint* Kx = (struct dataPoint*) malloc(numData* sizeof(struct dataPoint));
      struct dataPoint* Ky = (struct dataPoint*) malloc(numData* sizeof(struct dataPoint));
      
      int lenX = 0;
      int lenY = 0;

      start = clock();

      for (int i = 0; i < currentStack-1; i++){
        if (data[i].x == k){
            Kx[lenX++] = data[i];
          }
        if (data[i].y == k){
          Ky[lenY++] = data[i];
        }
      }

      for (int i = 0; i < lenX; i++) {
        for (int j = 0; j < lenY; j++) {
          int matchCoordX = Ky[j].x;
          int matchCoordY = Kx[i].y;

          if (array[matchCoordX][matchCoordY] > getPos(array,data[i]) + getPos(array,data[j])){
            if (array[matchCoordX][matchCoordY] == INFINITY){
              data[numData].x = matchCoordX;
              data[numData].y = matchCoordY;
              numData += 1;
            }
            array[matchCoordX][matchCoordY] = getPos(array,data[i]) + getPos(array,data[j]);
          }
        }
      }

      // printf("in %d loop, there is %d meanning, %f  \n",currentStack,meaning,100.0*meaning/currentStack);

      end = clock();
      cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
      // printf("loop %d has stack %d take time %f \n",k,numStack,cpu_time_used);
      log_value(cpu_time_used,numData);
      free(Kx);
      free(Ky);
      // if (k > 200){
      //   break;
      // }
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
