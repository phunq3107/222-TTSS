#include <stdio.h>
#include <stdlib.h>

#define fori(n) for (int i = 0; i < n; i++)
#define forj(n) for (int j = 0; j < n; j++)

int min(int a, int b)
{
    return a < b ? a : b;
}

void init_2d_array(int **data, int size)
{
    int **rs = (int **)malloc(size * sizeof(int *));
    fori(size) rs[i] = (int *)malloc(size * sizeof(int));
    *data = rs;
}
void creat_array(int **data, int size)
{
    *data = (int *)malloc(size * sizeof(int));
}

void print_2d_aray(int **data, int size)
{
    fori(size)
    {
        forj(size) printf("%d ", data[i][j]);
        printf("\n");
    }
}