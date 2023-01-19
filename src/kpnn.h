#ifndef KPNN_H
#define KPNN_H

#include <math.h>


// use net Pnet => 128x16x2 => opS /egS outputs
#define N_INPUTS   (64 * 2)
#define N_HIDDEN   (16)

int OUTPUT_BIAS1 = -1;
int OUTPUT_BIAS2 = -9;
int HIDDEN_BIAS[N_HIDDEN] = {6, 1, -1, 2, -1, 2, -4, -3, };
int OUTPUT_WEIGHTS1[N_HIDDEN] = {9, 9, -17, 17, 7, -18, 19, -16, };
int OUTPUT_WEIGHTS2[N_HIDDEN] = {11, 3, 2, 6, 7, -11, 3, -7, };


int HIDDEN_WEIGHTS[N_INPUTS * N_HIDDEN] = {
          -3, -3, -2, -3, 0, -3, -2, 5, 2,
          2, 0, 2, 2, -4, -2, 3, 3,
          2, 4, -2, 2, -4, -3, 2, 1,
          2, 2, 1, 3, 1, 1, 3, -4,
          1, 2, 9, 2, 6, 10, 12, 2,
          -3, -3, -9, -6, 14, -6, 16, -1,
          3, -1, -1, -2, -5, -8, -4, 3,
          2, 4, -4, -3, 1, 1, 2, -1,
          5, 0, -3, 3, -3, 4, -3, -7,
          -2, -7, 9, 9, -7, 0, -8, -5,
          -4, -1, 8, 8, 3, 5, 8, -7,
          -4, -3, 4, -0, 2, 1, -0, -4,
          -6, -2, -1, -4, 2, 4, 2, -4,
          -6, -2, 4, -3, 3, 3, -1, -4,
          -6, -1, -2, -4, 1, 2, -2, -4,
          0, -1, 4, 3, -1, -4, 1, -1,
          5, -3, 2, 5, 3, -1, 4, 4,
          2, -2, -3, -0, 2, 3, -4, -0,
          7, 0, -0, 1, 0, -1, -3, 0,
          7, -0, -2, -3, 2, -3, -9, 0,
          3, 6, -3, -4, 8, -3, 1, 4,
          8, 3, 6, -0, 5, 6, 4, 0,
          6, 4, 3, -3, -0, -0, -1, 0,
          -4, 2, -3, -2, 3, -4, 1, 5,
          1, 3, -4, 1, 1, 1, 3, -0,
          -7, -4, 3, -2, -8, -7, -0, 3,
          1, -0, 9, 0, -3, -2, 7, -1,
          -0, -0, -3, -2, 4, 8, -3, -3,
          -3, -1, 12, -2, -3, 1, -2, 0,
          -1, 2, -2, -4, 1, -4, 1, 2,
          12, -3, -5, 0, 5, 1, -2, 0,
          -2, 1, 1, 2, -2, 1, 1, 4,
          2, -4, 3, -3, -3, 4, 3, 0,
          1, 0, -3, 1, -5, 0, -0, 1,
          3, 7, 5, 0, -1, -0, -1, -0,
          3, 2, 4, 8, 1, 1, -1, -2,
          -1, -5, -1, 5, 9, 1, 5, 2,
          9, -3, -4, -3, -1, -2, -6, 3,
          -10, -2, -4, 4, -2, -3, -2, -2,
          -2, 3, 3, -4, 3, 4, -2, 4,
          5, 2, 5, 1, 0, 2, 5, 0,
          -9, -14, 0, -4, 1, -5, -9, 3,
          4, 12, 6, 13, -4, 4, 7, -3,
          4, 5, 3, 1, -2, -0, -4, -1,
          2, -0, 16, 3, -1, -6, -2, 1,
          2, -3, 6, -0, -0, -1, -1, 2,
          0, 4, -7, -0, 5, -2, -2, -2,
          4, 1, 0, -3, 5, -2, 1, 5,
          -4, -2, 0, -1, 3, -4, 4, -6,
          -5, -0, -4, -5, -1, 3, 5, -5,
          -5, -4, 1, 1, 3, 3, 3, -5,
          -5, -3, -1, -1, 5, 3, 2, -6,
          -3, -5, -3, -2, 2, 4, -3, -4,
          -2, -4, 4, 0, -0, -4, -3, -10,
          -4, 0, -3, -9, 0, 2, -1, -2,
          -4, 2, 4, 2, 3, 0, -1, -3,
          1, -2, -4, 2, -4, -4, 5, -1,
          5, -4, -5, -1, -4, -2, 4, 0,
          -5, -4, -1, -5, -3, -0, 6, 1,
          4, 6, -4, -5, -1, 1, 6, 2,
          5, -1, -1, 0, -1, -0, 2, 2,
          2, -1, -1, 4, 5, 6, 3, 2,
          2, -2, 10, 6, 6, 3, 2, -1,
          0, 5, 4, -1, -1, -1, 3, -4,
          2, 5, 1, -2, 2, -2, 1, -1,
          1, -2, 3, -3, 1, -2, -2, 1,
          1, -1, 4, 1, 3, 2, -6, 1,
          3, 4, -1, 8, 2, -1, 3, 4,
          1, 2, -5, 0, -7, -6, 2, 1,
          8, 7, -3, -3, 5, 10, 4, 12,
          -4, 6, -0, 1, -8, 2, 8, 2,
          2, 1, 2, 2, -4, 0, -1, -1,
          1, -4, -1, -4, -2, 5, 4, -0,
          -7, -4, -0, -5, -9, -11, -3, -1,
          4, 9, -5, 13, -6, -2, -8, 3,
          4, 5, 3, 5, -3, -5, -12, -2,
          0, 1, 4, 3, 1, -5, 2, -2,
          -3, 1, -2, 0, -1, -2, -1, -1,
          -6, 1, -3, 2, 3, -1, 1, 5,
          -2, 4, -4, -1, -3, 0, -4, 2,
          -2, 3, -1, -4, -2, -3, -4, 1,
          -2, -3, 11, 0, 6, 3, 0, 1,
          0, -2, 4, 1, 2, 3, 2, 3,
          7, -0, -5, -0, 1, -0, 1, 8,
          3, 0, -1, -2, -0, -0, 1, 3,
          -4, -3, -1, -9, -0, -2, 6, -2,
          2, -13, 3, -4, 1, 2, 4, 0,
          5, 5, 5, -2, -4, 5, 4, 3,
          -2, -1, 3, 2, 2, -4, -3, -4,
          6, -1, 2, -3, 2, 9, 2, 0,
          -4, -0, 3, 3, 5, 3, 1, -3,
          1, -3, 3, -1, 7, -1, 4, -4,
          -4, -2, -2, -1, 7, 3, 4, -4,
          -4, 0, -3, -1, 2, 5, 4, -3,
          -3, -1, -4, -1, -0, 5, 6, 3,
          -3, 5, -4, 5, -2, -4, 4, 3,
          1, -3, 5, -4, 5, -2, 3, 3,
          2, 3, 1, 2, 9, -2, -7, 2,
          3, 0, 2, -2, 2, -3, -4, 1,
          7, 3, 2, 0, -1, -4, -5, -1,
          9, 2, -1, -0, -4, -3, -2, 3,
          7, 6, 9, 4, -2, 1, -1, 1,
          -6, 7, 8, 3, 6, 4, 9, -1,
          5, 5, 4, -2, -1, 0, 4, -4,
          3, 2, 1, 0, 5, 0, -4, -5,
          1, -1, -1, 1, -5, -4, 0, -3,
          2, 2, 4, 3, 7, 0, -1, 2,
          4, 5, 2, 8, 10, -2, -0, 2,
          2, 2, 5, 5, 2, -1, 0, 3,
          8, 5, 1, 1, 1, -1, 1, 3,
          7, 4, 0, 0, -3, -1, 0, -3,
          -1, -3, 5, -1, 0, 0, 5, -1,
          1, -4, -4, -3, -3, -4, 4, 3,
          2, 4, 7, -4, -2, 1, 2, 4,
          4, 7, 7, 4, 1, -1, -1, -0,
          3, 18, 2, 4, 2, 2, 3, -4,
          2, 8, 1, 6, 2, 2, 2, 3,
          -4, -0, -2, 2, 7, -4, -3, -5,
          -8, -1, -3, 3, 1, 1, -5, -1,
          -4, 0, 3, -1, 5, 0, 0, -4,
          5, -1, -2, 2, -3, -3, 0, 3,
          8, 11, 3, 2, 3, 6, -3, -1,
          3, -1, 5, 0, -6, 5, -3, 1,
          -2, 9, 5, 1, -3, -1, -5, -1,
          0, -1, 3, -3, -1, -2, -2, -1,
          -1, -1, -0, -2, 2, 3, -1, -2,
          1, -1, 3, -3, 6, 2, -6, -2,
          1, -1, -2, 0, -2, -1, -2, };



int sigmoid(int x){
    return round(1 / (1 + exp(-1 * x)));
}

#endif