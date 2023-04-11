#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define fori(n) for (int i = 0; i < n; i++)
#define forj(n) for (int j = 0; j < n; j++)

long long timeInMilliseconds(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

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