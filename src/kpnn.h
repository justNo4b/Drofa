#ifndef KPNN_H
#define KPNN_H

#include <math.h>


// use net Pnet => 128x16x2 => opS /egS outputs
#define N_INPUTS   (64 * 2)
#define N_HIDDEN   (16)

int OUTPUT_BIAS1 = -15;
int OUTPUT_BIAS2 = 8;
int HIDDEN_BIAS[N_HIDDEN] = {-60, -11, -21, 6, 16, 18, 21, -5, 25, 26, -27, 7, 15, -45, 14, 5, };
int OUTPUT_WEIGHTS1[N_HIDDEN] = {-3, 21, 13, 24, -15, 16, -11, 20, -27, 11, -38, -17, -19, -26, 26, 5, };
int OUTPUT_WEIGHTS2[N_HIDDEN] = {-52, -13, -7, 4, -5, 11, -8, -5, 13, 3, -17, -6, -8, -6, 5, 10, };


int HIDDEN_WEIGHTS[N_INPUTS * N_HIDDEN] = {
          5, -3, -2, -1, 5, 1, 4, -2, -5,
          -0, -48, -66, 8, 20, 21, 21, -22,
          -15, -36, -2, 14, 16, 17, 22, 12,
          -2, 13, 2, 12, 10, 11, 7, -1,
          10, 3, 20, 1, 8, -22, -6, 31,
          35, -19, -44, -51, -28, -15, -27, 62,
          65, 1, -0, -13, -36, 24, 42, 3,
          5, -1, -2, 2, -1, 2, 2, 3,
          1, -3, 0, -4, -2, -1, 2, 32,
          41, 35, 36, 38, 28, -47, 19, 31,
          34, 33, 29, 16, 18, 2, 24, 29,
          18, 32, -19, -20, -12, -69, -10, 3,
          23, 6, 2, -96, -34, -40, -18, 0,
          -13, -3, 3, -79, -26, -13, -21, -5,
          -16, -18, -66, -12, -15, -25, -22, 1,
          5, 1, -2, 1, -4, 1, 1, 2,
          -1, 1, 1, 0, 5, -2, -2, -4,
          1, -14, -19, -20, -32, 1, -9, -2,
          -3, 24, -19, -19, -44, -31, -15, -23,
          4, -10, 19, 3, -14, -9, -2, -10,
          12, 26, 13, 15, -44, -71, -33, -9,
          85, 19, -34, 34, 67, 61, -46, 6,
          21, -11, 16, -63, 99, 17, 6, 1,
          1, -1, -2, 3, 3, 0, -4, -2,
          -2, -4, 3, -1, 5, -4, -1, 9,
          52, 33, 16, -60, 32, 14, 56, 8,
          2, 19, 33, 2, -35, -33, 1, 18,
          9, 31, 5, -8, -67, 4, 3, 19,
          13, 3, 29, -17, -67, 11, 27, 20,
          13, -6, -7, 8, -33, -36, -12, 14,
          -24, 0, -32, 10, 16, -40, -31, -3,
          1, 1, 3, 1, 4, 0, 3, -1,
          3, -2, 1, -4, 2, -1, -1, -21,
          2, -16, 24, -24, -7, -21, -16, -12,
          -0, -64, -54, 26, 6, 4, 3, -1,
          -22, -2, -9, -15, -3, 7, 8, 73,
          -3, 43, -34, -32, -14, 49, 23, 33,
          35, 74, 67, 85, 13, 4, 31, -9,
          47, 34, 54, -76, 69, 72, 14, -2,
          5, 3, 3, 5, -3, 2, 2, 3,
          -4, -1, 4, 0, -1, 0, 4, 16,
          -9, 46, 5, -32, -14, 28, 103, -72,
          -41, -35, 1, -99, 31, -70, -32, 50,
          9, -44, -113, 21, -39, 47, 73, 44,
          27, -5, -155, -36, 1, 39, 16, 27,
          10, 11, 31, 17, 16, 2, 6, 28,
          38, -2, 44, 16, -28, -20, 19, -4,
          3, 4, -3, 4, -2, 3, 4, 3,
          -3, -3, -1, 5, 0, -2, -3, -13,
          -47, -21, -23, -17, 4, 17, -6, -35,
          -53, -32, -28, -20, -2, 7, -3, -23,
          -47, -42, -8, -26, -2, 15, -8, -14,
          -53, -28, 22, -13, 3, 34, -8, 52,
          -65, 2, 12, -4, 1, 49, -2, -46,
          37, 13, 18, -16, 2, 11, 17, -2,
          -3, 2, -3, 5, -4, 4, 5, 2,
          5, 4, 0, -3, 1, 0, 1, 10,
          -45, -33, -51, -56, -50, 93, 27, 12,
          -5, -18, -74, 3, 12, 11, 38, 21,
          -31, -27, -14, 17, 38, 23, 22, 4,
          13, 21, -14, 16, -5, 27, -9, 24,
          7, 5, -23, 3, -28, -3, -4, 32,
          32, -31, 54, -59, -65, -26, -9, 0,
          -1, -2, -4, 3, -3, -4, -4, 5,
          0, 0, 5, -3, -1, -1, -2, -2,
          10, 12, -37, -37, -13, -5, 5, -12,
          -21, 15, -2, 2, 26, 18, 1, 0,
          -42, 3, 22, -3, 24, 33, -0, 11,
          -39, -20, 0, 30, 17, 33, 52, -16,
          -46, 4, -6, 11, 51, 26, 33, -14,
          31, -18, 10, 13, -22, 50, -35, 3,
          1, -3, 0, 4, 5, -3, 0, -3,
          2, -4, 2, -1, 3, 2, 2, 59,
          5, -20, -16, 1, 47, -9, -6, 67,
          22, 77, 5, 104, 57, 71, 47, 9,
          33, 3, -2, 10, -39, -14, 3, 45,
          38, -3, 21, 8, -21, -21, -26, 40,
          69, -22, 29, -14, 1, -19, -24, 39,
          73, -18, 18, 4, -0, -8, -25, -4,
          1, -1, -2, 5, -4, 2, 4, 4,
          3, -4, -2, -4, 4, 2, 5, -34,
          -9, 13, -6, 5, 17, 2, 53, -27,
          -16, 2, 8, 11, 43, 2, 47, -13,
          -36, -10, -11, 23, 58, 32, 62, -12,
          -25, 15, -33, 41, -6, 75, 10, -23,
          77, 69, 44, 107, -15, -40, -35, 119,
          -0, 72, 68, 44, -7, 7, -14, 3,
          0, 2, 4, 5, 3, 2, 3, 5,
          2, 4, 4, -3, 3, -4, -2, 31,
          31, 51, -31, -0, -29, -18, 18, -6,
          33, -10, 4, -73, -52, -25, 42, 65,
          7, 48, -23, -35, -35, -52, -9, 2,
          35, -6, -7, 18, 2, -34, -14, 6,
          25, -9, -24, 40, -2, -21, 1, 11,
          16, 41, 79, 69, -11, 7, 34, 1,
          -3, -3, 4, 4, -3, 5, 5, 4,
          3, -3, -1, 2, 1, -4, -1, -12,
          -30, 17, -15, -10, -16, 13, 1, -19,
          -22, 7, -2, -1, -16, 7, 8, -19,
          -26, -13, -3, -8, 11, 9, 28, -16,
          -17, -24, 4, 34, 61, 153, -6, -18,
          -48, -10, 18, 17, 80, 30, -24, -19,
          12, -23, 5, 43, 22, 75, 64, -3,
          -4, -2, 3, 5, -1, -4, -3, 2,
          0, -4, 1, -4, -1, -2, -3, -25,
          96, -35, -18, -69, 85, -37, -59, -31,
          -13, -24, -41, -97, -82, -24, 74, 35,
          -23, 26, 20, 10, -30, -26, -1, 33,
          6, 63, 6, 20, -29, -5, -22, 33,
          13, 32, 2, 26, -15, 4, -26, 34,
          29, 38, 1, -2, 4, -20, -36, 0,
          -1, -1, 0, -3, 3, 5, -2, -4,
          -4, -1, 2, 0, 1, 0, 2, 20,
          14, 6, 52, 18, 58, -14, -38, 3,
          14, -12, -13, -10, 8, -6, -23, 1,
          9, 24, -7, 7, 14, -35, -18, -15,
          6, 26, 10, -5, -4, -37, -11, 52,
          42, 67, 82, 72, 28, 12, 54, 102,
          37, 79, 63, -20, 15, -27, 32, -2,
          5, -2, 2, -3, 4, 5, -1, 3,
          5, 5, -3, -2, -4, 3, 1, -9,
          -35, -66, -58, -61, -69, -44, -28, -41,
          -10, -47, -19, -0, 5, -46, -1, 3,
          -9, -5, 26, 27, 42, 35, 0, 10,
          -10, 11, 154, 49, 21, 9, 1, 8,
          27, 53, 5, -11, 5, -33, -17, 15,
          68, 97, -15, -13, -11, -6, -1, 5,
          -4, -3, -3, -3, 2, -4, 5, 5,
          5, 5, 1, 2, -1, 4, 5, 16,
          -9, -53, -21, -7, -14, -35, -16, 14,
          27, 12, -33, 7, -34, -24, -30, 14,
          21, -6, 27, -2, -15, 7, 11, 16,
          37, 26, 38, -43, -26, -32, 4, -15,
          34, 30, 19, 9, -19, 42, -20, -10,
          41, -17, -22, 34, -20, 27, 67, 1,
          4, -1, -4, 1, 5, -1, 4, -4,
          -2, -4, 1, 3, 3, -1, 4, 22,
          -46, -1, 44, -3, 29, -6, -32, -17,
          -3, -71, 6, 93, 27, 21, -28, -30,
          -21, -12, -8, 14, 4, 12, 2, -47,
          -11, -28, 15, -22, -19, -18, -20, -12,
          -34, 12, -3, -68, -25, -40, -21, -18,
          -3, 7, -36, -58, -11, -14, -5, 4,
          -3, 4, -4, 3, 2, -1, 4, -1,
          -4, 1, 3, -4, 4, -2, -2, -20,
          -18, -8, -39, -3, -9, 38, 66, -16,
          -38, -14, -22, 40, 20, 44, 18, -15,
          -6, -26, -6, -20, 62, 3, 35, 1,
          -9, -37, 15, -37, 103, -10, 47, -44,
          -0, 18, 38, -10, 66, 78, 48, 54,
          -33, 36, 61, -55, -21, -141, 34, -4,
          3, 4, 1, -2, 3, 0, 3, -4,
          4, -2, -3, 0, 5, -3, -3, -24,
          45, -8, -7, -30, 11, 17, -18, 19,
          -14, -5, -32, -64, -36, -2, 4, -10,
          -3, -7, 4, -32, -7, -6, 17, 20,
          -11, -29, -29, -30, -8, -6, 8, 5,
          -6, -18, 17, -10, 12, 19, 1, 1,
          -12, -31, 25, 40, 68, 14, -24, 2,
          3, -4, -3, -3, 5, 3, 0, 5,
          0, -4, 3, -2, 1, 0, -3, 18,
          16, -0, 58, -69, -32, -13, -9, 31,
          28, -6, -33, -36, -53, -9, -24, 50,
          15, 4, -72, 7, -10, 30, -26, -3,
          -30, 20, -3, 33, 2, 1, 11, 16,
          -69, -62, 3, 31, 19, 54, 22, 31,
          20, -53, -23, 23, 9, -33, 20, -1,
          3, 3, -4, -3, -3, 3, 3, 1,
          -4, 2, -1, 4, 0, 0, -3, -8,
          -35, -1, 24, 3, 33, 46, -6, 22,
          -13, 11, 40, 40, 8, 14, 23, -60,
          -48, -14, 32, 17, -11, 21, 19, -48,
          -47, -4, -13, -29, -17, -6, 14, -16,
          -112, -28, -10, -21, -50, 14, -23, -18,
          -33, -22, -1, -62, -29, -9, -19, 3,
          3, -1, 5, 2, 5, 1, -1, -3,
          1, 5, 5, 4, -1, 2, -3, 11,
          -1, -14, 41, -5, 52, 47, -3, 8,
          -2, 11, -17, -3, 24, 43, 15, 9,
          53, 3, -32, -27, -18, 13, 14, 39,
          -1, 22, -19, -34, 0, 15, 77, 62,
          6, -23, -33, -54, -13, -67, 53, 8,
          -2, -52, 8, -5, -16, 22, -19, 1,
          0, -2, 1, 4, 5, 3, 1, 0,
          1, -2, -1, 5, 4, 5, 1, -14,
          -18, -28, 10, -20, -40, 67, -115, -27,
          -26, -27, -4, -107, 11, -17, -34, -17,
          -29, -43, -2, -34, 27, -20, 18, -11,
          -23, -9, -6, 4, 40, 30, 6, -16,
          -24, -25, -8, 11, -17, 50, 6, -24,
          -16, -16, -15, -21, -11, 31, 32, -3,
          -3, 4, 0, 4, -3, 5, -3, -2,
          5, -2, -2, 4, 4, -2, 3, 17,
          6, -4, -11, 39, -2, -1, 49, 10,
          -3, 6, -12, 6, 12, 29, 51, 15,
          1, -20, -6, 27, 25, -9, 4, 64,
          19, -30, -35, -31, -45, 3, -10, 7,
          -3, -51, -2, -38, -24, 16, 4, -7,
          -10, -45, -73, -52, -26, -22, -25, 3,
          -2, 1, -1, 2, -1, 5, -3, -3,
          3, -2, 4, 3, -4, -1, 1, -20,
          88, 56, 25, 9, -9, 78, 26, -20,
          -46, -33, -48, 15, -13, 5, -31, -37,
          -22, -13, -27, 53, 28, 38, 49, -21,
          -31, -7, -24, 18, 2, 28, 55, -33,
          -15, -6, 32, -21, 13, -8, 59, -40,
          12, 38, 23, -20, 3, 31, 40, 1,
          2, 1, 5, 2, -4, -2, -1, 2,
          2, 4, -3, -2, 5, 2, 1, 13,
          -9, -17, 31, 57, 19, 3, -40, 14,
          12, 20, 16, -12, 5, -20, -3, 28,
          2, 19, 13, -14, -40, -39, -31, 53,
          11, 22, -25, 5, -39, -12, -65, 52,
          -22, 16, -24, -36, -92, -22, -60, -65,
          48, -11, -31, -45, 37, -82, 40, 4,
          3, 4, 4, 2, -3, -1, 5, -3,
          5, 2, -3, -4, 3, -1, 4, 35,
          -6, 42, 11, 41, 21, 14, -15, 34,
          72, 20, 44, 26, -0, 20, -17, 6,
          14, 11, 13, 21, 14, 4, -22, 9,
          18, 19, -35, 12, 18, -11, -27, -4,
          37, -4, 5, 6, -12, -85, -46, 0,
          -13, 40, 22, -67, -48, -26, -41, 3,
          0, 0, 5, -3, 0, -1, 2, -4,
          4, -2, 1, 4, 3, 1, 1, 17,
          11, 28, 2, 73, 67, 21, -27, 10,
          32, 111, 6, -1, 32, 5, -7, 9,
          55, -8, -12, -61, -2, -32, -17, 4,
          73, 23, 18, -44, -6, -31, -9, -21,
          50, 23, 47, -27, -13, -29, -12, -20,
          1, 72, 14, -7, -55, 19, 94, -3,
          3, 2, -1, -1, 3, 3, 2, -3,
          0, 4, -1, 0, -3, 4, -3, 23,
          18, -67, 21, 23, -22, -18, -50, 22,
          -2, 40, 36, 40, 16, 51, 25, 32,
          18, -27, -12, 31, -13, 34, 46, 25,
          16, 46, 33, -4, -4, -6, -11, 35,
          43, 29, 32, -9, -23, -5, -13, 8,
          37, 13, 8, -3, -17, -6, -4, -2,
          -3, 3, -2, -4, -3, 3, -4, -3,
          -3, -2, -3, 2, 2, 2, 3, 26,
          23, 4, -15, 23, -9, 6, -13, 26,
          25, 22, 11, 17, 4, 32, -8, 32,
          16, 17, 14, 34, 1, 33, -6, 43,
          12, -1, -6, 15, -33, 25, 7, 31,
          -17, -61, -43, 1, -1, 93, 91, 59,
          153, -34, -47, -36, 6, -33, 27, 5,
          0, 1, 4, 4, 5, 4, -1, 1,
          4, -1, 3, 1, -3, 2, 4, -1,
          9, 53, 41, 73, 10, -11, -27, -1,
          14, 64, 36, 35, 78, 2, -72, -17,
          -0, -5, 26, 32, -72, -16, 87, -9,
          -29, 5, -16, 4, -73, -1, 133, -12,
          -21, -22, -6, -2, -13, -0, -13, -15,
          -19, -8, -25, -40, -10, -9, -16, 4,
          4, -1, 0, -4, -1, 2, -2, };

int sigmoid(int x){
    return round(1 / (1 + exp(-1 * x)));
}

#endif