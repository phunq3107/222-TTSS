#include <iostream>
#include <stdio.h>
#include "utils.cpp"

using namespace std;

int calCell(int **image, int h, int w, int curr, int i, int j)
{
    if (i < 0 || i >= h || j < 0 || j >= w)
        return 0;
    return curr < image[i][j] ? 1 : 0;
}

int getLbp(int **image, int h, int w, int i, int j)
{
    int lbp[8];
    int curr = image[i][j];
    lbp[0] = calCell(image, h, w, curr, i - 1, j - 1);
    lbp[1] = calCell(image, h, w, curr, i - 1, j);
    lbp[2] = calCell(image, h, w, curr, i - 1, j + 1);
    lbp[3] = calCell(image, h, w, curr, i, j + 1);
    lbp[4] = calCell(image, h, w, curr, i + 1, j + 1);
    lbp[5] = calCell(image, h, w, curr, i + 1, j);
    lbp[6] = calCell(image, h, w, curr, i + 1, j - 1);
    lbp[7] = calCell(image, h, w, curr, i, j - 1);
    int rs = 0;
    fori(8)
        rs += (1 << (7 - i)) * lbp[i];
    return rs;
}

void genHistogram(int **image, int h, int w, int *&histogram)
{
    fori(255) histogram[i] = 0;
    fori(h) forj(w) histogram[getLbp(image, h, w, i, j)]++;
}

double calDistance(int *h1, int *h2, int n)
{
    double rs = 0.0;
    fori(n)
    {
        rs += (h1[i] + h2[i] == 0) ? 0 : (pow(h1[i] - h2[i], 2) * 0.5 / (h1[i] + h2[i]));
    }
    return rs;
}

int findClosest(int ***trainSet, int nPerson, int nTraining, int *hist)
{
    double min = 0x7fffffff;
    int rs = 0;
    fori(nPerson) forj(nTraining)
    {
        double dist = calDistance(trainSet[i][j], hist, 255);
        if (dist < min)
        {
            rs = i;
            min = dist;
        }
    }
    return rs;
}

int main()
{
    int noPerson = 10;
    int noImagePerPerson = 20;
    int noTrainPerPerson = 15;
    int noTestPerPerson = noImagePerPerson - noTrainPerPerson;

    int ***traning_set = (int ***)malloc(noPerson * sizeof(int **)); // person -> image -> histogram

    // ------------ training
    fori(noPerson)
    {
        traning_set[i] = (int **)malloc(noTrainPerPerson * sizeof(int *));
        forj(noTrainPerPerson)
        {
            traning_set[i][j] = (int *)malloc(255 * sizeof(int));
            int **image;
            loadImage(i + 1, j + 1, HEIGHT, WIDTH, image);
            genHistogram(image, HEIGHT, WIDTH, traning_set[i][j]);
        }
    }

    // test
    fori(noPerson)
    {
        forj(noTestPerPerson)
        {
            int **image;
            loadImage(i + 1, j + 1 + noTrainPerPerson, HEIGHT, WIDTH, image);
            int *hist = (int *)malloc(255 * sizeof(int));
            genHistogram(image, HEIGHT, WIDTH, hist);
            int predict = findClosest(traning_set, noPerson, noTrainPerPerson, hist);
            cout << "Persion " << (i + 1) << " - Image " << (j + 1 + noTrainPerPerson) << " - Predict " << (predict + 1) << endl;
            // break;
        }
        // break;
    }

    return 0;
}