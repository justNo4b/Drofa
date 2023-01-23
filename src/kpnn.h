#ifndef KPNN_H
#define KPNN_H

#include <math.h>


// use net Pnet => 128x16x2 => opS /egS outputs
#define N_INPUTS   (64 * 2)
#define N_HIDDEN   (8)

int OUTPUT_BIAS1 = 2;
int OUTPUT_BIAS2 = -3;
int HIDDEN_BIAS[N_HIDDEN] = {5, -1, 2, 3, 7, 8, -3, -2, };
int OUTPUT_WEIGHTS1[N_HIDDEN] = {10, 13, -15, 19, 17, -21, -12, -12, };
int OUTPUT_WEIGHTS2[N_HIDDEN] = {5, 10, 1, -1, 6, -11, -7, -3, };


int HIDDEN_WEIGHTS[N_INPUTS * N_HIDDEN] = {
          4, -4, -4, -4, 3, 1, -3, -1, 0, 3, -0, -1, 4, 1, -2, -4, -2, -1, 1, -14, -1, 4, -1, -5, 3, -1, 1, -0, -0, 1, 2, -1, 4,
          -1, 1, -4, 1, -2, 5, -2, 4, 4, 3, -3, -4, 1, -2, -1, -3, 8, -3, -2, 0, -4, 2, -2, -2, -4, -1, 4, 3, 2, -3, 1, 4,
          1, 5, 0, 4, 0, -1, 5, -0, -4, 6, 15, 15, 17, 11, 5, -3, -4, 1, 2, 8, 5, -3, 4, -2, 5, 4, -10, 1, 2, 0, -5, -3,
          3, -0, 2, 1, 1, 4, 4, -5, 3, -8, 2, -0, 4, -0, -4, -2, 3, -12, 2, -1, -3, -0, -1, 2, -1, 1, 3, -3, -4, 2, -2, 1,
          5, -4, -2, 0, 2, 3, 0, -6, -4, 2, -7, 3, 5, -0, 1, -2, -2, -3, -1, -3, 5, -2, 3, -4, -0, -4, 6, -2, 4, -3, 1, 2,
          -0, -8, -3, -2, 3, 3, -2, 5, 3, 10, 4, 5, 2, -4, -2, -0, -3, 8, 13, 1, -9, 2, 2, -1, -2, 1, -4, -2, 0, -2, 5, -1,
          0, -3, 4, 0, 4, -2, -2, -10, -8, -10, -5, -2, -3, -5, -3, 0, 0, -4, -5, -6, -4, -3, -1, 4, 4, 2, -3, -1, -1, -2, 0, 3,
          6, 3, -1, 3, 1, -2, -3, 3, 7, 5, -0, 4, -0, -0, 6, 3, 6, 1, 8, 7, 2, 3, 6, -2, -4, 0, -4, -1, -3, -4, -2, -3,
          -4, -1, 3, -4, 1, -4, 5, 1, 0, -4, 6, -1, 3, 4, -1, 1, -0, 3, 2, -0, 3, 4, -1, -0, 2, 4, 0, -2, 0, 3, 2, -0,
          9, -1, -4, -3, 2, 2, 10, 6, 5, -0, -5, -6, -2, 2, 1, 6, -8, 3, 0, 3, -5, 2, 7, 0, 5, 3, 4, 5, -2, 0, 1, -3,
          4, -1, -2, 5, 2, -4, 3, -4, -7, -7, 4, -2, -4, -0, 1, -3, 1, -2, 0, -5, 1, 0, -3, 4, -1, 3, 0, -3, 7, -4, -5, -1,
          -2, -6, 7, 1, 3, 1, -3, -1, -3, -5, 0, 6, -0, 8, -1, -4, -1, -2, -2, -5, -1, 0, 0, 1, -1, 1, 5, 1, -1, 1, -1, 0,
          -3, -2, -3, 4, 1, -4, 1, 5, 2, 1, 2, 5, 4, -1, -6, 2, 8, 3, 2, -3, 0, -4, -4, -0, 9, 2, 4, 0, -0, -5, -4, -3,
          6, 3, 1, -1, -2, -4, -2, 4, 9, 6, 3, 6, -0, 15, 6, 1, 10, 10, 8, -1, 1, -7, 1, 4, 0, -2, 1, 0, -2, 2, 3, 0,
          5, 1, -3, -4, 5, 2, 0, -2, -3, -2, -3, 1, -14, -7, -2, -2, 2, -4, 4, -1, 6, -3, -2, 0, -2, 0, 3, 8, 7, 1, -5, -0,
          -4, 2, 1, 3, 1, 1, 0, 1, 6, 1, -1, 0, 0, -5, 1, 1, 11, 4, -6, -2, -3, -4, 1, 3, 5, 0, -2, -2, -3, -1, -1, -4,
          -1, 3, 5, -4, 0, 5, 3, -2, -3, -4, -2, -4, -4, 3, 7, -2, -3, -0, 1, 0, 2, 4, 5, -3, -2, -2, 2, 2, 6, 4, 7, -3,
          -2, -1, -1, 1, 6, 5, 3, -1, -2, -2, -2, -0, -1, 4, 3, 1, 3, -1, -5, -9, -2, 1, -3, -3, -4, 0, 5, 2, 3, 5, 2, -1,
          2, 3, 0, -1, -1, 2, 2, -4, 9, 10, -4, 4, -1, 4, 6, 8, 3, 8, 3, -8, -2, -4, 13, -0, -2, 0, -2, -5, -3, -0, 7, -1,
          -2, -1, -8, -6, -1, 0, 2, -1, -2, -4, -2, -1, 1, 2, 3, -1, -3, -2, 1, -1, 2, 1, 3, -1, 5, -3, 3, -2, -4, 2, 0, 3,
          3, 2, 2, 1, 1, 3, -1, 0, 0, -1, -7, -0, 5, 2, 3, 0, 0, -1, -1, 0, 2, 3, 4, 2, 5, -4, -2, -2, 1, 1, 2, 7,
          -1, 3, 3, -4, -1, 4, -0, 5, 8, -4, 1, -6, -1, -2, 13, -7, -2, -9, -4, 0, -2, 4, -2, -1, 4, 1, 1, -3, 5, 0, 0, -4,
          5, 5, 1, 5, 4, 0, -4, 2, -6, -3, 1, -4, -3, 9, 3, -3, -3, -3, -5, -9, -1, -2, -3, -3, -2, -4, -2, -1, 9, -1, 2, -5,
          -4, -3, -5, -0, 3, 6, 3, -5, -3, -2, -3, -2, -0, 2, 4, -4, -2, -3, 1, -1, -5, 5, 5, 4, 0, 1, 1, 4, 0, -2, 3, 1,
          -1, 3, -2, 5, 3, -1, 1, -0, -2, -0, 1, -0, -4, 0, 1, -1, -3, 8, -4, -1, 0, -2, 0, -2, -5, -0, 2, -1, 0, 0, 2, -2,
          -1, -10, 1, 6, 3, -0, 0, 1, -2, -14, 1, 4, 3, -16, -2, -3, 8, -11, -6, 0, 2, 1, -12, 3, -1, 4, -3, -1, 1, -1, 4, 5,
          2, 4, 5, -4, -1, 0, -1, -4, 9, -8, 2, 6, 6, 5, -0, -0, -4, -4, 3, -7, -7, 1, 6, 2, -2, 16, -3, -6, -1, 5, -1, 2,
          1, 9, -3, -0, -2, 3, -0, -1, 6, -10, -3, -2, 4, 3, 2, 1, 0, -4, -2, -4, 2, 2, 1, -4, 1, -1, 4, 2, 2, 4, -4, 4,
          0, 2, -4, -2, 0, -2, 5, 1, -0, 6, -2, 3, -3, -1, 1, -0, 1, -1, 1, -4, 1, -1, 0, 1, -0, 1, 4, 4, 4, -1, 2, 4,
          -0, -3, -2, 1, 17, 8, -6, -2, -2, 6, -6, -1, -7, 5, 0, 6, -3, -4, -1, -0, -6, 10, 5, 1, -2, -1, -2, 0, -2, 5, -1, 3,
          2, -2, 2, 0, 5, 4, 1, 1, -12, 5, -3, 1, 6, 3, 0, 2, -0, -6, 1, 14, -3, 2, 11, -2, 0, -7, 1, 4, -4, 0, 1, 1,
          1, 2, -5, -6, 2, -3, -2, 1, 3, 1, 6, 1, -1, -3, -2, 2, 11, 1, -0, -1, 0, -3, 1, 1, -2, 2, -4, 0, 3, -1, -4, };



int sigmoid(int x){
    return round(1 / (1 + exp(-1 * x)));
}

#endif