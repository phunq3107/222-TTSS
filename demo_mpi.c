#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int jobNum = 20;

    // Print off a hello world message
    printf("Init processor %d out of %d processors\n",
            world_rank, world_size);

    // If this is the root process (rank 0), send jobs to other processes
    if (world_rank == 0) {
        int value = 123;

        // Send jobs to all other processes
        for (int core=1; core < world_size; core++){
            printf("Sending job to core %d\n", core);
            MPI_Send(&value, 1, MPI_INT, core, 0, MPI_COMM_WORLD);
        }

        int remain = jobNum - world_size-1;
        MPI_Status status;
        
        // Keep receiving job completion messages until all jobs are done
        while (1)
        {
            // If all jobs are done, exit the loop
            if (remain <= -1*(world_size-2)){
                printf("All jobs completed. Exiting...\n");
                break;
            }

            // Wait for a job completion message from any source
            remain -= 1;
            MPI_Recv(&value, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            printf("Sending new job to core %d\n", status.MPI_SOURCE);

            // If no more jobs remain, send an exit signal to the source of the completion message
            if (remain <= 0){
                int value = -1;
                printf("Sending exit signal to core %d\n", status.MPI_SOURCE);
                MPI_Send(&value, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            }

            // Otherwise, send another job to the source of the completion message
            MPI_Send(&value, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
        }

    }
    // If this is not the root process, receive and process jobs
    else {
        MPI_Status status;
        int value = 1;

        // Keep receiving and processing jobs until an exit signal is received
        while ( 1)
        {
            MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

            // If the received job value is positive, process the job and send a completion message
            if (value > 0){
                printf("Core %d processing a job.\n",world_rank);
                usleep(100000);
                MPI_Send(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            } 
            // If the received job value is negative, exit the loop
            else {
                printf("Core %d received exit signal. Exiting...\n",world_rank);
                break;
            }
            
        }
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}