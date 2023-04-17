#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 1000
#define SUBARRAY_SIZE 50

clock_t start, end;
double cpu_time_used;
double time4Send = 0;
double time4process = 0;

// mpicc demo_FW_1.c -o demo_FW_1  
// mpirun -np 3 ./demo_FW_1

int main(int argc, char** argv) {
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int sqrAS = SUBARRAY_SIZE*SUBARRAY_SIZE;

  start = clock();

  if (rank == 0){
    int** array = NULL;
    if (rank == 0) {
      array = (int**) malloc(ARRAY_SIZE * sizeof(int*));
      for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = (int*) malloc(ARRAY_SIZE * sizeof(int));
        for (int j = 0; j < ARRAY_SIZE; j++) {
          array[i][j] = rand();
        }
      }
    }

    int *buffer = malloc((SUBARRAY_SIZE+2)*SUBARRAY_SIZE*sizeof(int));
    int *recvBuffer = malloc(sqrAS*sizeof(int));

    int *cachePosition = malloc(size*2*sizeof(int));

    // Send subarrays to worker cores and wait for their signals.
    MPI_Request request;
    MPI_Status status;
    int count = 0;
    int dest = 1;
    clock_t startSend;;
    for (int k = 0; k < ARRAY_SIZE; k ++){
      // printf("start sequent %d \n",k);
      for (int i = 0; i < ARRAY_SIZE; i += SUBARRAY_SIZE) {
        for (int j = 0; j < ARRAY_SIZE; j += SUBARRAY_SIZE) {
          // Add data.
          clock_t startProcess = clock();
          for (int x = 0; x < SUBARRAY_SIZE; x++) {
            for (int y = 0; y < SUBARRAY_SIZE; y++) {
              buffer[x*SUBARRAY_SIZE+y] = array[i + x][j + y];
            }
          }
          
          // add two line of k
          for (int k_line = 0; k_line < SUBARRAY_SIZE; k_line++){
            buffer[sqrAS + k_line] = array[i+k_line][k];
          }
          for (int k_line = 0; k_line < SUBARRAY_SIZE; k_line++){
            buffer[(SUBARRAY_SIZE+1)*SUBARRAY_SIZE + k_line] = array[k][j+k_line];
          }
          time4process += ((double) (clock() - startProcess)) / CLOCKS_PER_SEC;
          if (dest < size){
            cachePosition[dest*2] = i;
            cachePosition[dest*2+1] = j;
            // printf("send init package \n");
            startSend = clock();
            MPI_Send(buffer, (SUBARRAY_SIZE+2) * SUBARRAY_SIZE, MPI_INT, dest, 0, MPI_COMM_WORLD);
            time4Send += ((double) (clock() - startSend)) / CLOCKS_PER_SEC;
            dest += 1;
          } else {
            // printf("recive normal pakage \n");
            startSend = clock();
            MPI_Recv(recvBuffer, sqrAS, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            time4Send += ((double) (clock() - startSend)) / CLOCKS_PER_SEC;
            //todo: reassign data
            clock_t startProcess = clock();
            for (int x = 0; x < SUBARRAY_SIZE; x++) {
              for (int y = 0; y < SUBARRAY_SIZE; y++) {
                array[cachePosition[status.MPI_SOURCE*2] + x][cachePosition[status.MPI_SOURCE*2+1] + y] = recvBuffer[x*SUBARRAY_SIZE+y];
              }
            }
            time4process += ((double) (clock() - startProcess)) / CLOCKS_PER_SEC;
            cachePosition[status.MPI_SOURCE*2] = i;
            cachePosition[status.MPI_SOURCE*2+1] = j;
            startSend = clock();
            MPI_Send(buffer, (SUBARRAY_SIZE+2) * SUBARRAY_SIZE, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            time4Send += ((double) (clock() - startSend)) / CLOCKS_PER_SEC;
            // printf("start send normal pakage \n");
          }
          count++;
        }
      }
      while (dest != 1)
      {
        // printf("recive end sequent pakage \n");
        MPI_Recv(recvBuffer, sqrAS, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        //todo: reassign data
        clock_t startProcess = clock();
        for (int x = 0; x < SUBARRAY_SIZE; x++) {
          for (int y = 0; y < SUBARRAY_SIZE; y++) {
            array[cachePosition[status.MPI_SOURCE*2] + x][cachePosition[status.MPI_SOURCE*2+1] + y] = recvBuffer[x*SUBARRAY_SIZE+y];
          }
        }
        time4process += ((double) (clock() - startProcess)) / CLOCKS_PER_SEC;
        dest -= 1;
      }
    }

    
    buffer[0] = -1;
    for (int i = 1; i < size; i ++){
      MPI_Send(buffer, (SUBARRAY_SIZE+2) * SUBARRAY_SIZE, MPI_INT, i, 0, MPI_COMM_WORLD);
    } 

    // Free the memory allocated for the arrays.
    for (int i = 0; i < ARRAY_SIZE; i++) {
      free(array[i]);
    }
    free(array);
  } else {
    int *buffer = malloc((SUBARRAY_SIZE+2)*SUBARRAY_SIZE*sizeof(int));
    int *sendBuffer = malloc((SUBARRAY_SIZE+2)*SUBARRAY_SIZE*sizeof(int));
    MPI_Status status;
    while (1)
    {
      // printf("worker recive \n");
      clock_t startSend = clock();
      MPI_Recv(buffer, (SUBARRAY_SIZE+2) * SUBARRAY_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      time4Send += ((double) (clock() - startSend)) / CLOCKS_PER_SEC;
      if (buffer[0] == -1){
        // printf("worker die \n");
        break;
      }
      clock_t startProcess = clock();
      for (int x = 0; x < SUBARRAY_SIZE; x++) {
        for (int y = 0; y < SUBARRAY_SIZE; y++) {
          if (buffer[x*SUBARRAY_SIZE + y] > buffer[sqrAS + x] + buffer[sqrAS + SUBARRAY_SIZE + y]){
            sendBuffer[x*SUBARRAY_SIZE + y] = buffer[sqrAS + x] + buffer[sqrAS + SUBARRAY_SIZE + y];
          } else {
            sendBuffer[x*SUBARRAY_SIZE + y] = buffer[x*SUBARRAY_SIZE + y];
          }
        }
      }
      time4process += ((double) (clock() - startProcess)) / CLOCKS_PER_SEC;
      // printf("worker done \n");
      startSend = clock();
      MPI_Send(buffer, sqrAS, MPI_INT, 0, 0, MPI_COMM_WORLD);
      time4Send += ((double) (clock() - startSend)) / CLOCKS_PER_SEC;
    }
  }

  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("Time measure in core %d : %f \n",rank,cpu_time_used);
  printf("Time use for process in core %d : %f \n",rank,time4process);
  printf("Time use for send core %d : %f \n",rank,time4Send);
  
  MPI_Finalize();
  return 0;
}
