#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <fstream>
#include <string>
#include <math.h>

#define fori(n) for (int i = 0; i < n; i++)
#define forj(n) for (int j = 0; j < n; j++)

#define HEIGHT 200
#define WIDTH 180

using namespace std;

long getMillis()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

void create2dArray(int h, int w, int **&image)
{
    image = (int **)malloc(h * sizeof(int *));
    fori(h) image[i] = (int *)malloc(w * sizeof(int));
}
void createArray(int n, int *&rs)
{
    rs = (int *)malloc(n * sizeof(int));
}

void loadImage(string file, int h, int w, int **&image)
{
    // cout << "Load file: " << file << endl;
    create2dArray(h, w, image);
    std::ifstream reader(file);
    fori(h) forj(w)
    {
        reader >> image[i][j];
    };
    reader.close();
}

void loadImage(int personId, int idx, int h, int w, int **&image)
{
    string path = "./data/v1/" + to_string(personId) + "." + to_string(idx) + ".txt";
    loadImage(path, h, w, image);
}

void printImage(int **image, int h, int w)
{
    fori(h)
    {
        forj(w) cout << image[i][j] << " ";
        cout << endl;
    }
}

void printArray(int *arr, int n)
{
    fori(n) cout << arr[i] << " ";
    cout << endl;
}
