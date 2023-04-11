#include <stdio.h>
#include <mpi.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include "utils.h"
int **data;
int world_rank;
int world_size;
int size;

int roi_lb()
{
    int no_own = size / world_size;
    return world_rank * no_own;
}

int roi_ub()
{
    if (world_rank == world_size - 1)
        return size - 1;
    int no_own = size / world_size;
    return (world_rank + 1) * no_own - 1;
}
bool is_owner(int col)
{
    if (col >= roi_lb() && col <= roi_ub())
        return true;
    return false;
}

int get_owner(int col)
{
    int no_own = size / world_size;
    int rs = col / no_own;
    return rs >= world_size ? world_size - 1 : rs;
}

void publish_data(int col)
{
    printf("%d Publish data col %d\n", world_rank, col);
    int *msg = (int *)malloc((size + 1) * sizeof(int));
    fori(size)
    {
        msg[i] = data[i][col];
    }
    msg[size] = col;
    fori(world_size)
    {
        if (i == world_rank)
            continue;
        MPI_Send(msg, size + 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    // printf("Publish data col %d success\n", col);
}

void receive_data(int col)
{
    printf("%d Receive data col %d from %d \n", world_rank, col, get_owner(col));
    int *msg = (int *)malloc((size + 1) * sizeof(int));
    MPI_Status status;
    MPI_Recv(msg, size + 1, MPI_INT, get_owner(col), 0, MPI_COMM_WORLD, &status);
    col = msg[size];
    fori(size) data[i][col] = msg[i];
    printf("%d Receive data col %d (%d) success\n", world_rank, col, msg[size]);
}

void update_data(int base)
{
    fori(size)
    {
        for (int j = roi_lb(); j <= roi_ub(); j++)
        {

            data[i][j] = i == j ? 0 : min(data[i][j], data[i][base] + data[base][j]);
        }
    }
}

void init_data(char *filename)
{
    FILE *fp;
    fp = fopen(filename, "r");
    fscanf(fp, "%d\n", &size);
    data = (int **)malloc(size * sizeof(int *));
    fori(size) data[i] = (int *)malloc(size * sizeof(int));

    int buf;
    fori(size)
    {
        forj(size)
        {
            fscanf(fp, "%d\n", &buf);
            data[i][j] = buf;
        }
    }
    fclose(fp);
}

int main(int argc, char **argv)
{

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    if (world_size < 2)
    {
        printf("Requires at least two processes.\n");
        exit(-1);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    init_data("data/data1.txt");
    printf("word_size=%d, word_rank=%d, size=%d, roi_lb=%d, roi_ub=%d\n", world_size, world_rank, size, roi_lb(), roi_ub());

    fori(size)
    {
        if (is_owner(i))
        {
            publish_data(i);
        }
        else
        {
            receive_data(i);
        }
        update_data(i);
    }
    if (world_rank == world_size - 1)
    {
        fori(size)
        {
            if (is_owner(i))
                continue;
            receive_data(i);
        }
        print_2d_aray(data, size);
    }
    else
    {
        for (int i = roi_lb(); i <= roi_ub(); i++)
        {
            publish_data(i);
        }
    }

    MPI_Finalize();
}