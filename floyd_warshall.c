#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include "utils.h"

int **data;
int size;

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

void fw()
{
    for (int k = 0; k < size; k++)
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                data[i][j] = min(data[i][j], data[i][k] + data[k][j]);
            }
        }
    }
}

int main(int argc, char **argv)
{

    init_data("data/data_1000.txt");    
    long long begin = timeInMilliseconds();
    fw();
    long long end = timeInMilliseconds();

    printf("Done %lld \n", end - begin);
}
